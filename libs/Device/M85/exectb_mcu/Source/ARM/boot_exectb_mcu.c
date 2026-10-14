//------------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from Arm Limited or its affiliates.
//
// (C) COPYRIGHT 2020-2022 Arm Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Arm Limited or its affiliates.
//
//  Release Information : Cortex-M85-r1p0-00rel0
//------------------------------------------------------------------------------

//
// Boot (vectors) file for Olympus Execution Testbench
//

#include <stdio.h>
#include <stdint.h>
#include "exectb_mcu.h"

//
// Build a stand-alone image
//

//
// Dummy Interrupt Handlers
//
// The following functions are defined weakly to allow the user
// to override them at link time simply by declaring their own
// function of the same name.
//
// If no user function is provided, the weak function is used.
//

__attribute__((weak)) void NMI_Handler(void)
{
  while(1);
}

__attribute__((weak)) void HardFault_Handler(void)
{
  while(1);
}

__attribute__((weak)) void UsageFault_Handler(void)
{
  while(1);
}

__attribute__((weak)) void SVC_Handler(void)
{
  while(1);
}

__attribute__((weak)) void DebugMon_Handler(void)
{
  while(1);
}

__attribute__((weak)) void PendSV_Handler(void)
{
  while(1);
}

__attribute__((weak)) void SysTick_Handler(void)
{
  while(1);
}

__attribute__((weak)) void Default_IRQHandler(void)
{
  while(1);
}

__attribute__((weak)) void DMA0_IRQHandler(void)
{
  while(1);
}

__attribute__((weak)) void DMA1_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void DMA2_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void DMA3_IRQHandler(void)
{
  while(1);
}

__attribute__((weak)) void DMAComb_IRQHandler(void)
{
  while(1);
}


__attribute__((weak)) void QSPI_FINISHED_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_0_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_1_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_2_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_3_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_4_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_5_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_6_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_7_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_8_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_9_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_10_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_11_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_12_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_13_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_14_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO0_15_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_0_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_1_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_2_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_3_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_4_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_5_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_6_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_7_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_8_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_9_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_10_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_11_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_12_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_13_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_14_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void GPIO1_15_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void TIMER0_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void TIMER1_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void DUALTIMER1_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void DUALTIMER2_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void DUALTIMERCOMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT0_TX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT0_RX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT0_TXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT0_RXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT0_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT1_TX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT1_RX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT1_TXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT1_RXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void USRT1_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART0_TX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART0_RX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART0_TXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART0_RXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART0_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART1_TX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART1_RX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART1_TXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART1_RXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void UART1_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void WATCHDOG_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void RTC_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void SPI_COMB_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void SPI_RX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void SPI_TX_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void SPI_RXOF_IRQHandler(void)
{
  while(1);
}
__attribute__((weak)) void SPI_TXOF_IRQHandler(void)
{
  while(1);
}


//
// Reset Handler
//

extern void cache_enable(void);
extern void LPStateInit(void);
extern void fpu_enable(void);
extern void __main(void);

void Reset_Handler(void)
{

  #ifndef NS_IMAGE
  SystemInit();
  #endif // NS_IMAGE
  #ifdef BOOTROM
  LPStateInit();
  #ifndef NS_IMAGE
  #if ((defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)) || \
      (defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)))
  cache_enable();
  #endif /* #if Cache present */
  fpu_enable();
  #endif // NS_IMAGE
  #endif
  __main();
}


//
// Set up Vector Table
//

extern unsigned int Image$$ARM_LIB_STACK$$ZI$$Limit;

typedef void (*const vect_t)(void);

vect_t __Vectors[]
__attribute__ ((section("vectors"))) = {
  (vect_t)&Image$$ARM_LIB_STACK$$ZI$$Limit, // Top of Stack (SRAM limit)
  (vect_t)Reset_Handler,    // Reset Handler
  (vect_t)NMI_Handler,      // NMI Handler
  (vect_t)HardFault_Handler,// Hard Fault Handler
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)UsageFault_Handler, // UsageFault Handler
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  0,                        // Reserved
  (vect_t)SVC_Handler,      // SVCall Handler
  (vect_t)DebugMon_Handler, // Debug Monitor Handler
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
// Default exit function
//

__attribute__((weak)) void _sys_exit(int return_code)
{
  while(1);
}

