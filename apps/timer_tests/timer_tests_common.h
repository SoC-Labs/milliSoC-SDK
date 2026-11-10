/*
 * Shared timer / dualtimer verification for the milliSoC M85 and R5.
 *
 * The including file must define these two hooks *before* including this
 * header:
 *
 *   static int      core_cycles_init(void);   // 1 on success
 *   static uint32_t core_cycles_read(void);   // free-running core cycles
 *
 * They are only used to put a real time base on the polling loops and to
 * measure the timer clock against the core clock.  All functional checks
 * still run (with iteration-count timeouts) if no cycle counter exists.
 *
 * Each core owns its own timer blocks, so both images can run this file
 * concurrently.
 */

#ifndef TIMER_TESTS_COMMON_H
#define TIMER_TESTS_COMMON_H

#include <stdint.h>
#include <stdio.h>

/* Core hooks, provided by the including file. */
static int      core_cycles_init(void);
static uint32_t core_cycles_read(void);

/* Nominal core clock, only used to turn cycle counts into timeouts.  The
 * actual timer clock is measured, not assumed. */
#ifndef TIMER_TEST_CORE_HZ
#define TIMER_TEST_CORE_HZ   10000000u
#endif

#define TT_CYCLES_PER_MS     (TIMER_TEST_CORE_HZ / 1000u)
#define TT_MS(ms)            ((uint32_t)(ms) * TT_CYCLES_PER_MS)

#define TT_CID_CMSDK         0xB105F00Du

/* Set to 1 to dump the peripheral ID register blocks (debug aid). */
#ifndef TIMER_TESTS_DUMP_IDS
#define TIMER_TESTS_DUMP_IDS 0
#endif

/* ------------------------------------------------------------------------- */
/* Reporting                                                                 */
/* ------------------------------------------------------------------------- */
static unsigned tt_n_pass;
static unsigned tt_n_fail;
static unsigned tt_n_skip;
static unsigned tt_b_pass;
static unsigned tt_b_fail;
static unsigned tt_b_skip;
static int      tt_cycles_ok;

static void tt_block_begin(const char *name)
{
    tt_b_pass = 0u;
    tt_b_fail = 0u;
    tt_b_skip = 0u;
    printf("\n%s:\n", name);
}

static void tt_block_end(const char *name)
{
    printf("%s: %u checks, %u passed, %u failed, %u skipped\n",
           name, tt_b_pass + tt_b_fail + tt_b_skip,
           tt_b_pass, tt_b_fail, tt_b_skip);
}

static void tt_check(const char *name, int ok)
{
    if (ok) {
        ++tt_n_pass;
        ++tt_b_pass;
        printf("  %s: PASS\n", name);
    } else {
        ++tt_n_fail;
        ++tt_b_fail;
        printf("  %s: FAIL\n", name);
    }
}

static void tt_check_u32(const char *name, uint32_t got, uint32_t want)
{
    if (got == want) {
        ++tt_n_pass;
        ++tt_b_pass;
        printf("  %s: PASS (0x%08X)\n", name, got);
    } else {
        ++tt_n_fail;
        ++tt_b_fail;
        printf("  %s: FAIL (got 0x%08X, want 0x%08X)\n", name, got, want);
    }
}

static void tt_skip_check(const char *name)
{
    ++tt_n_skip;
    ++tt_b_skip;
    printf("  %s: SKIP\n", name);
}

/* ------------------------------------------------------------------------- */
/* Timing helpers                                                            */
/* ------------------------------------------------------------------------- */
static void tt_delay(uint32_t cycles)
{
    uint32_t start;
    volatile uint32_t spins = 0u;

    if (tt_cycles_ok) {
        start = core_cycles_read();
        while ((uint32_t)(core_cycles_read() - start) < cycles) {
            /* spin */
        }
    } else {
        while (spins < cycles) {
            ++spins;
        }
    }
}

static int tt_wait_mask(const volatile uint32_t *reg, uint32_t mask,
                        uint32_t timeout)
{
    uint32_t start;
    uint32_t now;
    volatile uint32_t spins = 0u;

    start = core_cycles_read();

    for (;;) {
        if ((*reg & mask) != 0u) {
            return 1;
        }
        if (tt_cycles_ok) {
            now = core_cycles_read();
            if ((uint32_t)(now - start) > timeout) {
                return 0;
            }
        } else if (++spins > timeout) {
            return 0;
        }
    }
}

static uint32_t tt_id32(uint32_t pid0, uint32_t pid1, uint32_t pid2,
                        uint32_t pid3)
{
    return (pid0 & 0xFFu) | ((pid1 & 0xFFu) << 8) | ((pid2 & 0xFFu) << 16) |
           ((pid3 & 0xFFu) << 24);
}

static void tt_report_rate(const char *what, uint32_t period_cycles,
                           uint32_t ticks)
{
    uint32_t milli;

    if (!tt_cycles_ok) {
        return;
    }
    milli = (uint32_t)(((uint64_t)period_cycles * 1000u) / (uint64_t)ticks);
    printf("  %s: %u.%03u core cycles per timer tick (load %u)\n",
           what, milli / 1000u, milli % 1000u, ticks);
}

static int tt_close(uint32_t a, uint32_t b, uint32_t percent)
{
    uint32_t diff = (a > b) ? (a - b) : (b - a);

    if (a == 0u) {
        return 0;
    }
    return (diff * 100u) <= (a * percent);
}

#if TIMER_TESTS_DUMP_IDS
static void tt_dump(const char *name, uint32_t base, uint32_t first,
                    uint32_t count)
{
    uint32_t i;

    printf("  %s[0x%03X]:", name, first);
    for (i = 0u; i < count; ++i) {
        printf(" %08X",
               *(volatile const uint32_t *)(base + first + (i * 4u)));
    }
    printf("\n");
}
#endif

/* ------------------------------------------------------------------------- */
/* CMSDK single timer                                                        */
/* ------------------------------------------------------------------------- */
static uint32_t tt_measure_cmsdk_period(CMSDK_TIMER_TypeDef *t, uint32_t reload,
                                        int *ok)
{
    uint32_t start;
    uint32_t end;

    t->CTRL = 0u;
    t->RELOAD = reload;
    t->VALUE = reload;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;

    start = core_cycles_read();
    t->CTRL = CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk;
    *ok = tt_wait_mask(&t->INTSTATUS, CMSDK_TIMER_INTSTATUS_Msk, TT_MS(500));
    end = core_cycles_read();

    t->CTRL = 0u;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;

    return end - start;
}

static void tt_test_cmsdk_timer(const char *name, CMSDK_TIMER_TypeDef *t)
{
    uint32_t pid;
    uint32_t cid;
    uint32_t value;
    uint32_t p1;
    uint32_t p2;
    int ok;

    tt_block_begin(name);

    pid = tt_id32(t->PID0, t->PID1, t->PID2, t->PID3);
    cid = tt_id32(t->CID0, t->CID1, t->CID2, t->CID3);
    printf("  %s: PID 0x%08X CID 0x%08X\n", name, pid, cid);
    tt_check("component ID", cid == TT_CID_CMSDK);

    /* Known state. */
    t->CTRL = 0u;
    t->RELOAD = 0u;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;

    /* Register readback. */
    t->RELOAD = 0x12345678u;
    tt_check_u32("RELOAD readback", t->RELOAD, 0x12345678u);
    t->RELOAD = 0u;

    t->CTRL = CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk;
    value = t->CTRL;
    t->CTRL = 0u;
    tt_check("CTRL EN/IRQEN readback",
             (value & (CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk)) ==
             (CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk));

    /* Down counter: load 100000, still counting after ~1 ms. */
    t->RELOAD = 0u;
    t->VALUE = 100000u;
    t->CTRL = CMSDK_TIMER_CTRL_EN_Msk;
    tt_delay(TT_MS(1));
    value = t->VALUE;
    printf("  %s: VALUE after ~1 ms: %u (loaded 100000)\n", name, value);
    tt_check("counts down from loaded value", (value < 100000u) && (value > 0u));
    t->CTRL = 0u;

    /* One-shot (RELOAD == 0) stops at zero and raises INTSTATUS. */
    t->RELOAD = 0u;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;
    t->VALUE = 100u;
    t->CTRL = CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk;
    ok = tt_wait_mask(&t->INTSTATUS, CMSDK_TIMER_INTSTATUS_Msk, TT_MS(50));
    tt_check("one-shot raises INTSTATUS", ok);
    tt_delay(TT_MS(1));
    value = t->VALUE;
    tt_check_u32("one-shot stops at zero", value, 0u);
    t->CTRL = 0u;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;

    /* Periodic reload: a second interrupt arrives without rewriting VALUE. */
    t->RELOAD = 1000u;
    t->VALUE = 1000u;
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;
    t->CTRL = CMSDK_TIMER_CTRL_EN_Msk | CMSDK_TIMER_CTRL_IRQEN_Msk;
    ok = tt_wait_mask(&t->INTSTATUS, CMSDK_TIMER_INTSTATUS_Msk, TT_MS(50));
    tt_check("periodic first interrupt", ok);
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;
    ok = tt_wait_mask(&t->INTSTATUS, CMSDK_TIMER_INTSTATUS_Msk, TT_MS(50));
    tt_check("periodic reload (second interrupt)", ok);
    t->INTCLEAR = CMSDK_TIMER_INTSTATUS_Msk;
    tt_check("INTCLEAR clears INTSTATUS",
             (t->INTSTATUS & CMSDK_TIMER_INTSTATUS_Msk) == 0u);
    t->CTRL = 0u;

    /* Clock rate: two consecutive periods must agree. */
    if (tt_cycles_ok) {
        p1 = tt_measure_cmsdk_period(t, 10000u, &ok);
        if (!ok) {
            tt_check("period measurement completed", 0);
        } else {
            p2 = tt_measure_cmsdk_period(t, 10000u, &ok);
            tt_check("period measurement completed", ok);
            tt_report_rate(name, p1, 10000u);
            tt_check("consecutive periods match", ok && tt_close(p1, p2, 2u));
        }
    } else {
        tt_skip_check("clock rate measurement");
    }

    tt_block_end(name);
}

/* ------------------------------------------------------------------------- */
/* CMSDK dual timer (CMSDK APB dual-input timer)                             */
/* ------------------------------------------------------------------------- */
static uint32_t tt_measure_dt_period(CMSDK_DUALTIMER_CHANNEL_TypeDef *t,
                                     uint32_t control, uint32_t load, int *ok)
{
    uint32_t start;
    uint32_t end;
    uint32_t cfg = control & ~CMSDK_DUALTIMER_CTRL_EN_Msk;

    /* Program mode/size/prescale while stopped, then the load value, then
     * start.  This keeps a 16/32-bit switch from disturbing the count. */
    t->CONTROL = 0u;
    t->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t->CONTROL = cfg;
    t->LOAD = load;
    t->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    start = core_cycles_read();
    t->CONTROL = control;
    *ok = tt_wait_mask(&t->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(1000));
    end = core_cycles_read();

    t->CONTROL = 0u;
    t->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    return end - start;
}

static void tt_test_dualtimer(void)
{
    CMSDK_DUALTIMER_TypeDef *dt = CMSDK_DUALTIMER;
    CMSDK_DUALTIMER_CHANNEL_TypeDef *t1 = &dt->TIMER[0];
    CMSDK_DUALTIMER_CHANNEL_TypeDef *t2 = &dt->TIMER[1];
    const uint32_t base = CMSDK_DUALTIMER_CTRL_SIZE_Msk |
                          CMSDK_DUALTIMER_CTRL_EN_Msk;
    const uint32_t periodic = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
                              CMSDK_DUALTIMER_CTRL_IRQEN_Msk;
    uint32_t pid;
    uint32_t cid;
    uint32_t control;
    uint32_t p32;
    uint32_t p16;
    uint32_t pfast;
    uint32_t p16x;
    uint32_t pslow;
    uint32_t ratio;
    uint32_t v;
    int ok;

    tt_block_begin("DUALTIMER");

    pid = tt_id32(dt->PID0, dt->PID1, dt->PID2, dt->PID3);
    cid = tt_id32(dt->CID0, dt->CID1, dt->CID2, dt->CID3);
    printf("  DUALTIMER: PID 0x%08X CID 0x%08X\n", pid, cid);
#if TIMER_TESTS_DUMP_IDS
    tt_dump("DUALTIMER", DUALTIMER_BASE, 0x000u, 8u);
    tt_dump("DUALTIMER", DUALTIMER_BASE, 0xFD0u, 12u);
#endif
    tt_check("component ID", cid == TT_CID_CMSDK);

    /* The documented reset value is 0x20 (interrupt enable set), but a warm
     * system reset does not reset the peripheral, so check that CONTROL is
     * writable/readable instead of asserting the power-on value. */
    t1->CONTROL = CMSDK_DUALTIMER_CTRL_IRQEN_Msk |
                  CMSDK_DUALTIMER_CTRL_SIZE_Msk;
    tt_check_u32("CONTROL readback", t1->CONTROL,
                 CMSDK_DUALTIMER_CTRL_IRQEN_Msk |
                 CMSDK_DUALTIMER_CTRL_SIZE_Msk);

    /* Known state. */
    t1->CONTROL = 0u;
    t2->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t2->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    /* Register readback. */
    t1->LOAD = 0x12345678u;
    tt_check_u32("timer1 LOAD readback", t1->LOAD, 0x12345678u);
    t2->LOAD = 0x0FEDCBA9u;
    tt_check_u32("timer2 LOAD readback", t2->LOAD, 0x0FEDCBA9u);

    /* Timer 1: 32-bit periodic. */
    t1->CONTROL = periodic & ~CMSDK_DUALTIMER_CTRL_EN_Msk;
    t1->LOAD = 10000u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->CONTROL = periodic;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer1 periodic first interrupt", ok);
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer1 periodic reload", ok);
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    tt_check("timer1 INTCLR clears RIS",
             (t1->RIS & CMSDK_DUALTIMER_INT_Msk) == 0u);
    t1->CONTROL = 0u;

    /* Timer 1: free-running wraps at 0xFFFFFFFF instead of reloading LOAD. */
    t1->CONTROL = CMSDK_DUALTIMER_CTRL_SIZE_Msk;
    t1->LOAD = 1000u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->CONTROL = CMSDK_DUALTIMER_CTRL_SIZE_Msk |
                  CMSDK_DUALTIMER_CTRL_EN_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer1 free-running first interrupt", ok);
    v = t1->VALUE;
    printf("  DUALTIMER: timer1 VALUE after free-running wrap: 0x%08X\n", v);
    tt_check("timer1 free-running wraps to 0xFFFFFFFF",
             ok && (v > 0x80000000u));
    t1->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    /* Timer 1: one-shot stops after one period.  MODE is set so LOAD is the
     * period; ONESHOT overrides the reload at zero. */
    control = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
              CMSDK_DUALTIMER_CTRL_ONESHOT_Msk |
              CMSDK_DUALTIMER_CTRL_IRQEN_Msk;
    t1->CONTROL = control & ~CMSDK_DUALTIMER_CTRL_EN_Msk;
    t1->LOAD = 1000u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->CONTROL = control;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer1 one-shot raises RIS", ok);
    v = t1->VALUE;
    printf("  DUALTIMER: timer1 one-shot VALUE after expiry: 0x%08X\n", v);
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(5));
    tt_check("timer1 one-shot does not reload", !ok);
    t1->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    /* RIS is set even when the interrupt is masked; MIS follows IRQEN. */
    t1->CONTROL = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk;
    t1->LOAD = 1000u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->CONTROL = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
                  CMSDK_DUALTIMER_CTRL_EN_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("masked timer raises RIS", ok);
    tt_check("masked timer keeps MIS clear",
             (t1->MIS & CMSDK_DUALTIMER_INT_Msk) == 0u);
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->CONTROL = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
                  CMSDK_DUALTIMER_CTRL_IRQEN_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("unmasked timer raises MIS",
             ok && ((t1->MIS & CMSDK_DUALTIMER_INT_Msk) != 0u));
    t1->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    /* Timer 1 and timer 2 are independent. */
    t1->CONTROL = 0u;
    t2->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t2->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t1->LOAD = 50000u;
    t2->CONTROL = base & ~CMSDK_DUALTIMER_CTRL_EN_Msk;
    t2->LOAD = 1000u;
    t2->CONTROL = base | CMSDK_DUALTIMER_CTRL_EN_Msk;
    ok = tt_wait_mask(&t2->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer2 runs with timer1 stopped", ok);
    tt_check("timer1 RIS stays clear",
             (t1->RIS & CMSDK_DUALTIMER_INT_Msk) == 0u);
    t2->CONTROL = 0u;
    t2->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    t1->CONTROL = base & ~CMSDK_DUALTIMER_CTRL_EN_Msk;
    t1->LOAD = 1000u;
    t2->CONTROL = base & ~CMSDK_DUALTIMER_CTRL_EN_Msk;
    t2->LOAD = 1000u;
    t1->CONTROL = base | CMSDK_DUALTIMER_CTRL_EN_Msk;
    t2->CONTROL = base | CMSDK_DUALTIMER_CTRL_EN_Msk;
    ok = tt_wait_mask(&t1->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    ok = ok && tt_wait_mask(&t2->RIS, CMSDK_DUALTIMER_INT_Msk, TT_MS(50));
    tt_check("timer1 and timer2 run together", ok);
    t1->CONTROL = 0u;
    t2->CONTROL = 0u;
    t1->INTCLR = CMSDK_DUALTIMER_INT_Msk;
    t2->INTCLR = CMSDK_DUALTIMER_INT_Msk;

    if (!tt_cycles_ok) {
        tt_skip_check("16-bit counter size");
        tt_skip_check("prescaler ratio");
        tt_block_end("DUALTIMER");
        return;
    }

    /* 16-bit mode truncates LOAD to 16 bits, so the same LOAD wraps after
     * half as many ticks as in 32-bit mode. */
    control = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
              CMSDK_DUALTIMER_CTRL_IRQEN_Msk;
    p32 = tt_measure_dt_period(t1, control, 0x1FFF0u, &ok);
    control &= ~CMSDK_DUALTIMER_CTRL_SIZE_Msk;
    p16 = tt_measure_dt_period(t1, control, 0x1FFF0u, &ok);
    ratio = (p32 != 0u) ? ((p16 * 100u) / p32) : 0u;
    printf("  DUALTIMER: 32-bit load 0x1FFF0: %u cycles, 16-bit: %u cycles,"
           " ratio %u%%\n", p32, p16, ratio);
    tt_check("16-bit counter wraps at 0xFFFF", (ratio > 45u) && (ratio < 55u));

    /* Prescaler: 01 divides by 16, 10 divides by 256. */
    control = base | CMSDK_DUALTIMER_CTRL_PERIODIC_Msk |
              CMSDK_DUALTIMER_CTRL_IRQEN_Msk;
    pfast = tt_measure_dt_period(t1, control, 1000u, &ok);
    control &= ~CMSDK_DUALTIMER_CTRL_PRESCALE_Msk;
    control |= CMSDK_DUALTIMER_CTRL_PRESCALE_DIV16;
    p16x = tt_measure_dt_period(t1, control, 1000u, &ok);
    ratio = (pfast != 0u) ? ((p16x * 100u) / pfast) : 0u;
    printf("  DUALTIMER: prescale /1: %u cycles, /16: %u cycles,"
           " ratio %u%%\n", pfast, p16x, ratio);
    tt_check("prescaler divides by 16", (ratio > 1500u) && (ratio < 1700u));

    control &= ~CMSDK_DUALTIMER_CTRL_PRESCALE_Msk;
    control |= CMSDK_DUALTIMER_CTRL_PRESCALE_DIV256;
    pslow = tt_measure_dt_period(t1, control, 1000u, &ok);
    ratio = (pfast != 0u) ? ((pslow * 100u) / pfast) : 0u;
    printf("  DUALTIMER: prescale /1: %u cycles, /256: %u cycles,"
           " ratio %u%%\n", pfast, pslow, ratio);
    tt_check("prescaler divides by 256", (ratio > 24000u) && (ratio < 27200u));

    tt_block_end("DUALTIMER");
}

/* ------------------------------------------------------------------------- */
/* Suite                                                                     */
/* ------------------------------------------------------------------------- */
static int tt_run_suite(const char *core)
{
    unsigned total;

    /* The R5 image has no startup code that zeroes .bss, so initialise the
     * accumulators explicitly instead of relying on static initialisation. */
    tt_n_pass = 0u;
    tt_n_fail = 0u;
    tt_n_skip = 0u;

    printf("\n%s: milliSoC timer / dualtimer verification\n", core);

    tt_cycles_ok = core_cycles_init();
    printf("%s: core cycle counter: %s\n", core,
           tt_cycles_ok ? "available" : "NOT available (timeouts by loop count)");
    if (tt_cycles_ok) {
        (void)core_cycles_read();
    }

    tt_test_cmsdk_timer("TIMER0", CMSDK_TIMER0);
    tt_test_cmsdk_timer("TIMER1", CMSDK_TIMER1);
    tt_test_dualtimer();

    total = tt_n_pass + tt_n_fail + tt_n_skip;
    printf("\n%s: %u checks: %u passed, %u failed, %u skipped\n",
           core, total, tt_n_pass, tt_n_fail, tt_n_skip);
    printf("%s: %s\n", core, (tt_n_fail == 0u) ? "PASS" : "FAIL");

    return (tt_n_fail == 0u) ? 1 : 0;
}

#endif /* TIMER_TESTS_COMMON_H */
