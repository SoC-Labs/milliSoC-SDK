#include "millisoc_platform_r.h"
#include <stdio.h>


int main(){
    UartStdOutInit();

    printf("Hello SoCLabs MilliSoC\n");

    UartEndSimulation();
}
