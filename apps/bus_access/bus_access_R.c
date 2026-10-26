#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "mhu.h"
#include "sys_memory_map.h"

#define SRAM_BASE 0x20000000 
#define TEST_SIZE 0x1000
#define TEST_OFFSET 0x1000

int main(){
    UartStdOutInit();

    printf("R5: MilliSoC Bus Access Tests\n");


    UartEndSimulation();
}