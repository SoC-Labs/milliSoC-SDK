/*
 * milliSoC MHU-320AE verification - Cortex-R5 side
 *
 * Runs the shared lockstep suite (mhu_tests_common.h) against both MHUs and
 * takes the R5 role in every step.  The bootloader leaves SCTLR.VE set, so
 * the VIC vectors interrupts directly to the handlers registered here; the
 * R5 image itself has no vector table.
 */

#include <stdint.h>
#include "millisoc_platform_r.h"
#include "vic.h"

/* irq_map.h spells the mailbox IRQ as "09", which is not a valid octal
 * constant, so use the numeric values directly. */
#define MHUT_IRQ_REC_FST   6u
#define MHUT_IRQ_REC_MBX   9u

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

#define MHUT_CORE_IS_M 0
#include "mhu_tests_common.h"

/* ------------------------------------------------------------------------- */
/* Vectored interrupt handlers                                               */
/* ------------------------------------------------------------------------- */
void __attribute__((naked)) mhut_r5_db_irq(void)
{
    __asm volatile(
        "stmdb sp!, {r0-r3,r12,lr}\n"
        "bl    mhut_db_irq_body\n"
        "ldmia sp!, {r0-r3,r12,lr}\n"
        "subs  pc, lr, #4\n"
    );
}

void __attribute__((naked)) mhut_r5_fc_irq(void)
{
    __asm volatile(
        "stmdb sp!, {r0-r3,r12,lr}\n"
        "bl    mhut_fc_irq_body\n"
        "ldmia sp!, {r0-r3,r12,lr}\n"
        "subs  pc, lr, #4\n"
    );
}

/* ------------------------------------------------------------------------- */
/* Interrupt controller hooks                                                */
/* ------------------------------------------------------------------------- */
static void mhut_irq_init(void)
{
    uint32_t mask = (1u << MHUT_IRQ_REC_MBX) | (1u << MHUT_IRQ_REC_FST);

    /* The bootloader leaves its DMA interrupt enabled; start from a clean
     * slate so only the two MHU receiver sources can vector. */
    VIC->IntEnClear = 0xFFFFFFFFu;
    VIC_VA->VectAddr[MHUT_IRQ_REC_MBX] = (uint32_t)mhut_r5_db_irq;
    VIC_VA->VectAddr[MHUT_IRQ_REC_FST] = (uint32_t)mhut_r5_fc_irq;
    VIC_VP->VectPriority[MHUT_IRQ_REC_MBX] = 0u;
    VIC_VP->VectPriority[MHUT_IRQ_REC_FST] = 0u;
    VIC->IntEnable = mask;
}

static void mhut_irq_mask(int masked)
{
    if (masked) {
        __asm volatile("cpsid i" ::: "memory");
    } else {
        __asm volatile("cpsie i" ::: "memory");
    }
}

static void mhut_irq_ack(void)
{
    VIC_AD->VICAddress = 0u;
}

int main(void)
{
    UartStdOutInit();

    mhut_run_suite("R5");

    printf("R5: end of simulation\n");
    UartEndSimulation();

    return 0;
}
