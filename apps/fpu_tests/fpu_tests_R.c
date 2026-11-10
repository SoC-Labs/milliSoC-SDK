/*
 * milliSoC Cortex-M85 FPU test companion (Cortex-R5 side)
 */

#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>

int main(void)
{
    UartStdOutInit();
    printf("R5: M85 FPU test companion (idle)\n");

    UartEndSimulation();
    return 0;
}
