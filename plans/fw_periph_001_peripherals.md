# FW-PERIPH-001 — Peripheral function and interrupt checks

> Testcase specification derived from the [firmware test plan](../../plans/firmware_test_plans.md).
> Apply the [global firmware verification procedure](./firmware_verification_procedure.md) for board setup, execution, result handling, and reporting.

**Purpose:** Validate firmware-visible basic operation of UART, timers, GPIO,
SPI, watchdog, and RTC that are present and safely accessible on the board.

**Participants:** R5 and M85 according to their respective peripheral maps.
**Priority:** P1.

**Current FPGA wrapper disposition:** UART0 for each core is connected to its
own board UART pins. UART1 TX/RX, GPIO0/1 input/output/enable, and SPI pins are
left unconnected in `millisoc_fpga_wrapper.sv`. UART1, GPIO pin-function, and
SPI transfer tests are therefore **BLOCKED** on this wrapper; do not treat an
APB register read/write as functional pin/transfer coverage. They become
testable only after a board wrapper/bitstream and board profile provide the
physical route or a documented internal loopback. Timers, RTC and watchdog
remain conditional on the configured clock/reset and safety profile.

The SDK map also lists a PL011 at `0x4000_A000`, but no PL011 instance appears
in the current peripheral RTL. Treat that entry as a stale map item, not an
integrated peripheral or a test target.

**Procedure**

1. Read identification/status registers where defined; verify reset values
   before enabling a peripheral.
2. UART0: transmit a known byte pattern on each core's connected host UART and
   verify it in the capture. Check receive behavior by sending controlled input
   from the host only when the board profile confirms RX routing. UART1 is
   excluded unless its pins are wired in the selected wrapper.
3. Timer: program a bounded interval, measure expiry against a calibrated
   reference, verify status/IRQ, clear, and repeat at a second prescale.
4. GPIO: run only with board-approved, physically connected pins and a
   documented loopback/input fixture. On the current wrapper, mark GPIO0/1
   pin tests BLOCKED rather than sampling unconnected inputs.
5. SPI: perform a loopback or exchange only when the selected wrapper routes
   SPI pins to an identified, safe peripheral. On the current wrapper, mark
   SPI transfer tests BLOCKED.
6. RTC: test counter progression and alarm only if clock source and timebase
   are known.
7. Watchdog: non-reset interrupt behavior may be checked here only if the
   watchdog supports it. Any reset-causing sequence belongs to FW-RESET-001.

**Checks and pass criteria**

* Every tested peripheral produces expected data/status within its deadline;
  each claimed interrupt matches the authoritative core-specific map.
* Interrupts and peripheral status are cleared and the device returns to a
  known quiescent state.
* Unsupported or unwired resources are identified as not applicable in the
  board profile, not reported as passing tests.

**Evidence:** Peripheral instance, register/configuration summary, wiring/
fixture, measured timing, IRQ identity, and result.
