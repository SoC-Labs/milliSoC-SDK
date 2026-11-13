# FW-BOOT-001 — Cold boot and core release

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify the real board's ROM-to-QSPI-to-TCM path and R5-first startup
from a cold power-on.

**Participants:** R5 and M85. **Priority:** P0.

**Setup and procedure**

1. Build a single image containing distinct build IDs and nontrivial
   initialization data in the R5 and M85 payloads. Include checksums or
   signatures in test-owned data where the boot format supports them; do not
   change the production image format solely for this test.
2. Program the image through the approved board process, power-cycle, and
   capture both UARTs from before reset release.
3. R5 reports its startup identity, verifies its expected local execution
   location and writable test data, and records the bootloader's documented
   image/preload outcome if exposed.
4. R5 releases M85 only after the normal bootloader preload sequence. M85
   reports its independent build ID, validates its initialized data, and
   acknowledges startup to R5 using a bounded test handshake.
5. Each core writes a distinct signature to a reserved test-owned shared-SRAM
   word, reads back its own signature, and confirms the other core's signature
   only if that shared-memory route is verified for both masters.

**Checks and pass criteria**

* The R5 executes and reports first; M85 does not report application startup
  before release.
* Both core build IDs and payload checks match the programmed image.
* M85 reaches its application without external intervention; both cores finish
  their bounded handshake and report PASS.
* No unexpected reset, exception, bus fault, or timeout is observed.

**Evidence:** Both UART logs, image hash, board/bitstream identity, reset method,
and per-core results.
