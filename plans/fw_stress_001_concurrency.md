# FW-STRESS-001 — Bounded concurrent-load and recovery

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Exercise normal concurrency and forward progress across processors,
shared SRAM, external DDR, DMA, peripherals, and U85 when available.

**Participants:** R5 and M85; DMA/U85 only after their individual tests pass.
**Priority:** P2.

**Prerequisites:** All participating component tests pass. Use a known-safe
board configuration with temperature/power monitoring and a reliable reset
path. This is not a substitute for electrical, timing, or protocol stress
sign-off.

**Procedure**

1. Assign non-overlapping shared-SRAM buffers and bounded workloads to each
   core. Include sequence-numbered MHU messages and timer/UART activity.
2. Run legal DMA copies concurrently on disjoint buffers; add U85 inference
   only when FW-U85-001 has passed and its memory route is verified. Include
   external DDR traffic only after FW-MEM-002 confirms DDR initialization,
   access range, and read/write correctness.
3. Maintain per-agent progress counters and deadlines. Verify final buffer
   checksums and message sequence counts. Use deterministic seeds/configuration
   for reproducing failures; optionally sweep a published seed set.
4. Repeat with cache-maintenance sequences enabled. Do not intentionally
   provoke reset during active transactions here. Reset-during-traffic remains
   a simulation/RTL negative test until safe behavior is established.
5. Stop all sources cleanly, verify quiescence, then optionally perform a
   supervised watchdog/system-reset recovery as a separate phase.

**Checks and pass criteria**

* Every agent advances before its deadline; no data corruption, lost messages,
  unexpected interrupt, timeout, or unexplained reset occurs.
* Concurrency does not alter the correctness criteria of the individual
  transfers/inferences.
* Any failure includes seed, agent progress, active configuration, and latest
  sequence number.

**Evidence:** Duration, seed, per-agent throughput/progress, buffer checksums,
IRQ/reset counts, and board telemetry.
