# FW-RESET-001 — Local, system, and watchdog reset behavior

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify reset scope, reset cause, boot/release behavior, and
documented retention across supported reset sources.

**Participants:** R5 and M85. **Priority:** P0.

The current SYSCTRL RTL exposes software requests for M85-local, R5-local and
system reset, and records reset-cause bits. The watchdog reset path is also
connected in the SoC RTL. These are source-level capabilities only: run each
case only on a bitstream built from that integration and confirm the observed
scope/boot behavior. The FPGA wrapper exposes an external POR input; it does not
make POR a firmware-triggerable reset source.

**Procedure**

1. Establish a baseline with both cores running a quiescent handshake. Record
   reset-cause bits and a test cookie in each state-retention location that the
   selected reset is documented to preserve.
2. Request an M85-local reset from the approved control path. Verify R5 remains
   active, M85 restarts according to its configured reset/CPUWAIT behavior, and
   the R5 observes the expected cause and handshake generation.
3. In a separate run, request an R5-local reset where implemented. Verify M85
   remains active and the R5 restart sequence is consistent with the boot
   architecture. Do not presume a local reset re-runs ROM or reloads TCM; record
   the actual documented behavior.
4. In a separate quiescent run, request system reset through the documented
   software path. Verify both cores restart and the boot path reloads/releases
   M85 as specified. Check reset cause and QSPI retention.
5. In a supervised isolated run, configure the watchdog using the authorized
   key/lock sequence and allow the intended reset. Capture the pre-reset
   watchdog event and post-reset cause. Ensure external recovery is available.
6. If the board exposes POR or DAP reset as part of this firmware suite, record
   them separately; firmware cannot itself prove the physical POR/DAP source.

Keep both UART captures active through every reset generation. Do not send the
runner's end-of-test marker until the final phase, or the host may stop
capturing after the first boot and misclassify the reset sequence.

**Checks and pass criteria**

* Each reset source has the documented scope: intended processors reset,
  unaffected processors and retained resources behave as specified.
* Reset-cause reporting identifies the initiating source and is cleared or
  retained according to the documented register semantics.
* The R5-first boot and M85 release work after system reset; local-reset
  behavior matches explicit architecture requirements.
* No reset is issued during active DMA/QSPI/U85 traffic in this testcase.
* Each stage has a bounded timeout and a board recovery procedure.

**Notes:** Local-reset transactions are performed only while the reset target
is quiescent until outstanding-transaction drain behavior is verified.
