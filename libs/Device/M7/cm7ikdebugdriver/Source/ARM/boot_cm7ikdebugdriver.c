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
// Boot (vectors) file for CORTEX-M7 Integration Kit Debug Driver
//

#include <stdio.h>
#include <rt_misc.h>
#include <stdint.h>
#include "cm7ikdebugdriver.h"

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
  while(1);
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

__weak void GPIO_IRQHandler(void)
{
  while(1);
}

__weak void Default_IRQHandler(void)
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
  (vect_t)(0x20100000),     // Top of Stack
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

  (vect_t)GPIO_IRQHandler,  // External Interrupt from GPIO

  // External Interrupts 1 - 32
  // These are unused, so will all
  // take the same default handler if invoked.

  // 1-3
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //4-7
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //8-11
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //12-15
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //16-19
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //20-23
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //24-27
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,

  //28-31
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler,
  (vect_t)Default_IRQHandler
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

