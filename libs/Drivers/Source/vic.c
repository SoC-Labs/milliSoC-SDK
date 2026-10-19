#include "vic.h"


void vic_initialisation(int irq, void (*vect_addr)){
    vic_disable_irq(irq);
    // set vector address
    VIC_VA->VectAddr[irq]=(uint32_t)vect_addr;
    // set vector priority
    VIC_VP->VectPriority[irq]=0;
    // set irq source
    // enable interrupts
    vic_enable_irq(irq);
    return;
}

void vic_enable_irq(uint32_t irq){
    VIC->IntEnable |= (1<<irq);
    return;
}

void vic_disable_irq(uint32_t irq){
    VIC->IntEnClear |= (1<<irq);
    return;
}


void IRQInterrupt(void){
    int source;
    

    return;
}

void irq_handler(void){
	__asm volatile("stmdb	sp!,{r0-r3,r12,lr}");		// state save from compiled code
	__asm volatile("bl	IRQInterrupt");			// IRQ vector
	__asm volatile("ldmia	sp!,{r0-r3,r12,lr}	");	// state restore from compiled code
	__asm volatile("subs	pc, lr, #4");			// adjust return
}