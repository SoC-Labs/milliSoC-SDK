#include "uart_stdout.h"
#include <stdint.h>
#include <stdio.h>

#define DEBUG_APBIC_ROM_BASE 0xE00F0000UL

static uint32_t read_component_id(uint32_t offset)
{
    volatile const uint32_t *reg =
        (volatile const uint32_t *)(DEBUG_APBIC_ROM_BASE + offset);
    return *reg;
}

int main(void)
{
    uint32_t cid0;
    uint32_t cid1;
    uint32_t cid2;
    uint32_t cid3;
    int passed;

    UartStdOutInit();
    cid0 = read_component_id(0xFF0);
    cid1 = read_component_id(0xFF4);
    cid2 = read_component_id(0xFF8);
    cid3 = read_component_id(0xFFC);
    passed = cid0 == 0x0000000D &&
             cid1 == 0x00000010 &&
             cid2 == 0x00000005 &&
             cid3 == 0x000000B1;

    printf("M85 system-bus CoreSight APBIC test\n");
    printf("M85 DEBUG APBIC CIDs: %02x %02x %02x %02x\n",
           cid0, cid1, cid2, cid3);
    printf("M85 DEBUG APBIC %s\n", passed ? "PASS" : "FAIL");
    UartEndSimulation();
    for (;;) {
        __asm volatile("wfi");
    }
}
