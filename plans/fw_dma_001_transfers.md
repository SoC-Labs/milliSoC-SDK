# FW-DMA-001 — DMA transfer, completion, error, and abort

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify DMA350 configuration and SoC-visible data movement,
completion IRQ, error reporting, and safe abort handling.

**Participants:** R5 configures DMA; M85 may validate shared buffers. **Priority:**
P1.

**Procedure**

1. Build the transfer matrix from the configured DMA master map. Begin with
   aligned, bounded transfers between a known-readable QSPI image region and
   legal TCM/shared-SRAM destinations; include shared-to-shared, TCM and
   external-DDR paths only when the actual address views, implemented memory
   depth, and DDR initialization are confirmed.
2. Use several legal lengths around documented alignment/burst boundaries.
   Fill sources with address- and sequence-dependent data, initialize
   destinations to a sentinel, and verify every destination byte/word after
   completion.
3. Enable only the intended DMA completion/error interrupt. Check channel
   status, transferred length, destination contents, and interrupt clear.
4. Where the hardware documents abort/disable semantics, start a sufficiently
   long transfer, request abort at a controlled point, and verify terminal
   channel status, no writes beyond the documented completed portion, and
   correct error/abort interrupt behavior.
5. For invalid addresses or bus errors, use a documented safe target and
   recovery path only. If a board fault could hang the fabric, perform that
   negative case in simulation instead.
6. Repeat a small transfer while a CPU operates on a separate buffer. Do not
   overlap buffers until arbitration and coherency requirements are understood.

**Checks and pass criteria**

* Every legal transfer has byte-exact destination data; completion is not
  accepted without independent readback.
* IRQ and status correspond to the correct channel and final outcome.
* Abort/error cases terminate within a deadline and do not masquerade as
  successful completion.
* No out-of-range destination corruption or unexplained CPU stall occurs.

**Evidence:** Channel/configuration, source/destination views, length/alignment,
status/IRQ, data-check result, and abort/error details.
