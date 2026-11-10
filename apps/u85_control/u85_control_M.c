/*
 * milliSoC Ethos-U85 control demo (Cortex-M85 side)
 *
 * The M85 acts as the host for the Ethos-U85 NPU:
 *   1. probes the NPU and performs a soft reset,
 *   2. configures the AXI attributes, memory regions and base pointers,
 *   3. builds a command stream of mem2mem DMA operations in shared SRAM,
 *   4. runs the stream repeatedly and checks the copied data,
 *   5. reports NPU efficiency (cycles/op, ops/cycle) from the U85 PMU cycle
 *      counter and the absolute rate from the M85 DWT cycle counter.
 *
 * The command stream is a hand-built sequence of Ethos-U85 commands, so the
 * demo does not need Vela or the full Ethos-U driver. To run a real network,
 * replace cmd_stream_build() with a Vela-generated command stream and keep the
 * run/measure code unchanged.
 */

#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "ethos_u85.h"

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */

/* Shared memory that both the M85 and the U85 can reach. */
#ifndef U85_SHARED_BASE
#define U85_SHARED_BASE         SRAM_BASE
#endif

/* Size of the shared window carved out of SRAM. */
#define U85_SHARED_SIZE         0x2000UL

/* Offsets inside the shared window (relative to U85_SHARED_BASE). */
#define U85_CMD_OFFSET          0x0000UL
#define U85_SRC_OFFSET          0x1000UL
#define U85_DST_OFFSET          0x1800UL

/* Workload: number of DMA operations per run and bytes per operation. */
#ifndef U85_DMA_OPS
#define U85_DMA_OPS             32U
#endif
#ifndef U85_COPY_BYTES
#define U85_COPY_BYTES          64U
#endif

/* Number of times the command stream is launched. */
#ifndef U85_RUNS
#define U85_RUNS                100U
#endif

/*
 * Clock rates. The M85 DWT counter is the absolute time reference, so
 * U85_HOST_CLOCK_HZ must be the real M85 clock for the Mops/s figure to be
 * correct. U85_NPU_CLOCK_HZ is only cross-checked against the measured
 * PMU/DWT cycle ratio; a mismatch is reported as a warning.
 *
 * Defaults match the current FPGA bitstream, where the M85 and the U85 share
 * a ~10 MHz clock. Override both for silicon or a different bitstream.
 */
#ifndef U85_NPU_CLOCK_HZ
#define U85_NPU_CLOCK_HZ        8000000UL
#endif
#ifndef U85_HOST_CLOCK_HZ
#define U85_HOST_CLOCK_HZ       8000000UL
#endif

#define U85_CMD_AREA_SIZE       0x1000UL
#define U85_DATA_AREA_SIZE      (U85_SHARED_SIZE - U85_DST_OFFSET)

#define U85_POLL_TIMEOUT        1000000UL

#if ((U85_DMA_OPS * U85_COPY_BYTES) > U85_DATA_AREA_SIZE)
#error "U85_DMA_OPS * U85_COPY_BYTES does not fit in the shared window"
#endif

#define U85_CMDS_PER_OP         5U   /* SRC + DST + DMA_START */
#define U85_CMD_PREAMBLE_WORDS  4U   /* SRC_REGION, DST_REGION, LEN */
#define U85_CMD_TAIL_WORDS      2U   /* DMA_WAIT, STOP */

#if (((U85_CMD_PREAMBLE_WORDS + (U85_DMA_OPS * U85_CMDS_PER_OP) + \
       U85_CMD_TAIL_WORDS) * 4U) > U85_CMD_AREA_SIZE)
#error "Command stream does not fit in the command area"
#endif

/* ------------------------------------------------------------------------- */
/* Shared buffers                                                            */
/* ------------------------------------------------------------------------- */
static uint32_t *const u85_cmd =
    (uint32_t *)(uintptr_t)(U85_SHARED_BASE + U85_CMD_OFFSET);
static uint8_t *const u85_src =
    (uint8_t *)(uintptr_t)(U85_SHARED_BASE + U85_SRC_OFFSET);
static uint8_t *const u85_dst =
    (uint8_t *)(uintptr_t)(U85_SHARED_BASE + U85_DST_OFFSET);

static uint32_t u85_cmd_bytes;

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

/* The NPU APB interface is 32-bit only, so split 64-bit register writes. */
static void u85_write64(volatile uint64_t *reg, uint64_t value)
{
    volatile uint32_t *word = (volatile uint32_t *)reg;

    word[0] = (uint32_t)(value & 0xFFFFFFFFULL);
    word[1] = (uint32_t)((value >> 32) & 0xFFFFFFFFULL);
}

static uint64_t u85_read64(volatile uint64_t *reg)
{
    volatile uint32_t *word = (volatile uint32_t *)reg;

    return ((uint64_t)word[1] << 32) | (uint64_t)word[0];
}

/*
 * Cache maintenance. The shared buffer is written by the M85 and read by the
 * NPU, so it must be cleaned before a run and invalidated before the M85 reads
 * the results back. The functions compile away when the build has no D-cache.
 */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
static void u85_clean_shared(void)
{
    SCB_CleanDCache_by_Addr((volatile void *)(uintptr_t)U85_SHARED_BASE,
                            (int32_t)U85_SHARED_SIZE);
}

static void u85_invalidate_shared(void)
{
    SCB_InvalidateDCache_by_Addr((volatile void *)(uintptr_t)U85_SHARED_BASE,
                                 (int32_t)U85_SHARED_SIZE);
}
#else
static void u85_clean_shared(void)
{
}

static void u85_invalidate_shared(void)
{
}
#endif

/* ------------------------------------------------------------------------- */
/* Command stream                                                            */
/* ------------------------------------------------------------------------- */
static uint32_t cmd_stream_build(void)
{
    uint32_t *w = u85_cmd;
    uint32_t i;

    /* Source and destination both live in region 0 (BASEP_ARRAY[0]). */
    *w++ = u85_cmd0(U85_SET_DMA0_SRC_REGION,
                    u85_dma0_src_region(0U,
                                        U85_DMA_REGION_MODE_EXTERNAL,
                                        U85_DMA_STRIDE_MODE_D1,
                                        U85_DMA_IDX_MODE_DISABLED));
    *w++ = u85_cmd0(U85_SET_DMA0_DST_REGION,
                    u85_dma0_dst_region(0U,
                                        U85_DMA_REGION_MODE_EXTERNAL,
                                        U85_DMA_IDX_MODE_DISABLED));

    /* 1D transfers, so DMA0_LEN alone defines each copy. */
    *w++ = u85_cmd1_header(U85_SET_DMA0_LEN, 0U);
    *w++ = u85_cmd1_payload(U85_COPY_BYTES);

    for (i = 0U; i < U85_DMA_OPS; ++i)
    {
        uint32_t src = U85_SRC_OFFSET + (i * U85_COPY_BYTES);
        uint32_t dst = U85_DST_OFFSET + (i * U85_COPY_BYTES);

        *w++ = u85_cmd1_header(U85_SET_DMA0_SRC, 0U);
        *w++ = u85_cmd1_payload(src);
        *w++ = u85_cmd1_header(U85_SET_DMA0_DST, 0U);
        *w++ = u85_cmd1_payload(dst);
        *w++ = u85_cmd0(U85_OP_DMA_START, 0U);
    }

    /* Wait until no descriptor is outstanding, then end the stream. */
    *w++ = u85_cmd0(U85_OP_DMA_WAIT, 0U);
    *w++ = u85_cmd0(U85_OP_STOP, 0U);

    return (uint32_t)((uintptr_t)w - (uintptr_t)u85_cmd);
}

/* ------------------------------------------------------------------------- */
/* NPU control                                                               */
/* ------------------------------------------------------------------------- */
static int u85_wait_reset_done(void)
{
    uint32_t timeout = U85_POLL_TIMEOUT;

    while (timeout-- != 0U)
    {
        if ((U85_Base->Status & U85_STATUS_RESET_Msk) == 0U)
            return 1;
    }

    return 0;
}

static int u85_init(void)
{
    /* Demand power (PE=0) and enable high-level clock gating (CE=1). */
    U85_Base->CMD = U85_CMD_CE_Msk;

    /* Soft reset to a known state. A write to RESET starts the sequence. */
    U85_Base->RESET = 0U;
    if (!u85_wait_reset_done())
        return 0;

    /* Unmask power control after the reset sequence. */
    U85_Base->CMD = U85_CMD_CE_Msk;

    /* Command stream and every region go through MEM_ATTR0 = SRAM port. */
    U85_Base->MEM_ATTR0 = U85_MEM_ATTR_SRAM;
    U85_Base->MEM_ATTR1 = U85_MEM_ATTR_SRAM;
    U85_Base->MEM_ATTR2 = U85_MEM_ATTR_EXT;
    U85_Base->MEM_ATTR3 = U85_MEM_ATTR_EXT;
    U85_Base->QCONFIG = 0U;
    U85_Base->REGIONCFG = 0U;

    U85_Base->AXI_SRAM = U85_AXI_SRAM_DEFAULT;
    U85_Base->AXI_EXT = U85_AXI_EXT_DEFAULT;

    /* Region 0 is the shared SRAM window used by the command stream and data. */
    u85_write64(&U85_Base_Pointers->BASEP_ARRAY[0], (uint64_t)U85_SHARED_BASE);

    return 1;
}

static void u85_pmu_init(void)
{
    /* PMU writes other than PMCR.cnt_en need counting enabled first. */
    U85_PMU->PMCR = U85_PMU_PMCR_CNT_EN_Msk;
    U85_PMU->PMCNTENSET = U85_PMU_PMCNTENSET_CYCLE_CNT_Msk;
    U85_PMU->PMCR = U85_PMU_PMCR_CNT_EN_Msk | U85_PMU_PMCR_CYCLE_CNT_RST_Msk;
}

static uint64_t u85_pmu_cycles(void)
{
    return u85_read64(&U85_PMU->PMCCNTR);
}

static void u85_dump_status(void)
{
    printf("  STATUS=0x%08lx QREAD=0x%08lx QSIZE=0x%08lx\n",
           (unsigned long)U85_Base->Status,
           (unsigned long)U85_Base->QREAD,
           (unsigned long)U85_Base->QSIZE);
}

/* 1 = completed, 0 = timeout, -1 = NPU fault. */
static int u85_run(void)
{
    uint32_t timeout;

    /*
     * Writing QBASE/QSIZE while stopped clears STATUS.cmd_end_reached and
     * makes the next transition-to-running start at QBASE.
     */
    u85_write64(&U85_Base->QBASE, (uint64_t)(U85_SHARED_BASE + U85_CMD_OFFSET));
    U85_Base->QSIZE = u85_cmd_bytes;

    /* Clear any stale interrupt, then start executing. */
    U85_Base->CMD = U85_CMD_CE_Msk | U85_CMD_CQ_Msk;
    U85_Base->CMD = U85_CMD_CE_Msk | U85_CMD_RS_Msk;

    for (timeout = 0U; timeout < U85_POLL_TIMEOUT; ++timeout)
    {
        uint32_t status = U85_Base->Status;

        if ((status & U85_STATUS_FAULT_Msk) != 0U)
            return -1;

        if (((status & U85_STATUS_STATE_Msk) == 0U) &&
            ((status & U85_STATUS_IRQ_Msk) != 0U))
            return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Data                                                                      */
/* ------------------------------------------------------------------------- */
static void fill_source(void)
{
    uint32_t i;
    const uint32_t total = U85_DMA_OPS * U85_COPY_BYTES;

    for (i = 0U; i < total; ++i)
        u85_src[i] = (uint8_t)(0xA5U ^ (uint8_t)(i * 7U));
}

static int verify_destination(void)
{
    uint32_t i;
    const uint32_t total = U85_DMA_OPS * U85_COPY_BYTES;

    for (i = 0U; i < total; ++i)
    {
        if (u85_dst[i] != u85_src[i])
            return 0;
    }

    return 1;
}

/* ------------------------------------------------------------------------- */
/* Host cycle counter (Cortex-M85 DWT)                                       */
/* ------------------------------------------------------------------------- */
static int host_cycles_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0U;
}

/* ------------------------------------------------------------------------- */
/* Reporting                                                                 */
/* ------------------------------------------------------------------------- */
static void report_throughput(uint64_t npu_cycles, uint64_t host_cycles,
                              uint32_t total_ops)
{
    if (npu_cycles != 0ULL)
    {
        uint64_t cycles_per_op_x1000 = (npu_cycles * 1000ULL) / total_ops;
        uint64_t kops_per_ghz_x1000 =
            ((uint64_t)total_ops * 1000000000ULL) / npu_cycles;

        printf("U85 PMU cycles: %llu total, %llu.%03llu cycles/op\n",
               (unsigned long long)npu_cycles,
               (unsigned long long)(cycles_per_op_x1000 / 1000ULL),
               (unsigned long long)(cycles_per_op_x1000 % 1000ULL));
        printf("U85 efficiency: %llu.%03llu kops/s/GHz\n",
               (unsigned long long)(kops_per_ghz_x1000 / 1000ULL),
               (unsigned long long)(kops_per_ghz_x1000 % 1000ULL));
    }
    else
    {
        printf("U85 PMU cycles: counter not running\n");
    }

    if (host_cycles == 0ULL)
    {
        printf("M85 DWT cycles: counter not running\n");
        return;
    }

    /*
     * The DWT count is anchored to the M85 clock, so it is the only absolute
     * time reference available. The PMU count is NPU cycles and therefore
     * needs the NPU clock to become a rate.
     */
    {
        uint64_t us_x1000 =
            (host_cycles * 1000000000ULL) / (uint64_t)U85_HOST_CLOCK_HZ;
        uint64_t mops_x1000 =
            ((uint64_t)total_ops * (uint64_t)U85_HOST_CLOCK_HZ * 1000ULL) /
            (host_cycles * 1000000ULL);
        uint64_t mbytes_x1000 =
            ((uint64_t)total_ops * (uint64_t)U85_COPY_BYTES *
             (uint64_t)U85_HOST_CLOCK_HZ * 1000ULL) /
            (host_cycles * 1000000ULL);

        printf("M85 wall clock: %llu cycles, %llu.%03llu us at %lu MHz\n",
               (unsigned long long)host_cycles,
               (unsigned long long)(us_x1000 / 1000ULL),
               (unsigned long long)(us_x1000 % 1000ULL),
               (unsigned long)(U85_HOST_CLOCK_HZ / 1000000UL));
        printf("U85 throughput: %llu.%03llu Mops/s, %llu.%03llu MB/s (wall clock)\n",
               (unsigned long long)(mops_x1000 / 1000ULL),
               (unsigned long long)(mops_x1000 % 1000ULL),
               (unsigned long long)(mbytes_x1000 / 1000ULL),
               (unsigned long long)(mbytes_x1000 % 1000ULL));

        if (npu_cycles != 0ULL)
        {
            /*
             * Both counters run over the same interval, so their ratio is the
             * NPU:M85 clock ratio. Use it to sanity-check the configured
             * clocks: the NPU is busy almost the whole loop, so this is a
             * good (slightly low) estimate of the real NPU clock.
             */
            uint64_t implied_npu_hz =
                (uint64_t)U85_HOST_CLOCK_HZ * npu_cycles / host_cycles;
            uint64_t configured_hz = (uint64_t)U85_NPU_CLOCK_HZ;
            uint64_t diff = (implied_npu_hz > configured_hz)
                                ? (implied_npu_hz - configured_hz)
                                : (configured_hz - implied_npu_hz);

            if ((diff * 100ULL) > (configured_hz * 20ULL))
            {
                printf("WARNING: PMU/DWT cycle ratio implies a %lu.%03lu MHz NPU clock,\n",
                       (unsigned long)(implied_npu_hz / 1000000ULL),
                       (unsigned long)((implied_npu_hz / 1000ULL) % 1000ULL));
                printf("         not the configured %lu MHz, so the configured-clock\n",
                       (unsigned long)(configured_hz / 1000000ULL));
                printf("         rate is not reported. Set U85_NPU_CLOCK_HZ and\n");
                printf("         U85_HOST_CLOCK_HZ to the real clocks.\n");
            }
            else
            {
                uint64_t npu_mops_x1000 =
                    ((uint64_t)total_ops * configured_hz * 1000ULL) /
                    (npu_cycles * 1000000ULL);

                printf("U85 throughput: %llu.%03llu Mops/s at %lu MHz NPU clock\n",
                       (unsigned long long)(npu_mops_x1000 / 1000ULL),
                       (unsigned long long)(npu_mops_x1000 % 1000ULL),
                       (unsigned long)(configured_hz / 1000000ULL));
            }
        }
    }
}

/* ------------------------------------------------------------------------- */
/* main                                                                      */
/* ------------------------------------------------------------------------- */
int main(void)
{
    const uint32_t total_ops = U85_DMA_OPS * U85_RUNS;
    uint32_t id;
    uint32_t runs_ok = 0U;
    uint32_t runs_failed = 0U;
    uint64_t npu_cycles = 0ULL;
    uint64_t host_cycles = 0ULL;
    uint32_t host_cycles_ok;
    uint32_t i;

    UartStdOutInit();

    printf("M85: MilliSoC Ethos-U85 control demo\n");
    printf("Shared window: 0x%08lx (%lu bytes)\n",
           (unsigned long)U85_SHARED_BASE,
           (unsigned long)U85_SHARED_SIZE);

    id = U85_Base->ID;
    printf("U85 ID: 0x%08lx (arch %lu.%lu, product %lu) %s\n",
           (unsigned long)id,
           (unsigned long)((id & U85_ID_ARCH_MAJOR_Msk) >> U85_ID_ARCH_MAJOR_Pos),
           (unsigned long)((id & U85_ID_ARCH_MINOR_Msk) >> U85_ID_ARCH_MINOR_Pos),
           (unsigned long)((id & U85_ID_PRODUCT_MAJOR_Msk) >> U85_ID_PRODUCT_MAJOR_Pos),
           ((id & U85_ID_ARCH_MAJOR_Msk) ==
            (U85_ID_ARCH_MAJOR_U85 << U85_ID_ARCH_MAJOR_Pos)) ? "PASS" : "FAIL");

    if ((id & U85_ID_ARCH_MAJOR_Msk) !=
        (U85_ID_ARCH_MAJOR_U85 << U85_ID_ARCH_MAJOR_Pos))
    {
        printf("U85 not responding, aborting\n");
        UartEndSimulation();
        return 0;
    }

    printf("U85 soft reset %s\n", u85_init() ? "PASS" : "FAIL");

    u85_pmu_init();

    u85_cmd_bytes = cmd_stream_build();
    fill_source();

    printf("Command stream: %lu bytes, %lu DMA ops x %lu bytes per run\n",
           (unsigned long)u85_cmd_bytes,
           (unsigned long)U85_DMA_OPS,
           (unsigned long)U85_COPY_BYTES);

    /* Warm-up run, not measured, also proves the stream works. */
    u85_clean_shared();
    __DSB();
    if (u85_run() != 1)
    {
        printf("U85 warm-up run FAIL\n");
        u85_dump_status();
        UartEndSimulation();
        return 0;
    }
    printf("U85 warm-up run PASS\n");

    host_cycles_ok = (uint32_t)host_cycles_init();

    for (i = 0U; i < U85_RUNS; ++i)
    {
        uint64_t before;
        uint64_t after;
        uint32_t host_before = DWT->CYCCNT;
        int result;

        u85_clean_shared();
        __DSB();

        before = u85_pmu_cycles();
        result = u85_run();
        after = u85_pmu_cycles();

        host_cycles += (uint32_t)(DWT->CYCCNT - host_before);

        if (result == 1)
        {
            npu_cycles += (after - before);
            ++runs_ok;
        }
        else
        {
            ++runs_failed;
        }
    }

    printf("U85 DMA runs: %lu/%lu PASS\n",
           (unsigned long)runs_ok,
           (unsigned long)U85_RUNS);

    if (runs_failed != 0U)
    {
        printf("U85 DMA runs: %lu/%lu FAIL\n",
               (unsigned long)runs_failed,
               (unsigned long)U85_RUNS);
        u85_dump_status();
    }

    u85_invalidate_shared();
    __DSB();
    printf("DMA data verify %s\n", verify_destination() ? "PASS" : "FAIL");

    if (!host_cycles_ok)
        host_cycles = 0ULL;

    report_throughput(npu_cycles, host_cycles, total_ops);

    UartEndSimulation();
    return 0;
}
