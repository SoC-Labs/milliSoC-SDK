#include "CMSDK.h"
#include "mhu.h"
#include "sysctrl.h"
#include "sys_memory_map.h"
#include "uart_stdout.h"
#include <stdio.h>
#include <stdint.h>

#define MHU_DOORBELL_MASK  (1UL)
#define MHU_POLL_LIMIT     (10000UL)
#define WATCHDOG_LOAD      (0x00001000UL)
#define WATCHDOG_LOCK_KEY  (0x1ACCE551UL)
#define WATCHDOG_LOAD_REG  (0x00U)
#define WATCHDOG_CONTROL   (0x08U)
#define WATCHDOG_LOCK      (0xC00U)
#define WATCHDOG_RESET_EN  (1UL << 1)

static inline void data_sync_barrier(void)
{
    __asm volatile ("dsb sy" ::: "memory");
}

static inline void cpu_nop(void)
{
    __asm volatile ("nop");
}

static void send_m85_message(void)
{
    MHUS_PBX->PDBCW[0].SET = MHU_DOORBELL_MASK;
}

static int m85_message_pending(void)
{
    return (MHUR_MBX->DBCH_INT_ST[0] & MHU_DOORBELL_MASK) != 0U;
}

static void clear_m85_message(void)
{
    MHUR_MBX->MDBCW[0].CLR = MHU_DOORBELL_MASK;
    while (m85_message_pending())
    {
    }
}

static int wait_for_m85_message(void)
{
    uint32_t timeout = MHU_POLL_LIMIT;

    while (timeout-- != 0U)
    {
        if (m85_message_pending())
        {
            clear_m85_message();
            return 1;
        }
    }

    return 0;
}

static void fail(const char *message)
{
    printf("RESET CONTROL TEST FAIL: %s\n", message);
    UartEndSimulation();
    for (;;)
    {
    }
}

static void trigger_watchdog_reset(void)
{
    volatile uint32_t * const watchdog =
        (volatile uint32_t *)SYS_WATCHDOG_BASE;

    printf("Requesting watchdog system reset\n");
    watchdog[WATCHDOG_LOCK / sizeof(uint32_t)] = WATCHDOG_LOCK_KEY;
    watchdog[WATCHDOG_LOAD_REG / sizeof(uint32_t)] = WATCHDOG_LOAD;
    watchdog[WATCHDOG_CONTROL / sizeof(uint32_t)] = WATCHDOG_RESET_EN;
    data_sync_barrier();

    for (;;)
    {
        cpu_nop();
    }
}

int main(void)
{
    const uint32_t reset_cause = sysctrl->reset_info;

    UartStdOutInit();
    printf("R5 reset/control test: reset cause 0x%08lx\n",
           (unsigned long)reset_cause);

    if (reset_cause == 0U)
    {
        if (sysctrl->cpu_mx_wait != 0U)
            fail("M85 was not released by the bootloader");

        sysctrl->cpu_mx_wait = 1U;
        if (sysctrl->cpu_mx_wait != 1U)
            fail("CPUWAIT did not assert");
        sysctrl->cpu_mx_wait = 0U;
        if (sysctrl->cpu_mx_wait != 0U)
            fail("CPUWAIT did not deassert");

        if (!wait_for_m85_message())
            fail("timed out waiting for M85 startup");

        send_m85_message();
        if (!wait_for_m85_message())
            fail("M85 did not respond to an MHU doorbell");

        printf("SYS_CTRL CPUWAIT and MHU handshake PASS\n");

        sysctrl->cpu_mx_wait = 1U;
        data_sync_barrier();
        sysctrl->reset_command = SYSCTRL_RESET_COMMAND_M85;
        data_sync_barrier();
        if (sysctrl->cpu_mx_wait != 1U)
            fail("CPUWAIT did not remain asserted during M85 reset");
        if ((sysctrl->reset_info & SYSCTRL_RESET_CAUSE_SW_M85) == 0U)
            fail("M85 software-reset cause was not recorded");
        if (m85_message_pending())
            fail("M85 restarted while CPUWAIT was asserted");
        printf("M85 local reset and CPUWAIT hold PASS\n");

        sysctrl->reset_command = SYSCTRL_RESET_COMMAND_R5;
        data_sync_barrier();
        for (;;)
        {
            cpu_nop();
        }
    }

    if (reset_cause == SYSCTRL_RESET_CAUSE_SW_R5)
    {
        if (sysctrl->cpu_mx_wait != 0U)
            fail("bootloader did not release M85 after R5 reset");
        if (!wait_for_m85_message())
            fail("M85 did not start after R5 local reset");

        printf("R5 local reset PASS\n");
        sysctrl->reset_command = SYSCTRL_RESET_COMMAND_SYSTEM;
        data_sync_barrier();
        for (;;)
        {
            cpu_nop();
        }
    }

    if (reset_cause == SYSCTRL_RESET_CAUSE_SW_SYSTEM)
    {
        if (!wait_for_m85_message())
            fail("M85 did not start after software system reset");
        printf("Software system reset PASS\n");
        sysctrl->cpu_mx_wait = 1U;
        data_sync_barrier();
        trigger_watchdog_reset();
    }

    if (reset_cause == SYSCTRL_RESET_CAUSE_WATCHDOG)
    {
        if (!wait_for_m85_message())
            fail("M85 did not start after watchdog reset");
        printf("Watchdog system reset PASS\n");
        printf("RESET CONTROL TEST PASS\n");
        UartEndSimulation();
        for (;;)
        {
        }
    }

    fail("unexpected SYS_CTRL reset cause");
    return 1;
}
