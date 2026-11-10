/*
 * milliSoC Ethos-U85 Vela model demo (Cortex-R5 side)
 *
 * The model demo runs entirely on the M85. This companion image only reports
 * that the R5 is alive and then parks, so it does not write the end-of-
 * simulation marker while the M85 is still running inferences on the NPU.
 */

#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>

int main(void)
{
    UartStdOutInit();

    printf("R5: MilliSoC Ethos-U85 model demo companion (idle)\n");

    for (;;)
    {
        __asm volatile("WFI");
    }
}
