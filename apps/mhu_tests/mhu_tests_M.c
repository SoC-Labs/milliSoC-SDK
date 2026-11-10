/*
 * milliSoC MHU-320AE verification - Cortex-M85 side
 *
 * Runs the shared lockstep suite (mhu_tests_common.h) against both MHUs and
 * takes the M85 role in every step.  The R5 runs the mirror image of the same
 * suite on its own UART.
 */

#include <stdint.h>
#include "millisoc_platform_m.h"

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

#define MHUT_CORE_IS_M 1
#include "mhu_tests_common.h"

/* ------------------------------------------------------------------------- */
/* Receiver interrupt handlers                                               */
/* ------------------------------------------------------------------------- */
void IPC_REC_MBXHandler(void)
{
    mhut_db_irq_body();
}

void IPC_REC_FSTHandler(void)
{
    mhut_fc_irq_body();
}

/* Not enabled by this app; recorded so a spurious interrupt is visible. */
void IPC_SND_ERRHandler(void)
{
    mhut_unexpected_irq = 1u;
}

void IPC_SND_FLTHandler(void)
{
    mhut_unexpected_irq = 2u;
}

void IPC_SND_PBXHandler(void)
{
    mhut_unexpected_irq = 3u;
}

void IPC_REC_ERRHandler(void)
{
    mhut_unexpected_irq = 4u;
}

void IPC_REC_FLTHandler(void)
{
    mhut_unexpected_irq = 5u;
}

/* ------------------------------------------------------------------------- */
/* Interrupt controller hooks                                                */
/* ------------------------------------------------------------------------- */
static void mhut_irq_init(void)
{
    NVIC_DisableIRQ(IPC_REC_MBX_IRQn);
    NVIC_DisableIRQ(IPC_REC_FST_IRQn);
    NVIC_ClearPendingIRQ(IPC_REC_MBX_IRQn);
    NVIC_ClearPendingIRQ(IPC_REC_FST_IRQn);
    NVIC_SetPriority(IPC_REC_MBX_IRQn, 1u);
    NVIC_SetPriority(IPC_REC_FST_IRQn, 1u);
    NVIC_EnableIRQ(IPC_REC_MBX_IRQn);
    NVIC_EnableIRQ(IPC_REC_FST_IRQn);
}

static void mhut_irq_mask(int masked)
{
    if (masked) {
        __disable_irq();
    } else {
        __enable_irq();
    }
}

static void mhut_irq_ack(void)
{
    /* The NVIC deasserts the pending bit once the MHU source is cleared. */
}

int main(void)
{
    UartStdOutInit();

    mhut_run_suite("M85");

    /* The R5 runs its own copy of the suite; give it time to print before
     * the harness stops on our end-of-simulation byte. */
    printf("\nM85: waiting for the R5 suite to finish\n");
    mh_delay(MHUT_MS(8000));

    printf("M85: end of simulation\n");
    UartEndSimulation();

    return 0;
}
