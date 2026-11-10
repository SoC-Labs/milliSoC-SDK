/*
 * milliSoC Cortex-M85 FPU test companion (Cortex-R5 side)
 */

#include "millisoc_platform_r.h"
#include <stdio.h>

int main(void)
{
    UartStdOutInit();
    printf("R5: M85 FPU test companion (idle)\n");

    UartEndSimulation();
    return 0;
}
