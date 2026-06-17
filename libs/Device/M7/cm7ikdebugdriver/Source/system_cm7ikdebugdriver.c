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
 *      Checked In          : $Date: 2014-04-02 11:23:31 +0100 (Wed, 02 Apr 2014) $
 *
 *      Revision            : $Revision: 275744 $
 *
 *      Release Information : CORTEX-M7-r1p2-00rel0
 *-----------------------------------------------------------------------------
 */

#include <stdint.h>
#include "cm7ikdebugdriver.h"

/*----------------------------------------------------------------------------
  Define SYSCLK
 *----------------------------------------------------------------------------*/
#define __HSI (5000000UL)

/*----------------------------------------------------------------------------
  Clock Definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemFrequency  = 50000000UL;   /*!< System Clock Frequency (Core Clock) */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *
 */
__asm void SystemInit (void)
{
  //
  // SystemInit
  // Initialise all registers to zero
  // Add PLL initialisation etc.
  //
  THUMB;

  MOV  r1, #0x0;
  MOV  r2, #0x0;
  MOV  r3, #0x0;
  MOV  r4, #0x0;
  MOV  r5, #0x0;
  MOV  r6, #0x0;
  MOV  r7, #0x0;
  MOV  r8, #0x0;
  MOV  r9, #0x0;
  MOV  r10,#0x0;
  MOV  r11,#0x0;
  MOV  r12,#0x0;
  MOV  r0, #0x30;
  LSL  r0,r0, #24;
  MSR  PSP, r0;  
  MOV  r0, #0x0;
  BX   lr;
}

/**
 * Enable Caches
 *
 * @param none
 * @return none
 *
 * @brief Enable the Caches
 *
 */
void cache_enable(void)
{
  //
  // cache_enable
  //

  SCB_EnableICache();
  SCB_EnableDCache();
}

/**
 * Invalidate Caches
 *
 * @param none
 * @return none
 *
 * @brief Invalidate the Caches
 *
 */
void cache_invalidate(void)
{
  //
  // cache_invalidate
  //

  SCB_InvalidateICache();
  SCB_InvalidateDCache();
}

void fpu_enable(void)
{
  //
  // fpu_enable
  //

  uint32_t value;

  value = SCB->CPACR;
  SCB->CPACR = value | 0x00F00000;
  __ISB();
  
}
