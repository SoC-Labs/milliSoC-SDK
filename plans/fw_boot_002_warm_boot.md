# FW-BOOT-002 — Repeated boot after supported reset

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Confirm that a warm system reset re-enters the intended boot flow
without requiring a manual flash reprogram.

**Participants:** R5 and M85. **Priority:** P1.

**Setup and procedure**

1. Run after FW-BOOT-001 passes. Use only a system reset source documented for
   this board and configuration; do not substitute a local-core reset.
2. On the first boot, record the reset cause and request a system reset while
   both cores are quiescent and all test-owned DMA/peripheral operations are
   complete. Use a retention cookie only if its storage is documented to
   survive this exact reset; do not assume SRAM/TCM contents are retained.
3. On reboot, validate the reset cause and the host-observed boot generation,
   then repeat the two-core startup handshake.
4. Repeat for a small fixed count selected by the board profile. Do not rely on
   unbounded reset loops.

The host runner must keep both UART captures open across reset generations and
must not interpret the first generation's normal end marker as completion of
the whole testcase. Emit the final terminal result only after the last generation
or report a timeout/failure.

**Checks and pass criteria**

* Every requested reset produces one fresh boot generation from the same
  programmed image.
* Reset cause agrees with the requested source; QSPI remains usable under the
  configured warm-reset retention behavior.
* Each generation has one result from every participating core, with no stale
  handshake accepted as a current result.
* Failure to boot within the per-generation deadline is a failure requiring
  operator recovery, not a pass.

**Notes:** Exhaustive source/scope coverage is FW-RESET-001. Cold POR belongs to
FW-BOOT-001. A watchdog-triggered reboot belongs to FW-RESET-001.
