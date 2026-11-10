# fpu_tests — Cortex-M85 single-precision floating-point demo

This app demonstrates ordinary C `float` operations on the M85 FPU. It checks
that CP10/CP11 coprocessor access is enabled, then exercises addition,
subtraction, multiplication, division, accumulation, comparison, integer
conversions and a mixed expression. Each result is checked and reported over
UART.

The M85 startup code enables FPU access before `main()`. With the project's
Arm DS-6 toolchain, `-mcpu=cortex-m85` compiles these C expressions to scalar
VFP instructions; no hand-written assembly is needed. This is a functional
single-precision FPU example, not a NEON/SVE2 test or performance benchmark.

Build and run on the FPGA:

```sh
make TESTNAME=fpu_tests
make run_hw TESTNAME=fpu_tests
```

The app prints `FPU tests: 10/10 PASS` when all checks pass. The R5 companion
prints its UART banner and idles while the M85 runs the test.
