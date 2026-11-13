# FW-MHU-001 — Bidirectional MHU and shared-memory handshake

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Verify R5↔M85 doorbell, mailbox, interrupt, and synchronization
behavior without relying on an undocumented interprocessor protocol.

**Participants:** R5 and M85. **Priority:** P1.

**Procedure**

1. Define a test-only message structure with magic, protocol version, sequence,
   length, payload pattern, and integrity check. Place payloads in a reserved,
   legal shared-SRAM region; keep mailbox/doorbell programming to documented
   registers.
2. Initialize both endpoints, clear stale doorbell/mailbox state, and exchange
   a bounded ready handshake.
3. Send small, maximum-documented, and alternating-pattern payloads in each
   direction. Receiver validates sequence, length, contents, and integrity
   before acknowledging.
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
* Shared-memory visibility uses explicit barriers and cache maintenance where
  required.

**Evidence:** Per-direction message counts, sequence/error totals, payload
sizes, IRQ mapping, and reset generation.
