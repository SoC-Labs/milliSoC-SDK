# milliSoC Firmware Verification Procedure

| | |
|---|---|
| **Status** | Draft — applies to firmware testcase plans in this directory |
| **Target** | Configured milliSoC FPGA; board-specific profile required |
| **Source** | [Firmware testcase plan](../../plans/firmware_test_plans.md) and [SoC verification plan](../../plans/verification_plan.md) |
| **Last updated** | 2026-11-13 |

This document defines the common setup, execution, pass/fail, and evidence
requirements for the testcase plans in this directory. It does not claim that
firmware or tests have been implemented, and it does not replace RTL simulation,
protocol checking, CDC/formal analysis, or FPGA timing sign-off.

## 1. Testcase index

| ID | Testcase | Priority | File |
|---|---|---:|---|
| FW-BOOT-001 | Cold boot and core release | P0 | [fw_boot_001_cold_boot.md](fw_boot_001_cold_boot.md) |
| FW-BOOT-002 | Repeated boot after supported reset | P1 | [fw_boot_002_warm_boot.md](fw_boot_002_warm_boot.md) |
| FW-CORE-001 | Configured CPU instruction-extension smoke | P1 | [fw_core_001_cpu_extensions.md](fw_core_001_cpu_extensions.md) |
| FW-MEM-001 | Shared SRAM capacity, boundaries, and alias | P0 | [fw_mem_001_shared_sram.md](fw_mem_001_shared_sram.md) |
| FW-MEM-002 | CPU and DMA memory-access matrix | P0 | [fw_mem_002_access_matrix.md](fw_mem_002_access_matrix.md) |
| FW-IRQ-001 | Core interrupt behavior and priority | P0 | [fw_irq_001_core_interrupts.md](fw_irq_001_core_interrupts.md) |
| FW-IRQ-002 | R5 VIC1 peripheral interrupt delivery | P0 | [fw_irq_002_vic1.md](fw_irq_002_vic1.md) |
| FW-RESET-001 | Local, system, and watchdog reset behavior | P0 | [fw_reset_001_reset_behavior.md](fw_reset_001_reset_behavior.md) |
| FW-MHU-001 | Bidirectional MHU and shared-memory handshake | P1 | [fw_mhu_001_interprocessor.md](fw_mhu_001_interprocessor.md) |
| FW-DMA-001 | DMA transfer, completion, error, and abort | P1 | [fw_dma_001_transfers.md](fw_dma_001_transfers.md) |
| FW-QSPI-001 | Read-only QSPI/XIP and warm-reset retention | P1 | [fw_qspi_001_flash_reads.md](fw_qspi_001_flash_reads.md) |
| FW-PERIPH-001 | Peripheral function and interrupt checks | P1 | [fw_periph_001_peripherals.md](fw_periph_001_peripherals.md) |
| FW-U85-001 | U85 configuration, memory, IRQ, and inference | P1 | [fw_u85_001_inference.md](fw_u85_001_inference.md) |
| FW-DEBUG-001 | Firmware debug targets and external DAP checks | P1 | [fw_debug_001_dap.md](fw_debug_001_dap.md) |
| FW-STRESS-001 | Bounded concurrent-load and recovery | P2 | [fw_stress_001_concurrency.md](fw_stress_001_concurrency.md) |
| FW-PERF-001 | Latency and throughput characterization | P2 | [fw_perf_001_characterization.md](fw_perf_001_characterization.md) |

## 2. Board profile and preflight

Before implementation or execution, maintain a versioned board profile alongside
the test image. Record:

* FPGA and board revisions, clock frequencies, reset and boot controls, UART
  routing, and required external loopback wiring or fixtures.
* The selected RTL configuration and authoritative, separate R5 and M85 memory
  and interrupt maps.
* Implemented SRAM/TCM capacities, permitted flash image layout, and reset
  sources physically available on the board. Record both RTL-source values and
  the values of the bitstream actually programmed; do not assume they match.
* Peripheral availability and safe operating policy, especially for flash
  erase/program, watchdog reset, GPIO/SPI pin routing, UART routing, and DDR.
* U85 RTL/IP configuration, available M85-compatible runtime, model/input and
  reference-output versions, legal memory route, and expected tolerances if U85
  testing is requested.
* Debugger/host tool version and approved debug-access policy.

Do not infer pinout, clocks, fixtures, memory visibility, accelerator support, or
reset behavior from simulation. If a required input is missing, mark the
associated testcase **BLOCKED** and identify the missing information.

Preflight every run:

1. Confirm a known-good bitstream and firmware image, their hashes/build IDs,
   and the board-profile revision.
2. Confirm UART capture is active before releasing reset and that the board's
   documented reset/reprogram recovery path is available.
3. Confirm test-owned memory ranges and fixtures are exclusive and safe.
4. Identify any destructive, reset-causing, or debugger-controlled phase and
   schedule it as an isolated case with recovery available.
5. Capture operator, date/time, FPGA/board identity, configuration, and test
   manifest in the run record.

### 2.1 Current FPGA feasibility and integrated-IP coverage

This is a source-level review of the current checkout, not a report that the
latest source has been built into or tested on a bitstream. The checkout has
uncommitted RTL/FPGA changes; bind every result to the exact source revision and
bitstream before using this matrix for sign-off.

| Integrated IP / function | Firmware plan owner | FPGA feasibility and limits |
|---|---|---|
| R5, M85 and boot ROM | FW-BOOT-001/002; FW-RESET-001 | Feasible as observable boot and execution tests with both UART logs. Firmware cannot inspect processor-internal correctness or replace RTL tests. |
| M85 ITCM/DTCM; R5 ATCM/BTCM | FW-MEM-002; FW-BOOT-001 | Feasible for safe in-range read/write checks in reserved regions and boot preload. Exact capacities/maps are blocked until the selected bitstream and firmware headers/linker files agree (see below). Do not probe outside TCM bounds on hardware. |
| Shared SRAM and SIE300 SRAM controller | FW-MEM-001/002; FW-MHU-001; FW-U85-001 | In-range data-path checks are feasible. Capacity/alias and subword-access results are bitstream-specific; verify the implemented depth and byte-lane behavior before using boundary or alias expectations. |
| NIC400 and AXI/AHB/APB bridges | FW-MEM-002; FW-DMA-001; FW-STRESS-001 | Legal routed transactions can be checked indirectly. Decode faults, protocol violations, deadlock, and exhaustive arbitration/coverage belong in RTL/formal verification, not destructive FPGA firmware probes. |
| DMA350 | FW-BOOT-001; FW-DMA-001; FW-MEM-002 | Valid, bounded transfers and completion IRQ are feasible. Error/abort cases are board tests only when a documented recoverable path exists; otherwise keep them in simulation. |
| QSPI controller and external flash | FW-BOOT-001; FW-QSPI-001; FW-DMA-001 | Boot/read-only checks are feasible on a configured board. Program/erase is excluded unless an approved scratch range and recovery process exist. M85 register access and U85 XIP are not assumed. |
| R5 PL192 VIC0/VIC1; M85 NVIC | FW-IRQ-001/002; FW-MHU-001; FW-DMA-001 | Feasible for connected sources with valid core-specific maps. Do not count an unwired source as tested; full routing/priority corner coverage remains an RTL/verification-plan responsibility. |
| MHU320 send/receive blocks | FW-MHU-001; FW-BOOT-001 | Feasible on both cores using documented channels, bounded handshakes and separate UART results. |
| CMSDK timers and dual timer | FW-PERIPH-001; FW-IRQ-001/002; FW-PERF-001 | Feasible as bounded timer/status/IRQ tests. Clock accuracy and CDC sign-off require separate measurement/RTL analysis. |
| M85 FPU/MVE and optional R5 FPU | FW-CORE-001 | Current M85 RTL has conflicting FPU settings (`FPU(0)` and `CFGFPU=1`) and enables the integer MVE subset. The R5 `FPU0` parameter defaults to 0. Reconcile elaborated configurations before extension tests; the existing `fpu_tests` app does not test MVE or resolve the M85 conflict. |
| CMSDK UART0 | FW-PERIPH-001; FW-IRQ-001/002 | The current FPGA wrapper routes the two core-specific UART0 instances to host UART pins. Host RX/TX behavior is testable with the board profile and captures. |
| CMSDK UART1 | FW-PERIPH-001; FW-IRQ-002 | Not board-testable with the current wrapper: UART1 pins are left unconnected. Mark unavailable unless a new wrapper/bitstream routes them. |
| PL011 UART (`0x4000_A000` in the SDK map) | None; not integrated | No PL011 instance was found in the current peripheral RTL. The address in the SDK map is not evidence of an implemented peripheral; do not test it or claim coverage until the RTL is integrated and the map is reconciled. |
| GPIO0/GPIO1 | FW-PERIPH-001; FW-IRQ-001/002 | Not board-testable with the current wrapper: GPIO inputs/outputs/enables are left unconnected. Internal register access alone is not a GPIO pin-function test. |
| SPI | FW-PERIPH-001; FW-IRQ-002 | Not board-testable with the current wrapper: SPI pins are left unconnected. Add a board route/fixture before claiming transfer or IRQ coverage. |
| RTC and watchdog | FW-PERIPH-001; FW-IRQ-002; FW-RESET-001 | RTC counter/alarm checks require the documented clock/timebase. Watchdog interrupt/reset tests are feasible only as isolated, supervised cases with recovery. |
| SYSCTRL, CPUWAIT and reset controller | FW-BOOT-001/002; FW-RESET-001 | Register and reset-path checks are feasible in a known-good bitstream. POR/DAP physical sources require board-level stimulus; boot-after-reset must be observed, not inferred from register writes. |
| Ethos-U85 | FW-U85-001; FW-STRESS-001; FW-PERF-001 | Full inference remains BLOCKED until the target bitstream, runtime, legal memory routes, IRQ and reference vectors are confirmed. The existing RTL instance/register access alone is not end-to-end NPU coverage. |
| CoreSight debug, DAP and trace (ETM/TPIU/funnel) | FW-DEBUG-001 | DAP access requires an authorized external host. Trace output is present at the FPGA wrapper but needs a supported capture path; firmware alone cannot verify SWD/JTAG or trace pins. |
| External DDR4 controller/PHY and memory | FW-MEM-002; FW-DMA-001; FW-PERF-001 | The current working wrapper/NIC source exposes an external-memory path, decoded at `0x9000_0000`–`0xAFFF_FFFF`. This is conditional on the DDR design being built into the programmed bitstream, completing initialization, and a board profile defining a safe test range. |
| Clock/reset distribution | FW-BOOT-001; FW-RESET-001; FW-PERF-001 | Firmware can show that a clocked block makes progress and reacts to reset. Clock quality, CDC, reset-domain crossing and timing closure are not firmware-testable sign-off items. |

#### Memory-map reconciliation required before memory-test implementation

The current working source configures M85 ITCM/DTCM as 64 KiB each and R5
ATCM/BTCM as 64/32 KiB. However, the current R5 SDK
`libs/Device/R5/Include/sys_memory_map.h` still declares 16 KiB TCM/SRAM values
and a 32 KiB shared-SRAM size. Do not build test bounds from that stale header.

There is also a capacity mismatch in the shared-SRAM description. The current
working `millisoc_sram_wrapper.sv` passes `ADDR_WIDTH=17` to the generic
`SRAM.v`; that implementation indexes `memaddr[ADDR_WIDTH-1:3]` and stores
64-bit words, which yields 2^(17-3) × 8 = 128 KiB if the SIE300 output is a
byte address. This is a source-derived capacity of 128 KiB; the NIC400 decodes
a 1 MiB window, so the RTL is expected to alias the SRAM at 128 KiB intervals.
The previous `ADDR_WIDTH=16` configuration would yield 64 KiB by the same
calculation, not 512 KiB. Confirm the SIE300 address convention, synthesized
memory depth, and actual bitstream behavior (including alias boundaries)
before setting `SHARED_SRAM_SIZE` or running full-range tests. Until confirmed,
mark FW-MEM-001/002 **BLOCKED** for capacity/boundary claims; a bounded smoke
access to a known-safe word may still be run and reported separately.

## 3. Shared safety and correctness requirements

* Do not program or erase QSPI except within a board-owner-designated scratch
  range whose bounds, erase geometry, and restoration procedure are confirmed.
  QSPI tests are read-only by default.
* Reset and watchdog tests run in isolation. Do not request a local reset while
  the target core has outstanding bus transactions; transaction-drain behavior
  is not established by the current platform notes.
* Use only documented implemented memory ranges for the exact bitstream.
  Earlier plans described 512 KiB of shared SRAM, but the current working RTL
  implies 128 KiB from its address slicing. Resolve the mismatch in §2.1
  against the target bitstream first. Keep ordinary buffers in the confirmed
  implemented range; perform alias characterization only as a dedicated,
  reversible test.
* Do not assume cache coherency. Apply the configured cache maintenance and
  memory barriers for CPU/DMA/U85 sharing and report the exact sequence.
* Do not write reserved registers or guess offsets. Use headers and integration
  documentation matched to the board build. Respect register-specific
  write-one-to-clear, write-one-to-set, and read-modify-write semantics.
* Every loop, handshake, transfer, peripheral event, and reset phase has a
  bounded deadline. Timeout is failure, not success or a silent skip.
* Do not perform destructive or unsafe negative tests on physical hardware when
  the expected bus fault could hang the fabric or damage state; retain those
  cases in the RTL testbench.

## 4. Firmware result protocol

Each participating core emits a uniquely identifiable start and exactly one
terminal result on its UART. For example:

```text
TEST_ID=FW-MEM-001 CORE=M85 START
...
TEST_ID=FW-MEM-001 CORE=M85 PASS
```

A failure reports the first failing check and a stable error code before its
terminal FAIL. If the repository runner uses the verification-plan contract,
each expected UART log additionally ends with exactly one `*** TEST PASS ***`
or `*** TEST FAIL ***` marker.

For multi-core tests:

* Use bounded handshakes and sequence/generation numbers to reject stale
  messages or reset-era state.
* Each core reports its own outcome; the R5 coordinator reports the combined
  outcome only after all expected participants have responded.
* Missing UART output, participant response, terminal marker, or result is not
  PASS. A timeout is FAIL.

Use these run states only:

* **PASS** — all defined checks completed with required evidence.
* **FAIL** — an expected behavior/check failed, a timeout occurred, or a
  required result was missing or malformed.
* **BLOCKED** — a prerequisite, fixture, runtime, or board access is unavailable;
  record the unmet requirement. BLOCKED is not PASS.
* **NOT RUN** — intentionally excluded from the run manifest with a reason.
  P0 tests cannot be silently omitted from sign-off.

## 5. Execution sequence

1. **Smoke:** cold boot/core release, bounded shared-SRAM check, and
   representative interrupt test (`FW-BOOT-001`, `FW-MEM-001`, `FW-IRQ-001`).
2. **P0 functional:** run all remaining P0 memory, IRQ, and reset tests before
   dependent P1 tests. Isolate reset/watchdog cases.
3. **Subsystem tests:** CPU-extension, MHU, DMA, QSPI, connected peripherals,
   U85, DDR4 and debugger/trace tests only when their board-profile prerequisites
   are met. Preserve BLOCKED status for unconnected UART1/GPIO/SPI, uninitialized
   DDR, or unavailable U85/debug capture paths; do not convert them to PASS.
4. **Extended:** run bounded concurrency and performance characterization only
   after the participating functional tests pass.
5. **Archive:** save every UART log (per core), firmware and bitstream hashes,
   board profile, test configuration/seed, fixtures/operator notes, raw failure
   context, and generated measurements.

Follow each testcase's own steps and pass criteria. Run destructive operations
(e.g. flash programming, watchdog reset, core/system reset, debugger halt/reset)
only under that testcase's isolation and recovery requirements.

## 6. Required evidence and reporting

For each test record:

* ID, result state, start/end time, firmware version/hash, bitstream hash, board
  profile/revision, and participant core(s).
* Per-core raw UART logs and parsed terminal markers; for external DAP tests,
  host/debugger version, transport, AP, and addresses exercised.
* Memory/IRQ map revision, address ranges, cache configuration/maintenance,
  reset method, fixtures, and any test-specific seed or input/model hash.
* Measurements and raw samples for performance tests; error address/expected/
  actual data and progress/sequence counters for failures.
* BLOCKED/NOT RUN rationale and the prerequisite or scope decision involved.

Do not report only an aggregate PASS. Keep raw logs so each summary can be
reproduced and audited.

## 7. FPGA sign-off criteria

* Every P0 testcase is PASS on the target FPGA image; failures are corrected and
  rerun rather than waived by changing expected output after the run.
* P1/P2 tests are PASS or have a reviewed board-specific disposition and
  rationale. No blocked U85/debug path is presented as verified.
* Results are traceable to firmware, bitstream, board profile, and raw logs.
* SRAM capacity/aliasing, core-specific maps, interrupt routes, reset scope and
  retention, and all unavailable board features are documented as measured
  facts or explicit limitations.
* Every integrated block in §2.1 has a linked firmware test owner or an explicit
  not-board-testable/RTL-only disposition. A component that is merely present
  in the RTL is not counted as covered.
* No sign-off claim depends on undocumented behavior, simulation-only stimulus,
  or existing firmware that was outside the testcase specification.

## 8. Scope boundary

These firmware plans verify observable functionality on the FPGA. They do not
replace the parent verification plan's RTL unit/integration tests for protocol
violations, unsafe unmapped accesses, reset during active traffic, flash/DMA
error injection, coverage closure, CDC, or formal properties. Nor do they
establish FPGA timing closure, ASIC readiness, electrical robustness, or
worst-case real-time guarantees.
