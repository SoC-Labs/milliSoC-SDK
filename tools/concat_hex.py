#!/usr/bin/env python3
#------------------------------------------------------------------------------------
# Concatenated M-class / R-class Hex Image Generation Script
# A joint work commissioned on behalf of SoC Labs, under Arm Academic Access license.
#
# Builds a single flash image from the two hex files produced by the all_ds6
# build (fromelf --vhx --8x1 : one hex byte per line, no address records).
#
# Image layout (offsets are relative to the start of the file, i.e. flash
# address 0, so the preamble itself occupies offsets 0x00..0x0F):
#
#   Offset  Size      Field
#   0x00    4 bytes   M-class image start offset (bytes)
#   0x04    4 bytes   M-class image size (in 32-bit words)
#   0x08    4 bytes   R-class image start offset (bytes)
#   0x0C    4 bytes   R-class image size (in 32-bit words)
#   0x10    ...       M-class image bytes
#   ...     ...       R-class image bytes
#
# Preamble words are written little-endian, one hex byte per line, so the whole
# file stays loadable by $readmemh into a byte-wide memory and any little-endian
# reader assembling 32-bit words from flash address 0 sees the correct values.
#------------------------------------------------------------------------------------

import argparse
import os
import sys

PREAMBLE_WORDS = 4          # m_offset, m_size, r_offset, r_size
BYTES_PER_WORD  = 4
# The flash programmer writes the QSPI FIFO four 32-bit words at a time, so the
# image length is aligned to 16 bytes.  A tail that is not a whole number of
# FIFO words makes the last page-program transaction partial, which the
# controller does not complete.
ALIGN_BYTES     = 4 * BYTES_PER_WORD
ERASED_BYTE     = 0xFF


def pad_to(data, align, fill=ERASED_BYTE):
    """Right-pad a byte string with `fill` up to a multiple of `align` bytes."""
    remainder = len(data) % align
    if remainder == 0:
        return data, 0
    padding = align - remainder
    return data + bytes([fill]) * padding, padding


def parse_hex(path):
    """Read a hex file into a contiguous byte payload.

    Supports the fromelf --vhx byte-per-line format produced by the all_ds6
    build, and Verilog hex files using '@' address records (byte addresses),
    with multiple data bytes per line.  The payload is rebased so that byte
    offset 0 of the returned data corresponds to the lowest address found in
    the file (i.e. offsets are relative to the image start).
    """
    mem = {}
    cursor = 0
    with open(path, "r") as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            if line.startswith('@'):
                try:
                    cursor = int(line[1:], 16)
                except ValueError:
                    sys.exit(f"ERROR: {path}:{lineno}: bad address record '{line}'")
                continue
            for token in line.split():
                try:
                    value = int(token, 16)
                except ValueError:
                    sys.exit(f"ERROR: {path}:{lineno}: bad hex byte '{token}'")
                if not 0 <= value <= 0xFF:
                    sys.exit(f"ERROR: {path}:{lineno}: value '{token}' does not fit in a byte")
                mem[cursor] = value
                cursor += 1

    if not mem:
        sys.exit(f"ERROR: {path}: no data found")

    start, end = min(mem), max(mem)
    payload = bytearray()
    for addr in range(start, end + 1):
        if addr not in mem:
            print(f"WARNING: {path}: gap at offset 0x{addr - start:X}, filling with 0x00")
        payload.append(mem.get(addr, 0x00))
    return bytes(payload)


def word_to_hex_bytes(value):
    """Encode a 32-bit word as 4 little-endian hex byte strings."""
    return [f"{(value >> (8 * i)) & 0xFF:02X}" for i in range(BYTES_PER_WORD)]


def parse_blob(spec):
    """Parse an OFFSET:FILE blob specification."""
    if ":" not in spec:
        sys.exit(f"ERROR: --blob expects OFFSET:FILE, got '{spec}'")
    offset_str, path = spec.split(":", 1)
    try:
        offset = int(offset_str, 0)
    except ValueError:
        sys.exit(f"ERROR: --blob: bad offset '{offset_str}'")
    if offset < 0:
        sys.exit(f"ERROR: --blob: offset 0x{offset:X} is negative")
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError as exc:
        sys.exit(f"ERROR: --blob: cannot read {path}: {exc}")
    if not data:
        sys.exit(f"ERROR: --blob: {path} is empty")
    return offset, data, path


def concat_hex(args):
    m_data = parse_hex(args.m_hex)
    r_data = parse_hex(args.r_hex)

    # Each core image must occupy a whole number of preamble words, otherwise
    # the next offset (and the size the boot code copies) is not word aligned.
    m_data, m_pad = pad_to(m_data, BYTES_PER_WORD)
    r_data, r_pad = pad_to(r_data, BYTES_PER_WORD)
    if m_pad or r_pad:
        print(f"NOTE: padded M by {m_pad} byte(s), R by {r_pad} byte(s) to whole words")

    m_size_words = len(m_data) // BYTES_PER_WORD
    r_size_words = len(r_data) // BYTES_PER_WORD

    m_offset = PREAMBLE_WORDS * BYTES_PER_WORD
    r_offset = m_offset + len(m_data)

    preamble_words = [m_offset, m_size_words, r_offset, r_size_words]

    # Build the code image (preamble + M + R), then append any extra blobs
    # (for example the Vela model image) at fixed flash offsets.
    image = bytearray()
    for word in preamble_words:
        for byte_str in word_to_hex_bytes(word):
            image.append(int(byte_str, 16))
    image.extend(m_data)
    image.extend(r_data)
    code_size = len(image)

    blobs = sorted((parse_blob(spec) for spec in args.blob),
                   key=lambda item: item[0])
    cursor = code_size
    for offset, data, path in blobs:
        if offset < code_size:
            sys.exit(f"ERROR: blob {path} at 0x{offset:08X} overlaps the code "
                     f"image (ends at 0x{code_size:08X})")
        if offset < cursor:
            sys.exit(f"ERROR: blob {path} at 0x{offset:08X} overlaps a previous "
                     f"blob (ends at 0x{cursor:08X})")
        # Erased flash is 0xFF; fill the gap so the image stays contiguous.
        image.extend(b"\xFF" * (offset - cursor))
        image.extend(data)
        cursor = offset + len(data)
    unpadded_size = len(image)

    # The flash programmer pushes the QSPI FIFO four 32-bit words at a time, so
    # finish on a 16-byte boundary; the tail is erased-flash padding.
    tail_pad = (-len(image)) % ALIGN_BYTES
    if tail_pad:
        image.extend(bytes([ERASED_BYTE]) * tail_pad)

    out_dir = os.path.dirname(args.output)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    with open(args.output, "w", newline="\n") as f:
        for byte_val in image:
            f.write(f"{byte_val:02X}\n")

    print(f"Generated {args.output}")
    print(f"  M image: offset 0x{m_offset:08X}, "
          f"{len(m_data)} bytes ({m_size_words} words), from {args.m_hex}")
    print(f"  R image: offset 0x{r_offset:08X}, "
          f"{len(r_data)} bytes ({r_size_words} words), from {args.r_hex}")
    for offset, data, path in blobs:
        print(f"  blob   : offset 0x{offset:08X}, "
              f"{len(data)} bytes, from {path}")
    if tail_pad:
        print(f"  padding: {tail_pad} byte(s) of 0x{ERASED_BYTE:02X} at "
              f"0x{unpadded_size:08X}..0x{len(image) - 1:08X} "
              f"(align image to {ALIGN_BYTES} bytes)")
    print(f"  total  : {len(image)} bytes ({len(image) // ALIGN_BYTES} x "
          f"{ALIGN_BYTES}-byte FIFO bursts)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Concatenate the M-class and R-class hex images from the '
                    'all_ds6 build into a single flash image with a 4-word '
                    'preamble (offsets and sizes in 32-bit words, relative to '
                    'the start of the image)')
    parser.add_argument("-m", "--m_hex", type=str, required=True,
                        help="Input M-class hex file (fromelf --vhx byte-per-line)")
    parser.add_argument("-r", "--r_hex", type=str, required=True,
                        help="Input R-class hex file (fromelf --vhx byte-per-line)")
    parser.add_argument("-o", "--output", type=str, required=True,
                        help="Output concatenated hex file")
    parser.add_argument("--blob", action="append", default=[], metavar="OFFSET:FILE",
                        help="Embed the raw bytes of FILE at flash byte offset OFFSET "
                             "after the code images (may be repeated, e.g. "
                             "--blob 0x008000:model.bin). Gaps are filled with 0xFF")

    args = parser.parse_args()
    concat_hex(args)
