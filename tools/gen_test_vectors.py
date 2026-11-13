#!/usr/bin/env python3
"""Generate the C test-vector header used by the u85_model testcase.

The Ethos-U runtime in apps/u85_model checks every inference against a
reference result, so for each model it can run it needs:

  * a set of int8 inputs to write into the input tensor,
  * the int8 outputs the same inputs produce on a reference implementation.

This script produces both from a .tflite model.  The reference comes from the
LiteRT (TFLite) interpreter with the integer kernels - the same quantised
arithmetic the Ethos-U is expected to match to within a few LSB.

The generated header is committed next to the model source in models/vectors/
so a normal SDK build does not need the interpreter.  Re-run it only when the
model or the vector set changes:

    python tools/gen_test_vectors.py \
        --model models/micro_speech_int8.tflite \
        --output models/vectors/micro_speech_int8.h

Requires `ai-edge-litert` (pip install ai-edge-litert) and numpy.
"""

import argparse
import hashlib
import os
import sys

import numpy as np


VECTOR_NAMES = [
    "silence",
    "constant -64",
    "constant +63",
    "frequency ramp",
    "tone burst",
    "pseudo-random noise",
    "impulse",
    "alternating",
]


def build_inputs(shape, count, seed):
    """Deterministic int8 inputs covering the input range.

    The values are *already quantised*: they go straight into the model's int8
    input tensor, so the patterns are expressed in quantised space and do not
    depend on the model's input scale/zero point.
    """
    rng = np.random.default_rng(seed)
    n = int(np.prod(shape))
    idx = np.arange(n)
    patterns = [
        np.full(n, -128, dtype=np.int16),                     # 0.0 -> -128
        np.full(n, -64, dtype=np.int16),
        np.full(n, 63, dtype=np.int16),
        np.floor(idx * 192.0 / max(n - 1, 1)) - 128,          # ramp
        127.0 * np.sin(idx * 2.0 * np.pi / 16.0),             # tone
        rng.integers(-128, 128, size=n, dtype=np.int16),      # noise
    ]
    impulse = np.full(n, -128, dtype=np.int16)                # single spike
    impulse[n // 2] = 127
    patterns.append(impulse)
    patterns.append(np.where(idx % 2 == 0, 96, -96))          # alternating

    patterns = patterns[:count]
    return np.array(patterns, dtype=np.int8).reshape((len(patterns),) + tuple(shape))


def load_model(model_path):
    from ai_edge_litert.interpreter import Interpreter

    interp = Interpreter(model_path=model_path)
    interp.allocate_tensors()
    in_detail = interp.get_input_details()[0]
    out_detail = interp.get_output_details()[0]

    if in_detail["dtype"] != np.int8:
        raise SystemExit("model input is %s, expected int8" % in_detail["dtype"])
    if out_detail["dtype"] != np.int8:
        raise SystemExit("model output is %s, expected int8" % out_detail["dtype"])

    in_shape = tuple(int(d) for d in in_detail["shape"])
    out_shape = tuple(int(d) for d in out_detail["shape"])
    return interp, in_detail, out_detail, in_shape, out_shape


def run_reference(interp, in_detail, out_detail, in_shape, inputs):
    outputs = []
    for vector in inputs:
        interp.set_tensor(in_detail["index"],
                          vector.reshape(in_shape).astype(np.int8))
        interp.invoke()
        outputs.append(interp.get_tensor(out_detail["index"]).reshape(-1).copy())
    return np.array(outputs, dtype=np.int8)


def format_c_array(name, values, rows, cols):
    lines = ["static const int8_t %s[%d][%d] = {" % (name, rows, cols)]
    for row in values:
        lines.append("    { %s }," % ", ".join("%4d" % int(v) for v in row))
    lines.append("};")
    return "\n".join(lines)


def main(argv):
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--model", required=True, help="source .tflite model")
    parser.add_argument("--output", required=True, help="header to write")
    parser.add_argument("--count", type=int, default=6,
                        help="number of test vectors (default: 6)")
    parser.add_argument("--seed", type=int, default=1,
                        help="seed for the pseudo-random vector (default: 1)")
    parser.add_argument("--tolerance", type=int, default=2,
                        help="accepted |NPU - reference| in LSB (default: 2)")
    args = parser.parse_args(argv)

    if not 1 <= args.count <= len(VECTOR_NAMES):
        raise SystemExit("--count must be between 1 and %d" % len(VECTOR_NAMES))

    # The input shape is only known once the interpreter has loaded the model.
    interp, in_detail, out_detail, shape, out_shape = load_model(args.model)
    inputs = build_inputs(shape, args.count, args.seed)
    expected = run_reference(interp, in_detail, out_detail, shape, inputs)

    in_size = int(np.prod(shape))
    out_size = int(np.prod(out_shape))
    inputs = inputs.reshape(len(inputs), in_size)
    expected = expected.reshape(len(expected), out_size)

    with open(args.model, "rb") as handle:
        digest = hashlib.sha256(handle.read()).hexdigest()

    name = os.path.splitext(os.path.basename(args.model))[0]
    text = [
        "/*",
        " * Test vectors for %s." % name,
        " *",
        " * Generated by tools/gen_test_vectors.py - do not edit by hand.",
        " *",
        " * Model   : %s (%d bytes)" % (os.path.basename(args.model),
                                       os.path.getsize(args.model)),
        " * SHA-256 : %s" % digest,
        " * Input   : %s int8 (%d bytes)" % (list(shape), in_size),
        " * Output  : %s int8 (%d bytes)" % (list(out_shape), out_size),
        " *",
        " * The expected outputs come from the LiteRT integer kernels, so the",
        " * NPU result is allowed MODEL_TEST_TOLERANCE LSB of difference.",
        " */",
        "",
        "#ifndef MODEL_VECTORS_H",
        "#define MODEL_VECTORS_H",
        "",
        '#define MODEL_TEST_NAME          "%s"' % name,
        "#define MODEL_TEST_INPUT_BYTES   %d" % in_size,
        "#define MODEL_TEST_OUTPUT_BYTES  %d" % out_size,
        "#define MODEL_TEST_TOLERANCE     %d" % args.tolerance,
        "#define MODEL_TEST_COUNT         %d" % len(inputs),
        "",
        format_c_array("model_test_inputs", inputs, len(inputs), in_size),
        "",
        format_c_array("model_test_expected", expected, len(expected), out_size),
        "",
        "#endif /* MODEL_VECTORS_H */",
        "",
    ]

    os.makedirs(os.path.dirname(os.path.abspath(args.output)), exist_ok=True)
    with open(args.output, "w", encoding="utf-8", newline="\n") as handle:
        handle.write("\n".join(text))

    print("vectors   : %s (%d x %d int8 in, %d int8 out)"
          % (args.output, len(inputs), in_size, out_size))
    print("tolerance : %d LSB" % args.tolerance)
    for i, (_, exp) in enumerate(zip(inputs, expected)):
        print("  [%d] %-20s -> %s"
              % (i, VECTOR_NAMES[i], " ".join("%4d" % int(v) for v in exp)))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
