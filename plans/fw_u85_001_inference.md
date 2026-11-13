# FW-U85-001 — U85 configuration, memory, IRQ, and inference

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify end-to-end SoC-visible operation of the configured NPU
demonstrator, including a reference-checked inference—not merely register
writes.

**Participants:** M85 as host; R5 may supervise. **Priority:** P1.

**Blocking prerequisites:** The FPGA image includes a configured, clocked, and
reset-connected U85; an M85-compatible driver/runtime and build path are
available; the allowed U85 memory regions and DMA visibility are verified; the
model and reference inputs/outputs are versioned. The U85 integration plan
identifies outstanding runtime, memory-route, and integration work. Until these
prerequisites are demonstrated on the board, this testcase is **blocked**, not
passed.

**Procedure**

1. Read U85 identification/configuration registers and verify reset/idle state
   using the official configured integration definitions.
2. Configure only documented control, QBASE/QSIZE, and memory settings. Verify
   legal address width/range and reject any model/tensor allocation outside
   the accessible region.
3. Run a small deterministic inference with fixed input vectors. Check command
   acceptance, bounded completion, completion/error IRQ to M85, and output
   against a versioned reference using exact or documented tolerance criteria.
4. Run a second inference with changed input and confirm the result changes as
   expected; validate output buffer ownership and cache maintenance.
5. If shared SRAM access by U85 is part of the intended design, run a separate
   legal shared-memory case and verify the data path. Do not assume the U85
   master is connected to that memory.
6. Check a documented software-visible error result if one is safely
   reproducible; otherwise leave error injection to the RTL testbench.

**Checks and pass criteria**

* Identification/configuration and memory range checks pass.
* Inference completes before timeout, the correct IRQ/status is observed, and
  output matches the reference criteria.
* No undocumented memory route, coherency behavior, or presence-based skip is
  used to claim a pass. U85 remains in the baseline as required by the
  verification-plan decision.

**Evidence:** RTL/IP configuration, runtime/driver/model versions, input and
reference hashes, memory route, IRQ/status, output comparison, latency.
