#!/usr/bin/env python3
# ---------------------------------------------------------------------------------------
# Build a milliSoC flash model image from Vela's raw (.npz) output.
#
# Vela --output-format raw produces the pieces the Ethos-U needs at run time:
#
#   cmd_data        the COP1 *driver payload* (see strip_driver_payload())
#   weight_data     the read-only weights / scale stream
#   *_region        the BASEP_ARRAY region each buffer belongs to
#   input/output    the offsets of the I/O tensors inside their region
#
# The NPU itself consumes the bare register command stream, not the driver
# payload that wraps it.  The payload starts with a "COP1" fourcc, a config
# record and a length-tagged command-stream header; feeding it to the NPU
# verbatim makes the command parser fault on the "COP1" word.  This tool
# strips that header and stores only the register command stream.
#
# This tool packs them into one self-describing image:
#
#   offset 0x00  model_image_header_t (24 x uint32, 96 bytes)
#   offset 0x60  command stream (raw register stream, 16-byte aligned)
#   ...          zero padding
#   weight_off   weights
#
# The image is what gets stored in flash (or copied to SRAM).  The header lets
# the runtime program BASEP_ARRAY / QCONFIG / REGIONCFG without parsing a
# .tflite flatbuffer.
#
# Outputs (all in --output-dir):
#   <name>_image.bin   raw image
#   <name>_image.hex   one byte per line, for armdbg/scripts/flash_programmer.py
#   <name>_image.json  manifest (offsets, sizes, regions, CRC32)
#   <name>_image.h     C constants and the header struct
# ---------------------------------------------------------------------------------------

import argparse
import csv
import json
import os
import struct
import sys
import zlib

import numpy as np

HEADER_SIZE = 96
HEADER_MAGIC = 0x314C564D  # "MVL1" little-endian
HEADER_VERSION = 1
CMD_ALIGN = 64
WEIGHT_ALIGN = 64
ARENA_ALIGN = 64
# The flash programmer writes the QSPI FIFO four 32-bit words at a time, so the
# byte-per-line hex written for it ends on a 16-byte boundary.  The .bin keeps
# its exact length; the header's image_size is what the runtime copies.
HEX_ALIGN = 16
ERASED_BYTE = 0xFF

# Driver payload tags (ethosu/vela/driver_actions.py: DACommands).
DA_CONFIG = 0x01
DA_CMD_STREAM = 0x02
DA_NOP = 0x05
DA_FOURCC_COP1 = 0x31504F43  # "COP1" little-endian


def align_up(value, align):
    return (value + align - 1) // align * align


def strip_driver_payload(payload):
    """Return (raw_cmd_stream_bytes, config_word, id_word).

    Vela's raw output wraps the register command stream in the payload the
    Ethos-U *driver* parses:

        word 0        "COP1" fourcc
        word 1        Config tag  (0x01)
        word 2        config word (MACs/CC, SHRAM size)
        word 3        NPU ID word
        words 4..     NOP tags padding the stream to 16 bytes
        word N        CmdStream tag: bits[7:0]=0x02, bits[15:8]=length>>16,
                      bits[31:16]=length & 0xFFFF
        words N+1..   raw register command stream (length words)

    The NPU queue wants the words after the CmdStream tag.  Vela guarantees
    the tag sits on a 16-byte boundary, so the stripped stream keeps the
    alignment the NPU expects.
    """
    if len(payload) < 8 * 4 or len(payload) % 4:
        raise ValueError("driver payload is not a multiple of 4 bytes / too short")

    words = struct.unpack("<%dI" % (len(payload) // 4), payload)
    if words[0] != DA_FOURCC_COP1:
        raise ValueError("driver payload does not start with COP1 (0x%08X)"
                         % words[0])

    config_word = None
    id_word = None
    index = 1
    while index < len(words):
        tag = words[index]
        index += 1
        tag_id = tag & 0xFF

        if tag_id == DA_NOP:
            continue
        if tag_id == DA_CONFIG:
            config_word = words[index]
            id_word = words[index + 1]
            index += 2
            continue
        if tag_id == DA_CMD_STREAM:
            length = ((tag >> 16) & 0xFFFF) | (((tag >> 8) & 0xFF) << 16)
            stream = payload[index * 4:(index + length) * 4]
            if len(stream) != length * 4 or not length:
                raise ValueError("command stream length %d does not match payload"
                                 % length)
            return stream, config_word, id_word
        raise ValueError("unexpected driver payload tag 0x%08X" % tag)

    raise ValueError("driver payload has no command stream")


def npz_bytes(z, key):
    """Return the raw bytes of an npz entry, whether stored as |S or uint8."""
    arr = z[key]
    if arr.ndim == 0:
        if arr.dtype.kind == "S":
            return arr.item()
        return arr.tobytes()
    return arr.tobytes()


def npz_int(z, key):
    arr = z[key]
    if arr.ndim == 0:
        return int(arr)
    return int(arr.reshape(-1)[0])


def tensor_size(z, shape_key, elem_key):
    shape = np.asarray(z[shape_key]).reshape(-1)
    count = 1
    for dim in shape:
        count *= int(dim)
    return count * npz_int(z, elem_key)


def read_summary_macs(path):
    """Return Vela's nn_macs from a --summary CSV, or None if it is missing.

    Vela writes one row per experiment; the milliSoC flow has a single one.
    """
    with open(path, newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        raise ValueError("%s: no rows" % path)
    if "nn_macs" not in rows[0]:
        raise ValueError("%s: no nn_macs column" % path)
    return int(round(float(rows[0]["nn_macs"])))


def build_header(cmd, weights, meta):
    cmd_offset = HEADER_SIZE
    assert cmd_offset % 16 == 0, "command stream must stay 16-byte aligned"
    weight_offset = align_up(cmd_offset + len(cmd), WEIGHT_ALIGN)
    image_size = weight_offset + len(weights)

    input_end = meta["input_offset"] + meta["input_size"]
    output_end = meta["output_offset"] + meta["output_size"]
    io_end = max(input_end, output_end)
    # Vela does not export the scratch offset, only its size.  Assume the worst
    # case (scratch placed after the I/O) and round up.
    arena_size = align_up(max(io_end, meta["scratch_size"]) +
                          meta["scratch_size"] +
                          meta["scratch_fast_size"], ARENA_ALIGN)

    crc = zlib.crc32(cmd + weights) & 0xFFFFFFFF

    fields = [
        HEADER_MAGIC,
        HEADER_VERSION,
        HEADER_SIZE,
        image_size,
        cmd_offset,
        len(cmd),
        weight_offset,
        len(weights),
        meta["weight_region"],
        meta["scratch_region"],
        meta["scratch_size"],
        meta["scratch_fast_region"],
        meta["scratch_fast_size"],
        meta["input_region"],
        meta["input_offset"],
        meta["input_size"],
        meta["output_region"],
        meta["output_offset"],
        meta["output_size"],
        arena_size,
        crc,
        meta["macs"],
        0,
        0,
    ]
    header = struct.pack("<24I", *fields)
    assert len(header) == HEADER_SIZE
    return header, image_size, weight_offset, arena_size, crc


def write_plain_hex(path, data, align=HEX_ALIGN, fill=ERASED_BYTE):
    """Write one byte per line, padding to `align` bytes with erased-flash 0xFF."""
    tail_pad = (-len(data)) % align
    with open(path, "w", newline="\n") as f:
        for byte in data:
            f.write("%02X\n" % byte)
        for _ in range(tail_pad):
            f.write("%02X\n" % fill)
    return tail_pad


def write_c_header(path, name, meta, header, image_size, weight_offset,
                   arena_size, crc, base):
    guard = "%s_IMAGE_H" % name.upper().replace("-", "_").replace(".", "_")
    with open(path, "w", newline="\n") as f:
        f.write("/* Generated by tools/model_to_image.py - do not edit. */\n")
        f.write("#ifndef %s\n#define %s\n\n" % (guard, guard))
        f.write("#include <stdint.h>\n\n")
        f.write("#define MODEL_IMAGE_MAGIC        0x%08XUL\n" % HEADER_MAGIC)
        f.write("#define MODEL_IMAGE_VERSION      %dUL\n" % HEADER_VERSION)
        f.write("#define MODEL_IMAGE_HEADER_SIZE  %dUL\n" % HEADER_SIZE)
        f.write("#define MODEL_IMAGE_SIZE         %dUL\n" % image_size)
        f.write("#define MODEL_IMAGE_BASE         0x%08XUL\n" % base)
        f.write("#define MODEL_IMAGE_CMD_OFFSET   %dUL\n" % HEADER_SIZE)
        f.write("#define MODEL_IMAGE_CMD_SIZE     %dUL\n" % len(meta["cmd"]))
        f.write("#define MODEL_IMAGE_WEIGHT_OFFSET %dUL\n" % weight_offset)
        f.write("#define MODEL_IMAGE_WEIGHT_SIZE  %dUL\n" % len(meta["weights"]))
        f.write("#define MODEL_IMAGE_WEIGHT_REGION %dUL\n" % meta["weight_region"])
        f.write("#define MODEL_IMAGE_SCRATCH_REGION %dUL\n" % meta["scratch_region"])
        f.write("#define MODEL_IMAGE_SCRATCH_SIZE %dUL\n" % meta["scratch_size"])
        f.write("#define MODEL_IMAGE_FAST_REGION  %dUL\n" % meta["scratch_fast_region"])
        f.write("#define MODEL_IMAGE_FAST_SIZE    %dUL\n" % meta["scratch_fast_size"])
        f.write("#define MODEL_IMAGE_INPUT_REGION %dUL\n" % meta["input_region"])
        f.write("#define MODEL_IMAGE_INPUT_OFFSET %dUL\n" % meta["input_offset"])
        f.write("#define MODEL_IMAGE_INPUT_SIZE   %dUL\n" % meta["input_size"])
        f.write("#define MODEL_IMAGE_OUTPUT_REGION %dUL\n" % meta["output_region"])
        f.write("#define MODEL_IMAGE_OUTPUT_OFFSET %dUL\n" % meta["output_offset"])
        f.write("#define MODEL_IMAGE_OUTPUT_SIZE  %dUL\n" % meta["output_size"])
        f.write("#define MODEL_IMAGE_ARENA_SIZE   %dUL\n" % arena_size)
        f.write("#define MODEL_IMAGE_MACS         %dUL\n" % meta["macs"])
        f.write("#define MODEL_IMAGE_CRC32        0x%08XUL\n\n" % crc)
        f.write("typedef struct {\n")
        names = [
            "magic", "version", "header_size", "image_size",
            "cmd_offset", "cmd_size", "weight_offset", "weight_size",
            "weight_region", "scratch_region", "scratch_size",
            "scratch_fast_region", "scratch_fast_size",
            "input_region", "input_offset", "input_size",
            "output_region", "output_offset", "output_size",
            "arena_size", "crc32", "macs", "reserved1", "reserved2",
        ]
        for field in names:
            f.write("    uint32_t %s;\n" % field)
        f.write("} model_image_header_t;\n\n")
        f.write("#endif /* %s */\n" % guard)


def main(argv):
    parser = argparse.ArgumentParser(
        description="Pack Vela raw output into a milliSoC flash model image.")
    parser.add_argument("--input", required=True,
                        help="Vela raw output (.npz)")
    parser.add_argument("--output-dir", required=True,
                        help="directory for the generated files")
    parser.add_argument("--name", required=True,
                        help="base name, e.g. hello_world_int8")
    parser.add_argument("--base", type=lambda v: int(v, 0), default=0x008000,
                        help="flash byte offset of the image (default 0x8000)")
    parser.add_argument("--summary", default=None,
                        help="Vela summary CSV; its nn_macs is stored in the "
                             "image header for the throughput report")
    parser.add_argument("--macs", type=lambda v: int(v, 0), default=None,
                        help="MAC count per inference, overriding --summary")
    args = parser.parse_args(argv)

    with np.load(args.input, allow_pickle=True) as z:
        payload = npz_bytes(z, "cmd_data")
        weights = npz_bytes(z, "weight_data")
        try:
            cmd, config_word, id_word = strip_driver_payload(payload)
        except ValueError as exc:
            sys.exit("ERROR: %s: %s" % (args.input, exc))
        meta = {
            "cmd": cmd,
            "payload_size": len(payload),
            "config_word": config_word,
            "id_word": id_word,
            "weights": weights,
            "weight_region": npz_int(z, "weight_region"),
            "scratch_region": npz_int(z, "scratch_region"),
            "scratch_size": npz_int(z, "scratch_size"),
            "scratch_fast_region": npz_int(z, "scratch_fast_region"),
            "scratch_fast_size": npz_int(z, "scratch_fast_size"),
            "input_region": npz_int(z, "input_region"),
            "input_offset": npz_int(z, "input_offset"),
            "input_size": tensor_size(z, "input_shape", "input_elem_size"),
            "output_region": npz_int(z, "output_region"),
            "output_offset": npz_int(z, "output_offset"),
            "output_size": tensor_size(z, "output_shape", "output_elem_size"),
        }

    macs = 0
    if args.macs is not None:
        macs = args.macs
    elif args.summary:
        try:
            macs = read_summary_macs(args.summary)
        except (OSError, ValueError) as exc:
            print("WARNING: cannot read nn_macs from %s: %s" % (args.summary, exc))
            macs = 0
    meta["macs"] = macs

    if not cmd:
        sys.exit("ERROR: %s: empty command stream" % args.input)
    if not weights:
        sys.exit("ERROR: %s: empty weights" % args.input)
    if args.base % 4096:
        print("WARNING: --base 0x%X is not 4-KiB aligned; the flash "
              "programmer will reject it" % args.base)

    header, image_size, weight_offset, arena_size, crc = build_header(
        cmd, weights, meta)

    image = bytearray(image_size)
    image[0:HEADER_SIZE] = header
    image[HEADER_SIZE:HEADER_SIZE + len(cmd)] = cmd
    image[weight_offset:weight_offset + len(weights)] = weights

    os.makedirs(args.output_dir, exist_ok=True)
    stem = os.path.join(args.output_dir, args.name + "_image")

    with open(stem + ".bin", "wb") as f:
        f.write(image)
    hex_pad = write_plain_hex(stem + ".hex", image)
    write_c_header(stem + ".h", args.name, meta, header, image_size,
                   weight_offset, arena_size, crc, args.base)

    manifest = {
        "model": args.name,
        "input": os.path.basename(args.input),
        "base": "0x%08X" % args.base,
        "image_size": image_size,
        "cmd": {"offset": HEADER_SIZE, "size": len(cmd),
                "driver_payload_size": meta["payload_size"],
                "config_word": None if meta["config_word"] is None
                else "0x%08X" % meta["config_word"],
                "id_word": None if meta["id_word"] is None
                else "0x%08X" % meta["id_word"]},
        "weights": {"offset": weight_offset, "size": len(weights),
                    "region": meta["weight_region"]},
        "scratch": {"region": meta["scratch_region"],
                    "size": meta["scratch_size"]},
        "scratch_fast": {"region": meta["scratch_fast_region"],
                         "size": meta["scratch_fast_size"]},
        "input": {"region": meta["input_region"],
                  "offset": meta["input_offset"],
                  "size": meta["input_size"]},
        "output": {"region": meta["output_region"],
                   "offset": meta["output_offset"],
                   "size": meta["output_size"]},
        "arena_size": arena_size,
        "macs": meta["macs"],
        "crc32": "0x%08X" % crc,
    }
    with open(stem + ".json", "w", newline="\n") as f:
        json.dump(manifest, f, indent=2)
        f.write("\n")

    print("Model image : %s" % (stem + ".bin"))
    print("  flash     : 0x%08X .. 0x%08X" % (args.base,
                                              args.base + image_size - 1))
    print("  cmd       : +0x%04X, %d bytes (from %d-byte COP1 driver payload)" %
          (HEADER_SIZE, len(cmd), meta["payload_size"]))
    print("  weights   : +0x%04X, %d bytes, region %d" %
          (weight_offset, len(weights), meta["weight_region"]))
    print("  arena     : %d bytes, region %d" %
          (arena_size, meta["input_region"]))
    print("  MACs      : %d per inference" % meta["macs"])
    print("  CRC32     : 0x%08X" % crc)
    if hex_pad:
        print("  hex pad   : %d byte(s) of 0x%02X to align the .hex to %d bytes"
              % (hex_pad, ERASED_BYTE, HEX_ALIGN))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
