# u85_control — Ethos-U85 control and throughput demo

The M85 is the host for the Ethos-U85 NPU. This app:

1. probes the NPU ID register and performs a soft reset,
2. configures the AXI limits, memory attributes, region mapping and
   `BASEP_ARRAY[0]`,
3. builds a command stream of memory-to-memory DMA operations in shared SRAM,
4. launches the stream repeatedly and verifies the copied data,
5. measures operations/second from the NPU PMU cycle counter, with the M85
   DWT cycle counter as a cross-check.

No Vela toolchain or Ethos-U driver is required. The command stream is built by
hand from the encodings in the Ethos-U85 TRM (102685_0000_05_en).

## Build

```sh
make TESTNAME=u85_control
```

The flash image is `build/u85_control/u85_control.hex`. Both `<app>_M.c` and
`<app>_R.c` are required by the Makefile; the R5 image is only a companion that
prints one line and then idles.

### Run on hardware

With the DSTREAM connected and the UART terminals closed:

```sh
make run_hw TESTNAME=u85_control
```

This builds, flashes over DSTREAM (typically ~1 min), opens the M85 and R5 UARTs, and
streams the output until the M85 sends the end-of-simulation byte. It saves
`uart_M85.log`, `uart_R5.log` and `flash.log` in `build/u85_control/` and exits
non-zero if a `FAIL` line appears or the end marker never arrives.

Defaults are M85 `COM3`, R5 `COM7`, 250000 baud; override with
`M85_COM=COMx R5_COM=COMy UART_BAUD=<baud>`. Use `make flash_hw` to flash only
and `make uart_hw` to capture only. The helper is `tools/hw_run.py` and needs
`pyserial` in the interpreter named by `HW_PYTHON` (default `python`).

## Memory layout

The demo owns a 8 KiB window at `U85_SHARED_BASE` (default `SRAM_BASE`,
`0x30000000`):

| Offset   | Size     | Use                                  |
| -------- | -------- | ------------------------------------ |
| `0x0000` | `0x1000` | command stream                       |
| `0x1000` | `0x0800` | DMA source (32 × 64 B)               |
| `0x1800` | `0x0800` | DMA destination (32 × 64 B)          |

All DMA offsets in the command stream are relative to `BASEP_ARRAY[0]`, which is
programmed to `U85_SHARED_BASE`. `QBASE` points at `U85_SHARED_BASE + 0x0000`
and `QSIZE` is the generated stream length.

## Configuration

Override the defaults at the top of `u85_control_M.c`. The Makefile does not
forward arbitrary variables to the compiler, so to set one from the command line
add it to `ARM_CC_OPTIONS` (or `USER_DEFINE`) in the Makefile, for example
`-DU85_SHARED_BASE=0x30000000`.

| Macro               | Default        | Meaning                                      |
| ------------------- | -------------- | -------------------------------------------- |
| `U85_SHARED_BASE`   | `SRAM_BASE`    | Address the NPU sees the shared buffer at    |
| `U85_DMA_OPS`       | `32`           | DMA operations per command stream            |
| `U85_COPY_BYTES`    | `64`           | Bytes per DMA operation (1D transfer)        |
| `U85_RUNS`          | `100`          | Number of command-stream launches measured   |
| `U85_NPU_CLOCK_HZ`  | `10000000`     | NPU clock used to convert cycles to ops/s    |
| `U85_HOST_CLOCK_HZ` | `10000000`     | M85 clock used for the DWT cross-check       |

Both defaults match the current FPGA bitstream, where the M85 and the U85 share
a ~10 MHz clock. Note that `SystemFrequency` in `system_exectb_mcu.c` is 50 MHz
and is *not* the real rate on this FPGA, which is why it is not used here.

The two clock values are assumptions. The NPU number is the meaningful one: the
`kops/s/GHz` figure is clock-independent.

## Expected output

```
M85: MilliSoC Ethos-U85 control demo
Shared window: 0x30000000 (8192 bytes)
U85 ID: 0x20007001 (arch 2.0, product 7) PASS
U85 soft reset PASS
Command stream: 664 bytes, 32 DMA ops x 64 bytes per run
U85 warm-up run PASS
U85 DMA runs: 100/100 PASS
DMA data verify PASS
U85 PMU cycles: 180416 total, 56.380 cycles/op
U85 efficiency: 17736.786 kops/s/GHz
M85 wall clock: 184416 cycles, 18441.600 us at 10 MHz
U85 throughput: 0.173 Mops/s, 11.105 MB/s (wall clock)
U85 throughput: 0.177 Mops/s at 10 MHz NPU clock
```

How to read this:

* `cycles/op` and `kops/s/GHz` come from the U85 PMU cycle counter and are
  independent of any clock assumption. These are the trustworthy efficiency
  figures: this FPGA build does one 64-byte DMA copy in about 56 NPU cycles.
* The wall-clock `Mops/s` / `MB/s` come from the M85 DWT cycle counter, which is
  the only absolute time reference. It is only correct if `U85_HOST_CLOCK_HZ`
  matches the real M85 clock. This rate includes host-side launch, polling and
  cache maintenance, so it is slightly lower than the NPU-busy-time rate.
* The PMU and DWT counters run over the same interval, so their ratio is the
  NPU:M85 clock ratio. The app uses it to sanity-check `U85_NPU_CLOCK_HZ` and
  refuses to print a configured-clock rate when the two disagree by more than
  20%. In the sample above the ratio implies about 9.78 MHz, within 2% of the
  configured 10 MHz, so the `at 10 MHz NPU clock` line is printed. The 2% gap is
  the time the NPU is idle while the M85 prepares and re-arms the next run.
  When the check fails, the app prints the implied NPU clock and a `WARNING`
  instead of that line, which means `U85_NPU_CLOCK_HZ` (or
  `U85_HOST_CLOCK_HZ`) is wrong.

On failure the app prints the final `STATUS`, `QREAD` and `QSIZE`, which is
usually enough to tell a bus fault from a command-parse error.

## Adapting to real hardware

* `U85_SHARED_BASE` must be an address the NPU can reach. If the NPU sees SRAM
  through a different alias than the M85, change it; `QBASE` and
  `BASEP_ARRAY[0]` both derive from it.
* The command stream and data use region 0, mapped by `QCONFIG`/`REGIONCFG` to
  `MEM_ATTR0`, which the demo programs as the SRAM AXI port
  (`U85_MEM_ATTR_SRAM`). If the shared buffer is behind the EXT port, point
  `MEM_ATTR0` at `U85_MEM_ATTR_EXT` (or map the regions to `MEM_ATTR2`).
* The demo performs `SCB_CleanDCache_by_Addr` / `SCB_InvalidateDCache_by_Addr`
  around every run when the build has a D-cache (`_DCACHE_PRESENT`). With the
  default build the caches are off, so those helpers compile to nothing.
* If the NPU does not answer at `U85_CFG_BASE` (`0x44032000`) the app reports
  `FAIL` on the ID check and stops before touching the rest of the register map.

## Replacing the workload with a Vela-generated command stream

`cmd_stream_build()` is the only workload-specific function. To run a real
network:

1. Generate a command stream with Vela for Ethos-U85 and place the blob at
   `U85_SHARED_BASE + U85_CMD_OFFSET` (embed it as a `const` array, or copy it
   from flash).
2. Set `u85_cmd_bytes` to the blob length instead of calling
   `cmd_stream_build()`, and remove the `fill_source()` / `verify_destination()`
   calls.
3. Program `BASEP_ARRAY[0..7]` to match the memory plan Vela used, and set
   `REGIONCFG`/`QCONFIG` to the `MEM_ATTR` indices that plan expects (Vela's
   defaults are `QCONFIG = 2` and `REGIONCFG = 3`, i.e. the EXT port).
4. Keep the `u85_run()` / PMU measurement loop unchanged.

The DMA workload here is one operation per `NPU_OP_DMA_START`, so the reported
"operations/second" is DMA descriptors per second. For a Vela stream the same
code reports command-stream launches or, if you divide by the number of
`NPU_OP_*` compute operations in the stream, MAC operations per second.

## Command stream details

Each DMA operation is five 32-bit words:

```
SET_DMA0_SRC     (CMD1: header word + low-32-bit offset word)
SET_DMA0_DST     (CMD1: header word + low-32-bit offset word)
NPU_OP_DMA_START (CMD0)
```

Both offsets are relative to `BASEP_ARRAY[0]`.

with a preamble of `SET_DMA0_SRC_REGION`, `SET_DMA0_DST_REGION` and
`SET_DMA0_LEN` (1D transfer, 64 bytes), and a tail of `NPU_OP_DMA_WAIT(0)` plus
`NPU_OP_STOP`.

## R5 companion

`u85_control_R.c` prints one banner line and then parks in `WFI`. It deliberately
does **not** call `UartEndSimulation()`, so the end-of-simulation marker comes
from the M85 after the benchmark finishes.
