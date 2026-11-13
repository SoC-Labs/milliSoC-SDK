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

The model is selected with `MODEL` and defaults to the TensorFlow Lite Micro
`hello_world_int8` sine benchmark (3 fully-connected layers, 280 MACs per
inference). Selecting `MODEL=micro_speech_int8` runs the keyword-spotting
depthwise-convolution model instead (176,008 MACs per inference, 4-class
softmax output). See [`models/README.md`](../../models/README.md) for the model
table and the SRAM budget.

The 100-repeat benchmark replays every test vector (9 scalar inputs for
hello_world, 6 spectrogram frames for micro_speech) and checks every measured
output. The sine model's small size makes it a functional/throughput smoke
test; the keyword model is real enough to give a meaningful MAC-array
utilisation number.

The reported `MMAC/s` is calculated from the model's MAC count (taken from the
image header) and U85 PMU cycles. `GOPS` uses the common convention of two
operations per MAC. Effective MAC-array utilization compares measured
MACs/cycle against the configured 128-MAC array's theoretical
one-MAC-per-unit-per-cycle peak; it is an inferred workload utilization, not a
hardware activity-counter reading. The absolute throughput assumes an 8 MHz NPU
clock, matching the current FPGA configuration. Change `MODEL_NPU_CLOCK_HZ` and
`MODEL_HOST_CLOCK_HZ` in `u85_model_M.c` if the bitstream clocks differ.

## Build

The model image is built automatically as part of the app build when
`APPEND_MODEL=1`:

```sh
make TESTNAME=u85_model APPEND_MODEL=1
make TESTNAME=u85_model APPEND_MODEL=1 MODEL=micro_speech_int8
```

`APPEND_MODEL=1` makes the final `build/u85_model/u85_model.hex` contain both
the code images and the model image at `0x008000`, so a single flash operation
programs everything. Without it, only the code is in the hex and the model has
to be flashed separately with `make flash_model`.

`MODEL` chooses the model source (`models/<MODEL>.tflite`) and the matching
test vectors (`models/vectors/<MODEL>.h`). Switching `MODEL` rebuilds the app
because the vectors are compiled in; the build fails with a clear message if no
vector header exists for that model.

The build produces:

| File                                  | Contents                                       |
| ------------------------------------- | ---------------------------------------------- |
| `build/u85_model/u85_model.hex`       | flash image: preamble + M85 + R5 + model       |
| `build/u85_model/u85_model_M.hex`     | M85 image only                                 |
| `build/u85_model/u85_model_R.hex`     | R5 image only                                  |
| `build/models/<MODEL>_image.bin`      | packed model image (header + cmd + weights)    |
| `build/models/<MODEL>_image.hex`      | same bytes, one per line, for the programmer   |
| `build/models/<MODEL>_image.json`     | offsets, sizes, regions, MACs, CRC32           |

### Run on hardware

With the DSTREAM connected and the UART terminals closed:

```sh
make run_hw TESTNAME=u85_model APPEND_MODEL=1
make run_hw TESTNAME=u85_model APPEND_MODEL=1 MODEL=micro_speech_int8
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
| `0x54` | `macs`                 | Vela `nn_macs` per inference (0 = unknown)     |
| `0x58` | reserved               | zero                                           |
| `0x5C` | reserved               | zero                                           |

The app declares the same structure and has a compile-time size check, so it
does not need to include a generated header. `macs` is taken from Vela's
summary CSV by `tools/model_to_image.py --summary`, which is how the throughput
report knows the model's MAC count without a per-model constant.

For `hello_world_int8` and `models/millisoc_vela.ini`:

| Item        | Value                                             |
| ----------- | ------------------------------------------------- |
| Image size  | 1,632 bytes                                       |
| Command     | `+0x060`, 476 bytes                               |
| Weights     | `+0x240`, 1,056 bytes, region 0 (flash)           |
| Arena       | 64 bytes, region 1 (SRAM)                         |
| Input       | region 1, offset 16, 1 byte                       |
| Output      | region 1, offset 0, 1 byte                        |
| MACs        | 280                                               |
| CRC32       | `0xE90F65AE`                                      |

For `micro_speech_int8`:

| Item        | Value                                             |
| ----------- | ------------------------------------------------- |
| Image size  | 24,673 bytes                                      |
| Command     | `+0x060`, 3,968 bytes                             |
| Weights     | `+0x1000`, 20,577 bytes, region 0 (flash)         |
| Arena       | 14,400 bytes, region 1 (SRAM)                     |
| Input       | region 1, offset 4,000, 1,960 bytes               |
| Output      | region 1, offset 128, 4 bytes                     |
| MACs        | 176,008                                           |
| CRC32       | `0x0812EA6E`                                      |

## Memory layout

The app uses two buffers in the NPU-visible SRAM at `MODEL_SRAM_BASE`
(`0x30000000`):

| Offset   | Size     | Use                                        |
| -------- | -------- | ------------------------------------------ |
| `0x0000` | 32,768 B | copy of the model image (SRAM-copy mode)   |
| `0x8000` | 16,384 B | feature-map arena (input, output, scratch) |

That leaves the fast-scratch buffer at `0xC000` and fits the 64 KiB of shared
SRAM with 16 KiB to spare. See
[`models/README.md`](../../models/README.md#sram-budget) for the per-model
numbers and why `person_detect` does not fit.

The runtime rejects an image larger than `MODEL_ARENA_OFFSET` (`Model image
FAIL`) or an arena larger than `MODEL_ARENA_SIZE` (`Model arena FAIL`), so
raising them is all a bigger model needs — provided the SRAM is there.

The weight region and the arena region are independent of the image layout:
`BASEP_ARRAY[weight_region]` points at the weight data and
`BASEP_ARRAY[input_region]` points at the arena. The scratch region uses the
same arena base (Vela reports it as region 1 for these models). The command
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

The vectors come from `models/vectors/<MODEL>.h`, included at compile time:

* `hello_world_int8` — nine quantised scalar inputs; the expected outputs were
  generated with the TFLite reference kernels:

  | Input  | -128 | -96 | -64 | -32 | 0 | 32 | 64 | 96 | 127 |
  | ------ | ---- | --- | --- | --- | - | -- | -- | -- | --- |
  | Output | 4    | 89  | 126 | 90  | 4 | -74| -126| -88| -9  |

* `micro_speech_int8` — six 40 × 49 int8 spectrogram frames (silence, two
  constants, a frequency ramp, a tone burst and pseudo-random noise); the
  expected 4-class softmax outputs were generated with
  `tools/gen_test_vectors.py` from the LiteRT integer kernels:

  | Vector | Output (4 int8 probabilities) |
  | ------ | ----------------------------- |
  | silence | `-64 -64 -64 -64` |
  | constant -64 | `-128 -95 -95 62` |
  | constant +63 | `-128 -92 -95 59` |
  | frequency ramp | `-128 -123 -119 114` |
  | tone burst | `-128 -123 -120 115` |
  | pseudo-random noise | `-128 -119 -118 110` |

The comparison tolerance is 2 LSB to allow for fixed-point rounding, and the
check is a max-abs-diff over the whole output tensor. A mismatch, NPU fault or
timeout is reported per vector and summarised as `Model run PASS` /
`Model run FAIL`.

## Expected output

`make run_hw TESTNAME=u85_model APPEND_MODEL=1`:

```
M85: MilliSoC Ethos-U85 Vela model demo
U85 ID: 0x20007001 (arch 2.0, product 7) PASS
Model flash: 0x60008000
Model image: 1632 bytes, cmd 476 @ +0x60, weights 1056 @ +0x240
Model: hello_world_int8, input 1 B, output 1 B, 280 MAC/inference
Model CRC32: 0xE90F65AE PASS
Model source: SRAM copy at 0x30000000
Regions: weights=0 arena=1 fast=0, arena 0x30008000 (16384 bytes)
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
U85 effective MAC-array utilization: 0.139% (assuming 128 MAC units)
M85 benchmark wall time: 99973.750 us at 8 MHz, 2.520 MMAC/s
Model run PASS
```

With `MODEL=micro_speech_int8` the model line, the arena size and the vector
lines change; the shape of the report does not:

```
Model image: 24673 bytes, cmd 3968 @ +0x60, weights 20577 @ +0x1000
Model: micro_speech_int8, input 1960 B, output 4 B, 176008 MAC/inference
Model CRC32: 0x0812EA6E PASS
Regions: weights=0 arena=1 fast=0, arena 0x30008000 (16384 bytes)
U85 inference:
  [0] vector -> 4 outputs, max |diff| 0 PASS
  ...
U85 inference: 6/6 PASS
U85 compute benchmark: 100 repeats x 6 vectors, 600/600 PASS
Model compute: 176008 MAC/inference, 600 inferences, 105604800 MACs total
```

The sine numbers above are representative FPGA measurements, not guaranteed
constants. Its 280 MACs per inference are tiny compared with the 128-unit
array, and command setup and synchronization also cost cycles, so it does not
approach the theoretical peak (2.048 GOPS at 8 MHz with 128 MACs). The
keyword-spotting model has 630× the MACs and will report a much higher
utilisation.

The R5 companion prints one banner line and parks in `WFI`; it deliberately
does not call `UartEndSimulation()`, so the end marker comes from the M85 after
the inferences finish.

## Regenerating the model image

`make model` compiles `models/<MODEL>.tflite` with Vela using
`models/millisoc_vela.ini` and packs the result:

```sh
make model
make model MODEL=micro_speech_int8
```

The Vela configuration puts the weights on the `Axi1` port (flash) and the
arena/cache on `Axi0` (SRAM). Vela emits no DMA staging operations, so the NPU
reads the weights directly from region 0. See
[`models/README.md`](../../models/README.md) for the Vela knobs and the
manifest fields.

To flash the model without touching the code image:

```sh
make flash_model
make flash_model MODEL=micro_speech_int8
```

## Limitations

* The Ethos-U85 on the current FPGA cannot reach the QSPI flash window, which
  is why the default mode copies the image to SRAM first. The direct-from-flash
  path is implemented and selected with `MODEL_RUN_FROM_FLASH=1`, but needs the
  RTL fix before it can pass on hardware.
* The app runs the command stream synchronously, polling `STATUS`; it does not
  use the NPU interrupt. The compute benchmark uses the U85 PMU cycle counter.
* The SRAM budget is fixed at a 32 KiB image window plus a 16 KiB arena, which
  fits both shipped models. A larger model needs `MODEL_ARENA_OFFSET` /
  `MODEL_ARENA_SIZE` raised (and more than 64 KiB of shared SRAM); the runtime
  reports `Model image FAIL` / `Model arena FAIL` if the image does not fit.
* The test vectors are synthetic, not real captured data: they exercise the
  full input range and the quantised arithmetic, not speech recognition
  accuracy. `tools/gen_test_vectors.py` regenerates them for a new model.
