# timer_tests — CMSDK timer and dualtimer verification

Exercises the CMSDK timer blocks that each core owns:

| Core | Blocks under test                         |
| ---- | ----------------------------------------- |
| M85  | `TIMER0`, `TIMER1`, `DUALTIMER`           |
| R5   | `TIMER0`, `TIMER1`, `DUALTIMER`           |

Both cores run the same suite (`timer_tests_common.h`) at the same time and
print `PASS`/`FAIL` on their own UART. The blocks are per-core, so the two runs
do not interfere with each other.

The suite polls `RIS`/`MIS`/`INTSTATUS`; it does **not** route interrupts. The
R5 image has no vector table, so keeping the tests polled lets the same source
build for both cores.

## What is checked

For each CMSDK timer (`TIMER0`, `TIMER1`):

* component ID (CMSDK `0xB105F00D`)
* `RELOAD` and `CTRL` readback
* down-count from a loaded value
* one-shot raises `INTSTATUS` and stops at zero
* periodic first and second interrupt, `INTCLEAR`
* measured period and consecutive-period stability

For the dualtimer:

* component ID
* `CONTROL` readback
* per-channel `LOAD` readback
* timer 1 periodic mode, reload and `INTCLR`
* timer 1 free-running wrap at `0xFFFFFFFF`
* timer 1 one-shot (stops at zero, no reload)
* masked vs unmasked `RIS`/`MIS`
* timer 1 and timer 2 independence
* 16-bit counter size (period is ~50% of the 32-bit period)
* prescaler divide by 16 and by 256

That is 11 checks per CMSDK timer plus 20 for the dualtimer, 42 checks per core.

## Clock measurement

The timer clock is not assumed. Each core measures the timer period against its
own free-running core-cycle counter:

* M85: DWT `CYCCNT`
* R5: PMU `PMCCNTR`

The output reports `core cycles per timer tick`, so the result is valid even if
the real core/timer clock differs from the nominal `TIMER_TEST_CORE_HZ`
(10 MHz, used only to turn cycle counts into polling timeouts).

## RTC

The CMSDK RTC (PL031) is **not** tested yet. On the current FPGA the ID block
answers, but the functional registers read as zero and ignore writes, which
suggests the RTC clock is not connected in the RTL. The register definitions
remain in `libs/common/CMSDK/CMSDK.h` for when that is reviewed.

## Build

```sh
make TESTNAME=timer_tests
```

The flash image is `build/timer_tests/timer_tests.hex`.

The R5 image is linked into the 16 KiB ITCM that the bootloader fills, so this
app is built with `-O2` on the R5 (`R5_OPT` in the `Makefile`). At `-O0` the
image overflows the ITCM and the bootloader fails with `Unknown IRQ on CH0!`
floods.

## Run on hardware

With the DSTREAM connected and the UART terminals closed:

```sh
make run_hw TESTNAME=timer_tests
```

This flashes over DSTREAM (~50 s), captures both UARTs until the M85 sends the
end-of-simulation byte, saves `uart_M85.log`, `uart_R5.log` and `flash.log` in
`build/timer_tests/`, and exits non-zero if a `FAIL` line appears or the end
marker never arrives.

Defaults are M85 `COM3`, R5 `COM7`, 250000 baud; override with
`M85_COM=COMx R5_COM=COMy UART_BAUD=<baud>`. Use `make flash_hw` to flash only
and `make uart_hw` to capture only.

## Expected output

Each core ends with:

```text
TIMER0: 11 checks, 11 passed, 0 failed, 0 skipped
TIMER1: 11 checks, 11 passed, 0 failed, 0 skipped
DUALTIMER: 20 checks, 20 passed, 0 failed, 0 skipped

M85: 42 checks: 42 passed, 0 failed, 0 skipped
M85: PASS
```

and the same summary from the R5. The measured rate is around
`1.003–1.004 core cycles per timer tick` on the current FPGA.

## Notes

* `CONTROL readback` is checked instead of the documented `0x20` reset value:
  a warm system reset does not reset the dualtimer, so asserting the power-on
  value makes the test depend on how the board was started.
* The M85 busy-waits for 8 s after its suite so the R5 output is captured
  before the M85 sends the end-of-simulation byte.
* The R5 image has no startup code that zeroes `.bss`, so the shared suite
  initialises its counters explicitly at the start of `tt_run_suite()`.
