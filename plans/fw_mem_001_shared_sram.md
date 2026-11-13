# FW-MEM-001 — Shared SRAM capacity, boundaries, and alias

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Exercise the implemented shared SRAM from both processors and
measure the physical boundary independently of the decoded address window.

**Participants:** R5 and M85. **Priority:** P0.

**Address constraints:** Use the implemented base `0x3000_0000` and the
capacity recorded in the board profile for the exact bitstream. Do not use
`0x3001_FFFF` as a presumed end address until confirmed: the current wrapper's
address slicing implies 128 KiB, while the programmed bitstream and SIE300
address convention have not been verified (see §2.1 of the common procedure).
Until confirmed, restrict testing to a known-safe word and mark
capacity/boundary coverage BLOCKED. Probe an alias only as a deliberate,
reversible characterization after a confirmed physical boundary.

**Procedure**

1. Reserve a mutually exclusive test window and save its contents if the
   application could be running on a live system. Prefer a dedicated test
   image with no production data in the region.
2. Each core runs address-dependent patterns, walking bits, inverse patterns,
   and pseudorandom **word** patterns over the complete confirmed capacity in
   bounded chunks. Include first/last aligned words. Subword and unaligned
   accesses are separate checks and may be run only if the selected bitstream
   is known to support them; an earlier FPGA build had a shared-SRAM byte-store
   limitation.
3. Perform address-distinct writes followed by readback and an address-alias
   check. Do not infer independence from a single repeated-value pattern.
4. In a separate quiescent phase, R5 writes a sequence-numbered block and M85
   verifies it; reverse writer and reader. Apply required cache maintenance
   and memory barriers. Do not claim coherency from a cache-enabled test unless
   that property is explicitly implemented.
5. If an alias exists in the selected bitstream, save the lower-window word,
   write a test value through the first confirmed alias address, verify the
   corresponding lower address, and restore the saved word. Avoid alias
   probing on a live image or when the mapping is not confirmed.

**Checks and pass criteria**

* All in-range patterns pass on each core and both cores observe the explicit
  cross-core handoff after cache maintenance.
* First and last implemented words are accessible; no access crosses outside
  the confirmed physical range during ordinary testing.
* Capacity and alias behavior match the measured bitstream. Decoded address
  space is never counted as physical storage without a depth check.
* Any mismatch reports core, address, expected/actual value, pattern, and
  iteration.

**Evidence:** Address range/size, pattern seed, cache mode and maintenance
sequence, alias result, error details, and elapsed time.
