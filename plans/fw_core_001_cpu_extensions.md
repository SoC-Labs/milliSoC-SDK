# FW-CORE-001 — Configured CPU instruction-extension smoke

> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for setup, execution, result handling, and evidence.

**Purpose:** Verify each optional CPU execution extension that is enabled in
the selected RTL and supported by the configured compiler, without treating a
successful firmware build as proof that the hardware implements the extension.

**Participants:** M85 and R5 when an extension is enabled on that core.
**Priority:** P1.

**Current configuration note:** The M85 RTL enables the integer MVE subset,
but has conflicting FPU settings (`FPU(0)` and `CFGFPU=1`). The R5 integration
parameter `FPU0` defaults to 0. Resolve the elaborated core configurations
before deciding whether FPU testing is applicable. The existing `fpu_tests`
application exercises scalar floating-point C operations; it does not test MVE
and does not resolve the M85 configuration conflict.

**Procedure**

1. Record the bitstream's M85 feature configuration and confirm the compiler
   target/flags match it. Do not execute an optional instruction before its
   implementation is confirmed.
2. For each enabled extension on each core, run deterministic operations with
   known inputs and expected results, including arithmetic,
   comparison/selection, boundary lane values, and repeated accumulation where
   applicable.
3. For MVE, inspect the linked disassembly to confirm the tested code actually
   contains MVE instructions; avoid accepting scalar fallback as a pass.
4. For FPU, run the scalar floating-point checks only if the selected core
   configuration is confirmed to include the FPU. If disabled, record
   NOT APPLICABLE; if the RTL settings remain inconsistent, mark FPU coverage
   BLOCKED.
5. Report UsageFault/HardFault or timeout as FAIL, with the PC and fault status
   captured if the configured handler can recover safely.

**Checks and pass criteria**

* Every enabled and claimed extension executes the intended instructions and
  produces the expected results.
* Disabled features are not claimed as tested. An unresolved core
  configuration, unsupported compiler target, or missing test disassembly is
  BLOCKED, not PASS.

**Evidence:** Bitstream/configuration ID, compiler version and flags, relevant
disassembly, test vectors, per-operation results, and fault status.
