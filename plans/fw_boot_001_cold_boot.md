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
4. Confirm from the bootloader UART log that both images were preloaded and the
   bootloader released M85. M85 then reports its independent build ID,
   validates its initialized data, and acknowledges startup to the R5
   application using a bounded test handshake.
5. If the selected bitstream has a qualified shared-SRAM route and a reserved
   test-owned region, each core writes a distinct signature, reads back its
   own signature, and confirms the other core's signature. Otherwise, report
   this optional check as not run; do not infer cross-core memory visibility.

**Checks and pass criteria**

* The R5 bootloader log shows preload and M85 release before either
  application-level result is accepted. Do not require the R5 application
  banner to precede the M85 banner: the bootloader releases M85 before
  branching to the R5 application.
* Both core build IDs and payload checks match the programmed image.
* M85 reaches its application without external intervention; both cores finish
  their bounded handshake and report PASS.
* No unexpected reset, exception, bus fault, or timeout is observed.

**Evidence:** Both UART logs, image hash, board/bitstream identity, reset method,
and per-core results.

**Current SDK app scope:** `apps/cold_boot/` checks each core's local image
data and coordinates over MHU. It does not perform the optional shared-SRAM
signature check; use the memory plans after the target memory route and safe
test region are qualified.
