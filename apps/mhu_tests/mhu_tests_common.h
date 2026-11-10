/*
 * Shared milliSoC MHU-320AE verification for the Cortex-M85 and Cortex-R5.
 *
 * The SoC has one MHU per direction: MHU_SND is the postbox this core writes
 * and MHU_REC is the mailbox the other core writes.  Both MHUs are configured
 * with 4 doorbell channels and 4 fast channels.
 *
 * The two cores run a fixed, identical step list in lockstep.  For every step
 * one core is the sender and the other is the receiver:
 *
 *   sender:   drive the stimulus, then wait for an acknowledge token
 *   receiver: wait for the stimulus, verify it, clear/consume it, then send
 *             the acknowledge token
 *
 * The acknowledge token is doorbell channel 3, bit 31 on the *reverse* MHU,
 * so it never collides with the data under test (channel 3 data leaves bit 31
 * clear).  The receiver waits for the peer to clear the token before moving on
 * so channel 3 is clean for the next step.
 *
 * The including file must define, before including this header:
 *
 *   #define MHUT_CORE_IS_M 1   // 1 for the M85, 0 for the R5
 *
 * and must provide these hooks (they may be defined after the include):
 *
 *   static int      core_cycles_init(void);
 *   static uint32_t core_cycles_read(void);
 *   static void     mhut_irq_init(void);
 *   static void     mhut_irq_mask(int masked);
 *   static void     mhut_irq_ack(void);
 */

#ifndef MHU_TESTS_COMMON_H
#define MHU_TESTS_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include "mhu.h"

#ifndef MHUT_CORE_IS_M
#error "MHUT_CORE_IS_M must be defined (1 = M85, 0 = R5)"
#endif

/* Hooks supplied by the including file. */
static int      core_cycles_init(void);
static uint32_t core_cycles_read(void);
static void     mhut_irq_init(void);
static void     mhut_irq_mask(int masked);
static void     mhut_irq_ack(void);

/* ------------------------------------------------------------------------- */
/* Configuration                                                             */
/* ------------------------------------------------------------------------- */
#define MHUT_CID              0xB105F00Du

#define MHUT_DB_CHANNELS      4u
#define MHUT_FC_CHANNELS      4u
#define MHUT_FC_GROUPS        1u
#define MHUT_FC_PER_GROUP     4u

/* Channel 3 bit 31 is the acknowledge token, so channel-3 data leaves it
 * clear.  The token is always carried on the opposite MHU, but keeping the
 * bit free makes the channel-3 test unambiguous. */
#define MHUT_ACK_CH           3u
#define MHUT_ACK_BIT          0x80000000u

/* Nominal core clock, only used to convert cycle timeouts to time. */
#ifndef MHUT_CORE_HZ
#define MHUT_CORE_HZ          10000000u
#endif
#define MHUT_CYCLES_PER_MS    (MHUT_CORE_HZ / 1000u)
#define MHUT_MS(ms)           ((uint32_t)(ms) * MHUT_CYCLES_PER_MS)

#define MHUT_ARRAY_LEN(a)     (sizeof(a) / sizeof((a)[0]))

/* ------------------------------------------------------------------------- */
/* State                                                                     */
/* ------------------------------------------------------------------------- */
static unsigned mh_n_pass;
static unsigned mh_n_fail;
static unsigned mh_n_skip;
static unsigned mh_b_pass;
static unsigned mh_b_fail;
static unsigned mh_b_skip;
static int      mh_cycles_ok;

static int      mhut_fch_ws64;
static int      mhut_fcgi_spt;

/* Written by the receiver interrupt handlers, read by the test body. */
static volatile uint32_t mhut_db_irq_count;
static volatile uint32_t mhut_fc_irq_count;
static volatile uint32_t mhut_db_irq_last;
static volatile uint32_t mhut_db_irq_pay[MHUT_DB_CHANNELS];
static volatile uint32_t mhut_fc_irq_pay_lo[MHUT_FC_CHANNELS];
static volatile uint32_t mhut_fc_irq_pay_hi[MHUT_FC_CHANNELS];
static volatile uint32_t mhut_unexpected_irq;

/* ------------------------------------------------------------------------- */
/* Reporting                                                                 */
/* ------------------------------------------------------------------------- */
static void mh_block_begin(const char *name)
{
    mh_b_pass = 0u;
    mh_b_fail = 0u;
    mh_b_skip = 0u;
    printf("\n%s:\n", name);
}

static void mh_block_end(const char *name)
{
    printf("%s: %u checks, %u passed, %u failed, %u skipped\n",
           name, mh_b_pass + mh_b_fail + mh_b_skip,
           mh_b_pass, mh_b_fail, mh_b_skip);
}

static void mh_check(const char *name, int ok)
{
    if (ok) {
        ++mh_n_pass;
        ++mh_b_pass;
        printf("  %s: PASS\n", name);
    } else {
        ++mh_n_fail;
        ++mh_b_fail;
        printf("  %s: FAIL\n", name);
    }
}

static void mh_check_u32(const char *name, uint32_t got, uint32_t want)
{
    if (got == want) {
        ++mh_n_pass;
        ++mh_b_pass;
        printf("  %s: PASS (0x%08X)\n", name, got);
    } else {
        ++mh_n_fail;
        ++mh_b_fail;
        printf("  %s: FAIL (got 0x%08X, want 0x%08X)\n", name, got, want);
    }
}

/* ------------------------------------------------------------------------- */
/* Timing helpers                                                            */
/* ------------------------------------------------------------------------- */
static void mh_delay(uint32_t cycles)
{
    uint32_t start;
    volatile uint32_t spins = 0u;

    if (mh_cycles_ok) {
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

static int mh_wait_all(volatile const uint32_t *reg, uint32_t mask,
                       uint32_t timeout)
{
    uint32_t start = core_cycles_read();
    volatile uint32_t spins = 0u;

    for (;;) {
        if ((*reg & mask) == mask) {
            return 1;
        }
        if (mh_cycles_ok) {
            if ((uint32_t)(core_cycles_read() - start) > timeout) {
                return 0;
            }
        } else if (++spins > timeout) {
            return 0;
        }
    }
}

static int mh_wait_zero(volatile const uint32_t *reg, uint32_t mask,
                        uint32_t timeout)
{
    uint32_t start = core_cycles_read();
    volatile uint32_t spins = 0u;

    for (;;) {
        if ((*reg & mask) == 0u) {
            return 1;
        }
        if (mh_cycles_ok) {
            if ((uint32_t)(core_cycles_read() - start) > timeout) {
                return 0;
            }
        } else if (++spins > timeout) {
            return 0;
        }
    }
}

static int mh_wait_counter(volatile uint32_t *counter, uint32_t before,
                           uint32_t timeout)
{
    uint32_t start = core_cycles_read();
    volatile uint32_t spins = 0u;

    for (;;) {
        if (*counter != before) {
            return 1;
        }
        if (mh_cycles_ok) {
            if ((uint32_t)(core_cycles_read() - start) > timeout) {
                return 0;
            }
        } else if (++spins > timeout) {
            return 0;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* Component discovery                                                       */
/* ------------------------------------------------------------------------- */
static uint32_t mhut_cid_snd(void)
{
    return (MHUS_PBX->CIDR0 & 0xFFu) |
           ((MHUS_PBX->CIDR1 & 0xFFu) << 8) |
           ((MHUS_PBX->CIDR2 & 0xFFu) << 16) |
           ((MHUS_PBX->CIDR3 & 0xFFu) << 24);
}

static uint32_t mhut_cid_rec(void)
{
    return (MHUR_MBX->CIDR0 & 0xFFu) |
           ((MHUR_MBX->CIDR1 & 0xFFu) << 8) |
           ((MHUR_MBX->CIDR2 & 0xFFu) << 16) |
           ((MHUR_MBX->CIDR3 & 0xFFu) << 24);
}

/* ------------------------------------------------------------------------- */
/* Fast channel payload access                                               */
/* ------------------------------------------------------------------------- */
/* The mhu.h PFCW/MFCW arrays are declared with an 8-byte stride, which is
 * only correct for 64-bit fast channels.  In 32-bit mode the payload windows
 * are 4 bytes apart, so use explicit offsets:
 *
 *   32-bit: base + 0x3000 + 4*n
 *   64-bit: base + 0x3000 + 8*n
 */
static uint32_t mhut_fc_stride(void)
{
    return mhut_fch_ws64 ? 8u : 4u;
}

static volatile uint32_t *mhut_fc_snd_reg(uint32_t ch)
{
    return (volatile uint32_t *)(MHU_SND + 0x3000u + (mhut_fc_stride() * ch));
}

static volatile uint32_t *mhut_fc_rec_reg(uint32_t ch)
{
    return (volatile uint32_t *)(MHU_REC + 0x3000u + (mhut_fc_stride() * ch));
}

static uint64_t mhut_fc_recv_read(uint32_t ch)
{
    volatile uint32_t *reg = mhut_fc_rec_reg(ch);

    if (mhut_fch_ws64) {
        uint32_t lo = reg[0];
        uint32_t hi = reg[1];

        return ((uint64_t)hi << 32) | (uint64_t)lo;
    }
    return (uint64_t)reg[0];
}

static void mhut_fc_snd_write(uint32_t ch, uint32_t lo, uint32_t hi)
{
    volatile uint32_t *reg = mhut_fc_snd_reg(ch);

    if (mhut_fch_ws64) {
        reg[0] = lo;
        reg[1] = hi;
    } else {
        reg[0] = lo;
    }
}

static uint64_t mhut_fc_snd_read(uint32_t ch)
{
    volatile uint32_t *reg = mhut_fc_snd_reg(ch);

    if (mhut_fch_ws64) {
        uint32_t lo = reg[0];
        uint32_t hi = reg[1];

        return ((uint64_t)hi << 32) | (uint64_t)lo;
    }
    return (uint64_t)reg[0];
}

/* ------------------------------------------------------------------------- */
/* Receiver interrupt bodies                                                 */
/* ------------------------------------------------------------------------- */
__attribute__((used)) void mhut_db_irq_body(void)
{
    uint32_t st = MHUR_MBX->DBCH_INT_ST[0];
    uint32_t ch;

    mhut_db_irq_last = st;
    for (ch = 0u; ch < MHUT_DB_CHANNELS; ++ch) {
        if ((st & (1u << ch)) != 0u) {
            uint32_t clear = 0xFFFFFFFFu;

            mhut_db_irq_pay[ch] = MHUR_MBX->MDBCW[ch].ST;
            /* Never clear the acknowledge token: channel 3 is also the
             * handshake channel and the token is polled by the test body. */
            if (ch == MHUT_ACK_CH) {
                clear &= ~MHUT_ACK_BIT;
            }
            MHUR_MBX->MDBCW[ch].CLR = clear;
        }
    }
    mhut_irq_ack();
    ++mhut_db_irq_count;
}

__attribute__((used)) void mhut_fc_irq_body(void)
{
    uint32_t st = MHUR_MBX->FCH_GRP_INT_ST[0];
    uint32_t ch;

    if (mhut_fcgi_spt && (st != 0u)) {
        for (ch = 0u; ch < MHUT_FC_CHANNELS; ++ch) {
            if ((st & (1u << ch)) != 0u) {
                uint64_t v = mhut_fc_recv_read(ch);
                mhut_fc_irq_pay_lo[ch] = (uint32_t)v;
                mhut_fc_irq_pay_hi[ch] = (uint32_t)(v >> 32);
            }
        }
    } else {
        for (ch = 0u; ch < MHUT_FC_CHANNELS; ++ch) {
            uint64_t v = mhut_fc_recv_read(ch);
            mhut_fc_irq_pay_lo[ch] = (uint32_t)v;
            mhut_fc_irq_pay_hi[ch] = (uint32_t)(v >> 32);
        }
    }
    mhut_irq_ack();
    ++mhut_fc_irq_count;
}

/* ------------------------------------------------------------------------- */
/* Doorbell transport                                                        */
/* ------------------------------------------------------------------------- */
static void mhut_wait_ack(void)
{
    int ok;

    ok = mh_wait_all(&MHUR_MBX->MDBCW[MHUT_ACK_CH].ST, MHUT_ACK_BIT,
                     MHUT_MS(2000));
    mh_check("acknowledge token received", ok);
    MHUR_MBX->MDBCW[MHUT_ACK_CH].CLR = MHUT_ACK_BIT;
}

static void mhut_send_ack(void)
{
    int ok;

    MHUS_PBX->PDBCW[MHUT_ACK_CH].SET = MHUT_ACK_BIT;
    /* Wait for the peer to clear the token: channel 3 is also test data, so
     * the next step must start from a clean status register. */
    ok = mh_wait_zero(&MHUS_PBX->PDBCW[MHUT_ACK_CH].ST, MHUT_ACK_BIT,
                      MHUT_MS(2000));
    mh_check("acknowledge token accepted", ok);
}

static void mhut_db_send(uint32_t ch, uint32_t pattern)
{
    int ok;

    MHUS_PBX->PDBCW[ch].INT_CLR = 1u;
    MHUS_PBX->PDBCW[ch].INT_EN = 1u;
    MHUS_PBX->PDBCW[ch].SET = pattern;

    /* The receiver clears the flags; the sender's status must follow. */
    ok = mh_wait_zero(&MHUS_PBX->PDBCW[ch].ST, pattern, MHUT_MS(2000));
    mh_check("doorbell handshake (sender flags cleared)", ok);

    ok = mh_wait_all(&MHUS_PBX->PDBCW[ch].INT_ST, 1u, MHUT_MS(200));
    mh_check("doorbell sender acknowledge interrupt", ok);

    MHUS_PBX->PDBCW[ch].INT_CLR = 1u;
    MHUS_PBX->PDBCW[ch].INT_EN = 0u;
}

static void mhut_db_recv(uint32_t ch, uint32_t pattern)
{
    int ok;

    ok = mh_wait_all(&MHUR_MBX->MDBCW[ch].ST, pattern, MHUT_MS(2000));
    mh_check("doorbell received", ok);
    mh_check_u32("doorbell payload", MHUR_MBX->MDBCW[ch].ST, pattern);
    mh_check("doorbell receiver interrupt status",
             (MHUR_MBX->DBCH_INT_ST[0] & (1u << ch)) != 0u);

    MHUR_MBX->MDBCW[ch].CLR = pattern;
    ok = mh_wait_zero(&MHUR_MBX->MDBCW[ch].ST, pattern, MHUT_MS(100));
    mh_check("doorbell cleared", ok);
}

static void mhut_db_recv_irq(uint32_t ch, uint32_t pattern)
{
    uint32_t before = mhut_db_irq_count;
    int ok;

    ok = mh_wait_counter(&mhut_db_irq_count, before, MHUT_MS(2000));
    mh_check("doorbell interrupt delivered", ok);
    mh_check("doorbell interrupt channel status",
             (mhut_db_irq_last & (1u << ch)) != 0u);
    mh_check_u32("doorbell interrupt payload", mhut_db_irq_pay[ch], pattern);

    ok = mh_wait_zero(&MHUR_MBX->MDBCW[ch].ST, pattern, MHUT_MS(100));
    mh_check("doorbell interrupt handler cleared flags", ok);
}

/* ------------------------------------------------------------------------- */
/* Fast channel transport                                                    */
/* ------------------------------------------------------------------------- */
static int mhut_fc_wait_value(uint32_t ch, uint32_t lo, uint32_t hi,
                              uint32_t timeout)
{
    uint32_t start = core_cycles_read();
    volatile uint32_t spins = 0u;

    for (;;) {
        uint64_t v = mhut_fc_recv_read(ch);
        if (((uint32_t)v == lo) &&
            (!mhut_fch_ws64 || ((uint32_t)(v >> 32) == hi))) {
            return 1;
        }
        if (mh_cycles_ok) {
            if ((uint32_t)(core_cycles_read() - start) > timeout) {
                return 0;
            }
        } else if (++spins > timeout) {
            return 0;
        }
    }
}

static void mhut_fc_send(uint32_t ch, uint32_t lo, uint32_t hi)
{
    uint64_t got;

    mhut_fc_snd_write(ch, lo, hi);
    got = mhut_fc_snd_read(ch);
    mh_check_u32("fast channel sender readback low", (uint32_t)got, lo);
    if (mhut_fch_ws64) {
        mh_check_u32("fast channel sender readback high",
                     (uint32_t)(got >> 32), hi);
    }
}

static void mhut_fc_recv(uint32_t ch, uint32_t lo, uint32_t hi)
{
    uint64_t v;
    int ok;

    if (mhut_fcgi_spt) {
        ok = mh_wait_all(&MHUR_MBX->FCH_GRP_INT_ST[0], 1u << ch,
                         MHUT_MS(2000));
        mh_check("fast channel transfer interrupt", ok);
        mh_check("fast channel group interrupt status",
                 (MHUR_MBX->FCG_INT_ST & 1u) != 0u);
    } else {
        ok = mhut_fc_wait_value(ch, lo, hi, MHUT_MS(2000));
        mh_check("fast channel received", ok);
    }

    v = mhut_fc_recv_read(ch);
    mh_check_u32("fast channel payload low", (uint32_t)v, lo);
    if (mhut_fch_ws64) {
        mh_check_u32("fast channel payload high", (uint32_t)(v >> 32), hi);
    }

    if (mhut_fcgi_spt) {
        mh_check("fast channel read clears interrupt",
                 (MHUR_MBX->FCH_GRP_INT_ST[0] & (1u << ch)) == 0u);
    }
}

static void mhut_fc_recv_irq(uint32_t ch, uint32_t lo, uint32_t hi)
{
    uint32_t before = mhut_fc_irq_count;
    int ok;

    ok = mh_wait_counter(&mhut_fc_irq_count, before, MHUT_MS(2000));
    mh_check("fast channel interrupt delivered", ok);
    mh_check_u32("fast channel interrupt payload low",
                 mhut_fc_irq_pay_lo[ch], lo);
    if (mhut_fch_ws64) {
        mh_check_u32("fast channel interrupt payload high",
                     mhut_fc_irq_pay_hi[ch], hi);
    }
    if (mhut_fcgi_spt) {
        mh_check("fast channel interrupt cleared",
                 (MHUR_MBX->FCH_GRP_INT_ST[0] & (1u << ch)) == 0u);
    }
}

/* ------------------------------------------------------------------------- */
/* Lockstep step list                                                        */
/* ------------------------------------------------------------------------- */
enum {
    MHUT_K_DB = 0,
    MHUT_K_FC,
    MHUT_K_DB_IRQ,
    MHUT_K_FC_IRQ
};

typedef struct {
    uint8_t  kind;
    uint8_t  from_m;   /* 1 = M85 drives the step, 0 = R5 drives it */
    uint8_t  ch;
    uint32_t lo;
    uint32_t hi;
} mhut_step_t;

static const mhut_step_t mhut_steps_polled[] = {
    /* Doorbell, channel 0. */
    { MHUT_K_DB, 1u, 0u, 0xFFFFFFFFu, 0u },
    { MHUT_K_DB, 0u, 0u, 0xFFFF0000u, 0u },
    /* Doorbell, channel 1. */
    { MHUT_K_DB, 1u, 1u, 0xAAAAAAAAu, 0u },
    { MHUT_K_DB, 0u, 1u, 0xAAAA5555u, 0u },
    /* Doorbell, channel 2. */
    { MHUT_K_DB, 1u, 2u, 0x55555555u, 0u },
    { MHUT_K_DB, 0u, 2u, 0x5555AAAAu, 0u },
    /* Doorbell, channel 3 (bit 31 is the handshake token). */
    { MHUT_K_DB, 1u, 3u, 0x7FFFFFFFu, 0u },
    { MHUT_K_DB, 0u, 3u, 0x7FFF0000u, 0u },
    /* Fast channels, both directions. */
    { MHUT_K_FC, 1u, 0u, 0x11223344u, 0xA1B2C3D4u },
    { MHUT_K_FC, 0u, 0u, 0x01020304u, 0x11111111u },
    { MHUT_K_FC, 1u, 1u, 0x55667788u, 0xE5F6A7B8u },
    { MHUT_K_FC, 0u, 1u, 0x05060708u, 0x22222222u },
    { MHUT_K_FC, 1u, 2u, 0x99AABBCCu, 0x1234ABCDu },
    { MHUT_K_FC, 0u, 2u, 0x090A0B0Cu, 0x33333333u },
    { MHUT_K_FC, 1u, 3u, 0xDDEEFF00u, 0xDEADBEEFu },
    { MHUT_K_FC, 0u, 3u, 0x0D0E0F10u, 0x44444444u }
};

static const mhut_step_t mhut_steps_irq[] = {
    { MHUT_K_DB_IRQ, 1u, 2u, 0x0000F00Du, 0u },
    { MHUT_K_DB_IRQ, 0u, 2u, 0x00000FF0u, 0u },
    { MHUT_K_FC_IRQ, 1u, 2u, 0xCAFEBABEu, 0xFEEDFACEu },
    { MHUT_K_FC_IRQ, 0u, 2u, 0x1234FEDCu, 0x8765ABCDu }
};

static int mhut_is_sender(const mhut_step_t *s)
{
    return (s->from_m != 0u) == (MHUT_CORE_IS_M != 0);
}

/* ------------------------------------------------------------------------- */
/* Suite                                                                     */
/* ------------------------------------------------------------------------- */
static void mhut_drain(void)
{
    uint32_t ch;

    /* Clear local sender-side acknowledge bookkeeping.  Do not touch the
     * incoming doorbell flags or the handshake token: the peer may already be
     * waiting at the phase barrier or may have sent the first stimulus. */
    for (ch = 0u; ch < MHUT_DB_CHANNELS; ++ch) {
        MHUS_PBX->PDBCW[ch].INT_CLR = 1u;
        MHUS_PBX->PDBCW[ch].INT_EN = 0u;
    }

    /* Reading a fast channel consumes its transfer interrupt. */
    for (ch = 0u; ch < MHUT_FC_CHANNELS; ++ch) {
        (void)mhut_fc_recv_read(ch);
    }

    mhut_db_irq_count = 0u;
    mhut_fc_irq_count = 0u;
    mhut_db_irq_last = 0u;
    for (ch = 0u; ch < MHUT_DB_CHANNELS; ++ch) {
        mhut_db_irq_pay[ch] = 0u;
    }
    for (ch = 0u; ch < MHUT_FC_CHANNELS; ++ch) {
        mhut_fc_irq_pay_lo[ch] = 0u;
        mhut_fc_irq_pay_hi[ch] = 0u;
    }

    /* Let the clears settle before the next phase. */
    mh_delay(MHUT_MS(1));
}

/* Exchange the handshake token without reporting: this is phase
 * synchronisation, not a test.  Both cores must call it. */
static void mhut_barrier(void)
{
    MHUS_PBX->PDBCW[MHUT_ACK_CH].SET = MHUT_ACK_BIT;
    (void)mh_wait_all(&MHUR_MBX->MDBCW[MHUT_ACK_CH].ST, MHUT_ACK_BIT,
                      MHUT_MS(2000));
    MHUR_MBX->MDBCW[MHUT_ACK_CH].CLR = MHUT_ACK_BIT;
    (void)mh_wait_zero(&MHUS_PBX->PDBCW[MHUT_ACK_CH].ST, MHUT_ACK_BIT,
                       MHUT_MS(2000));
}

static void mhut_run_steps(const mhut_step_t *steps, unsigned count)
{
    unsigned i;

    for (i = 0u; i < count; ++i) {
        const mhut_step_t *s = &steps[i];

        if (mhut_is_sender(s)) {
            switch (s->kind) {
            case MHUT_K_DB:
            case MHUT_K_DB_IRQ:
                mhut_db_send(s->ch, s->lo);
                break;
            case MHUT_K_FC:
            case MHUT_K_FC_IRQ:
                mhut_fc_send(s->ch, s->lo, s->hi);
                break;
            default:
                break;
            }
            mhut_wait_ack();
        } else {
            switch (s->kind) {
            case MHUT_K_DB:
                mhut_db_recv(s->ch, s->lo);
                break;
            case MHUT_K_FC:
                mhut_fc_recv(s->ch, s->lo, s->hi);
                break;
            case MHUT_K_DB_IRQ:
                mhut_db_recv_irq(s->ch, s->lo);
                break;
            case MHUT_K_FC_IRQ:
                mhut_fc_recv_irq(s->ch, s->lo, s->hi);
                break;
            default:
                break;
            }
            mhut_send_ack();
        }
    }
}

static void mhut_test_discovery(void)
{
    uint32_t snd_db = MHUS_PBX->DBCH_CFG0;
    uint32_t rec_db = MHUR_MBX->DBCH_CFG0;
    uint32_t snd_fc = MHUS_PBX->FCH_CFG0;
    uint32_t rec_fc = MHUR_MBX->FCH_CFG0;
    uint32_t snd_feat = MHUS_PBX->FEAT_SPT0;
    uint32_t rec_feat = MHUR_MBX->FEAT_SPT0;
    uint32_t snd_ws = (snd_fc >> 21) & 0xFFu;
    uint32_t rec_ws = (rec_fc >> 21) & 0xFFu;

    mh_block_begin("Discovery");

    printf("  SND: IIDR 0x%08X AIDR 0x%08X PIDR0 0x%02X CID 0x%08X\n",
           MHUS_PBX->IIDR, MHUS_PBX->AAIDR, MHUS_PBX->PIDR0 & 0xFFu,
           mhut_cid_snd());
    printf("  REC: IIDR 0x%08X AIDR 0x%08X PIDR0 0x%02X CID 0x%08X\n",
           MHUR_MBX->IIDR, MHUR_MBX->AAIDR, MHUR_MBX->PIDR0 & 0xFFu,
           mhut_cid_rec());
    printf("  SND: DBCH_CFG0 0x%08X FCH_CFG0 0x%08X FEAT_SPT0 0x%08X\n",
           snd_db, snd_fc, snd_feat);
    printf("  REC: DBCH_CFG0 0x%08X FCH_CFG0 0x%08X FEAT_SPT0 0x%08X\n",
           rec_db, rec_fc, rec_feat);

    mh_check_u32("sender component ID", mhut_cid_snd(), MHUT_CID);
    mh_check_u32("receiver component ID", mhut_cid_rec(), MHUT_CID);
    mh_check_u32("sender block ID", MHUS_PBX->PBX_BLK_ID & 0xFu, 0u);
    /* MHU-320AE: postbox BLK_ID is 0, mailbox BLK_ID is 1. */
    mh_check_u32("receiver block ID", MHUR_MBX->PBX_BLK_ID & 0xFu, 1u);

    mh_check_u32("sender doorbell channels", (snd_db & 0xFFu) + 1u,
                 MHUT_DB_CHANNELS);
    mh_check_u32("receiver doorbell channels", (rec_db & 0xFFu) + 1u,
                 MHUT_DB_CHANNELS);

    mh_check_u32("sender fast channels", (snd_fc & 0x3FFu) + 1u,
                 MHUT_FC_CHANNELS);
    mh_check_u32("receiver fast channels", (rec_fc & 0x3FFu) + 1u,
                 MHUT_FC_CHANNELS);
    mh_check_u32("sender fast channel groups", ((snd_fc >> 11) & 0x1Fu) + 1u,
                 MHUT_FC_GROUPS);
    mh_check_u32("receiver fast channel groups",
                 ((rec_fc >> 11) & 0x1Fu) + 1u, MHUT_FC_GROUPS);
    mh_check_u32("sender fast channels per group",
                 ((snd_fc >> 16) & 0x1Fu) + 1u, MHUT_FC_PER_GROUP);
    mh_check_u32("receiver fast channels per group",
                 ((rec_fc >> 16) & 0x1Fu) + 1u, MHUT_FC_PER_GROUP);
    mh_check("sender fast channel word size",
             (snd_ws == 0x20u) || (snd_ws == 0x40u));
    mh_check("receiver fast channel word size",
             (rec_ws == 0x20u) || (rec_ws == 0x40u));

    mh_check_u32("sender doorbell support", snd_feat & 0xFu, 1u);
    mh_check_u32("sender fast channel support", (snd_feat >> 8) & 0xFu, 1u);
    mh_check_u32("receiver doorbell support", rec_feat & 0xFu, 1u);
    mh_check_u32("receiver fast channel support", (rec_feat >> 8) & 0xFu, 1u);

    /* The FIFO extension is optional and this SoC does not implement it, so
     * report it instead of failing the suite on it. */
    printf("  FIFO extension: sender %s, receiver %s\n",
           ((snd_feat >> 4) & 0xFu) != 0u ? "implemented" : "not implemented",
           ((rec_feat >> 4) & 0xFu) != 0u ? "implemented" : "not implemented");

    mhut_fch_ws64 = (rec_ws == 0x40u);
    mhut_fcgi_spt = ((rec_fc >> 10) & 1u) != 0u;
    printf("  fast channels are %u-bit, group interrupts %s\n",
           mhut_fch_ws64 ? 64u : 32u,
           mhut_fcgi_spt ? "implemented" : "not implemented");

    mh_block_end("Discovery");
}

static void mhut_run_suite(const char *core)
{
    /* The R5 startup does not zero .bss, so the counters would otherwise
     * carry values over from whatever ran before this image. */
    mh_n_pass = 0u;
    mh_n_fail = 0u;
    mh_n_skip = 0u;
    mh_b_pass = 0u;
    mh_b_fail = 0u;
    mh_b_skip = 0u;
    mhut_unexpected_irq = 0u;

    mh_cycles_ok = core_cycles_init();

    printf("\n%s: milliSoC MHU-320AE verification\n", core);
    printf("%s: cycle counter %s\n", core,
           mh_cycles_ok ? "available" : "not available");

    mhut_test_discovery();

    /* Make sure the fast-channel transfer interrupt is enabled; both fields
     * reset to "enabled" but the test does not rely on that. */
    MHUR_MBX->FCH_CTRL = 4u;
    MHUR_MBX->FCG_INT_EN = 0xFFFFFFFFu;

    /* Keep interrupts masked while the polling tests drive the channels. */
    mhut_irq_mask(1);
    mhut_irq_init();
    mhut_drain();

    /* The two cores boot at different times; sync before the first stimulus
     * so neither side can drain or miss the other's first doorbell. */
    mhut_barrier();

    mh_block_begin("Doorbell and fast channel transport");
    mhut_run_steps(mhut_steps_polled, MHUT_ARRAY_LEN(mhut_steps_polled));
    mh_block_end("Doorbell and fast channel transport");

    /* Let the receiver interrupt handlers run for the IRQ block.  The barrier
     * makes sure neither core starts the IRQ phase while the other is still
     * draining. */
    mhut_drain();
    mhut_barrier();

    /* The acknowledge token is polled, not interrupt driven.  Keep doorbell
     * channel 3 out of the mailbox combined interrupt from here on so the
     * receiver's handler cannot consume the token before the test loop sees
     * it.  The polled block above still exercises the real interrupt status
     * of all four channels. */
    MHUR_MBX->MDBCW[MHUT_ACK_CH].CTRL = 0u;
    mhut_irq_mask(0);

    mh_block_begin("Interrupt delivery");
    mhut_run_steps(mhut_steps_irq, MHUT_ARRAY_LEN(mhut_steps_irq));
    mh_block_end("Interrupt delivery");

    mhut_irq_mask(1);

    printf("\n%s: %u checks, %u passed, %u failed, %u skipped\n",
           core, mh_n_pass + mh_n_fail + mh_n_skip,
           mh_n_pass, mh_n_fail, mh_n_skip);
    if (mh_n_fail == 0u) {
        printf("%s: PASS\n", core);
    } else {
        printf("%s: FAIL\n", core);
    }
}

#endif /* MHU_TESTS_COMMON_H */
