#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>

int main(){
    int PID;
    UartStdOutInit();

    printf("Hello SoCLabs MilliSoC\n");

    printf("MHU SND 0x%08x \n", *(volatile uint32_t *) (MHU_SND+0xfe0));
    printf("MHU SND 0x%08x \n", *(volatile uint32_t *) (MHU_SND+0x10fe0));
    printf("MHU REC 0x%08x \n", *(volatile uint32_t *) (MHU_REC+0xfe0));
    printf("MHU REC 0x%08x \n", *(volatile uint32_t *) (MHU_REC+0x10fe0));

    UartEndSimulation();
}

