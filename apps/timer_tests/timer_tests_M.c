/*
 * milliSoC timer / dualtimer verification - Cortex-M85 side
 *
 * Runs the shared suite (timer_tests_common.h) against this core's TIMER0,
 * TIMER1 and dual timer, then holds off the end-of-simulation marker long
 * enough for the R5 suite to finish on the other UART.
 */

#include <stdint.h>
#include "exectb_mcu.h"

/* Time reference: Cortex-M85 DWT cycle counter. */
static int core_cycles_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0u;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0u;
}

static uint32_t core_cycles_read(void)
{
    return DWT->CYCCNT;
}

#include "timer_tests_common.h"

int main(void)
{
    UartStdOutInit();

    (void)tt_run_suite("M85");

    /* The R5 runs its own copy of the suite; give it time to print before
     * the harness stops on our end-of-simulation byte.  The nominal core
     * clock is used here only as a generous fixed delay. */
    printf("\nM85: waiting for the R5 suite to finish\n");
    tt_delay(TT_MS(8000));

    printf("M85: end of simulation\n");
    UartEndSimulation();

    return 0;
}
