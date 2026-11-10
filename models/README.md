# models — Ethos-U model images for the milliSoC flash

This directory holds the source model(s) and the Vela configuration used by
the milliSoC flow. The compiled model image is stored in QSPI flash **above the
code images**, so it can be updated independently of the application.

## Model choice

`hello_world_int8.tflite` is the TensorFlow Lite Micro "hello world" sine
benchmark, the smallest standard MCU model:

| Property            | Value                                   |
| ------------------- | --------------------------------------- |
| Source              | `tensorflow/tflite-micro` examples      |
| Input               | one int8 value (quantised)              |
| Topology            | 2 hidden dense layers + dense output    |
| Source file         | 2,704 bytes                             |
| NPU operators       | 3 (100 % offloaded to the Ethos-U85)    |
| MACs per inference  | 280                                     |
| Runtime image       | 1,696 bytes (header + cmd + weights)    |
| Command stream      | 508 bytes                               |
| Weights             | 1,056 bytes                             |

It is deliberately tiny: the point is to demonstrate the Vela → flash → NPU
pipeline, not to benchmark the NPU. The whole image fits in one 4 KiB flash
sector.

Downloaded from:

```text
https://raw.githubusercontent.com/tensorflow/tflite-micro/main/tensorflow/lite/micro/examples/hello_world/models/hello_world_int8.tflite
```

SHA-256 of the source model:

```text
505ee4fae7fa46ab67bea4c08b4969eb3eb8b9114c50595ec4a29d9a27993202
```

The model is part of the TensorFlow project and is distributed under the
Apache License 2.0.

## Flash layout

The application image occupies flash from `0x000000` up to about `0x003600`
(current M85 + R5 images). The model starts at `0x008000`, well clear of the
code and on a 4 KiB sector boundary:

| Flash offset | Size     | Contents                                  |
| ------------ | -------- | ----------------------------------------- |
| `0x000000`   | ~14 KiB  | boot preamble + M85 image + R5 image      |
| `0x008000`   | 1,696 B  | Vela model image (header + cmd + weights) |
| ...          |          | free                                      |

Keep the application image below `MODEL_FLASH_BASE`: flashing the app erases
every 4 KiB sector its image spans, so a larger app that crosses `0x008000`
would wipe the model. Move `MODEL_FLASH_BASE` (and re-flash the model) if the
code image grows past it. `tools/concat_hex.py` refuses to build a combined
image when the code overlaps the model blob.

The QSPI controller has a memory-mapped XiP/AHB window at `0x60000000`
(`FLASH_BASE` in `sys_memory_map.h`). Once the controller is in XiP/AHB mode,
the model is visible at:

```text
0x60000000 + 0x008000 = 0x60008000
```

Without XiP, the same bytes are read with the standard SPI READ command
(`0x03`) at flash address `0x008000`.

## Flow

Requirements:

* `vela` on `PATH` (developed with Ethos-U Vela 5.2.0)
* Python 3 with `numpy` for `tools/model_to_image.py`

Build the image:

```sh
make model
```

This runs:

1. `vela models/hello_world_int8.tflite --config models/millisoc_vela.ini
   --system-config MilliSoC_U85 --memory-mode MilliSoC_Flash
   --output-format raw`
2. `tools/model_to_image.py` on the raw `.npz` output

and writes to `build/models/`:

| File                                  | Use                                       |
| ------------------------------------- | ----------------------------------------- |
| `hello_world_int8_vela.npz`           | Vela raw output (cmd/weights/regions)     |
| `hello_world_int8_image.bin`          | packed runtime image                      |
| `hello_world_int8_image.hex`          | one byte per line, for the flash programmer |
| `hello_world_int8_image.json`         | base, offsets, sizes, regions, CRC32      |
| `hello_world_int8_image.h`            | C constants for a future linked-in build  |
| `hello_world_int8_summary_*.csv`      | Vela per-layer summary                    |

The image is not a `.tflite` file: Vela's raw output already separates the
command stream from the weights, and the Ethos-U consumes exactly those two
buffers. `tools/model_to_image.py` adds a 96-byte header so the runtime can
program the NPU without parsing a flatbuffer.

### Combined code + model image

The Makefile can append the model image to an application hex:

```sh
make TESTNAME=u85_model APPEND_MODEL=1
```

`tools/concat_hex.py --blob 0x008000:<image.bin>` fills the gap with `0xFF`,
checks that the code does not overlap the blob, and writes one hex file that
programs both. This is the recommended flow: one flash operation, no stale
model left behind.

To update only the model:

```sh
make flash_model
```

This flashes `build/models/hello_world_int8_image.hex` at `0x008000` through
the same DSTREAM/`armdbg` path as the application. It erases only the 4 KiB
sectors the model spans, so the code image below it is untouched.

## Vela configuration

`millisoc_vela.ini` describes the milliSoC memory system:

```ini
[System_Config.MilliSoC_U85]
core_clock=1e9
axi0_port=Sram
axi1_port=OffChipFlash
...

[Memory_Mode.MilliSoC_Flash]
const_mem_area=Axi1
arena_mem_area=Axi0
cache_mem_area=Axi0
```

* `axi0_port` is the SRAM port at `0x30000000`, reachable by both the M85 and
  the Ethos-U.
* `axi1_port` is the off-chip flash / XiP port. Vela places the read-only
  weights there.
* `const_mem_area=Axi1` puts the weights in flash; `arena_mem_area` and
  `cache_mem_area` put feature maps and scratch in SRAM.

With this plan Vela emits no DMA staging operations and no fast-scratch buffer:
the NPU reads weights directly from region 0 (flash), and all feature maps live
in region 1 (SRAM).

The Makefile knobs are:

| Variable             | Default                    | Meaning                                      |
| -------------------- | -------------------------- | -------------------------------------------- |
| `MODEL`              | `hello_world_int8`         | source model in `models/` (without `.tflite`) |
| `MODEL_FLASH_BASE`   | `0x008000`                 | flash offset of the image                    |
| `ACCEL_CONFIG`       | `ethos-u85-256`            | Vela accelerator (`ethos-u85-128/256/512/1024/2048`) |
| `VELA_CONFIG`        | `models/millisoc_vela.ini` | Vela system/memory configuration             |
| `VELA_SYSTEM_CONFIG` | `MilliSoC_U85`             | section in `VELA_CONFIG`                     |
| `VELA_MEMORY_MODE`   | `MilliSoC_Flash`           | memory mode section in `VELA_CONFIG`         |
| `APPEND_MODEL`       | `0`                        | `1` embeds the image in the app hex          |
| `MODEL_RUN_FROM_FLASH` | `0`                      | `1` lets the NPU fetch cmd/weights from flash |

Example:

```sh
make model MODEL=mobilenet_v2_1.0_224_int8 ACCEL_CONFIG=ethos-u85-512
```

`make model_report` re-runs Vela with `--supported-ops-report` and writes
`SUPPORTED_OPS.md` into `build/models/`.

## Notes

* Vela resolves `--config` relative to its own `ethosu/config_files` directory,
  so the Makefile passes an absolute path (`$(abspath ...)`). A relative path
  produces a confusing "file not found" error.
* The sine model declares a dynamic batch axis. Vela fixes it to 1 and prints
  four `dynamic signature in axis 0` notices; this is expected.
* `ACCEL_CONFIG` must match the NPU configuration in the RTL. The current
  default is `ethos-u85-256`; change it if the bitstream uses a different MAC
  count. The NPU `CONFIG` register (`0x44032028`) reports the implemented
  configuration.
* The runtime image is self-describing (magic `MVL1`, version 1, CRC32 over
  cmd + weights). The header layout is documented in
  [`apps/u85_model/README.md`](../apps/u85_model/README.md).

## Running the model

The testcase is [`apps/u85_model`](../apps/u85_model). It reads the image from
flash, checks the header and CRC, and then runs the command stream on the
Ethos-U85. Because the current FPGA does not give the NPU bus access to the
flash window, the default mode copies the image into SRAM at `0x30000000`
first. The direct-from-flash path is implemented behind
`MODEL_RUN_FROM_FLASH=1` and only needs the RTL change.

The model needs an input tensor of one int8 value and produces one int8 value.
The TFLite Micro `hello_world` example contains the matching preprocessing and
postprocessing code if a full inference demo is added later.
