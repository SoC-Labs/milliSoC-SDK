#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "mhu.h"
#include "sys_memory_map.h"

int main(){
    UartStdOutInit();

    printf("R5: MilliSoC U85 Tests\n");


    UartEndSimulation();
}