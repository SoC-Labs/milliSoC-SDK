# u85_model — run a Vela-compiled model on the Ethos-U85

This app is the end-to-end AI/ML demonstrator for the milliSoC:

1. the M85 reads a self-describing model image from QSPI flash at
   `MODEL_FLASH_BASE` (`0x008000` by default),
2. it checks the header and CRC32,
3. it either copies the image into the NPU-visible SRAM at `0x30000000` or
   leaves it in flash (see [Run modes](#run-modes)),
4. it configures the Ethos-U85 memory attributes, regions and `BASEP_ARRAY`,
5. it runs the Vela command stream for each test vector and checks the int8
   output against the reference results,
6. it repeats the verified inferences to measure NPU compute throughput.

The model is the TensorFlow Lite Micro `hello_world_int8` sine benchmark
(3 fully-connected layers, 280 MACs per inference). The 100-repeat benchmark
uses nine known inputs (900 inferences, 252,000 model MACs) and checks every
measured output. This is a real compute workload, but its small size makes it
a functional/throughput smoke test rather than a representative large-network
benchmark.

The reported `MMAC/s` is calculated from model MAC count and U85 PMU cycles.
`GOPS` uses the common convention of two operations per MAC. Effective
MAC-array utilization compares measured MACs/cycle against the configured
256-MAC array's theoretical one-MAC-per-unit-per-cycle peak; it is an
inferred workload utilization, not a hardware activity-counter reading. The
absolute throughput assumes an 8 MHz NPU clock, matching the current FPGA
configuration. Change `MODEL_NPU_CLOCK_HZ` and `MODEL_HOST_CLOCK_HZ` in
`u85_model_M.c` if the bitstream clocks differ.

## Build

The model image is built automatically as part of the app build when
`APPEND_MODEL=1`:

```sh
make TESTNAME=u85_model APPEND_MODEL=1
```

`APPEND_MODEL=1` makes the final `build/u85_model/u85_model.hex` contain both
the code images and the model image at `0x008000`, so a single flash operation
programs everything. Without it, only the code is in the hex and the model has
to be flashed separately with `make flash_model`.

The build produces:

| File                                  | Contents                                       |
| ------------------------------------- | ---------------------------------------------- |
| `build/u85_model/u85_model.hex`       | flash image: preamble + M85 + R5 + model       |
| `build/u85_model/u85_model_M.hex`     | M85 image only                                 |
| `build/u85_model/u85_model_R.hex`     | R5 image only                                  |
| `build/models/hello_world_int8_image.bin` | packed model image (header + cmd + weights) |
| `build/models/hello_world_int8_image.hex` | same bytes, one per line, for the programmer |
| `build/models/hello_world_int8_image.json`| offsets, sizes, regions, CRC32                |

### Run on hardware

With the DSTREAM connected and the UART terminals closed:

```sh
make run_hw TESTNAME=u85_model APPEND_MODEL=1
```

This flashes the combined image and captures both UARTs until the M85 sends
the end-of-simulation byte. Defaults are M85 `COM3`, R5 `COM7`, 250000 baud;
override with `M85_COM=`, `R5_COM=` and `UART_BAUD=`. See
[`tools/hw_run.py`](../../tools/hw_run.py) and the top-level `AGENTS.md`.

## Model image format

`tools/model_to_image.py` packs Vela's raw output into a runtime image. All
fields are little-endian `uint32`:

| Offset | Field                  | Meaning                                        |
| ------ | ---------------------- | ---------------------------------------------- |
| `0x00` | `magic`                | `0x314C564D` (`"MVL1"`)                        |
| `0x04` | `version`              | `1`                                            |
| `0x08` | `header_size`          | `96`                                           |
| `0x0C` | `image_size`           | total image bytes                               |
| `0x10` | `cmd_offset`           | command stream offset (`0x60`)                 |
| `0x14` | `cmd_size`             | command stream bytes                           |
| `0x18` | `weight_offset`        | weights offset (64-byte aligned, `0x280`)      |
| `0x1C` | `weight_size`          | weight bytes                                   |
| `0x20` | `weight_region`        | Vela region for the weights                    |
| `0x24` | `scratch_region`       | Vela region for scratch                        |
| `0x28` | `scratch_size`         | scratch bytes                                  |
| `0x2C` | `scratch_fast_region`  | Vela fast-scratch region                       |
| `0x30` | `scratch_fast_size`    | fast-scratch bytes                             |
| `0x34` | `input_region`         | Vela region for the input feature map          |
| `0x38` | `input_offset`         | input offset inside its region                 |
| `0x3C` | `input_size`           | input tensor bytes                             |
| `0x40` | `output_region`        | Vela region for the output feature map         |
| `0x44` | `output_offset`        | output offset inside its region                |
| `0x48` | `output_size`          | output tensor bytes                            |
| `0x4C` | `arena_size`           | scratch/feature-map arena bytes                |
| `0x50` | `crc32`                | CRC32 over command stream + weights            |
| `0x54` | reserved               | zero                                           |
| `0x58` | reserved               | zero                                           |
| `0x5C` | reserved               | zero                                           |

The app declares the same structure and has a compile-time size check, so it
does not need to include a generated header.

For the current model and `models/millisoc_vela.ini`:

| Item        | Value                                             |
| ----------- | ------------------------------------------------- |
| Image size  | 1,696 bytes                                       |
| Command     | `+0x060`, 508 bytes                               |
| Weights     | `+0x280`, 1,056 bytes, region 0 (flash)           |
| Arena       | 64 bytes, region 1 (SRAM)                         |
| Input       | region 1, offset 16, 1 byte                       |
| Output      | region 1, offset 0, 1 byte                        |
| CRC32       | `0x8279697C`                                      |

## Memory layout

The app uses two buffers in the NPU-visible SRAM at `MODEL_SRAM_BASE`
(`0x30000000`):

| Offset   | Size     | Use                                        |
| -------- | -------- | ------------------------------------------ |
| `0x0000` | 1,696 B  | copy of the model image (SRAM-copy mode)   |
| `0x2000` | 4,096 B  | feature-map arena (input, output, scratch) |

The weight region and the arena region are independent of the image layout:
`BASEP_ARRAY[weight_region]` points at the weight data and
`BASEP_ARRAY[input_region]` points at the arena. The scratch region uses the
same arena base (Vela reports it as region 1 for this model). The command
stream's own addresses are region-relative, so the same image works from flash
or SRAM as long as the base pointers match.

## Run modes

`MODEL_RUN_FROM_FLASH` selects where the NPU fetches the command stream and
weights from:

| Value | Mode | Description |
| ----- | ---- | ----------- |
| `0` (default) | SRAM copy | The M85 copies the image from flash to `0x30000000`, sets `QCONFIG=0` and `REGIONCFG=0` (everything through `MEM_ATTR0`, the SRAM port). Works on the current FPGA, where the Ethos-U has no bus access to the flash window. |
| `1` | Direct from flash | The NPU fetches the command stream and weights from the XiP window. `QCONFIG=1` selects `MEM_ATTR1` (EXT port) for the command stream and `REGIONCFG` maps region 0 to `MEM_ATTR1`. Needs the RTL change that gives the Ethos-U access to QSPI flash. |

Set it on the make command line:

```sh
make TESTNAME=u85_model APPEND_MODEL=1 MODEL_RUN_FROM_FLASH=1
```

In both modes the feature maps, scratch and I/O stay in SRAM.

The flash address used by the M85 is `FLASH_BASE + MODEL_FLASH_BASE`
(`0x60008000`). If the NPU's EXT port sees the flash through a different
address, change the `MODEL_RUN_FROM_FLASH` path in `u85_model_M.c`; the
SRAM-copy mode is unaffected.

## Test vectors

The app runs nine quantised inputs through the model. The expected outputs
were generated with the TFLite reference kernels:

| Input  | -128 | -96 | -64 | -32 | 0 | 32 | 64 | 96 | 127 |
| ------ | ---- | --- | --- | --- | - | -- | -- | -- | --- |
| Output | 4    | 89  | 126 | 90  | 4 | -74| -126| -88| -9  |

The comparison tolerance is 2 LSB to allow for fixed-point rounding. A
mismatch, NPU fault or timeout is reported per vector and summarised as
`Model run PASS` / `Model run FAIL`.

## Expected output

```
M85: MilliSoC Ethos-U85 Vela model demo
U85 ID: 0x20007001 (arch 2.0, product 7) PASS
Model flash: 0x60008000
Model image: 1696 bytes, cmd 508 @ +0x60, weights 1056 @ +0x280
Model CRC32: 0x8279697C PASS
Model source: SRAM copy at 0x30000000
Regions: weights=0 arena=1 fast=0, arena 0x30002000 (4096 bytes)
U85 soft reset PASS
U85 inference:
  [0] input -128 -> out    4 (expected    4) PASS
  ...
  [8] input  127 -> out   -9 (expected   -9) PASS
U85 inference: 9/9 PASS
Model inference verification PASS
U85 compute benchmark: 100 repeats x 9 vectors, 900/900 PASS
Model compute: 280 MAC/inference, 900 inferences, 252000 MACs total
U85 compute PMU: 706373 cycles, 2.803 cycles/MAC
U85 compute throughput: 2.854 MMAC/s, 0.005708 GOPS at 8 MHz NPU clock
U85 effective MAC-array utilization: 0.139% (assuming 256 MAC units)
M85 benchmark wall time: 99973.750 us at 8 MHz, 2.520 MMAC/s
Model run PASS
```

These are representative FPGA measurements, not guaranteed constants. The
model's 280 MACs per inference are tiny compared with the 256-unit array, and
command setup and synchronization also cost cycles; this result is therefore
not expected to approach the theoretical peak of 4.096 GOPS at 8 MHz.

The R5 companion prints one banner line and parks in `WFI`; it deliberately
does not call `UartEndSimulation()`, so the end marker comes from the M85 after
the inferences finish.

## Regenerating the model image

`make model` compiles `models/hello_world_int8.tflite` with Vela using
`models/millisoc_vela.ini` and packs the result:

```sh
make model
```

The Vela configuration puts the weights on the `Axi1` port (flash) and the
arena/cache on `Axi0` (SRAM). Vela emits no DMA staging operations, so the NPU
reads the weights directly from region 0. See
[`models/README.md`](../../models/README.md) for the Vela knobs and the
manifest fields.

To flash the model without touching the code image:

```sh
make flash_model
```

## Limitations

* The Ethos-U85 on the current FPGA cannot reach the QSPI flash window, which
  is why the default mode copies the image to SRAM first. The direct-from-flash
  path is implemented and selected with `MODEL_RUN_FROM_FLASH=1`, but needs the
  RTL fix before it can pass on hardware.
* The app runs the command stream synchronously, polling `STATUS`; it does not
  use the NPU interrupt. The compute benchmark uses the U85 PMU cycle counter.
* The arena is deliberately small (4 KiB) and the model only needs 64 bytes.
  Increase `MODEL_ARENA_SIZE` if a larger model is used.
