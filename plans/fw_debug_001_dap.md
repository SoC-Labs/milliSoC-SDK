# FW-DEBUG-001 — Firmware debug targets and external DAP checks

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Provide safe firmware state for an external debugger to validate
DAP access and the routed M85 trace output, without claiming firmware alone can
test SWD/JTAG or trace pins.

**Participants:** R5 firmware plus authorized external debug host. **Priority:**
P1.

**Procedure**

1. Firmware initializes deterministic, non-sensitive scratch locations in
   shared SRAM and (where safe) each core's documented debug-visible memory.
   It records expected values and enters a bounded wait state without
   continually modifying those locations.
2. From the external host, connect using the documented SWD/JTAG procedure.
   Read identification/component IDs and the test signatures through each
   supported AP. Verify exact data and address behavior.
3. Test only debug memory/register accesses authorized for this board. Exercise
   writes solely to dedicated scratch locations; verify readback through
   firmware after the debugger resumes execution.
4. If core halt/resume or reset is part of the approved policy and is
   implemented, test each operation in isolation with a recovery path. Do not
   infer core-control support from successful ROM-table reads.
5. Record whether R5, M85, system-bus access, and APB access are actually
   exposed in the FPGA build. Report unsupported paths as blocked.
6. If a board-supported trace capture instrument is available, enable a short,
   identifiable M85 trace window and capture the routed trace clock, control,
   and four data bits. Confirm the capture is decodable and corresponds to the
   expected test window. Do not claim R5 ETM or the full internal trace funnel
   is externally verified unless its route is separately demonstrated.

**Checks and pass criteria**

* External host access returns the expected IDs and signatures on each tested
  AP/path.
* Firmware confirms debugger writes only in scratch and resumes with state
  intact, if write access is part of the authorized test.
* Halt/resume/reset pass only if explicitly implemented and authorized.
* Trace passes only with a captured, decodable waveform from the board's
  documented capture path; enabling trace in firmware is not sufficient.
* Security/authentication limitations are documented; no production security
  claim is made from a development-board test.

**Evidence:** Host/tool version, transport, AP and address tested, access
results, authorization configuration, and firmware post-check.
