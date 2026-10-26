#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "ethos_u85.h"

int main(){
    UartStdOutInit();

    printf("M85: MilliSoC U85 Tests\n");

    printf("U85 ID = 0x%08x\n", U85_Base->ID);

    UartEndSimulation();
}
