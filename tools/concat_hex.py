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


def concat_hex(args):
    m_data = parse_hex(args.m_hex)
    r_data = parse_hex(args.r_hex)

    m_size_words = (len(m_data) + BYTES_PER_WORD - 1) // BYTES_PER_WORD
    r_size_words = (len(r_data) + BYTES_PER_WORD - 1) // BYTES_PER_WORD

    if len(m_data) % BYTES_PER_WORD:
        print(f"WARNING: {args.m_hex}: {len(m_data)} bytes is not a multiple of "
              f"{BYTES_PER_WORD}, M size rounded up to {m_size_words} words")
    if len(r_data) % BYTES_PER_WORD:
        print(f"WARNING: {args.r_hex}: {len(r_data)} bytes is not a multiple of "
              f"{BYTES_PER_WORD}, R size rounded up to {r_size_words} words")

    m_offset = PREAMBLE_WORDS * BYTES_PER_WORD
    r_offset = m_offset + len(m_data)

    preamble_words = [m_offset, m_size_words, r_offset, r_size_words]

    out_dir = os.path.dirname(args.output)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    with open(args.output, "w") as f:
        for word in preamble_words:
            for byte_str in word_to_hex_bytes(word):
                f.write(byte_str + "\n")
        for data in (m_data, r_data):
            for byte_val in data:
                f.write(f"{byte_val:02X}\n")

    print(f"Generated {args.output}")
    print(f"  M image: offset 0x{m_offset:08X}, "
          f"{len(m_data)} bytes ({m_size_words} words), from {args.m_hex}")
    print(f"  R image: offset 0x{r_offset:08X}, "
          f"{len(r_data)} bytes ({r_size_words} words), from {args.r_hex}")


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

    args = parser.parse_args()
    concat_hex(args)
