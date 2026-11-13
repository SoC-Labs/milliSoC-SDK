#include <stdio.h>

#include "millisoc_platform_r.h"
#include "cold_boot_common.h"

static volatile uint32_t r5_payload[] = {
    0x52354231u, 0x01234567u, 0x89ABCDEFu, 0x13579BDFu,
    0x2468ACE0u, 0xA5A55A5Au, 0xC001D00Du, 0x5EED1234u
};
static volatile uint32_t r5_write_probe = 0x5A17C0DEu;
static const char build_id[] = "COLD-R5-01";

static int r5_cycle_counter_init(void)
{
    uint32_t pmcr;

    __asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    pmcr = (pmcr | 0x7u) & ~0x8u;
    __asm volatile("mcr p15, 0, %0, c9, c12, 0" :: "r"(pmcr));
    __asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x80000000u));
    __asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));

    return (pmcr & 1u) != 0u;
}

static uint32_t cold_boot_cycles(void)
{
    uint32_t value;
    __asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(value));
    return value;
}

int main(void)
{
    int cycles_ok = r5_cycle_counter_init();
    int r5_ok;
    uint32_t m85_status;
    int m85_ok;
    int all_ok;

    UartStdOutInit();
    printf("R5 app %s\n", build_id);

    r5_ok = cold_boot_image_ok((uintptr_t)&main, 0x00000000u, 0x10000u,
                               r5_payload, 8u,
                               0x2DB28124u, &r5_write_probe,
                               0x5A17C0DEu, 0x20000000u, 0x8000u);
    r5_ok = r5_ok && cycles_ok;
    printf("R5 ATCM entry/BTCM data: %s\n", r5_ok ? "PASS" : "FAIL");
    if (!cycles_ok)
        printf("R5 cycle counter: FAIL\n");

    if (!cold_boot_wait_bits(&MHUR_MBX->MDBCW[0].ST,
                             COLD_BOOT_M85_READY, cycles_ok)) {
        printf("M85 startup handshake: FAIL (timeout)\n");
        for (;;)
            __asm volatile("wfi");
    }

    m85_status = MHUR_MBX->MDBCW[0].ST;
    m85_ok = (m85_status & COLD_BOOT_M85_DATA_OK) != 0u;
    MHUR_MBX->MDBCW[0].CLR =
        m85_status & (COLD_BOOT_M85_READY | COLD_BOOT_M85_DATA_OK);

    all_ok = r5_ok && m85_ok;
    printf("M85 ITCM/image data: %s\n", m85_ok ? "PASS" : "FAIL");
    printf("Cold-boot checks: %s\n", all_ok ? "PASS" : "FAIL");

    __asm volatile("dsb sy" ::: "memory");
    MHUS_PBX->PDBCW[0].SET =
        COLD_BOOT_R5_ACK | (all_ok ? COLD_BOOT_ALL_OK : 0u);

    for (;;)
        __asm volatile("wfi");
}
