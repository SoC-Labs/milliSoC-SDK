#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "mhu.h"

int main(){
    int PID;
    UartStdOutInit();

    printf("M85: MilliSoC MHU Tests\n");
    
    NVIC_EnableIRQ(IPC_SND_ERR_IRQn);
    NVIC_EnableIRQ(IPC_SND_FLT_IRQn);
    NVIC_EnableIRQ(IPC_SND_PBX_IRQn);
    NVIC_EnableIRQ(IPC_REC_FST_IRQn);
    NVIC_EnableIRQ(IPC_REC_ERR_IRQn);
    NVIC_EnableIRQ(IPC_REC_FLT_IRQn);
    NVIC_EnableIRQ(IPC_REC_MBX_IRQn);
    __enable_irq();
    printf("MHU SND 0x%02x \n", MHUS_PBX->PIDR0);
    printf("MHU SND 0x%02x \n", *(volatile uint32_t *) (MHU_SND+0x10fe0));
    printf("MHU REC 0x%02x \n", MHUR_MBX->PIDR0);
    printf("MHU REC 0x%02x \n", *(volatile uint32_t *) (MHU_REC+0x10fe0));
    __WFI();
    printf("Got doorbell\n");
    UartEndSimulation();
}


void IPC_SND_ERRHandler(void){
    printf("IPC_SND_ERR\n");
    while(1);
}
void IPC_SND_FLTHandler(void){
    printf("IPC SND FLT\n");
    while(1);
}
void IPC_SND_PBXHandler(void){
    printf("IPC SND PBX \n");
    while(1);
}
void IPC_REC_FSTHandler(void){
    printf("IPC REC FST\n");
    while(1);
}
void IPC_REC_ERRHandler(void){
    printf("IPC REC ERR\n");
    while(1);
}
void IPC_REC_FLTHandler(void){
    printf("IPC REC FLT\n");
    while(1);
}
void IPC_REC_MBXHandler(void){
    __disable_irq();
    uint32_t DB_ST;
    DB_ST=MHUR_MBX->DBCH_INT_ST[0];
    if(DB_ST&0x1){
        // bit 0 set
        MHUR_MBX->MDBCW[0].CLR=1;
        while(MHUR_MBX->DBCH_INT_ST[0]&0x1){;} // Wait for actual clear
    } else{
        printf("IPX MBX err\n");
        UartEndSimulation();
    }
    __enable_irq();
}

