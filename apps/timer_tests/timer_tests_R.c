/*
 * milliSoC timer / dualtimer verification - Cortex-R5 side
 *
 * Runs the shared suite against this core's own TIMER0, TIMER1 and dual
 * timer.  The R5 image has no vector table, so the suite only polls the
 * raw/masked interrupt status registers.
 */

#include <stdint.h>

/* Time reference: Cortex-R5 PMU cycle counter (PMCCNTR). */
static int core_cycles_init(void)
{
    uint32_t pmcr;

    __asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));
    /* E (enable), P (count every cycle), C (reset cycle counter). */
    pmcr |= 0x7u;
    /* D (cycle counter divide by 64) off. */
    pmcr &= ~0x8u;
    __asm volatile("mcr p15, 0, %0, c9, c12, 0" :: "r"(pmcr));
    /* PMCNTENSET: enable the cycle counter. */
    __asm volatile("mcr p15, 0, %0, c9, c12, 1" :: "r"(0x80000000u));
    __asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(pmcr));

    return (pmcr & 0x1u) != 0u;
}

static uint32_t core_cycles_read(void)
{
    uint32_t value;

    __asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(value));

    return value;
}

#include "timer_tests_common.h"

int main(void)
{
    UartStdOutInit();

    (void)tt_run_suite("R5");

    UartEndSimulation();

    return 0;
}
