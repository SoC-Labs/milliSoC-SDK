#include "exectb_mcu.h"
#include "CMSDK.h"
#include "mhu.h"
#include "uart_stdout.h"
#include <stdio.h>
#include <stdint.h>

#define MHU_DOORBELL_MASK  (1UL)

static void send_r5_message(void)
{
    MHUS_PBX->PDBCW[0].SET = MHU_DOORBELL_MASK;
}

static int receive_r5_message(void)
{
    if ((MHUR_MBX->DBCH_INT_ST[0] & MHU_DOORBELL_MASK) == 0U)
        return 0;

    MHUR_MBX->MDBCW[0].CLR = MHU_DOORBELL_MASK;
    while ((MHUR_MBX->DBCH_INT_ST[0] & MHU_DOORBELL_MASK) != 0U)
    {
    }

    return 1;
}

int main(void)
{
    UartStdOutInit();
    printf("M85 reset/control test application started\n");

    send_r5_message();

    for (;;)
    {
        if (receive_r5_message())
            send_r5_message();
    }
}
