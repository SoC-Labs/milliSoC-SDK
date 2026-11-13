# FW-IRQ-001 — Core interrupt behavior and priority

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Check representative interrupt delivery, acknowledgement, priority,
and wake-up behavior on both the R5 PL192 VIC and M85 NVIC.

**Participants:** R5 and M85. **Priority:** P0.

**Procedure**

1. Install handlers using the selected core's authoritative interrupt map.
   Initialize data shared with handlers before enabling interrupts; clear
   pending state and configure only the sources under test.
2. Trigger a low-rate timer interrupt on each core. Verify one handler entry
   per programmed event, source identity, acknowledgement/clear behavior, and
   return to the interrupted context.
3. Exercise one R5 source routed through VIC0 and at least one source routed
   through VIC1. Verify VIC vector/dispatch and daisy-chain behavior through
   the production driver.
4. Exercise representative M85 NVIC sources, including a peripheral IRQ and
   MHU IRQ. Test pending while disabled followed by enable only where the
   peripheral's documented semantics make this deterministic.
5. Exercise two sources at different configured priorities; verify the higher
   priority handler can preempt only when the architecture and masking policy
   permit it. Test nesting with bounded depth and no shared-state race.
6. Enter WFI only after the source and wake policy are configured; verify the
   intended interrupt wakes the core.

**Checks and pass criteria**

* The observed source, handler, vector, and count match the configured source
  on each core. No unrelated or duplicate interrupt is accepted.
* IRQs are disabled/masked during setup and reliably quiesced at teardown.
* VIC0 and VIC1 are both exercised; an R5 interrupt ≥32 is never handled as a
  VIC0-local source.
* Priority, nesting, and WFI expectations match the selected core and driver
  configuration.

**Limit:** This is representative source behavior; FW-IRQ-002 checks the
firmware-visible R5 VIC1 peripheral sources. Every IRQ-number mapping still
requires map consistency and directed source coverage from the verification
plan.
