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

#ifndef __CM7IKDEBUGDRIVER_H__
#define __CM7IKDEBUGDRIVER_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex M7 Processor Exceptions Numbers ****************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2  Non Maskable Interrupt                             */
  HardFault_IRQn	      = -13,    /*!< 3  Cortex M7 Hard Fault Interrupt                     */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex M7 SV Call Interrupt                        */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex M7 Pend SV Interrupt                        */
  SysTick_IRQn                = -1,     /*!< 15 Cortex M7 System Tick Interrupt                    */

/******  CM7IKMCU Cortex M7 specific Interrupt Numbers *********************************************/
  GPIO_IRQn                   = 0       /*!< GPIO Interrupt                                        */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex M7 Processor and Core Peripherals */
#define __MPU_PRESENT           0       /*!< cm7ikmcu does not provide a MPU present or not        */
#define __NVIC_PRIO_BITS        2       /*!< cm7ikmcu Supports 2 Bits for the Priority Levels      */
#define __Vendor_SysTickConfig  0       /*!< Set to 1 if different SysTick Config is used          */
#define __ICACHE_PRESENT        1       /*!< Set to 1 if I-Cache is present                        */
#define __DCACHE_PRESENT        1       /*!< Set to 1 if D-Cache is present                        */
#define __FPU_PRESENT           1       /*!< Set to 1 if FPU is present                            */


#include "core_cm7.h"                  /* Cortex M7 processor and core peripherals                 */
#include "system_cm7ikdebugdriver.h"   /* cm7ikdebugdriver System                                  */



/**
 * Initialize the system clock
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialize the PLL and update the SystemFrequency variable
 */
extern void SystemInit (void);

/**
 * Enable Caches
 */
extern void cache_enable(void);

/**
 * Invalidate Caches
 */
extern void cache_invalidate(void);


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

/*--------------------- General Purpose Input and Ouptut ---------------------*/
typedef union
{
  __IO uint32_t WORD;
  __IO uint16_t HALFWORD[2];
  __IO uint8_t  BYTE[4];
} GPIO_Data_TypeDef;

typedef struct
{
  GPIO_Data_TypeDef DATA [256];
  GPIO_Data_TypeDef DIR;
  uint32_t RESERVED[3];
  GPIO_Data_TypeDef IE;
} GPIO_TypeDef;

/*------------------------- Trace Port Capture Unit ---------------------------*/
typedef struct
{
  __IO uint32_t CONFIG;
  __I  uint32_t DATA;
  __I  uint32_t DEPTH;
  __I  uint32_t SYNCS;
  __I  uint32_t HSYNCS;
  __I  uint32_t STATUS;

} TRACE_TypeDef;

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* CoreSight Components */
#define TPIU_BASE                 0xE0040000UL
#define ETM_BASE                  0xE0041000UL
#define CTI_BASE                  0xE0042000UL

/* Peripheral and SRAM base address */
#define SRAM_BASE                 0x20000000UL
#define PERIPH_BASE               0x40000000UL

/* Peripheral memory map */
#define GPIO_BASE	          PERIPH_BASE

#define GPIO0_BASE	          (GPIO_BASE)
#define TRACE_BASE                (GPIO_BASE       + 0x0800UL)

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define GPIO0		          ((GPIO_TypeDef  *) GPIO0_BASE)
#define TRACE                     ((TRACE_TypeDef *) TRACE_BASE)

#endif
