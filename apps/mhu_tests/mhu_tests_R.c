#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "mhu.h"
#include "sys_memory_map.h"


int main(){
    UartStdOutInit();

    printf("R5: MilliSoC MHU Tests\n");

    printf("MHU SND 0x%02x \n", MHUS_PBX->PIDR0);
    printf("MHU SND 0x%02x \n", *(volatile uint32_t *) (MHU_SND+0x10fe0));
    printf("MHU REC 0x%02x \n", MHUR_MBX->PIDR0);
    printf("MHU REC 0x%02x \n", *(volatile uint32_t *) (MHU_REC+0x10fe0));

    // Send Doorbell
    MHUS_PBX->PDBCW[0].SET=1;
    UartEndSimulation();
}

