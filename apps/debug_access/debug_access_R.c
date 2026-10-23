#include "uart_stdout.h"
#include <stdint.h>
#include <stdio.h>

#define DEBUG_APBIC_ROM_BASE 0xE00F0000UL

static int check_component_id(const char *name, uint32_t offset, uint32_t expected)
{
    volatile const uint32_t *reg =
        (volatile const uint32_t *)(DEBUG_APBIC_ROM_BASE + offset);
    uint32_t actual = *reg;

    printf("R5 DEBUG APBIC %s: 0x%08x\n", name, actual);
    if (actual != expected) {
        printf("R5 DEBUG APBIC FAIL: expected 0x%08x\n", expected);
        return 0;
    }
    return 1;
}

int main(void)
{
    int passed;

    UartStdOutInit();
    printf("R5 system-bus CoreSight APBIC test\n");

    passed = check_component_id("CID0", 0xFF0, 0x0000000D) &&
             check_component_id("CID1", 0xFF4, 0x00000010) &&
             check_component_id("CID2", 0xFF8, 0x00000005) &&
             check_component_id("CID3", 0xFFC, 0x000000B1);

    printf("R5 DEBUG APBIC %s\n", passed ? "PASS" : "FAIL");
    UartEndSimulation();
    return passed ? 0 : 1;
}
