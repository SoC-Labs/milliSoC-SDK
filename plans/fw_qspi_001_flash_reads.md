# FW-QSPI-001 — Read-only QSPI/XIP and warm-reset retention

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify flash reads used by the boot path, safe memory-mapped/XIP
reads if configured, and the documented warm-reset retention behavior.

**Participants:** R5; M85 participates only if its map grants a legal QSPI view.
**Priority:** P1.

**Procedure**

1. Read the flash preamble and selected known image ranges through the supported
   controller path. Compare against build-time hashes or checksums prepared by
   the image-generation process.
2. Read selected locations repeatedly and, where XIP is enabled, compare
   controller-mediated and XIP results. Include aligned and boundary reads
   supported by the interface; do not fetch beyond the provisioned image.
3. If caches are present and enabled, verify cold and repeated reads, and use
   the documented invalidate procedure before comparing after a mode change.
4. Record the controller configuration, then perform one supported warm reset
   after quiescing accesses. Verify retained configuration and successful
   post-reset reads without relying on undocumented retention.
5. Flash program/erase is out of the default test. Add it only when the board
   profile identifies a disposable scratch region, its erase geometry, and a
   proven restoration/recovery procedure.

**Checks and pass criteria**

* Read data matches independently generated reference data, including at the
  tested range boundaries.
* XIP and controller results agree for equivalent reads.
* Warm-reset configuration behavior matches the configured design; any
  required reinitialization is performed and validated.
* The test makes no writes outside an explicitly authorized scratch range.

**Evidence:** Flash image hash/ranges, access mode, cache state, reset type,
controller state, and read comparison results.
