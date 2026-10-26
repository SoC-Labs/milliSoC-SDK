#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    SRAM_TEST_PASS = 0,
    SRAM_TEST_INVALID_ARGUMENT,
    SRAM_TEST_ADDRESS_FAILURE,
    SRAM_TEST_DATA_FAILURE,
    SRAM_TEST_PATTERN_FAILURE
} sram_test_status_t;

typedef struct {
    sram_test_status_t status;
    uintptr_t          fail_address;
    uint32_t           expected;
    uint32_t           actual;
    const char        *phase;
} sram_test_result_t;


static inline void sram_test_barrier(void);
static sram_test_result_t sram_result(
    sram_test_status_t status,
    uintptr_t address,
    uint32_t expected,
    uint32_t actual,
    const char *phase);
static sram_test_result_t sram_fill_verify(
    volatile uint32_t *mem,
    size_t words,
    uint32_t pattern,
    const char *phase);
static sram_test_result_t sram_address_test(
    volatile uint32_t *mem,
    size_t words);
static uint32_t sram_address_pattern(uintptr_t address);
static sram_test_result_t sram_address_pattern_test(
    volatile uint32_t *mem,
    size_t words);
sram_test_result_t sram_test(
    uintptr_t base_address,
    size_t test_size);

#define TEST_SIZE 0x1000
#define TEST_OFFSET 0x1000

int main(){
    UartStdOutInit();

    printf("M85: MilliSoC SRAM Tests\n");

    sram_test_result_t result;

    result = sram_test(SRAM_BASE, TEST_SIZE);

    if (result.status == SRAM_TEST_PASS) {
        printf("SRAM test PASSED\n");
    } else {
        printf("SRAM test FAILED: phase=%s\n",
               result.phase);
        printf("Address:  0x%08lx\n",
               (unsigned long)result.fail_address);
        printf("Expected: 0x%08lx\n",
               (unsigned long)result.expected);
        printf("Actual:   0x%08lx\n",
               (unsigned long)result.actual);
    }

    UartEndSimulation();
}



/* Ensure writes complete before beginning readback on Arm. */
static inline void sram_test_barrier(void)
{
#if defined(__arm__) || defined(__aarch64__)
    __asm volatile ("dsb sy" ::: "memory");
#else
    __asm volatile ("" ::: "memory");
#endif
}

static sram_test_result_t sram_result(
    sram_test_status_t status,
    uintptr_t address,
    uint32_t expected,
    uint32_t actual,
    const char *phase)
{
    sram_test_result_t result = {
        status, address, expected, actual, phase
    };
    return result;
}

/* Write and verify a constant value throughout the SRAM region. */
static sram_test_result_t sram_fill_verify(
    volatile uint32_t *mem,
    size_t words,
    uint32_t pattern,
    const char *phase)
{
    for (size_t i = 0; i < words; i++) {
        mem[i] = pattern;
    }

    sram_test_barrier();

    for (size_t i = 0; i < words; i++) {
        uint32_t actual = mem[i];

        if (actual != pattern) {
            return sram_result(
                SRAM_TEST_PATTERN_FAILURE,
                (uintptr_t)&mem[i],
                pattern, actual, phase);
        }
    }

    return sram_result(SRAM_TEST_PASS, 0, 0, 0, phase);
}

/*
 * Address bus test.
 * Uses the first word as a reference and tests each power-of-two
 * word offset. The test region must be exclusively owned.
 */
static sram_test_result_t sram_address_test(
    volatile uint32_t *mem,
    size_t words)
{
    const uint32_t base_pattern = 0xAAAAAAAAu;
    const uint32_t test_pattern = 0x55555555u;

    mem[0] = base_pattern;

    for (size_t offset = 1; offset < words; ) {
        mem[offset] = test_pattern;
        offset <<= 1;
    }

    sram_test_barrier();

    if (mem[0] != base_pattern) {
        return sram_result(
            SRAM_TEST_ADDRESS_FAILURE,
            (uintptr_t)&mem[0],
            base_pattern, mem[0], "address alias");
    }

    for (size_t offset = 1; offset < words; ) {
        if (mem[offset] != test_pattern) {
            return sram_result(
                SRAM_TEST_ADDRESS_FAILURE,
                (uintptr_t)&mem[offset],
                test_pattern, mem[offset], "address decode");
        }

        offset <<= 1;
    }

    return sram_result(SRAM_TEST_PASS, 0, 0, 0, "address test");
}

/* Address-dependent pattern to expose mirrored or misplaced words. */
static uint32_t sram_address_pattern(uintptr_t address)
{
    uint32_t x = (uint32_t)(address >> 2);

    x ^= x >> 16;
    x *= 0x45D9F3Bu;
    x ^= x >> 16;
    x *= 0x45D9F3Bu;
    x ^= x >> 16;

    return x;
}

static sram_test_result_t sram_address_pattern_test(
    volatile uint32_t *mem,
    size_t words)
{
    for (size_t i = 0; i < words; i++) {
        mem[i] = sram_address_pattern((uintptr_t)&mem[i]);
    }

    sram_test_barrier();

    for (size_t i = 0; i < words; i++) {
        uint32_t expected =
            sram_address_pattern((uintptr_t)&mem[i]);
        uint32_t actual = mem[i];

        if (actual != expected) {
            return sram_result(
                SRAM_TEST_PATTERN_FAILURE,
                (uintptr_t)&mem[i],
                expected, actual, "address pattern");
        }
    }

    return sram_result(
        SRAM_TEST_PASS, 0, 0, 0, "address pattern");
}

/*
 * Main SRAM test.
 *
 * base_address: byte address of SRAM start
 * test_size:    number of bytes to test
 *
 * Requirements:
 * - Base address is 32-bit aligned.
 * - Test size is non-zero and a multiple of 4 bytes.
 * - The complete region is valid, writable SRAM.
 *
 * Returns the first failure, or SRAM_TEST_PASS.
 */
sram_test_result_t sram_test(
    uintptr_t base_address,
    size_t test_size)
{
    if ((base_address & 3u) != 0u ||
        test_size == 0u ||
        (test_size & 3u) != 0u ||
        test_size > UINTPTR_MAX - base_address) {
        return sram_result(
            SRAM_TEST_INVALID_ARGUMENT,
            base_address, 0, 0, "arguments");
    }

    volatile uint32_t *mem =
        (volatile uint32_t *)base_address;
    size_t words = test_size / sizeof(uint32_t);

    sram_test_result_t result;

    /* 1. Address bus / aliasing test */
    result = sram_address_test(mem, words);
    if (result.status != SRAM_TEST_PASS)
        return result;

    /* 2. Walking 1s: each data bit tested individually */
    for (unsigned bit = 0; bit < 32; bit++) {
        result = sram_fill_verify(
            mem, words, (uint32_t)1u << bit, "walking 1");
        if (result.status != SRAM_TEST_PASS)
            return result;
    }

    /* 3. Walking 0s */
    for (unsigned bit = 0; bit < 32; bit++) {
        result = sram_fill_verify(
            mem, words, ~((uint32_t)1u << bit), "walking 0");
        if (result.status != SRAM_TEST_PASS)
            return result;
    }

    /* 4. Alternating data patterns */
    result = sram_fill_verify(
        mem, words, 0xAAAAAAAAu, "alternating A");
    if (result.status != SRAM_TEST_PASS)
        return result;

    result = sram_fill_verify(
        mem, words, 0x55555555u, "alternating 5");
    if (result.status != SRAM_TEST_PASS)
        return result;

    /* 5. All-zero and all-one patterns */
    result = sram_fill_verify(
        mem, words, 0x00000000u, "all zero");
    if (result.status != SRAM_TEST_PASS)
        return result;

    result = sram_fill_verify(
        mem, words, 0xFFFFFFFFu, "all one");
    if (result.status != SRAM_TEST_PASS)
        return result;

    /* 6. Address-dependent data */
    result = sram_address_pattern_test(mem, words);
    if (result.status != SRAM_TEST_PASS)
        return result;

    return sram_result(SRAM_TEST_PASS, 0, 0, 0, "complete");
}
