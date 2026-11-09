#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "ethos_u85.h"

int main(){
    UartStdOutInit();

    printf("M85: MilliSoC U85 Tests\n");

    printf("U85 ID = 0x%08x\n", U85_Base->ID);

//load_mem memorymap.hex32 0
//writereg 00000140 FFFFFFFF

    U85_PMU->PMCR=1;
    U85_Base->POWER_CTRL=0;
    U85_Base->AXI_SRAM=0x00021f3f;
    U85_Base->AXI_EXT=0x00021f3f;
    U85_Base->QBASE=0x000000001990; // Base address needs changing
    U85_Base->QSIZE=0x110;

    U85_Base_Pointers->BASEP_ARRAY[0]=0x2ea0;
    U85_Base_Pointers->BASEP_ARRAY[1]=0;
    U85_Base_Pointers->BASEP_ARRAY[2]=0x01aa0;
    U85_Base_Pointers->BASEP_ARRAY[3]=0x60020000;

    U85_Base->CMD=0xd;

    UartEndSimulation();
}
