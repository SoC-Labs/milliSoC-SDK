# FW-PERF-001 — Latency and throughput characterization

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Produce repeatable baseline measurements for shared SRAM, external
DDR, DMA, QSPI reads, and U85 inference when available; this is
characterization, not an unqualified performance requirement.

**Participants:** R5 and/or M85 per tested path. **Priority:** P2.

**Procedure**

1. Identify and calibrate the timer source, frequency, counter width, and
   wraparound behavior. Record core frequency, memory/cache configuration,
   FPGA build, board revision, and ambient/thermal state.
2. Measure timer overhead and an empty-loop control. For each path, warm up,
   run a fixed set of transfer sizes and iteration counts, then report raw
   samples and summary statistics rather than a single best result.
3. Measure shared-SRAM reads/writes from each core, DDR reads/writes when the
   external controller is initialized and a safe range is reserved, DMA
   transfers by legal source/destination pair, QSPI sequential/random reads,
   and U85 inference only when its required runtime and memory route are
   verified.
4. Use separate uncontended and controlled concurrent-load runs. Record cache
   state and maintenance; do not compare unlike configurations.
5. Establish pass/fail performance thresholds only after a stable board
   baseline and product requirement are approved. Until then, this testcase
   passes on valid measurement collection and correctness checks, not on an
   invented latency target.

**Checks and pass criteria**

* Measurements are nonzero where expected, timer wrap is handled, and all
  compared data/results remain correct.
* Reports include units, timer calibration, sample count, min/median/max and
  configuration. Invalid or incomplete measurements fail.
* Performance claims are limited to the recorded FPGA configuration; they do
  not imply ASIC timing or deterministic worst-case latency.
