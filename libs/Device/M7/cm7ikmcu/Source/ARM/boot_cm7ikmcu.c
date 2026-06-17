/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 *            (C) COPYRIGHT 2011-2014  ARM Limited.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 *
 *      SVN Information
 *
 *      Checked In          : $Date: 2014-02-10 14:26:41 +0000 (Mon, 10 Feb 2014) $
 *
 *      Revision            : $Revision: 269837 $
 *
 *      Release Information : CORTEX-M7-r1p2-00rel0
 *-----------------------------------------------------------------------------
 */

//
// Boot (vectors) file for Cortex-M7 Integration Kit
//

#include <stdio.h>
#include <rt_misc.h>
#include <stdint.h>
#include "cm7ikmcu.h"
#include "uart_stdout.h"

//
// Build a stand-alone image
//

#pragma import(__use_no_semihosting)


//
// Dummy Interrupt Handlers
//
// The following functions are defined weakly to allow the user
// to override them at link time simply by declaring their own
// function of the same name.
//
// If no user function is provided, the weak function is used.
//

__weak void NMI_Handler(void)
{
  while(1);
}

__weak void HardFault_Handler(void)
{
  printf("ERR: HardFault\n");
  printf("CFSR: 0x%08X\n", SCB->CFSR);
  printf("HFSR: 0x%08X\n", SCB->HFSR);
  printf("ABFSR: 0x%08X\n", SCB->ABFSR);
  printf("BFAR: 0x%08X\n", SCB->BFAR);
  printf("MMFAR: 0x%08X\n", SCB->MMFAR);

  UartEndSimulation();
}

__weak void SVC_Handler(void)
{
  while(1);
}

__weak void PendSV_Handler(void)
{
  while(1);
}

__weak void SysTick_Handler(void)
{
  while(1);
}

__weak void DMA0_IRQHandler(void)
{
  while(1);
}

__weak void DMA1_IRQHandler(void)
{
  while(1);
}
__weak void DMA2_IRQHandler(void)
{
  while(1);
}
__weak void DMA3_IRQHandler(void)
{
  while(1);
}

__weak void DMAComb_IRQHandler(void)
{
  while(1);
}


__weak void Default_IRQHandler(void)
{
  while(1);
}


__weak void QSPI_FINISHED_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_COMB_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_COMB_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_0_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_1_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_2_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_3_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_4_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_5_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_6_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_7_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_8_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_9_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_10_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_11_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_12_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_13_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_14_IRQHandler(void)
{
  while(1);
}
__weak void GPIO0_15_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_0_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_1_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_2_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_3_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_4_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_5_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_6_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_7_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_8_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_9_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_10_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_11_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_12_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_13_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_14_IRQHandler(void)
{
  while(1);
}
__weak void GPIO1_15_IRQHandler(void)
{
  while(1);
}
__weak void TIMER0_IRQHandler(void)
{
  while(1);
}
__weak void TIMER1_IRQHandler(void)
{
  while(1);
}
__weak void DUALTIMER1_IRQHandler(void)
{
  while(1);
}
__weak void DUALTIMER2_IRQHandler(void)
{
  while(1);
}
__weak void DUALTIMERCOMB_IRQHandler(void)
{
  while(1);
}
__weak void USRT0_TX_IRQHandler(void)
{
  while(1);
}
__weak void USRT0_RX_IRQHandler(void)
{
  while(1);
}
__weak void USRT0_TXOF_IRQHandler(void)
{
  while(1);
}
__weak void USRT0_RXOF_IRQHandler(void)
{
  while(1);
}
__weak void USRT0_COMB_IRQHandler(void)
{
  while(1);
}
__weak void USRT1_TX_IRQHandler(void)
{
  while(1);
}
__weak void USRT1_RX_IRQHandler(void)
{
  while(1);
}
__weak void USRT1_TXOF_IRQHandler(void)
{
  while(1);
}
__weak void USRT1_RXOF_IRQHandler(void)
{
  while(1);
}
__weak void USRT1_COMB_IRQHandler(void)
{
  while(1);
}
__weak void UART0_TX_IRQHandler(void)
{
  while(1);
}
__weak void UART0_RX_IRQHandler(void)
{
  while(1);
}
__weak void UART0_TXOF_IRQHandler(void)
{
  while(1);
}
__weak void UART0_RXOF_IRQHandler(void)
{
  while(1);
}
__weak void UART0_COMB_IRQHandler(void)
{
  while(1);
}
__weak void UART1_TX_IRQHandler(void)
{
  while(1);
}
__weak void UART1_RX_IRQHandler(void)
{
  while(1);
}
__weak void UART1_TXOF_IRQHandler(void)
{
  while(1);
}
__weak void UART1_RXOF_IRQHandler(void)
{
  while(1);
}
__weak void UART1_COMB_IRQHandler(void)
{
  while(1);
}
__weak void WATCHDOG_IRQHandler(void)
{
  while(1);
}
__weak void RTC_IRQHandler(void)
{
  while(1);
}
__weak void SPI_COMB_IRQHandler(void)
{
  while(1);
}
__weak void SPI_RX_IRQHandler(void)
{
  while(1);
}
__weak void SPI_TX_IRQHandler(void)
{
  while(1);
}
__weak void SPI_RXOF_IRQHandler(void)
{
  while(1);
}
__weak void SPI_TXOF_IRQHandler(void)
{
  while(1);
}

//
// Reset Handler
//

extern void SystemInit(void);
extern void cache_enable(void);
extern void cache_invalidate(void);
extern void fpu_enable(void);
extern void __main(void);

void Reset_Handler(void)
{
  SystemInit();
  cache_invalidate();
  cache_enable();
  fpu_enable();
  __main();
}

//
// Set up Vector Table
//

typedef void (*const vect_t)(void) __irq;

vect_t __Vectors[]
__attribute__ ((section("vectors"))) = {
  (vect_t)(0x2000FFF0),     // Top of Stack - Allowing 4 words for DEBUGDRIVERDATA
  (vect_t)Reset_Handler,    // Reset Handler
  (vect_t)NMI_Handler,      // NMI Handler
  (vect_t)HardFault_Handler,// Hard Fault Handler
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)SVC_Handler,      // SVCall Handler
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)PendSV_Handler,   // PendSV Handler
  (vect_t)SysTick_Handler,  // SysTick Handler

  // External Interrupts

  (vect_t)DMA0_IRQHandler,  // External Interrupt from DMA Channel 0 Done
  (vect_t)DMA1_IRQHandler,  // External Interrupt from DMA Channel 1 Done
  (vect_t)DMA2_IRQHandler,  // External Interrupt from DMA Channel 2 Done
  (vect_t)DMA3_IRQHandler,  // External Interrupt from DMA Channel 3 Done

  //4-7
  (vect_t)DMAComb_IRQHandler, //DMA Combined Interrupt handler
  (vect_t)QSPI_FINISHED_IRQHandler,
  (vect_t)GPIO0_COMB_IRQHandler,
  (vect_t)GPIO1_COMB_IRQHandler,
  (vect_t)GPIO0_0_IRQHandler,
  (vect_t)GPIO0_1_IRQHandler,
  (vect_t)GPIO0_2_IRQHandler,
  (vect_t)GPIO0_3_IRQHandler,
  (vect_t)GPIO0_4_IRQHandler,
  (vect_t)GPIO0_5_IRQHandler,
  (vect_t)GPIO0_6_IRQHandler,
  (vect_t)GPIO0_7_IRQHandler,
  (vect_t)GPIO0_8_IRQHandler,
  (vect_t)GPIO0_9_IRQHandler,
  (vect_t)GPIO0_10_IRQHandler,
  (vect_t)GPIO0_11_IRQHandler,
  (vect_t)GPIO0_12_IRQHandler,
  (vect_t)GPIO0_13_IRQHandler,
  (vect_t)GPIO0_14_IRQHandler,
  (vect_t)GPIO0_15_IRQHandler,
  (vect_t)GPIO1_0_IRQHandler,
  (vect_t)GPIO1_1_IRQHandler,
  (vect_t)GPIO1_2_IRQHandler,
  (vect_t)GPIO1_3_IRQHandler,
  (vect_t)GPIO1_4_IRQHandler,
  (vect_t)GPIO1_5_IRQHandler,
  (vect_t)GPIO1_6_IRQHandler,
  (vect_t)GPIO1_7_IRQHandler,
  (vect_t)GPIO1_8_IRQHandler,
  (vect_t)GPIO1_9_IRQHandler,
  (vect_t)GPIO1_10_IRQHandler,
  (vect_t)GPIO1_11_IRQHandler,
  (vect_t)GPIO1_12_IRQHandler,
  (vect_t)GPIO1_13_IRQHandler,
  (vect_t)GPIO1_14_IRQHandler,
  (vect_t)GPIO1_15_IRQHandler,
  (vect_t)TIMER0_IRQHandler,
  (vect_t)TIMER1_IRQHandler,
  (vect_t)DUALTIMER1_IRQHandler,
  (vect_t)DUALTIMER2_IRQHandler,
  (vect_t)DUALTIMERCOMB_IRQHandler,
  (vect_t)USRT0_TX_IRQHandler,
  (vect_t)USRT0_RX_IRQHandler,
  (vect_t)USRT0_TXOF_IRQHandler,
  (vect_t)USRT0_RXOF_IRQHandler,
  (vect_t)USRT0_COMB_IRQHandler,
  (vect_t)USRT1_TX_IRQHandler,
  (vect_t)USRT1_RX_IRQHandler,
  (vect_t)USRT1_TXOF_IRQHandler,
  (vect_t)USRT1_RXOF_IRQHandler,
  (vect_t)USRT1_COMB_IRQHandler,
  (vect_t)UART0_TX_IRQHandler,
  (vect_t)UART0_RX_IRQHandler,
  (vect_t)UART0_TXOF_IRQHandler,
  (vect_t)UART0_RXOF_IRQHandler,
  (vect_t)UART0_COMB_IRQHandler,
  (vect_t)UART1_TX_IRQHandler,
  (vect_t)UART1_RX_IRQHandler,
  (vect_t)UART1_TXOF_IRQHandler,
  (vect_t)UART1_RXOF_IRQHandler,
  (vect_t)UART1_COMB_IRQHandler,
  (vect_t)WATCHDOG_IRQHandler,
  (vect_t)RTC_IRQHandler,
  (vect_t)SPI_COMB_IRQHandler,
  (vect_t)SPI_RX_IRQHandler,
  (vect_t)SPI_TX_IRQHandler,
  (vect_t)SPI_RXOF_IRQHandler,
  (vect_t)SPI_TXOF_IRQHandler
};


//
// Set up initial stack and heap
//

__value_in_regs struct __initial_stackheap
__user_initial_stackheap(unsigned hb, unsigned sb, unsigned hl, unsigned sl)
{
  struct __initial_stackheap s;

  s.heap_base   = hb;
  s.stack_base  = sb;
  s.heap_limit  = s.stack_base;
  s.stack_limit = s.heap_base;
  return s;
}


//
// Default exit function
//

__weak void _sys_exit(int return_code)
{
  while(1);
}
