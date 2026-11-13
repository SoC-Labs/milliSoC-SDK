#include <stdio.h>

#include "millisoc_platform_m.h"
#include "cold_boot_common.h"

static volatile uint32_t m85_payload[] = {
    0x4D383531u, 0xFEDCBA98u, 0x76543210u, 0x2468ACE1u,
    0x13579BDEu, 0x5A5AA5A5u, 0xD00DC001u, 0x1234EED5u
};
static volatile uint32_t m85_write_probe = 0x85C01DBEu;
static const char build_id[] = "COLD-M85-01";

static int m85_cycle_counter_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0u;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0u;
}

static uint32_t cold_boot_cycles(void)
{
    return DWT->CYCCNT;
}

int main(void)
{
    int cycles_ok;
    int m85_ok;
    int got_ack;
    uint32_t r5_status;
    int all_ok;

    UartStdOutInit();
    cycles_ok = m85_cycle_counter_init();
    printf("M85 app %s\n", build_id);

    m85_ok = cold_boot_image_ok((uintptr_t)&main, 0x00000000u, 0x10000u,
                                m85_payload, 8u,
                                0xEFCF28D4u, &m85_write_probe,
                                0x85C01DBEu, 0x20000000u, 0x10000u);
    m85_ok = m85_ok && cycles_ok;
    printf("M85 ITCM entry/DTCM data: %s\n", m85_ok ? "PASS" : "FAIL");
    if (!cycles_ok)
        printf("M85 cycle counter: FAIL\n");

    __DSB();
    MHUS_PBX->PDBCW[0].SET =
        COLD_BOOT_M85_READY | (m85_ok ? COLD_BOOT_M85_DATA_OK : 0u);

    got_ack = cold_boot_wait_bits(&MHUR_MBX->MDBCW[0].ST,
                                  COLD_BOOT_R5_ACK, cycles_ok);
    r5_status = got_ack ? MHUR_MBX->MDBCW[0].ST : 0u;
    if (got_ack)
        MHUR_MBX->MDBCW[0].CLR =
            r5_status & (COLD_BOOT_R5_ACK | COLD_BOOT_ALL_OK);

    all_ok = m85_ok && got_ack &&
             ((r5_status & COLD_BOOT_ALL_OK) != 0u);
    printf("Cold-boot checks: %s%s\n",
           all_ok ? "PASS" : "FAIL",
           got_ack ? "" : " (R5 timeout)");

    UartEndSimulation();
    return all_ok ? 0 : 1;
}
