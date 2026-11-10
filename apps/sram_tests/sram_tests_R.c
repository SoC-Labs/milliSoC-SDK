/*
 * milliSoC SRAM test companion (Cortex-R5 side)
 *
 * The M85 sram_tests app owns the shared SRAM for the duration of the run
 * (0x30000000-0x30000FFF from the M85 is 0x20000000-0x20000FFF from the R5),
 * so this core must not touch the SRAM while that test runs.  It only prints
 * a marker and idles; the M85 end-of-simulation byte ends the capture.
 */

#include "millisoc_platform_r.h"
#include <stdio.h>

int main(void)
{

    UartStdOutInit();

    printf("R5: MilliSoC SRAM test companion (idle)\n");


    UartEndSimulation();
}
