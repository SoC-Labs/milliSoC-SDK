# FW-IRQ-002 — R5 VIC1 peripheral interrupt delivery

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Specifically prove that the R5 driver can enable, dispatch, and
clear IRQs routed to VIC1 at `VIC_BASE + 0x1000`.

**Participants:** R5. **Priority:** P0.

**Sources:** Timer0/1, dual timer, connected UART0, watchdog, and RTC where
implemented and safe. Use authoritative `irq_map.h`; do not copy the M85 IRQ
numbering. Verify source-to-IRQ routing against the selected RTL before the
run.

On the current FPGA wrapper, UART1, GPIO0/1 and SPI pins are unconnected.
Do not require their pin-driven IRQ cases for a pass on that wrapper. Mark them
BLOCKED unless the board build provides a documented internal stimulus or a
new physical route. Timer and dual-timer interrupts are suitable deterministic
VIC1 sources; UART0 RX additionally requires working host-to-board RX.

**Procedure**

1. For each available safe source, configure one event at a time with all other
   sources masked. Read back the source's status and VIC enable/priority/vector
   programming where registers support readback.
2. Trigger each available event using a documented stimulus: timer expiry,
   host-driven UART0 receive, or RTC alarm using a valid board timebase.
   Include UART1/SPI/GPIO only on a wrapper that physically supports them.
3. For each event, check correct IRQ number, VIC1 dispatch, handler count,
   peripheral status clear, VIC end-of-interrupt behavior, and source deassert.
4. For watchdog, use only a separately supervised run: test interrupt-only
   behavior if supported; otherwise isolate the reset case under
   FW-RESET-001. Never let a watchdog expire unexpectedly during another test.
5. Exercise two enabled VIC1 sources with distinct priorities and check the
   documented arbitration order. Include rapid enable/disable only if the
   peripheral and VIC programming sequence is defined.

**Checks and pass criteria**

* Every available and claimed source is delivered once on VIC1 and
  acknowledged without leaving the interrupt line asserted.
* Unconnected UART1/GPIO/SPI sources are explicitly marked unavailable in the
  board profile, not silently counted as passed. If GPIO combined IRQs are
  part of the board's intended R5 map, test them only after a pin route exists.

**Evidence:** Source, configured IRQ, observed VIC instance/vector, handler
count, peripheral status before/after, and board loopback details.
