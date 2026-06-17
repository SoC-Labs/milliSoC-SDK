#include "cm7ikmcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>


int main(){
    UartStdOutInit();

    printf("Hello SoCLabs MilliSoC\n");
    TEST_PASS();
}

