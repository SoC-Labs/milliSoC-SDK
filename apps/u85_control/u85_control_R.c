/*
 * milliSoC Ethos-U85 control demo (Cortex-R5 side)
 *
 * The demo itself runs entirely on the M85. This companion image only reports
 * that the R5 is alive and then parks, so it does not write the end-of-
 * simulation marker while the M85 is still measuring the NPU.
 */

#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>

int main(void)
{
    UartStdOutInit();

    printf("R5: MilliSoC Ethos-U85 demo companion (idle)\n");

    UartEndSimulation();
    return 0;
}
