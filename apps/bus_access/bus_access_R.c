#include "millisoc_platform_r.h"
#include <stdio.h>

#define TEST_SIZE 0x1000
#define TEST_OFFSET 0x1000

int main(){
    UartStdOutInit();

    printf("R5: MilliSoC Bus Access Tests\n");


    UartEndSimulation();
}