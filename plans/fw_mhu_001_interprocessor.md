# FW-MHU-001 — Bidirectional MHU with optional shared memory

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify R5↔M85 doorbell, mailbox, interrupt, and synchronization
behavior without relying on an undocumented interprocessor protocol. The
primary MHU test must not depend on shared-SRAM capacity or alias assumptions.

**Participants:** R5 and M85. **Priority:** P1.

**Procedure**

1. Define a compact test-only message with magic, protocol version, run
   generation, sequence, payload pattern, and integrity check. Carry the
   primary test payload through documented MHU channels. Test a shared-SRAM
   payload only as a separate extension after both masters' routes, the safe
   address range, and cache policy are qualified.
2. Initialize endpoint state before advertising readiness. Clear stale
   doorbell/mailbox state only while the peer is known quiescent; do not clear a
   pending message from a core that started earlier. Include a run generation
   so stale state can be rejected. Then exchange a bounded ready handshake.
3. Send small, maximum-documented, and alternating-pattern payloads through
   MHU in each direction. The receiver validates sequence, length, contents,
   and integrity before acknowledging.
4. Exercise each documented channel or doorbell direction, one at a time, and
   then run a bounded sequence of back-to-back messages with distinct
   sequence numbers. Check IRQ source and clear semantics on both cores.
5. Test error/fault reporting only using a documented firmware-safe mechanism.
   Do not corrupt a live channel or invent invalid register writes to provoke
   hardware errors.
6. Reset endpoints only after all messages are acknowledged; test stale-state
   handling after reboot if reset behavior is specified.

**Checks and pass criteria**

* No lost, duplicated, reordered, or stale message is accepted.
* Each interrupt corresponds to a pending message and is cleared after service.
* Both directions complete before deadlines; any error/fault status is
  reported and handled as documented.
* MHU transport passes independently of shared-SRAM testing. Shared-memory
  visibility is claimed only in the optional qualified phase, with explicit
  barriers and cache maintenance where required; otherwise report it as not
  run.

**Evidence:** Per-direction message counts, sequence/error totals, payload
sizes, IRQ mapping, and reset generation.
