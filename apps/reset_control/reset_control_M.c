#include "millisoc_platform_m.h"
#include "mhu.h"
#include <stdio.h>
#include <stdint.h>

#define MHU_DOORBELL_MASK  (1UL)
#define MHU_TEST_DONE_MASK (1UL << 1)

static void send_r5_message(void)
{
    MHUS_PBX->PDBCW[0].SET = MHU_DOORBELL_MASK;
}

static int receive_r5_message(void)
{
    uint32_t status = MHUR_MBX->DBCH_INT_ST[0];
    uint32_t mask;

    if ((status & MHU_TEST_DONE_MASK) != 0U)
        mask = MHU_TEST_DONE_MASK;
    else if ((status & MHU_DOORBELL_MASK) != 0U)
        mask = MHU_DOORBELL_MASK;
    else
        return 0;

    MHUR_MBX->MDBCW[0].CLR = mask;
    while ((MHUR_MBX->DBCH_INT_ST[0] & mask) != 0U)
    {
    }

    return (mask == MHU_TEST_DONE_MASK) ? 2 : 1;
}

int main(void)
{
    UartStdOutInit();
    printf("M85 reset/control test application started\n");

    send_r5_message();

    for (;;)
    {
        int message = receive_r5_message();

        if (message == 2)
        {
            printf("M85 reset/control test complete\n");
            UartEndSimulation();
            return 0;
        }
        if (message == 1)
            send_r5_message();
    }
}
