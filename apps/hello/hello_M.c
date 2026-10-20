#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>


int main(){
    UartStdOutInit();

    printf("Hello SoCLabs MilliSoC\n");

    UartEndSimulation();
}

