/*
 * milliSoC Cortex-M85 floating-point test application
 *
 * Demonstrates single-precision C arithmetic using the M85 FPU.
 */

#include "millisoc_platform_m.h"
#include <stdio.h>
#include <stdint.h>

#define FPU_TEST_COUNT 10U
#define FPU_ACCESS_Msk (0xFUL << 20)

static uint32_t passed;
static uint32_t failed;

static int float_near(float actual, float expected)
{
    float difference = actual - expected;

    if (difference < 0.0F)
        difference = -difference;

    return difference <= 0.0001F;
}

static void check(const char *name, int success)
{
    printf("FPU %-24s %s\n", name, success ? "PASS" : "FAIL");
    if (success)
        ++passed;
    else
        ++failed;
}

int main(void)
{
    volatile float lhs;
    volatile float rhs;
    volatile float result;
    volatile float accumulator;
    volatile int32_t integer;
    uint32_t i;

    UartStdOutInit();

    printf("M85: single-precision FPU tests\n");

    check("CP10/CP11 access",
          (SCB->CPACR & FPU_ACCESS_Msk) == FPU_ACCESS_Msk);

    lhs = 1.5F;
    rhs = 2.25F;
    result = lhs + rhs;
    check("addition", float_near(result, 3.75F));

    lhs = 7.5F;
    rhs = 2.0F;
    result = lhs - rhs;
    check("subtraction", float_near(result, 5.5F));

    lhs = 1.5F;
    rhs = 4.0F;
    result = lhs * rhs;
    check("multiplication", float_near(result, 6.0F));

    lhs = 7.5F;
    rhs = 2.5F;
    result = lhs / rhs;
    check("division", float_near(result, 3.0F));

    accumulator = 0.0F;
    for (i = 0U; i < 10U; ++i)
        accumulator += 0.125F;
    check("accumulation", float_near(accumulator, 1.25F));

    lhs = 1.25F;
    rhs = 1.0F;
    check("comparison", lhs > rhs);

    lhs = 3.75F;
    integer = (int32_t)lhs;
    check("float-to-integer", integer == 3);

    integer = 7;
    result = (float)integer;
    check("integer-to-float", float_near(result, 7.0F));

    lhs = -2.5F;
    rhs = 2.0F;
    result = lhs * rhs + 1.0F;
    check("mixed expression", float_near(result, -4.0F));

    if (failed == 0U)
        printf("FPU tests: %lu/%u PASS\n",
               (unsigned long)passed, FPU_TEST_COUNT);
    else
        printf("FPU tests: %lu/%u PASS, %lu FAIL\n",
               (unsigned long)passed, FPU_TEST_COUNT, (unsigned long)failed);

    UartEndSimulation();
    return 0;
}
