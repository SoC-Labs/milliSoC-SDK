/*
 * milliSoC Ethos-U85 Vela model demo (Cortex-M85 side)
 *
 * Runs the Vela-compiled hello_world_int8 model on the Ethos-U85.
 *
 * The model image (command stream + weights + a small header, produced by
 * tools/model_to_image.py) is stored in QSPI flash at MODEL_FLASH_BASE.  The
 * M85 reads it through the XiP window at FLASH_BASE and either:
 *
 *   MODEL_RUN_FROM_FLASH=0 (default)
 *       Copies the image into the 0x30000000 SRAM and points the NPU at the
 *       copy.  This works on the current FPGA, where the Ethos-U cannot reach
 *       the flash window.
 *
 *   MODEL_RUN_FROM_FLASH=1
 *       Leaves the image in flash and lets the NPU fetch the command stream
 *       and weights through its EXT port.  This needs the RTL change that
 *       gives the Ethos-U access to the QSPI flash window.
 *
 * In both cases the feature maps, scratch and I/O live in the SRAM at
 * MODEL_SRAM_BASE (0x30000000) that the NPU can access directly.
 *
 * The model is the TensorFlow Lite Micro hello_world_int8 sine benchmark.  The
 * test vectors and expected outputs were generated with the TFLite reference
 * kernel; the comparison tolerance is 2 LSB to allow for the NPU's fixed-point
 * rounding.
 */

#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include <stdint.h>
#include "ethos_u85.h"
#include "sys_memory_map.h"

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */

#ifndef MODEL_FLASH_BASE
#define MODEL_FLASH_BASE        0x00008000UL
#endif

#ifndef MODEL_RUN_FROM_FLASH
#define MODEL_RUN_FROM_FLASH    0
#endif

/* SRAM that both the M85 and the U85 can reach. */
#ifndef MODEL_SRAM_BASE
#define MODEL_SRAM_BASE         0x30000000UL
#endif

/* Model image copy (MODEL_RUN_FROM_FLASH == 0) and arena offsets. */
#ifndef MODEL_COPY_OFFSET
#define MODEL_COPY_OFFSET       0x0000UL
#endif
#ifndef MODEL_ARENA_OFFSET
#define MODEL_ARENA_OFFSET      0x2000UL
#endif
#ifndef MODEL_ARENA_SIZE
#define MODEL_ARENA_SIZE        0x1000UL
#endif

#define U85_POLL_TIMEOUT        1000000UL

/* ------------------------------------------------------------------------- */
/* Model image header (must match tools/model_to_image.py)                    */
/* ------------------------------------------------------------------------- */

#define MODEL_IMAGE_MAGIC       0x314C564DUL    /* "MVL1" little-endian */
#define MODEL_IMAGE_VERSION     1UL

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t image_size;
    uint32_t cmd_offset;
    uint32_t cmd_size;
    uint32_t weight_offset;
    uint32_t weight_size;
    uint32_t weight_region;
    uint32_t scratch_region;
    uint32_t scratch_size;
    uint32_t scratch_fast_region;
    uint32_t scratch_fast_size;
    uint32_t input_region;
    uint32_t input_offset;
    uint32_t input_size;
    uint32_t output_region;
    uint32_t output_offset;
    uint32_t output_size;
    uint32_t arena_size;
    uint32_t crc32;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t reserved2;
} model_image_header_t;

/* C89/C99-compatible compile-time size check. */
typedef char model_image_header_size_check[
    (sizeof(model_image_header_t) == 96U) ? 1 : -1];

/* ------------------------------------------------------------------------- */
/* Test vectors (int8 quantised sine)                                        */
/* ------------------------------------------------------------------------- */

static const int8_t model_test_input[] = {
    -128, -96, -64, -32, 0, 32, 64, 96, 127
};
static const int8_t model_test_expected[] = {
    4, 89, 126, 90, 4, -74, -126, -88, -9
};
#define MODEL_TEST_COUNT \
    (sizeof(model_test_input) / sizeof(model_test_input[0]))
#define MODEL_TEST_TOLERANCE 2

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

static int abs_diff_i8(int8_t a, int8_t b)
{
    int diff = (int)a - (int)b;

    return (diff < 0) ? -diff : diff;
}

static uint32_t crc32_bytes(uint32_t crc, const uint8_t *data, uint32_t len)
{
    uint32_t i;
    uint32_t bit;

    for (i = 0U; i < len; ++i)
    {
        crc ^= (uint32_t)data[i];
        for (bit = 0U; bit < 8U; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320UL & (0UL - (crc & 1UL)));
    }

    return crc;
}

#if !MODEL_RUN_FROM_FLASH
static void copy_bytes(uint8_t *dst, const uint8_t *src, uint32_t len)
{
    uint32_t i;

    for (i = 0U; i < len; ++i)
        dst[i] = src[i];
}
#endif

/* Cache maintenance for the SRAM buffers shared with the NPU. */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
#define MODEL_CACHE_LINE 32U

static void cache_clean(const void *addr, uint32_t size)
{
    uint32_t rounded = (size + MODEL_CACHE_LINE - 1U) & ~(MODEL_CACHE_LINE - 1U);

    SCB_CleanDCache_by_Addr((volatile void *)(uintptr_t)addr, (int32_t)rounded);
}

static void cache_invalidate(const void *addr, uint32_t size)
{
    uint32_t rounded = (size + MODEL_CACHE_LINE - 1U) & ~(MODEL_CACHE_LINE - 1U);

    SCB_InvalidateDCache_by_Addr((volatile void *)(uintptr_t)addr,
                                 (int32_t)rounded);
}
#else
static void cache_clean(const void *addr, uint32_t size)
{
    (void)addr;
    (void)size;
}

static void cache_invalidate(const void *addr, uint32_t size)
{
    (void)addr;
    (void)size;
}
#endif

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

/*
 * QCONFIG selects the MEM_ATTR used to fetch the command stream; REGIONCFG
 * selects the MEM_ATTR for each BASEP_ARRAY region (2 bits per region).
 * MEM_ATTR0 is the SRAM port, MEM_ATTR1 is the EXT/flash port.
 */
static int u85_init(uint32_t qconfig, uint32_t regioncfg,
                    uint32_t weight_region, uint64_t weight_base,
                    uint32_t arena_region, uint64_t arena_base,
                    uint32_t scratch_region,
                    uint32_t fast_region, uint32_t fast_size,
                    uint64_t fast_base)
{
    U85_Base->CMD = U85_CMD_CE_Msk;

    U85_Base->RESET = 0U;
    if (!u85_wait_reset_done())
        return 0;

    U85_Base->CMD = U85_CMD_CE_Msk;

    U85_Base->MEM_ATTR0 = U85_MEM_ATTR_SRAM;
    U85_Base->MEM_ATTR1 = U85_MEM_ATTR_EXT;
    U85_Base->MEM_ATTR2 = U85_MEM_ATTR_EXT;
    U85_Base->MEM_ATTR3 = U85_MEM_ATTR_EXT;
    U85_Base->QCONFIG = qconfig;
    U85_Base->REGIONCFG = regioncfg;

    U85_Base->AXI_SRAM = U85_AXI_SRAM_DEFAULT;
    U85_Base->AXI_EXT = U85_AXI_EXT_DEFAULT;

    if (weight_region >= 8U || arena_region >= 8U || scratch_region >= 8U ||
        (fast_size != 0U && fast_region >= 8U))
        return 0;

    /*
     * A non-empty fast-scratch buffer needs its own region.  The current Vela
     * plan has no fast scratch (spilling is disabled), so this only guards
     * against a future plan that would alias the weight or arena base.
     */
    if (fast_size != 0U &&
        (fast_region == weight_region || fast_region == arena_region ||
         fast_region == scratch_region))
        return 0;

    u85_write64(&U85_Base_Pointers->BASEP_ARRAY[weight_region], weight_base);
    u85_write64(&U85_Base_Pointers->BASEP_ARRAY[arena_region], arena_base);
    /* The scratch shares the arena buffer unless Vela put it in its own region. */
    if (scratch_region != weight_region)
        u85_write64(&U85_Base_Pointers->BASEP_ARRAY[scratch_region], arena_base);
    if (fast_size != 0U)
        u85_write64(&U85_Base_Pointers->BASEP_ARRAY[fast_region], fast_base);

    return 1;
}

static void u85_dump_status(void)
{
    printf("  STATUS=0x%08lx QREAD=0x%08lx QSIZE=0x%08lx\n",
           (unsigned long)U85_Base->Status,
           (unsigned long)U85_Base->QREAD,
           (unsigned long)U85_Base->QSIZE);
}

/* 1 = completed, 0 = timeout, -1 = NPU fault. */
static int u85_run(uint64_t qbase, uint32_t qsize)
{
    uint32_t timeout;

    u85_write64(&U85_Base->QBASE, qbase);
    U85_Base->QSIZE = qsize;

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
/* main                                                                      */
/* ------------------------------------------------------------------------- */

int main(void)
{
    const uint8_t *flash_image;
    const model_image_header_t *hdr;
    const uint8_t *cmd;
    const uint8_t *weights;
#if !MODEL_RUN_FROM_FLASH
    uint8_t *sram_image;
#endif
    uint8_t *arena;
    volatile int8_t *input;
    volatile int8_t *output;
    uint64_t qbase;
    uint64_t weight_base;
    uint64_t arena_base;
    uint64_t fast_base;
    uint32_t qconfig;
    uint32_t regioncfg;
    uint32_t crc;
    uint32_t id;
    uint32_t i;
    uint32_t pass = 0U;
    uint32_t fail = 0U;

    UartStdOutInit();

    printf("M85: MilliSoC Ethos-U85 Vela model demo\n");

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

    /*
     * The QSPI controller registers at 0x44010000 are not reachable from the
     * M85's AXI port: a read there wedges the core's bus and even the
     * debugger cannot halt it.  The R5 bootloader has already run
     * SPI_STARTUP() and enabled XiP, so the M85 only uses the memory-mapped
     * flash window at FLASH_BASE.
     */
    flash_image = (const uint8_t *)(uintptr_t)(FLASH_BASE + MODEL_FLASH_BASE);
    hdr = (const model_image_header_t *)flash_image;

    printf("Model flash: 0x%08lx\n",
           (unsigned long)(FLASH_BASE + MODEL_FLASH_BASE));

    if (hdr->magic != MODEL_IMAGE_MAGIC ||
        hdr->version != MODEL_IMAGE_VERSION ||
        hdr->header_size != (uint32_t)sizeof(model_image_header_t) ||
        hdr->image_size < (uint32_t)sizeof(model_image_header_t) ||
        (hdr->cmd_offset + hdr->cmd_size) > hdr->image_size ||
        (hdr->weight_offset + hdr->weight_size) > hdr->image_size)
    {
        printf("Model header FAIL (magic 0x%08lx version %lu size %lu)\n",
               (unsigned long)hdr->magic,
               (unsigned long)hdr->version,
               (unsigned long)hdr->image_size);
        UartEndSimulation();
        return 0;
    }

    printf("Model image: %lu bytes, cmd %lu @ +0x%lx, weights %lu @ +0x%lx\n",
           (unsigned long)hdr->image_size,
           (unsigned long)hdr->cmd_size,
           (unsigned long)hdr->cmd_offset,
           (unsigned long)hdr->weight_size,
           (unsigned long)hdr->weight_offset);

    crc = 0xFFFFFFFFUL;
    crc = crc32_bytes(crc, flash_image + hdr->cmd_offset, hdr->cmd_size);
    crc = crc32_bytes(crc, flash_image + hdr->weight_offset, hdr->weight_size);
    crc ^= 0xFFFFFFFFUL;

    printf("Model CRC32: 0x%08lx %s\n",
           (unsigned long)crc,
           (crc == hdr->crc32) ? "PASS" : "FAIL");

    if (crc != hdr->crc32)
    {
        UartEndSimulation();
        return 0;
    }

    arena = (uint8_t *)(uintptr_t)(MODEL_SRAM_BASE + MODEL_ARENA_OFFSET);
    arena_base = (uint64_t)(uintptr_t)arena;

    if (hdr->arena_size > MODEL_ARENA_SIZE)
    {
        printf("Model arena FAIL (%lu bytes > %lu)\n",
               (unsigned long)hdr->arena_size,
               (unsigned long)MODEL_ARENA_SIZE);
        UartEndSimulation();
        return 0;
    }

    if ((hdr->image_size + MODEL_COPY_OFFSET) > MODEL_ARENA_OFFSET)
    {
        printf("Model image FAIL (%lu bytes does not fit below 0x%lx)\n",
               (unsigned long)hdr->image_size,
               (unsigned long)MODEL_ARENA_OFFSET);
        UartEndSimulation();
        return 0;
    }

#if MODEL_RUN_FROM_FLASH
    printf("Model source: flash via NPU EXT port\n");
    cmd = flash_image + hdr->cmd_offset;
    weights = flash_image + hdr->weight_offset;
    qbase = (uint64_t)(uintptr_t)cmd;
    weight_base = (uint64_t)(uintptr_t)weights;
    qconfig = 1U;                                   /* MEM_ATTR1 = EXT */
    regioncfg = (uint32_t)1U << (2U * hdr->weight_region);
#else
    sram_image = (uint8_t *)(uintptr_t)(MODEL_SRAM_BASE + MODEL_COPY_OFFSET);
    printf("Model source: SRAM copy at 0x%08lx\n",
           (unsigned long)(uintptr_t)sram_image);
    copy_bytes(sram_image, flash_image, hdr->image_size);
    __DSB();
    cache_clean(sram_image, hdr->image_size);
    cmd = sram_image + hdr->cmd_offset;
    weights = sram_image + hdr->weight_offset;
    qbase = (uint64_t)(uintptr_t)cmd;
    weight_base = (uint64_t)(uintptr_t)weights;
    qconfig = 0U;                                   /* MEM_ATTR0 = SRAM */
    regioncfg = 0U;
#endif

    fast_base = (uint64_t)(uintptr_t)(arena + MODEL_ARENA_SIZE);

    printf("Regions: weights=%lu arena=%lu fast=%lu, arena 0x%08lx (%lu bytes)\n",
           (unsigned long)hdr->weight_region,
           (unsigned long)hdr->input_region,
           (unsigned long)hdr->scratch_fast_region,
           (unsigned long)(uintptr_t)arena,
           (unsigned long)MODEL_ARENA_SIZE);

    input = (volatile int8_t *)(arena + hdr->input_offset);
    output = (volatile int8_t *)(arena + hdr->output_offset);

    if (!u85_init(qconfig, regioncfg,
                  hdr->weight_region, weight_base,
                  hdr->input_region, arena_base,
                  hdr->scratch_region,
                  hdr->scratch_fast_region, hdr->scratch_fast_size,
                  fast_base))
    {
        printf("U85 soft reset FAIL\n");
        UartEndSimulation();
        return 0;
    }
    printf("U85 soft reset PASS\n");

    printf("U85 inference:\n");
    for (i = 0U; i < MODEL_TEST_COUNT; ++i)
    {
        int result;
        int8_t got;

        input[0] = model_test_input[i];
        __DSB();
        cache_clean(arena, MODEL_ARENA_SIZE);

        result = u85_run(qbase, hdr->cmd_size);

        __DSB();
        cache_invalidate(arena, MODEL_ARENA_SIZE);

        if (result != 1)
        {
            printf("  [%lu] input %4d -> NPU %s FAIL\n",
                   (unsigned long)i,
                   (int)model_test_input[i],
                   (result == -1) ? "FAULT" : "TIMEOUT");
            ++fail;
            continue;
        }

        got = output[0];
        if (abs_diff_i8(got, model_test_expected[i]) <= MODEL_TEST_TOLERANCE)
        {
            printf("  [%lu] input %4d -> out %4d (expected %4d) PASS\n",
                   (unsigned long)i,
                   (int)model_test_input[i],
                   (int)got,
                   (int)model_test_expected[i]);
            ++pass;
        }
        else
        {
            printf("  [%lu] input %4d -> out %4d (expected %4d) FAIL\n",
                   (unsigned long)i,
                   (int)model_test_input[i],
                   (int)got,
                   (int)model_test_expected[i]);
            ++fail;
        }
    }

    printf("U85 inference: %lu/%lu PASS\n",
           (unsigned long)pass,
           (unsigned long)MODEL_TEST_COUNT);

    if (fail != 0U)
    {
        printf("U85 inference: %lu/%lu FAIL\n",
               (unsigned long)fail,
               (unsigned long)MODEL_TEST_COUNT);
        u85_dump_status();
        printf("Model run FAIL\n");
    }
    else
    {
        printf("Model run PASS\n");
    }

    UartEndSimulation();
    return 0;
}
