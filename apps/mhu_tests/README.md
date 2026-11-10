# mhu_tests — Arm MHU-320AE verification

Exercises both Message Handling Units in the milliSoC:

| Direction | Postbox (sender) | Mailbox (receiver) |
| --------- | ---------------- | ------------------ |
| M85 → R5  | M85 `MHU_SND`    | R5 `MHU_REC`       |
| R5 → M85  | R5 `MHU_SND`     | M85 `MHU_REC`      |

Each MHU is configured with **4 doorbell channels** and **4 fast channels**
(one fast-channel group, four channels per group). On the current FPGA the fast
channels are 32-bit wide.

Both cores run the same suite (`mhu_tests_common.h`) at the same time. The two
cores execute a fixed, identical step list in lockstep: for every step one core
is the sender and the other is the receiver.

* **sender** — drive the stimulus, then wait for an acknowledge token
* **receiver** — wait for the stimulus, verify it, consume it, then send the
  acknowledge token

The acknowledge token is doorbell **channel 3, bit 31** on the *reverse* MHU,
so it never collides with the data under test (the channel-3 data patterns keep
bit 31 clear). This keeps the step list deterministic without a shared clock.

## What is checked

### Discovery (18 checks)

* postbox/mailbox component ID (`0xB105F00D`)
* postbox block ID (`0x0`) and mailbox block ID (`0x1`)
* doorbell channel count (4)
* fast channel count (4), fast channel groups (1), channels per group (4)
* fast channel word size (32 or 64 bits)
* doorbell and fast-channel feature support

The FIFO extension is reported as informational output, not a PASS/FAIL check:
this SoC does not implement it (`FEAT_SPT0` FIFO field reads 0).

### Polled doorbell and fast-channel transport (60 checks)

Eight doorbell steps (channels 0–3, both directions) and eight fast-channel
steps (channels 0–3, both directions), all with interrupts masked:

* doorbell flag set, payload visible on the receiver, receiver interrupt status,
  flag cleared by the receiver
* sender-side handshake: the postbox flag follows the receiver's clear
* sender doorbell acknowledge interrupt
* fast-channel transfer, payload, and (on the receiver) the transfer interrupt
  status and its clear-on-read
* fast-channel sender readback
* acknowledge token received / accepted

### Interrupt delivery (14 checks)

Two doorbell and two fast-channel steps with the receiver interrupts enabled:

* M85: NVIC `IPC_REC_MBX` / `IPC_REC_FST`
* R5: VIC vectored interrupts for the mailbox and fast-channel sources

The receiver interrupt handlers record the channel status and payload; the test
body checks that the interrupt arrived, carried the right payload, and that the
handler cleared the source.

## Hardware notes

* The `mhu.h` `PFCW`/`MFCW` arrays use an 8-byte stride, which is only correct
  for 64-bit fast channels. In 32-bit mode the payload windows are 4 bytes
  apart (`4*n + 0x3000`), so the suite addresses them explicitly
  (`8*n + 0x3000` for 64-bit).
* The mailbox block ID reset value is `0x1`; the postbox is `0x0`.
* The acknowledge token is polled. During the interrupt block, doorbell
  channel 3 is removed from the mailbox combined interrupt
  (`MDBCW<3>_CTRL.MBX_COMB_EN = 0`) so the receiver's doorbell handler cannot
  consume the token before the test loop sees it. The polled block above still
  exercises the real interrupt status of all four channels.
* `irq_map.h` spells the R5 mailbox IRQ as invalid octal `09`, so the R5 image
  defines the two IRQ numbers locally.
* The R5 startup code does not zero `.bss`, so the shared suite initialises its
  counters explicitly at the start of `mhut_run_suite()`.
* The R5 image must fit in the 16 KiB ITCM the bootloader fills. This app is
  built with `-O2` on the R5 (`R5_OPT` in the `Makefile`).

## Build

```sh
make TESTNAME=mhu_tests
```

The flash image is `build/mhu_tests/mhu_tests.hex`.

## Run on hardware

With the DSTREAM connected and the UART terminals closed:

```sh
make run_hw TESTNAME=mhu_tests
```

This flashes over DSTREAM (typically ~1 min), captures both UARTs until the M85
sends the end-of-simulation byte, saves `uart_M85.log`, `uart_R5.log` and
`flash.log` in `build/mhu_tests/`, and exits non-zero if a `FAIL` line appears
or the end marker never arrives.

Defaults are M85 `COM3`, R5 `COM7`, 250000 baud; override with
`M85_COM=COMx R5_COM=COMy UART_BAUD=<baud>`. Use `make flash_hw` to flash only
and `make uart_hw` to capture only.

## Expected output

Each core ends with:

```text
Discovery: 18 checks, 18 passed, 0 failed, 0 skipped
Doorbell and fast channel transport: 60 checks, 60 passed, 0 failed, 0 skipped
Interrupt delivery: 14 checks, 14 passed, 0 failed, 0 skipped

M85: 92 checks, 92 passed, 0 failed, 0 skipped
M85: PASS
```

and the same summary from the R5. The discovery block reports the fast-channel
width and whether group interrupts and the FIFO extension are implemented.
