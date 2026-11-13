# FW-MEM-002 — CPU and DMA memory-access matrix

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Validate firmware-visible read/write and DMA access paths across
the memory regions supported by the configured system.

**Participants:** R5 and M85; R5 configures the DMA350. **Priority:** P0.

**Setup:** Before coding, produce a per-master table from the generated
interconnect and processor maps covering R5, M85, DMA350, and—where relevant—
U85/debug. For each region record read/write permission, local versus bus
address, cacheability, error behavior, and whether writes are safe. Do not
derive a DMA or peer-core address by copying another core's address. Reconcile
the R5 SDK memory-map header and linker/scatter files with the selected RTL
before defining any boundary: the current R5 header does not match the TCM
sizes in the current RTL source.

The source-level regions to account for are M85 ITCM (`0x0000_0000`, 64 KiB)
and DTCM (`0x2000_0000`, 64 KiB); R5 ATCM (`0x0000_0000`, 64 KiB) and BTCM
(`0x2000_0000`, 32 KiB); the R5 DMA/AXI ATCM view at `0x8000_0000`; shared
SRAM at `0x3000_0000` (capacity unresolved; see the common procedure); and,
when present in the programmed bitstream, the external DDR path at
`0x9000_0000`–`0xAFFF_FFFF`. Include QSPI/ROM as read-only sources where
permitted. These are source expectations, not substitutes for the board
profile's verified access matrix.

**Procedure**

1. On each CPU, read safe identification/status registers and perform
   non-destructive write/readback only in documented scratch memory/registers.
2. Verify legal CPU paths using reserved, non-overlapping scratch locations in
   each implemented private TCM (M85 ITCM/DTCM and R5 ATCM/BTCM), the verified
   shared-SRAM range, and DDR only when the controller is initialized and a
   safe range is reserved. Never overwrite code, vectors, stacks, heap, or
   another core's live data. Verify the R5 ATCM local and DMA/AXI views
   separately; do not assume two aliases are equivalent without readback.
3. Configure DMA transfers only for matrix entries marked legal. Include
   R5 ATCM through its verified DMA-visible view, shared SRAM, QSPI read
   sources, and DDR only when wired and initialized. Use unique
   source/destination patterns and verify data at the destination with the
   CPU that owns that address view.
4. Use required cache clean/invalidate and barriers; run a control case with
   cache disabled only if supported and safe.
5. Do not make illegal/unmapped accesses part of the FPGA pass criteria unless
   the exact target core and interconnect have a demonstrated recoverable fault
   path. Keep unmapped, forbidden, and bus-error injection cases in RTL
   simulation by default; a firmware exception handler alone does not ensure
   the interconnect will return an error instead of stalling.

**Checks and pass criteria**

* Every required legal positive case performs the expected access and
  independent readback. The report lists any regions not available in the
  selected board build, including DDR if not initialized.
* Illegal entries are not accessed from firmware unless a recoverable response
  has been independently demonstrated and approved for that board.
* DMA completion, destination contents, and CPU visibility agree; no transfer
  is considered successful based on a completion flag alone.

**Evidence:** Versioned access matrix, address/view used, cache policy,
transaction dimensions, DMA status, and fault details.
