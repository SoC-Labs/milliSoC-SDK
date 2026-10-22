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

#ifndef __EXECTBMCU_H__
#define __EXECTBMCU_H__

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */
  MemoryManagement_IRQn         = -12,     /*  4 Memory Management Interrupt */
  BusFault_IRQn                 = -11,     /*  5 Bus Fault Interrupt */
  UsageFault_IRQn               = -10,     /*  6 Usage Fault Interrupt */
  SecureFault_IRQn              =  -9,     /*  7 Secure Fault Interrupt */
  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */
  DebugMonitor_IRQn             =  -4,     /* 12 Debug Monitor Interrupt */
  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  DMA_DONE_2_IRQn             = 0,      /*!< System DMA channel 2 done Interrupt                */
  DMA_DONE_3_IRQn             = 1,      /*!< System DMA channel 3 done Interrupt                */
  IPC_SND_ERR_IRQn            = 2,      /*!<                 */
  IPC_SND_FLT_IRQn            = 3,      /*!<                 */
  IPC_SND_PBX_IRQn            = 4,      /*!<                 */
  IPC_REC_FST_IRQn            = 5,      /*!<                 */
  IPC_REC_ERR_IRQn            = 6,      /*!<                 */
  IPC_REC_FLT_IRQn            = 7,      /*!<                 */
  IPC_REC_MBX_IRQn            = 8,      /*!<                 */
  GPIO0_COMB_IRQn             = 9,      /*!< GPIO Port 0 combined intterupt                     */
  GPIO1_COMB_IRQn             = 10,     /*!< GPIO Port 1 combined intterupt                     */
  GPIO0_0_IRQn                = 11,     /*!< GPIO Port 0 Pin 0 intterupt                        */
  GPIO0_1_IRQn                = 12,     /*!< GPIO Port 0 Pin 1 intterupt                        */
  GPIO0_2_IRQn                = 13,     /*!< GPIO Port 0 Pin 2 intterupt                        */
  GPIO0_3_IRQn                = 14,     /*!< GPIO Port 0 Pin 3 intterupt                        */
  GPIO0_4_IRQn                = 15,     /*!< GPIO Port 0 Pin 4 intterupt                        */
  GPIO0_5_IRQn                = 16,     /*!< GPIO Port 0 Pin 5 intterupt                        */
  GPIO0_6_IRQn                = 17,     /*!< GPIO Port 0 Pin 6 intterupt                        */
  GPIO0_7_IRQn                = 18,     /*!< GPIO Port 0 Pin 7 intterupt                        */
  GPIO0_8_IRQn                = 19,     /*!< GPIO Port 0 Pin 8 intterupt                        */
  GPIO0_9_IRQn                = 20,     /*!< GPIO Port 0 Pin 9 intterupt                        */
  GPIO0_10_IRQn               = 21,     /*!< GPIO Port 0 Pin 10 intterupt                       */
  GPIO0_11_IRQn               = 22,     /*!< GPIO Port 0 Pin 11 intterupt                       */
  GPIO0_12_IRQn               = 23,     /*!< GPIO Port 0 Pin 12 intterupt                       */
  GPIO0_13_IRQn               = 24,     /*!< GPIO Port 0 Pin 13 intterupt                       */
  GPIO0_14_IRQn               = 25,     /*!< GPIO Port 0 Pin 14 intterupt                       */
  GPIO0_15_IRQn               = 26,     /*!< GPIO Port 0 Pin 15 intterupt                       */
  GPIO1_0_IRQn                = 27,     /*!< GPIO Port 1 Pin 0 intterupt                        */
  GPIO1_1_IRQn                = 28,     /*!< GPIO Port 1 Pin 1 intterupt                        */
  GPIO1_2_IRQn                = 29,     /*!< GPIO Port 1 Pin 2 intterupt                        */
  GPIO1_3_IRQn                = 30,     /*!< GPIO Port 1 Pin 3 intterupt                        */
  GPIO1_4_IRQn                = 31,     /*!< GPIO Port 1 Pin 4 intterupt                        */
  GPIO1_5_IRQn                = 32,     /*!< GPIO Port 1 Pin 5 intterupt                        */
  GPIO1_6_IRQn                = 33,     /*!< GPIO Port 1 Pin 6 intterupt                        */
  GPIO1_7_IRQn                = 34,     /*!< GPIO Port 1 Pin 7 intterupt                        */
  GPIO1_8_IRQn                = 35,     /*!< GPIO Port 1 Pin 8 intterupt                        */
  GPIO1_9_IRQn                = 36,     /*!< GPIO Port 1 Pin 9 intterupt                        */
  GPIO1_10_IRQn               = 37,     /*!< GPIO Port 1 Pin 10 intterupt                       */
  GPIO1_11_IRQn               = 38,     /*!< GPIO Port 1 Pin 11 intterupt                       */
  GPIO1_12_IRQn               = 39,     /*!< GPIO Port 1 Pin 12 intterupt                       */
  GPIO1_13_IRQn               = 40,     /*!< GPIO Port 1 Pin 13 intterupt                       */
  GPIO1_14_IRQn               = 41,     /*!< GPIO Port 1 Pin 14 intterupt                       */
  GPIO1_15_IRQn               = 42,     /*!< GPIO Port 1 Pin 15 intterupt                       */
  TIMER0_IRQn                 = 43,     /*!< Timer 0 interrupt                                  */
  TIMER1_IRQn                 = 44,     /*!< Timer 1 interrupt                                  */
  DUALTIMER1_IRQn             = 45,     /*!< Dual Timer 1 interrupt                             */
  DUALTIMER2_IRQn             = 46,     /*!< Dual Timer 2 interrupt                             */
  DUALTIMERCOMB_IRQn          = 47,     /*!< Dual Timer Combined interrupt                      */
  UART0_TX_IRQn               = 48,     /*!< UART 0 TX interrupt                                */
  UART0_RX_IRQn               = 49,     /*!< UART 0 RX interrupt                                */
  UART0_TXOF_IRQn             = 50,     /*!< UART 0 TX Overflow interrupt                       */
  UART0_RXOF_IRQn             = 51,     /*!< UART 0 RX Overflow interrupt                       */
  UART0_COMB_IRQn             = 52,     /*!< UART 0 Combined interrupt                          */
  UART1_TX_IRQn               = 53,     /*!< UART 1 TX interrupt                                */
  UART1_RX_IRQn               = 54,     /*!< UART 1 RX interrupt                                */
  UART1_TXOF_IRQn             = 55,     /*!< UART 1 TX Overflow interrupt                       */
  UART1_RXOF_IRQn             = 56,     /*!< UART 1 RX Overflow interrupt                       */
  UART1_COMB_IRQn             = 57,     /*!< UART 1 Combined interrupt                          */
  WATCHDOG_IRQn               = 58,     /*!< Watchdog interrupt                                 */
  RTC_IRQn                    = 59,     /*!< Real Time Counter interrupt                        */
  SPI_COMB_IRQn               = 60,     /*!< SPI Combined interrupt                             */
  SPI_RX_IRQn                 = 61,     /*!< SPI RX interrupt                                   */
  SPI_TX_IRQn                 = 62,     /*!< SPI TX interrupt                                   */
  SPI_RXOF_IRQn               = 63,     /*!< SPI RX Overflow interrupt                          */
  SPI_TXOF_IRQn               = 64      /*!< SPI TX OVerflow interrupt                          */

/* Interrupts 10 .. 480 are left out */
} IRQn_Type;

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

// Configuration of the Olympus Processor and Core Peripherals
#define __OLYMPUS_REV               0x0000    // Core Revision r0p0
#define __MPU_PRESENT             1         // Set to 1 if MPU is present
#define __PMU_PRESENT             1         // Set to 1 if PMU is present
#define __PMU_NUM_EVENTCNT        1         // Set to 1 if PMU is present
#define __VTOR_PRESENT            1         // Set to 1 if VTOR is present
#define __NVIC_PRIO_BITS          3         // Number of Bits used for Priority Levels
#define __Vendor_SysTickConfig    0         // Set to 1 if different SysTick Config is used
#define __FPU_PRESENT             1         // Set to 1 if FPU is present
#define __FPU_DP                  0         // Set to 1 if FPU is double precision FPU (default is single precision FPU)
//#define __ICACHE_PRESENT          0         // Set to 1 if I-Cache is present
//#define __DCACHE_PRESENT          0         // Set to 1 if D-Cache is present
//#define __DTCM_PRESENT            0         // Set to 1 if DTCM is present
#define __DSP_PRESENT             1         // Set to 1 if DSP is present

#define __SAU_PRESENT             1         // exectb_mcu can support an SAU
#define __SAUREGION_PRESENT       1         // exectb_mcu can support an SAU

#ifdef _DCACHE_PRESENT
  #define __DCACHE_PRESENT      1           // Set to 1 if D-Cache is present
#else
  #define __DCACHE_PRESENT      0           // Set to 1 if D-Cache is present
#endif

#ifdef _ICACHE_PRESENT
  #define __ICACHE_PRESENT      1           // Set to 1 if I-Cache is present
#else
  #define __ICACHE_PRESENT      0           // Set to 1 if I-Cache is present
#endif
#define __ARM_FEATURE_CMSE      3

#include "core_cm85.h"                     // Arm Olympus processor and core peripherals
#include "system_exectb_mcu.h"              // exectb_mcu System

/* ========================================  Start of section using anonymous unions  ======================================== */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/*
 * Initialize the system clock
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialize the PLL and update the SystemFrequency variable
 */
extern void SystemInit (void);

/******************************************************************************/
/*                      Olympus ETM registers structures                        */
/******************************************************************************/

/*----------------------- Embedded Trace Macrocell ---------------------------*/
typedef struct
{
       uint32_t RESERVED0;
  __IO uint32_t TRCPRGCTLR;                  /*!< Offset: 0x004  ETM Programming Control Register                                        */
  __IO uint32_t TRCPROCSELR;                 /*!< Offset: 0x008  ETM PE Select Control Register                                          */
  __I  uint32_t TRCSTATR;                    /*!< Offset: 0x00C  ETM Trace Status Register                                               */
  __IO uint32_t TRCCONFIGR;                  /*!< Offset: 0x010  ETM Trace Configuration Register                                        */
       uint32_t RESERVED1;
  __IO uint32_t TRCAUXCTLR;                  /*!< Offset: 0x018  ETM Auxiliary Control Register                                          */
       uint32_t RESERVED2;
  __IO uint32_t TRCEVENTCTL0R;               /*!< Offset: 0x020  ETM Event Control Register 0                                            */
  __IO uint32_t TRCEVENTCTL1R;               /*!< Offset: 0x024  ETM Event Control Register 1                                            */
       uint32_t RESERVED3;
  __IO uint32_t TRCSTALLCTLR;                /*!< Offset: 0x02C  ETM Stall Control Register                                              */
  __IO uint32_t TRCTSCTLR;                   /*!< Offset: 0x030  ETM Global Timestamp Control Register                                   */
  __IO uint32_t TRCSYNCPR;                   /*!< Offset: 0x034  ETM Synchronization Period Register                                     */
  __IO uint32_t TRCCCCTLR;                   /*!< Offset: 0x038  ETM Cycle Count Control Register                                        */
  __IO uint32_t TRCBBCTLR;                   /*!< Offset: 0x03C  ETM Branch Broadcast Control Register                                   */
  __IO uint32_t TRCTRACEIDR;                 /*!< Offset: 0x040  ETM Q Element Control Register                                          */
  __IO uint32_t TRCQCTLR;                    /*!< Offset: 0x044                                                                          */
       uint32_t RESERVED4[14];
  __IO uint32_t TRCVICTLR;                   /*!< Offset: 0x080  ETM ViewInst Main Control Register                                      */
  __IO uint32_t TRCVIIECTLR;                 /*!< Offset: 0x084  ETM ViewInst Include/Exclude Control Register                           */
  __IO uint32_t TRCVISSCTLR;                 /*!< Offset: 0x088  ETM ViewInst Start/Stop Control Register                                */
  __IO uint32_t TRCVIPCSSCTLR;               /*!< Offset: 0x08C  ETM ViewInst Start/Stop PE Comparator Control Register                  */
       uint32_t RESERVED5[4];
  __IO uint32_t TRCVDCTLR;                   /*!< Offset: 0x0A0  ETM ViewData Main Control Register                                      */
  __IO uint32_t TRCVDSACCTLR;                /*!< Offset: 0x0A4  ETM ViewData Include/Exclude Single Address Comparator Control Register */
  __IO uint32_t TRCVDARCCTLR;                /*!< Offset: 0x0A8  ETM ViewData Include/Exclude Address Range Comparator Control Register  */
       uint32_t RESERVED6[21];
  __IO uint32_t TRCSEQEVR0;                  /*!< Offset: 0x100  ETM Sequencer State Transition Control Register 0                       */
  __IO uint32_t TRCSEQEVR1;                  /*!< Offset: 0x104  ETM Sequencer State Transition Control Register 1                       */
  __IO uint32_t TRCSEQEVR2;                  /*!< Offset: 0x108  ETM Sequencer State Transition Control Register 2                       */
       uint32_t RESERVED7[3];
  __IO uint32_t TRCSEQRSTEVR;                /*!< Offset: 0x118  ETM Sequencer Reset Control Register                                    */
  __IO uint32_t TRCSEQSTR;                   /*!< Offset: 0x11C  ETM Sequencer State Register                                            */
  __IO uint32_t TRCEXTINSELR;                /*!< Offset: 0x120  ETM External Input Select Register                                      */
       uint32_t RESERVED8[7];
  __IO uint32_t TRCCNTRLDVR0;                /*!< Offset: 0x140  ETM Counter Reload Value Register 0                                     */
  __IO uint32_t TRCCNTRLDVR1;                /*!< Offset: 0x144  ETM Counter Reload Value Register 1                                     */
       uint32_t RESERVED9[2];
  __IO uint32_t TRCCNTCTLR0;                 /*!< Offset: 0x150  ETM Counter Control Register 0                                          */
  __IO uint32_t TRCCNTCTLR1;                 /*!< Offset: 0x154  ETM Counter Control Register 1                                          */
       uint32_t RESERVED10[2];
  __IO uint32_t TRCCNTVR0;                   /*!< Offset: 0x160  ETM Counter Value Register 0                                            */
  __IO uint32_t TRCCNTVR1;                   /*!< Offset: 0x164  ETM Counter Value Register 1                                            */
       uint32_t RESERVED11[6];
  __I  uint32_t TRCIDR8;                     /*!< Offset: 0x180  ETM ID Register 8                                                       */
  __I  uint32_t TRCIDR9;                     /*!< Offset: 0x184  ETM ID Register 9                                                       */
  __I  uint32_t TRCIDR10;                    /*!< Offset: 0x188  ETM ID Register 10                                                      */
  __I  uint32_t TRCIDR11;                    /*!< Offset: 0x18C  ETM ID Register 11                                                      */
  __I  uint32_t TRCIDR12;                    /*!< Offset: 0x190  ETM ID Register 12                                                      */
  __I  uint32_t TRCIDR13;                    /*!< Offset: 0x194  ETM ID Register 13                                                      */
       uint32_t RESERVED12[18];
  __I  uint32_t TRCIDR0;                     /*!< Offset: 0x1E0  ETM ID Register 0                                                       */
  __I  uint32_t TRCIDR1;                     /*!< Offset: 0x1E4  ETM ID Register 1                                                       */
  __I  uint32_t TRCIDR2;                     /*!< Offset: 0x1E8  ETM ID Register 2                                                       */
  __I  uint32_t TRCIDR3;                     /*!< Offset: 0x1EC  ETM ID Register 3                                                       */
  __I  uint32_t TRCIDR4;                     /*!< Offset: 0x1F0  ETM ID Register 4                                                       */
  __I  uint32_t TRCIDR5;                     /*!< Offset: 0x1F4  ETM ID Register 5                                                       */
  __I  uint32_t TRCIDR6;                     /*!< Offset: 0x1F8  ETM ID Register 6                                                       */
  __I  uint32_t TRCIDR7;                     /*!< Offset: 0x1FC  ETM ID Register 7                                                       */
       uint32_t RESERVED13[2];
  __IO uint32_t TRCRSCTLR2;                  /*!< Offset: 0x208  ETM Resource Selection Control Register 2                               */
  __IO uint32_t TRCRSCTLR3;                  /*!< Offset: 0x20C  ETM Resource Selection Control Register 3                               */
  __IO uint32_t TRCRSCTLR4;                  /*!< Offset: 0x210  ETM Resource Selection Control Register 4                               */
  __IO uint32_t TRCRSCTLR5;                  /*!< Offset: 0x214  ETM Resource Selection Control Register 5                               */
  __IO uint32_t TRCRSCTLR6;                  /*!< Offset: 0x218  ETM Resource Selection Control Register 6                               */
  __IO uint32_t TRCRSCTLR7;                  /*!< Offset: 0x21C  ETM Resource Selection Control Register 7                               */
  __IO uint32_t TRCRSCTLR8;                  /*!< Offset: 0x220  ETM Resource Selection Control Register 8                               */
  __IO uint32_t TRCRSCTLR9;                  /*!< Offset: 0x224  ETM Resource Selection Control Register 9                               */
  __IO uint32_t TRCRSCTLR10;                 /*!< Offset: 0x228  ETM Resource Selection Control Register 10                              */
  __IO uint32_t TRCRSCTLR11;                 /*!< Offset: 0x22C  ETM Resource Selection Control Register 11                              */
  __IO uint32_t TRCRSCTLR12;                 /*!< Offset: 0x230  ETM Resource Selection Control Register 12                              */
  __IO uint32_t TRCRSCTLR13;                 /*!< Offset: 0x234  ETM Resource Selection Control Register 13                              */
  __IO uint32_t TRCRSCTLR14;                 /*!< Offset: 0x238  ETM Resource Selection Control Register 14                              */
  __IO uint32_t TRCRSCTLR15;                 /*!< Offset: 0x23C  ETM Resource Selection Control Register 15                              */
       uint32_t RESERVED14[16];
  __IO uint32_t TRCSSCCR0;                   /*!< Offset: 0x280  ETM Single-Shot Comparator Control Register 0                           */
       uint32_t RESERVED15[7];
  __IO uint32_t TRCSSCSR0;                   /*!< Offset: 0x2A0  ETM Single-Shot Comparator Status Register 0                            */
       uint32_t RESERVED16[7];
  __IO uint32_t TRCSSPCICR0;                 /*!< Offset: 0x2C0  ETM Single-Shot PE Comparator Input Control Register 0                  */
       uint32_t RESERVED17[15];
  __O  uint32_t TRCOSLAR;                    /*!< ETM OS Lock Access Register                                                            */
  __I  uint32_t TRCOSLSR;                    /*!< ETM OS Lock Status Register                                                            */
       uint32_t RESERVED18[2];
  __IO uint32_t TRCPDCR;                     /*!< Offset: 0x310  ETM Power Down Control Register                                         */
  __I  uint32_t TRCPDSR;                     /*!< Offset: 0x314  ETM Power Down Status Register                                          */
       uint32_t RESERVED19[58];
  __IO uint32_t TRCACVR0;                    /*!< Offset: 0x410  ETM Address Comparator Value Register 0                                 */
       uint32_t RESERVED20;
  __IO uint32_t TRCACVR1;                    /*!< ETM Address Comparator Value Register 1                                                */
       uint32_t RESERVED21;
  __IO uint32_t TRCACVR2;                    /*!< ETM Address Comparator Value Register 2                                                */
       uint32_t RESERVED22;
  __IO uint32_t TRCACVR3;                    /*!< ETM Address Comparator Value Register 3                                                */
       uint32_t RESERVED23;
  __IO uint32_t TRCACVR4;                    /*!< ETM Address Comparator Value Register 4                                                */
       uint32_t RESERVED24;
  __IO uint32_t TRCACVR5;                    /*!< ETM Address Comparator Value Register 5                                                */
       uint32_t RESERVED25;
  __IO uint32_t TRCACVR6;                    /*!< ETM Address Comparator Value Register 6                                                */
       uint32_t RESERVED26;
  __IO uint32_t TRCACVR7;                    /*!< ETM Address Comparator Value Register 7                                                */
       uint32_t RESERVED27;
  __IO uint32_t TRCACVR8;                    /*!< ETM Address Comparator Value Register 8                                                */
       uint32_t RESERVED28;
  __IO uint32_t TRCACVR9;                    /*!< ETM Address Comparator Value Register 9                                                */
       uint32_t RESERVED29;
  __IO uint32_t TRCACVR10;                   /*!< ETM Address Comparator Value Register 10                                               */
       uint32_t RESERVED30;
  __IO uint32_t TRCACVR11;                   /*!< ETM Address Comparator Value Register 11                                               */
       uint32_t RESERVED31;
  __IO uint32_t TRCACVR12;                   /*!< ETM Address Comparator Value Register 12                                               */
       uint32_t RESERVED32;
  __IO uint32_t TRCACVR13;                   /*!< ETM Address Comparator Value Register 13                                               */
       uint32_t RESERVED33;
  __IO uint32_t TRCACVR14;                   /*!< ETM Address Comparator Value Register 14                                               */
       uint32_t RESERVED34;
  __IO uint32_t TRCACVR15;                   /*!< ETM Address Comparator Value Register 15                                               */
       uint32_t RESERVED35;
  __IO uint32_t TRCACATR0;                   /*!< ETM Address Comparator Access Type Register 0                                          */
       uint32_t RESERVED36;
  __IO uint32_t TRCACATR1;                   /*!< ETM Address Comparator Access Type Register 1                                          */
       uint32_t RESERVED37;
  __IO uint32_t TRCACATR2;                   /*!< ETM Address Comparator Access Type Register 2                                          */
       uint32_t RESERVED38;
  __IO uint32_t TRCACATR3;                   /*!< ETM Address Comparator Access Type Register 3                                          */
       uint32_t RESERVED39;
  __IO uint32_t TRCACATR4;                   /*!< ETM Address Comparator Access Type Register 4                                          */
       uint32_t RESERVED40;
  __IO uint32_t TRCACATR5;                   /*!< ETM Address Comparator Access Type Register 5                                          */
       uint32_t RESERVED41;
  __IO uint32_t TRCACATR6;                   /*!< ETM Address Comparator Access Type Register 6                                          */
       uint32_t RESERVED42;
  __IO uint32_t TRCACATR7;                   /*!< ETM Address Comparator Access Type Register 7                                          */
       uint32_t RESERVED43;
  __IO uint32_t TRCACATR8;                   /*!< ETM Address Comparator Access Type Register 8                                          */
       uint32_t RESERVED44;
  __IO uint32_t TRCACATR9;                   /*!< ETM Address Comparator Access Type Register 9                                          */
       uint32_t RESERVED45;
  __IO uint32_t TRCACATR10;                  /*!< ETM Address Comparator Access Type Register 10                                         */
       uint32_t RESERVED46;
  __IO uint32_t TRCACATR11;                  /*!< ETM Address Comparator Access Type Register 11                                         */
       uint32_t RESERVED47;
  __IO uint32_t TRCACATR12;                  /*!< ETM Address Comparator Access Type Register 12                                         */
       uint32_t RESERVED48;
  __IO uint32_t TRCACATR13;                  /*!< ETM Address Comparator Access Type Register 13                                         */
       uint32_t RESERVED49;
  __IO uint32_t TRCACATR14;                  /*!< ETM Address Comparator Access Type Register 14                                         */
       uint32_t RESERVED50;
  __IO uint32_t TRCACATR15;                  /*!< ETM Address Comparator Access Type Register 15                                         */
       uint32_t RESERVED51;
  __IO uint32_t TRCDVCVR0;                   /*!< ETM Data Value Comparator Value Register 0                                             */
       uint32_t RESERVED52[3];
  __IO uint32_t TRCDVCVR1;                   /*!< ETM Data Value Comparator Value Register 1                                             */
       uint32_t RESERVED53[27];
  __IO uint32_t TRCDVCMR0;                   /*!< ETM Data Value Comparator Mask Register 0                                              */
       uint32_t RESERVED54[3];
  __IO uint32_t TRCDVCMR1;                   /*!< ETM Data Value Comparator Mask Register 1                                              */
       uint32_t RESERVED55[603];
  __IO uint32_t TRCITCTRL;                   /*!< ETM Integration Mode Control Register                                                  */
       uint32_t RESERVED56[39];
  __IO uint32_t TRCCLAIMSET;                 /*!< ETM Claim Tag Set Register                                                             */
  __IO uint32_t TRCCLAIMCLR;                 /*!< ETM Claim Tag Clear Register                                                           */
  __I  uint32_t TRCDEVAFF0;                  /*!< ETM Device Affinity Register 0                                                         */
  __I  uint32_t TRCDEVAFF1;                  /*!< ETM Device Affinity Register 1                                                         */
  __O  uint32_t TRCLAR;                      /*!< ETM Software Lock Access Register                                                      */
  __I  uint32_t TRCLSR;                      /*!< ETM Software Lock Status Register                                                      */
  __I  uint32_t TRCAUTHSTATUS;               /*!< ETM Authentication Status Register                                                     */
  __I  uint32_t TRCDEVARCH;                  /*!< ETM Device Architecture Register                                                       */
       uint32_t RESERVED57[2];
  __I  uint32_t TRCDEVID;                    /*!< ETM Device ID Register                                                                 */
  __I  uint32_t TRCDEVTYPE;                  /*!< ETM Device Type Register                                                               */
  __I  uint32_t TRCPIDR4;                    /*!< ETM CoreSight Peripheral ID4 Register                                                  */
  __I  uint32_t TRCPIDR5;                    /*!< ETM CoreSight Peripheral ID5 Register                                                  */
  __I  uint32_t TRCPIDR6;                    /*!< ETM CoreSight Peripheral ID6 Register                                                  */
  __I  uint32_t TRCPIDR7;                    /*!< ETM CoreSight Peripheral ID7 Register                                                  */
  __I  uint32_t TRCPIDR0;                    /*!< ETM CoreSight Peripheral ID0 Register                                                  */
  __I  uint32_t TRCPIDR1;                    /*!< ETM CoreSight Peripheral ID1 Register                                                  */
  __I  uint32_t TRCPIDR2;                    /*!< ETM CoreSight Peripheral ID2 Register                                                  */
  __I  uint32_t TRCPIDR3;                    /*!< ETM CoreSight Peripheral ID3 Register                                                  */
  __I  uint32_t TRCCIDR0;                    /*!< ETM CoreSight Component ID0 Register                                                   */
  __I  uint32_t TRCCIDR1;                    /*!< ETM CoreSight Component ID1 Register                                                   */
  __I  uint32_t TRCCIDR2;                    /*!< ETM CoreSight Component ID2 Register                                                   */
  __I  uint32_t TRCCIDR3;                    /*!< ETM CoreSight Component ID3 Register                                                   */
} ETM_Type;

#define ETM_TRCPRGCTLR_EN_Pos            0                                      /*!< ETM TRCPRGCTLR: Trace Enable Position               */
#define ETM_TRCPRGCTLR_EN_Msk            (1UL << ETM_TRCPRGCTLR_EN_Pos)         /*!< ETM TRCPRGCTLR: Trace Enable Mask                   */

#define ETM_TRCPROCSELR_PROCSEL_Pos      0                                      /*!< ETM TRCPROCSELR: Processor Select Position          */
#define ETM_TRCPROCSELR_PROCSEL_Msk      (0x3UL << ETM_TRCPROCSELR_PROCSEL_Pos) /*!< ETM TRCPROCSELR: Processor Select Mask              */

#define ETM_TRCSTATR_IDLE_Pos            0                                      /*!< ETM TRCSTATR: Idle Status Position                  */
#define ETM_TRCSTATR_IDLE_Msk            (1UL << ETM_TRCSTATR_IDLE_Pos)         /*!< ETM TRCSTATR: Idle Status Mask                      */

#define ETM_TRCSTATR_PMSTABLE_Pos        1                                      /*!< ETM TRCSTATR: Programmers' Model Status Position    */
#define ETM_TRCSTATR_PMSTABLE_Msk        (1UL << ETM_TRCSTATR_PMSTABLE_Pos)     /*!< ETM TRCSTATR: Programmers' Model Status Mask        */

#define ETM_TRCCONFIGR_INSTP0_Pos        1
#define ETM_TRCCONFIGR_INSTP0_Msk        (0x3UL << ETM_TRCCONFIGR_INSTP0_Pos)

#define ETM_TRCCONFIGR_BB_Pos            3
#define ETM_TRCCONFIGR_BB_Msk            (1UL << ETM_TRCCONFIGR_BB_Pos)

#define ETM_TRCCONFIGR_CCI_Pos           4
#define ETM_TRCCONFIGR_CCI_Msk           (1UL << ETM_TRCCONFIGR_CCI_Pos)

#define ETM_TRCCONFIGR_COND_Pos          8
#define ETM_TRCCONFIGR_COND_Msk          (0x7UL << ETM_TRCCONFIGR_COND_Pos)

#define ETM_TRCCONFIGR_TS_Pos            11
#define ETM_TRCCONFIGR_TS_Msk            (1UL << ETM_TRCCONFIGR_TS_Pos)

#define ETM_TRCCONFIGR_RS_Pos            12
#define ETM_TRCCONFIGR_RS_Msk            (1UL << ETM_TRCCONFIGR_RS_Pos)

#define ETM_TRCCONFIGR_DA_Pos            16
#define ETM_TRCCONFIGR_DA_Msk            (1UL << ETM_TRCCONFIGR_DA_Pos)

#define ETM_TRCCONFIGR_DV_Pos            17
#define ETM_TRCCONFIGR_DV_Msk            (1UL << ETM_TRCCONFIGR_DV_Pos)

#define ETM_TRCSYNCPR_PERIOD_Pos         0
#define ETM_TRCSYNCPR_PERIOD_Msk         (0x1FUL << ETM_TRCSYNCPR_PERIOD_Pos)

#define ETM_TRCTRACEIDR_TRACEID_Pos      0
#define ETM_TRCTRACEIDR_TRACEID_Msk      (0x7FUL << ETM_TRCTRACEIDR_TRACEID_Pos)

#define ETM_TRCVICTLR_EVENT_Pos          0
#define ETM_TRCVICTLR_EVENT_Msk          (0xFFUL << ETM_TRCVICTLR_EVENT_Pos)

#define ETM_TRCVICTLR_SSSTATUS_Pos       9
#define ETM_TRCVICTLR_SSSTATUS_Msk       (1UL << ETM_TRCVICTLR_SSSTATUS_Pos)

#define ETM_TRCVICTLR_TRCRESET_Pos       10
#define ETM_TRCVICTLR_TRCRESET_Msk       (1UL << ETM_TRCVICTLR_TRCRESET_Pos)

#define ETM_TRCVICTLR_TRCERR_Pos         11
#define ETM_TRCVICTLR_TRCERR_Msk         (1UL << ETM_TRCVICTLR_TRCERR_Pos)

#define ETM_TRCVICTLR_EXLEVEL_S_Pos      16
#define ETM_TRCVICTLR_EXLEVEL_S_Msk      (0xFUL << ETM_TRCVICTLR_EXLEVEL_S_Pos)

#define ETM_TRCVICTLR_EXLEVEL_NS_Pos     20
#define ETM_TRCVICTLR_EXLEVEL_NS_Msk     (0xFUL << ETM_TRCVICTLR_EXLEVEL_NS_Pos)

#define ETM_TRCPIDR4_DES_2_Pos           0
#define ETM_TRCPIDR4_DES_2_Msk           (0xFUL << ETM_TRCPIDR4_DES_2_Pos)

#define ETM_TRCPIDR4_SIZE_Pos            4
#define ETM_TRCPIDR4_SIZE_Msk            (0xFUL << ETM_TRCPIDR4_SIZE_Pos)

#define ETM_TRCPIDR0_PART_0_Pos          0
#define ETM_TRCPIDR0_PART_0_Msk          (0xFFUL << ETM_TRCPIDR4_PART_0_Pos)

#define ETM_TRCPIDR1_PART_1_Pos          0
#define ETM_TRCPIDR1_PART_1_Msk          (0xFUL << ETM_TRCPIDR1_PART_1_Pos)

#define ETM_TRCPIDR1_DES_0_Pos           4
#define ETM_TRCPIDR1_DES_0_Msk           (0xFUL << ETM_TRCPIDR1_DES_0_Pos)

#define ETM_TRCPIDR2_DES_1_Pos           0
#define ETM_TRCPIDR2_DES_1_Msk           (0x7UL << ETM_TRCPIDR2_DES_1_Pos)

#define ETM_TRCPIDR2_REVISION_Pos        4
#define ETM_TRCPIDR2_REVISION_Msk        (0xFUL << ETM_TRCPIDR2_REVISION_Pos)

#define ETM_TRCPIDR3_CMOD_Pos            0
#define ETM_TRCPIDR3_CMOD_Msk            (0xFUL << ETM_TRCPIDR3_CMOD_Pos)

#define ETM_TRCPIDR3_REVAND_Pos          4
#define ETM_TRCPIDR3_REVAND_Msk          (0xFUL << ETM_TRCPIDR3_REVAND_Pos)

/******************************************************************************/
/*                       Olympus CTI registers structures                       */
/******************************************************************************/

/*--------------------------- Cross Trigger Interface ------------------------*/
typedef struct
{
  __IO uint32_t CONTROL;                     /*!< Offset: 0x000 (R/W) CTI Control Register                   */
       uint32_t RESERVED0[3];
  __O  uint32_t INTACK;                      /*!< Offset: 0x010 ( /W) CTI Output Trigger Ack Register        */
  __IO uint32_t APPSET;                      /*!< Offset: 0x014 (R/w) CTI Application Trigger Set Register   */
  __O  uint32_t APPCLR;                      /*!< Offset: 0x018 ( /W) CTI Application Trigger Clear Register */
  __O  uint32_t APPPULSE;                    /*!< Offset: 0x01C ( /W) CTI Application Pulse Register         */
  __IO uint32_t INEN0;                       /*!< Offset: 0x020 (R/W) CTI Input Trigger to Output Chn Enable */
       uint32_t RESERVED1[31];
  __IO uint32_t OUTEN0;                      /*!< Offset: 0x0A0 (R/W) CTI Input Channel to Output Trg Enable */
  __IO uint32_t OUTEN1;                      /*!< Offset: 0x0A4 (R/W) CTI Input Channel to Output Trg Enable */
  __IO uint32_t OUTEN2;                      /*!< Offset: 0x0A8 (R/W) CTI Input Channel to Output Trg Enable */
  __IO uint32_t OUTEN3;                      /*!< Offset: 0x0AC (R/W) CTI Input Channel to Output Trg Enable */
  __IO uint32_t OUTEN4;                      /*!< Offset: 0x0B0 (R/W) CTI Input Channel to Output Trg Enable */
  __IO uint32_t OUTEN5;                      /*!< Offset: 0x0B4 (R/W) CTI Input Channel to Output Trg Enable */
       uint32_t RESERVED3;
  __IO uint32_t OUTEN7;                      /*!< Offset: 0x0BC (R/W) CTI Input Channel to Output Trg Enable */
       uint32_t RESERVED4[28];
  __I  uint32_t TRIGINSTATUS;                /*!< Offset: 0x130 (R/ ) CTI Trigger In Status Register         */
  __I  uint32_t TRIGOUTSTATUS;               /*!< Offset: 0x134 (R/ ) CTI Trigger Out Status Register        */
  __I  uint32_t CHINSTATUS;                  /*!< Offset: 0x138 (R/ ) CTI Channel In Status Register         */
  __I  uint32_t CHOUTSTATUS;                 /*!< Offset: 0x13C (R/ ) CTI Channel Out Status Register        */
  __IO uint32_t GATE;                        /*!< Offset: 0x140 (R/W) CTI Channel Gate Enable Register       */
       uint32_t RESERVED5[870];
  __O  uint32_t ITCHINACK;                   /*!< Offset: 0xEDC ( /W) CTI Integration Register               */
  __O  uint32_t ITTRIGINACK;                 /*!< Offset: 0xEE0 ( /W) CTI Integration Register               */
  __O  uint32_t ITCHOUT;                     /*!< Offset: 0xEE4 ( /W) CTI Integration Register               */
  __O  uint32_t ITTRIGOUT;                   /*!< Offset: 0xEE8 ( /W) CTI Integration Register               */
  __I  uint32_t ITCHOUTACK;                  /*!< Offset: 0xEEC ( /W) CTI Integration Register               */
  __I  uint32_t ITTRIGOUTACK;                /*!< Offset: 0xEF0 ( /W) CTI Integration Register               */
  __I  uint32_t ITCHIN;                      /*!< Offset: 0xEF4 ( /W) CTI Integration Register               */
  __I  uint32_t ITTRIGIN;                    /*!< Offset: 0xEF8 ( /W) CTI Integration Register               */
       uint32_t RESERVED6;
  __IO uint32_t ITCTRL;                      /*!< Offset: 0xF00 (R/W) CTI Integration Mode Control Register  */
       uint32_t RESERVED7[39];
  __IO uint32_t CLAIMSET;                    /*!< Offset: 0xFA0 (R/W) CTI Claim Set Register                 */
  __IO uint32_t CLAIMCLR;                    /*!< Offset: 0xFA4 (R/W) CTI Claim Clear Register               */
  __I  uint32_t DEVAFF0;                     /*!< Offset: 0xFA8 (R/W) CTI Device Affinity Register 0         */
  __I  uint32_t DEVAFF1;                     /*!< Offset: 0xFAC (R/W) CTI Device Affinity Register 1         */
  __O  uint32_t LOCKACCESS;                  /*!< Offset: 0xFB0 ( /W) CTI Lock Access Register               */
  __I  uint32_t LOCKSTATUS;                  /*!< Offset: 0xFB4 (R/ ) CTI Lock Status Register               */
  __I  uint32_t AUTHSTATUS;                  /*!< Offset: 0xFB8 (R/ ) CTI Authentication Status Register     */
  __I  uint32_t DEVARCH;                     /*!< Offset: 0xFBC (R/ ) CTI Device Architecture Register       */
  __I  uint32_t DEVID2;                      /*!< Offset: 0xFC0 (R/ ) CTI Device Configuration Register      */
  __I  uint32_t DEVID1;                      /*!< Offset: 0xFC4 (R/ ) CTI Device Configuration Register      */
  __I  uint32_t DEVID;                       /*!< Offset: 0xFC8 (R/ ) CTI Device Configuration Register      */
  __I  uint32_t DEVTYPE;                     /*!< Offset: 0xFCC (R/ ) CTI Device Type Register               */
  __I  uint32_t PID4;                        /*!< Offset: 0xFD0 (R/ ) CoreSight register                     */
  __I  uint32_t PID5;                        /*!< Offset: 0xFD4 (R/ ) CoreSight register                     */
  __I  uint32_t PID6;                        /*!< Offset: 0xFD8 (R/ ) CoreSight register                     */
  __I  uint32_t PID7;                        /*!< Offset: 0xFDC (R/ ) CoreSight register                     */
  __I  uint32_t PID0;                        /*!< Offset: 0xFE0 (R/ ) CoreSight register                     */
  __I  uint32_t PID1;                        /*!< Offset: 0xFE4 (R/ ) CoreSight register                     */
  __I  uint32_t PID2;                        /*!< Offset: 0xFE8 (R/ ) CoreSight register                     */
  __I  uint32_t PID3;                        /*!< Offset: 0xFEC (R/ ) CoreSight register                     */
  __I  uint32_t CID0;                        /*!< Offset: 0xFF0 (R/ ) CoreSight register                     */
  __I  uint32_t CID1;                        /*!< Offset: 0xFF4 (R/ ) CoreSight register                     */
  __I  uint32_t CID2;                        /*!< Offset: 0xFF8 (R/ ) CoreSight register                     */
  __I  uint32_t CID3;                        /*!< Offset: 0xFFC (R/ ) CoreSight register                     */
} CTI_Type;

/******************************************************************************/
/*                      Olympus TPIU registers structures                       */
/******************************************************************************/

/*--------------------- Trace Port Interface Unit ----------------------------*/
typedef struct
{
  __I  uint32_t SSPSR;                        /*!< Offset 0x000 (R ) TPIU Supported Synchronous Port Size register */
  __IO uint32_t CSPSR;                        /*!< Offset 0x004 (RW) TPIU Current Synchronous Port Size register   */
       uint32_t RESERVED0[2];
  __IO uint32_t COSDR;                        /*!< Offset 0x010 (RW) TPIU Current Output Speed Divisor register    */
       uint32_t RESERVED1[55];
  __IO uint32_t SPPR;                         /*!< Offset 0x0F0 (RW) TPIU Selected Pin Protocol register           */
       uint32_t RESERVED2[3];
  __IO uint32_t SPTMR;                        /*!< Offset 0x100  TPIU Supported Trigger Modes Register             */
  __IO uint32_t TCVR;                         /*!< Offset 0x104  TPIU Trigger Count Value Register                 */
  __IO uint32_t TMR;                          /*!< Offset 0x108  TPIU Trigger Multiplier Register                  */
       uint32_t RESERVED3[125];
  __I  uint32_t FFSR;                         /*!< Offset 0x300 (R ) TPIU Formatter and Flush Status Register      */
  __IO uint32_t FFCR;                         /*!< Offset 0x304 (RW) TPIU Formatter and Flush Control Register     */
       uint32_t RESERVED4[760];
  __I  uint32_t ITTRFLIN;                     /*!< Offset 0xEE8 (R ) TPIU Test Trigger In and Flush In Register    */
  __I  uint32_t ITFTTD0;                      /*!< Offset 0xEEC (R ) TPIU Integration ETM Data Register 0          */
  __O  uint32_t ITATBCTR2;                    /*!< Offset 0xEF0 ( W) TPIU Integration ATB Control2 Register        */
  __I  uint32_t ITATBCTR;                     /*!< Offset 0xEF4 (R ) TPIU Integration ITM Data Register            */
       uint32_t RESERVED7;
  __I  uint32_t ITFTTD1;                      /*!< Offset 0xEFC (R ) TPIU Integration ETM Data Register 1          */
  __IO uint32_t ITCTRL;                       /*!< Offset 0xF00 (RW) TPIU Integration Control Register             */
       uint32_t RESERVED5[39];
  __IO uint32_t CLAIMSET;                     /*!< Offset 0xFA0 (RW) TPIU Claim Tag Set Register                   */
  __IO uint32_t CLAIMCLR;                     /*!< Offset 0xFA4 (RW) TPIU Claim Tag Clear Register                 */
       uint32_t RESERVED6[8];
  __I  uint32_t DEVID;                        /*!< Offset 0xFC8 (R ) TPIU Device ID Register                       */
  __I  uint32_t DEVTYPE;                      /*!< Offset 0xFCC (R ) TPIU Device Type Register                     */
  __I  uint32_t PID4;                         /*!< Offset 0xFD0 (R ) CoreSight register                            */
       uint32_t RESERVED8[3];
  __I  uint32_t PID0;                         /*!< Offset 0xFE0 (R ) CoreSight register                            */
  __I  uint32_t PID1;                         /*!< Offset 0xFE4 (R ) CoreSight register                            */
  __I  uint32_t PID2;                         /*!< Offset 0xFE8 (R ) CoreSight register                            */
  __I  uint32_t PID3;                         /*!< Offset 0xFEC (R ) CoreSight register                            */
  __I  uint32_t CID0;                         /*!< Offset 0xFF0 (R ) CoreSight register                            */
  __I  uint32_t CID1;                         /*!< Offset 0xFF4 (R ) CoreSight register                            */
  __I  uint32_t CID2;                         /*!< Offset 0xFF8 (R ) CoreSight register                            */
  __I  uint32_t CID3;                         /*!< Offset 0xFFC (R ) CoreSight register                            */
} TPIU_Type;

#define TPIU_FFCR_STOPF1_Pos      12
#define TPIU_FFCR_STOPF1_MSK      (0x1UL << TPIU_FFCR_STOPF1_Pos)

#define TPIU_FFSR_FLINPROG_Pos    0
#define TPIU_FFSR_FLINPROG_MSK    (0x1UL << TPIU_FFCR_STOPF1_Pos)

#define TPIU_SPPR_TXMODE_Pos      0
#define TPIU_SPPR_TXMODE_Msk      (0x3UL << TPIU_SPPR_TXMODE_Pos)

#define TPIU_PIN_TRACEPORT        0          /*!< TPIU Selected Pin Protocol Parallel Port  */
#define TPIU_PIN_SWO_MANCHESTER   1          /*!< TPIU Selected Pin Protocol SWO Manchester */
#define TPIU_PIN_SWO_NRZ          2          /*!< TPIU Selected Pin Protocol SWO NRZ (uart) */

/******************************************************************************/
/*                      Olympus FPB registers structures                        */
/******************************************************************************/
typedef struct
{
  __IO  uint32_t CTRL;                        /*!< Offset 0x000 (RW) FPB Flash Patch Control Register      */
  __O   uint32_t REMAP;                       /*!< Offset 0x004 (RW) FPB Flash Patch Remap Register        */
  __IO  uint32_t COMP0;                       /*!< Offset 0x008 (RW) FPB Flash Patch Comparator Register 0 */
  __IO  uint32_t COMP1;                       /*!< Offset 0x00C (RW) FPB Flash Patch Comparator Register 1 */
  __IO  uint32_t COMP2;                       /*!< Offset 0x010 (RW) FPB Flash Patch Comparator Register 2 */
  __IO  uint32_t COMP3;                       /*!< Offset 0x014 (RW) FPB Flash Patch Comparator Register 3 */
  __IO  uint32_t COMP4;                       /*!< Offset 0x008 (RW) FPB Flash Patch Comparator Register 4 */
  __IO  uint32_t COMP5;                       /*!< Offset 0x00C (RW) FPB Flash Patch Comparator Register 5 */
  __IO  uint32_t COMP6;                       /*!< Offset 0x010 (RW) FPB Flash Patch Comparator Register 6 */
  __IO  uint32_t COMP7;                       /*!< Offset 0x014 (RW) FPB Flash Patch Comparator Register 7 */
} FPB_Type;

#define FPB            ((FPB_Type      *)     FPB_BASE      ) /*!< Flash Patch Unit */

/******************************************************************************/
/*                      Olympus L1 Imp Def registers structures                 */
/******************************************************************************/
typedef struct
{
  __IO  uint32_t MSCR;                        /*!< Offset 0x000 (RW) L1-cache control register          */
  __IO  uint32_t PFCR;                        /*!< Offset 0x004 (RW) Prefetch control register          */
        uint32_t RESERVED[2];
  __IO  uint32_t ITCMCR;                      /*!< Offset 0x010 (RW) ITCM control register              */
  __IO  uint32_t DTCMCR;                      /*!< Offset 0x014 (RW) DTCM control register              */
} L1_Type;

/******************************************************************************/
/*                      Olympus Imp Def registers structures                    */
/******************************************************************************/
typedef struct
{
  __IO  uint32_t ERRFR0;                      /*!< Offset 0x05000 (RW) Error Record Feature Register 0                             */
        uint32_t RESERVED0;
  __IO  uint32_t ERRCTRL0;                    /*!< Offset 0x05008 (RW) Error Record Control Register 0                             */
        uint32_t RESERVED1;
  __IO  uint32_t ERRSTATUS0;                  /*!< Offset 0x05010 (RW) Error Record Primary Status Register 0                      */
        uint32_t RESERVED2;
  __IO  uint32_t ERRADDR0;                    /*!< Offset 0x05018 (RW) Error Record Address Register 0                             */
  __IO  uint32_t ERRADDR20;                   /*!< Offset 0x0501C (RW) Error Record Address Register 2 Register 0                  */
  __IO  uint32_t ERRMISC00;                   /*!< Offset 0x05020 (RW) Error Record Miscellaneous 0 Register 0                     */
  __IO  uint32_t ERRMISC10;                   /*!< Offset 0x05024 (RW) Error Record Miscellaneous 1 Register 0                     */
  __IO  uint32_t ERRMISC20;                   /*!< Offset 0x05028 (RW) Error Record Miscellaneous 2 Register 0                     */
  __IO  uint32_t ERRMISC30;                   /*!< Offset 0x0502C (RW) Error Record Miscellaneous 3 Register 0                     */
  __IO  uint32_t ERRMISC40;                   /*!< Offset 0x05030 (RW) Error Record Miscellaneous 4 Register 0                     */
  __IO  uint32_t ERRMISC50;                   /*!< Offset 0x05034 (RW) Error Record Miscellaneous 5 Register 0                     */
  __IO  uint32_t ERRMISC60;                   /*!< Offset 0x05038 (RW) Error Record Miscellaneous 6 Register 0                     */
  __IO  uint32_t ERRMISC70;                   /*!< Offset 0x0503C (RW) Error Record Miscellaneous 7 Register 0                     */
        uint32_t RESERVED3[880];
  __IO  uint32_t ERRGSR0;                     /*!< Offset 0x05E00 (RW) RAS Fault Group Status Register                             */
        uint32_t RESERVED4[113];
  __IO  uint32_t ERRDEVID;                    /*!< Offset 0x05FC8 (RW) Error Record Device ID Register                             */
        uint32_t RESERVED5[8207];
  __IO  uint32_t ACTLR;                       /*!< Offset 0x0E008 (RW) Auxiliary Control Register                                  */
        uint32_t RESERVED6[844];
  __IO  uint32_t AFSR;                        /*!< Offset 0x0ED3C (RW) Auxiliary Fault Status Register                             */
        uint32_t RESERVED7[113];
  __IO  uint32_t RFSR;                        /*!< Offset 0x0EF04 (RW) RAS Fault Status Register                                   */
        uint32_t RESERVED8[15422];
  __IO  uint32_t MSCR;                        /*!< Offset 0x1E000 (RW) L1-cache control register                                   */
  __IO  uint32_t PFCR;                        /*!< Offset 0x1E004 (RW) Prefetch control register                                   */
        uint32_t RESERVED9[2];
  __IO  uint32_t ITCMCR;                      /*!< Offset 0x1E010 (RW) ITCM control register                                       */
  __IO  uint32_t DTCMCR;                      /*!< Offset 0x1E014 (RW) DTCM control register                                       */
  __IO  uint32_t PAHBCR;                      /*!< Offset 0x1E018 (RW) P-AHB Control Register                                      */
        uint32_t RESERVED10[57];
  __IO  uint32_t IEBR0;                       /*!< Offset 0x1E100 (RW) Instruction cache error bank register 0                     */
  __IO  uint32_t IEBR1;                       /*!< Offset 0x1E104 (RW) Instruction cache error bank register 1                     */
        uint32_t RESERVED11[2];
  __IO  uint32_t DEBR0;                       /*!< Offset 0x1E110 (RW) Data cache error bank register 0                            */
  __IO  uint32_t DEBR1;                       /*!< Offset 0x1E114 (RW) Data cache error bank register 1                            */
        uint32_t RESERVED12[2];
  __IO  uint32_t TEBR0;                       /*!< Offset 0x1E120 (RW) TCU error bank register 0                                   */
  __IO  uint32_t TEBRDATA0;                   /*!< Offset 0x1E124 (RW) Data for TCU error bank register 0                          */
  __IO  uint32_t TEBR1;                       /*!< Offset 0x1E128 (RW) TCU error bank register 1                                   */
  __IO  uint32_t TEBRDATA1;                   /*!< Offset 0x1E12C (RW) Data for TCU error bank register 1                          */
        uint32_t RESERVED22[52];
  __IO  uint32_t DCADCRR;                     /*!< Offset 0x1E200 (RW) Direct cache access Data cache read register                */
  __IO  uint32_t DCAICRR;                     /*!< Offset 0x1E204 (RW) Direct cache access Instruction cache read register         */
        uint32_t RESERVED13[2];
  __IO  uint32_t DCADCLR;                     /*!< Offset 0x1E210 (RW) Direct cache access Data cache location register            */
  __IO  uint32_t DCAICLR;                     /*!< Offset 0x1E214 (RW) Direct cache access Instruction cache location register     */
        uint32_t RESERVED14[58];
  __IO  uint32_t CPDLPSTATE;                  /*!< Offset 0x1E300 (RW) Core Power Domain Low Power State register                  */
  __IO  uint32_t DPDLPSTATE;                  /*!< Offset 0x1E304 (RW) Debug Power Domain Low Power State register                 */
        uint32_t RESERVED15[62];
  __IO  uint32_t EVENTSPR;                    /*!< Offset 0x1E400 (RW) Event Set Pending register                                  */
        uint32_t RESERVED16[31];
  __IO  uint32_t EVENTMASKA;                  /*!< Offset 0x1E480 (RW) Event Mask register                                         */
  __IO  uint32_t EVENTMASK[16];               /*!< Offset 0x1E484+4n (RW) IRQ Event Mask register n, 0 =< n < 15                   */
        uint32_t RESERVED17[15];
  __IO  uint32_t ITGU_CTRL;                   /*!< Offset 0x1E500 (RW) ITCM Gate control Register                                  */
  __IO  uint32_t ITGU_CFG;                    /*!< Offset 0x1E504 (RW) ITCM Gate configuration register                            */
        uint32_t RESERVED18[2];
  __IO  uint32_t ITGU_LUT[16];                /*!< Offset 0x1E510+4n (RW) ITCM Gate look-up table register n: blocks 32n to 32n+31 */
        uint32_t RESERVED19[44];
  __IO  uint32_t DTGU_CTRL;                   /*!< Offset 0x1E600 (RW) DTCM Gate control Register                                  */
  __IO  uint32_t DTGU_CFG;                    /*!< Offset 0x1E604 (RW) DTCM Gate configuration register                            */
        uint32_t RESERVED20[2];
  __IO  uint32_t DTGU_LUT[16];                /*!< Offset 0x1E610+4n (RW) DTCM Gate look-up table register n: blocks 32n to 32n+31 */
        uint32_t RESERVED21[44];
  __IO  uint32_t CFGINFOSEL;                  /*!< Offset 0x1E700 (RW) Processor configuration information select register         */
  __IO  uint32_t CFGINFORD;                   /*!< Offset 0x1E704 (RW) Processor configuration information read data register      */
} IMPDEF_Type;

/******************************************************************************/
/*                      Olympus EWIC registers structures                        */
/******************************************************************************/
typedef struct
{
  __IO  uint32_t EWIC_CR;                     /*!< Offset 0x000 (RW) EWIC Control Register                                                  */
  __IO  uint32_t EWIC_ASCR;                   /*!< Offset 0x004 (RW) EWIC Automatic sequence control register                               */
  __I   uint32_t EWIC_CLRMASK;                /*!< Offset 0x008 (WO) EWIC Clear all mask register                                           */
  __O   uint32_t EWIC_NUMID;                  /*!< Offset 0x00C (RO) EWIC ID register for the number of events supported                    */
        uint32_t RESERVED1[124];
  __IO  uint32_t EWIC_MASKA;                  /*!< Offset 0x200 (RW) EWIC Set which internal events cause wakeup                            */
  __IO  uint32_t EWIC_MASK[15];               /*!< Offset 0x204-0x23C (RW) EWIC Set which internal events cause wakeup                      */
        uint32_t RESERVED2[112];
  __O   uint32_t EWIC_PENDA;                  /*!< Offset 0x400 (RO) EWIC Which internal events were pended while the EWIC was enabled      */
  __IO  uint32_t EWIC_PEND[15];               /*!< Offset 0x404-0x43C (RW) Which external interrupts were pended while the EWIC was enabled */
        uint32_t RESERVED3[112];
  __O   uint32_t EWIC_PSR;                    /*!< Offset 0x600 (RO) EWIC Summary register for non-zero EWIC_PENDn registers                */
        uint32_t RESERVED4[575];
  __IO  uint32_t ITCTRL;                      /*!< Offset 0xF00 (RW) EWIC Integration Control Register                                      */
        uint32_t RESERVED5[39];
  __IO  uint32_t CLAIMSET;                    /*!< Offset 0xFA0 (RW) EWIC Claim Tag Set Register                                            */
  __IO  uint32_t CLAIMCLR;                    /*!< Offset 0xFA4 (RW) EWIC Claim Tag Clear Register                                          */
  __I   uint32_t DEVAFF0;                     /*!< Offset 0xFA8 (R )                                                                        */
  __I   uint32_t DEVAFF1;                     /*!< Offset 0xFAC (R )                                                                        */
  __O   uint32_t LAR;                         /*!< Offset 0xFB0 ( W)                                                                        */
  __I   uint32_t LSR;                         /*!< Offset 0xFB4 (R )                                                                        */
  __I   uint32_t AUTHSTATUS;                  /*!< Offset 0xFB8 (R )                                                                        */
  __I   uint32_t DEVARCH;                     /*!< Offset 0xFBC (R )                                                                        */
  __I   uint32_t DEVID2;                      /*!< Offset 0xFC0 (R )                                                                        */
  __I   uint32_t DEVID1;                      /*!< Offset 0xFC4 (R )                                                                        */
  __I   uint32_t DEVID;                       /*!< Offset 0xFC8 (R ) EWIC Device ID Register                                                */
  __I   uint32_t DEVTYPE;                     /*!< Offset 0xFCC (R ) EWIC Device Type Register                                              */
  __I   uint32_t PIDR4;                       /*!< Offset 0xFD0 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR5;                       /*!< Offset 0xFD4 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR6;                       /*!< Offset 0xFD8 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR7;                       /*!< Offset 0xFDC (R ) CoreSight register                                                     */
  __I   uint32_t PIDR0;                       /*!< Offset 0xFE0 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR1;                       /*!< Offset 0xFE4 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR2;                       /*!< Offset 0xFE8 (R ) CoreSight register                                                     */
  __I   uint32_t PIDR3;                       /*!< Offset 0xFEC (R ) CoreSight register                                                     */
  __I   uint32_t CIDR0;                       /*!< Offset 0xFF0 (R ) CoreSight register                                                     */
  __I   uint32_t CIDR1;                       /*!< Offset 0xFF4 (R ) CoreSight register                                                     */
  __I   uint32_t CIDR2;                       /*!< Offset 0xFF8 (R ) CoreSight register                                                     */
  __I   uint32_t CIDR3;                       /*!< Offset 0xFFC (R ) CoreSight register                                                     */
} EWIC_MCU_Type;

#define EWIC_MCU       ((EWIC_MCU_Type      *)     EWIC_MCU_BASE       ) /*!< External Wakeup Interrupt Controller */

/* MPU Region Base Address Register Definitions */
#define MPU_RBAR_ADDR_Pos                   5U                                            /*!< MPU RBAR: ADDR Position */
#define MPU_RBAR_ADDR_Msk                  (0x7FFFFFFUL << MPU_RBAR_ADDR_Pos)             /*!< MPU RBAR: ADDR Mask */

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

/*--------------------- Interrupt Check---------------------------------------*/
typedef struct
{
  __IO uint32_t DATA [32];
} IRQDef;

/*----------------- Execution Testbench DMA Controller Model -----------------*/
/* DMAC CTRL Register Definitions */
#define DMAC_CTRL_REG_BASE              (DMAC_BASE +  0x200UL)                      /**/
#define DMAC_CTRL                       ((uint32_t *) DMAC_CTRL_REG_BASE)

#define DMAC_CTRL_BURST_LENGTH_Pos      0U                                          /*!< DMAC CTRL: BURST_LENGTH Position */
#define DMAC_CTRL_BURST_LENGTH_Msk      (15UL << DMAC_CTRL_BURST_LENGTH_Pos)               /*!< DMAC CTRL: BURST_LENGTH Mask */

#define DMAC_CTRL_HWRITE_Pos            8U                                          /*!< DMAC CTRL: HWRITE Position */
#define DMAC_CTRL_HWRITE_Msk            (1UL << DMAC_CTRL_HWRITE_Pos)               /*!< DMAC CTRL: HWRITE Mask */

#define DMAC_CTRL_HNONSEC_Pos           9U                                          /*!< DMAC CTRL: HNONSEC Position */
#define DMAC_CTRL_HNONSEC_Msk           (1UL << DMAC_CTRL_HNONSEC_Pos)              /*!< DMAC CTRL: HNONSEC Mask */

/* DMAC GOF (go-finish) Register Definitions */
#define DMAC_GOF_REG_BASE               (DMAC_BASE +  0x204UL)                      /**/
#define DMAC_GOF                        ((uint32_t *) DMAC_GOF_REG_BASE)


#define DMAC_GOF_GO_Pos                 0U                                          /*!< DMAC GOF: go-finish Position */
#define DMAC_GOF_GO_Msk                 (1UL << DMAC_GOF_GO_Pos)                    /*!< DMAC GOF: go-finish Mask */

/* DMAC ADDR Register Definitions */
#define DMAC_ADDR_REG_BASE              (DMAC_BASE +  0x0UL)                        /**/
#define DMAC_ADDR                        ((uint32_t *) DMAC_ADDR_REG_BASE)

#define DMAC_ADDR_INDEX_Pos             2U                                          /*!< DMAC ADDR: index Position */
#define DMAC_ADDR_INDEX_Msk             (15UL << DMAC_ADDR_INDEX_Pos)               /*!< DMAC ADDR: index Mask */

/* DMAC DATA Register Definitions */
#define DMAC_DATA_REG_BASE              (DMAC_BASE +  0x100UL)                      /**/
#define DMAC_DATA                       ((uint32_t *) DMAC_DATA_REG_BASE)

/******************************************************************************/
/*                Olympus SBIST registers structures                            */
/******************************************************************************/
typedef struct
{
  __IOM uint32_t FCTLR;           /*!< Offset: 0x00 (R/W) Fault Control Register                      */
  __IOM uint32_t FPIR;            /*!< Offset: 0x04 (R/W) Fault Partition Identifier Register         */
  __IOM uint32_t FFMIR;           /*!< Offset: 0x08 (R/W) Fault Failure Mode Identification Register  */
  __IM  uint32_t FPSTR;           /*!< Offset: 0x0C (R/)  Fault PS Type Register                      */
  __IOM uint32_t FPSPR0;          /*!< Offset: 0x10 (R/W) Fault PS Payload Register 0                 */
  __IOM uint32_t FPSPR1;          /*!< Offset: 0x14 (R/W) Fault PS Payload Register 1                 */
  __IOM uint32_t FPSPR2;          /*!< Offset: 0x18 (R/W) Fault PS Payload Register 2                 */
  __IOM uint32_t FPSPR3;          /*!< Offset: 0x1C (R/W) Fault PS Payload Register 3                 */
  __IOM uint32_t FPSPR4;          /*!< Offset: 0x20 (R/W) Fault PS Payload Register 4                 */
  __IOM uint32_t FPSPR5;          /*!< Offset: 0x24 (R/W) Fault PS Payload Register 5                 */
  __IOM uint32_t FPSPR6;          /*!< Offset: 0x28 (R/W) Fault PS Payload Register 6                 */
  __IOM uint32_t FPSPR7;          /*!< Offset: 0x2C (R/W) Fault PS Payload Register 7                 */
  __IOM uint32_t FPSPR8;          /*!< Offset: 0x30 (R/W) Fault PS Payload Register 8                 */
  __IM uint32_t IWICSENSEREG;     /*!< Offset: 0x34 (R/)  IWIC sense register                         */
  __IM uint32_t EVENTBUSREG;      /*!< Offset: 0x38 (R/)  EVENTBUS sense register                     */
} SBIST_Type;

#define SBIST_BASE           (0xE0048000UL)                                 /*!< SBIST Controller Base Address */
#define SBIST                ((SBIST_Type      *)     SBIST_BASE        )   /*!< SBIST configuration struct */

#define        OLYMPUS_SBIST_FCTLR_STATUS_IDLE    0
#define        OLYMPUS_SBIST_FCTLR_STATUS_INIT    1
#define        OLYMPUS_SBIST_FCTLR_STATUS_PING    2
#define        OLYMPUS_SBIST_FCTLR_STATUS_DONE    3
#define        OLYMPUS_SBIST_FCTLR_STATUS_FAIL    4
#define        OLYMPUS_SBIST_FCTLR_STATUS_PDONE   5

#define        OLYMPUS_SBIST_FFMIR_FMID_NOTVAL    0
#define        OLYMPUS_SBIST_FFMIR_FMID_DEADLOCK  1
#define        OLYMPUS_SBIST_FFMIR_FMID_EXCEPTION 2
#define        OLYMPUS_SBIST_FFMIR_FMID_PMU       3
#define        OLYMPUS_SBIST_FFMIR_FMID_DATAC     4

/* SBIST Controller Fault Control Register Definitions */
#define SBIST_FCTLR_STATUS_Pos      0U                                          /*!< FCTLR: STATUS Position */
#define SBIST_FCTLR_STATUS_Msk            (0xFUL << SBIST_FCTLR_STATUS_Pos)     /*!< FCTLR: STATUS Mask */

#define SBIST_FCTLR_FREQ_Pos       16U                                          /*!< FCTLR: FREQ Position */
#define SBIST_FCTLR_FREQ_Msk              (0xFFFFUL << SBIST_FCTLR_FREQ_Pos)    /*!< FCTLR: FREQ Mask */

/* SBIST Controller Fault Partition Identifier Register Definitions */
#define SBIST_FPIR_ITER_Pos        0U                                           /*!< FPIR: ITER Position */
#define SBIST_FPIR_ITER_Msk               (0xFFUL << SBIST_FPIR_ITER_Pos)       /*!< FPIR: ITER Mask */

#define SBIST_FPIR_CID_Pos         8U                                           /*!< FPIR: CID Position */
#define SBIST_FPIR_CID_Msk                (0xFFUL << SBIST_FPIR_CID_Pos)        /*!< FPIR: CID Mask */

#define SBIST_FPIR_STID_Pos       16U                                           /*!< FPIR: STID Position */
#define SBIST_FPIR_STID_Msk               (0x3FUL << SBIST_FPIR_STID_Pos)       /*!< FPIR: STID Mask */

#define SBIST_FPIR_MTID_Pos       24U                                           /*!< FPIR: MTID Position */
#define SBIST_FPIR_MTID_Msk               (0x3FUL << SBIST_FPIR_MTID_Pos)       /*!< FPIR: MTID Mask */

/* SBIST Controller Fault Failure Mode Identification Register Definitions */
#define SBIST_FFMIR_FMID_Pos       0U                                           /*!< FFMIR: FMID Position */
#define SBIST_FFMIR_FMID_Msk              (0xFUL << SBIST_FFMIR_FMID_Pos)       /*!< FFMIR: FMID Mask */

/* SBIST Controller Fault PS Payload Register 0 Definitions */
//    a. bit 0     = Indicates whether the payload is active or not
//    b. bit [9:1] = Interrupt number to be generated
//    c. bit 12    = Indicates start of periodic interrupt generation with
//                   increasing interrupt numbers
//    d. bit 13    = WICSENSE override
//    e. bit 11    = 0 => Periodic interrupt generation
//                   1 => Single interrupt
#define SBIST_FPSPR0_ACTIVE_Pos               0U                                            /*!< FPSPR0: ACTIVE Position */
#define SBIST_FPSPR0_ACTIVE_Msk               (1UL << SBIST_FPSPR0_ACTIVE_Pos)              /*!< FPSPR0: ACTIVE Mask */

#define SBIST_FPSPR0_IRQNUM_Pos               1U                                            /*!< FPSPR0: IRQNUM Position */
#define SBIST_FPSPR0_IRQNUM_Msk               (0x1FFUL << SBIST_FPSPR0_IRQNUM_Pos)          /*!< FPSPR0: IRQNUM Mask */

#define SBIST_FPSPR0_INCPERINTSTART_Pos       12U                                           /*!< FPSPR0: INCPERINTSTART Position */
#define SBIST_FPSPR0_INCPERINTSTART_Msk       (0x1UL << SBIST_FPSPR0_INCPERINTSTART_Pos)    /*!< FPSPR0: INCPERINTSTART Mask */

#define SBIST_FPSPR0_SINGLEINTR_Pos           11U                                           /*!< FPSPR0: WSENSEOVERWR Position */
#define SBIST_FPSPR0_SINGLEINTR_Msk           (0x1UL << SBIST_FPSPR0_SINGLEINTR_Pos)        /*!< FPSPR0: WSENSEOVERWR Mask */

#define SBIST_FPSPR0_WSENSEOVERWR_Pos         13U                                           /*!< FPSPR0: WSENSEOVERWR Position */
#define SBIST_FPSPR0_WSENSEOVERWR_Msk         (0x1UL << SBIST_FPSPR0_WSENSEOVERWR_Pos)      /*!< FPSPR0: WSENSEOVERWR Mask */

#define SBIST_FPSPR0_INTRPERIOD_Pos           16U                                           /*!< FPSPR0: INTRPERIOD Position */
#define SBIST_FPSPR0_INTRPERIOD_Msk           (0xFFFFUL << SBIST_FPSPR0_INTRPERIOD_Pos)     /*!< FPSPR0: INTRPERIOD Mask */

/* SBIST AXI SLAVE FPSPR register definitions */
#define SBIST_FPSPR4_SLAVEEN_Pos              0U
#define SBIST_FPSPR4_AWDELAY_Pos              1U
#define SBIST_FPSPR4_WDELAY_Pos               5U
#define SBIST_FPSPR4_BDELAY_Pos               9U
#define SBIST_FPSPR4_ARDELAY_Pos              13U
#define SBIST_FPSPR4_RDELAY_Pos               17U
#define SBIST_FPSPR4_BRESP_Pos                21U
#define SBIST_FPSPR4_RRESP_Pos                23U
#define SBIST_FPSPR4_WOS_Pos                  25U
#define SBIST_FPSPR4_ROS_Pos                  30U


/******************************************************************************/
/*                CoreSight SoC-600 Trace Memory Controller ETB               */
/******************************************************************************/

/**********************************************************************/
// from cx_tmc_regs.h, which is delivered as part of CoreSight SoC-600 bundle:
//
// Trace Memory Controller TMC
//
#define TMC_RSZ_OFFSET     0x004
#define TMC_STS_OFFSET     0x00C
#define TMC_RRD_OFFSET     0x010
#define TMC_RRP_OFFSET     0x014
#define TMC_RWP_OFFSET     0x018
#define TMC_TRG_OFFSET     0x01C
#define TMC_CTL_OFFSET     0x020
#define TMC_RWD_OFFSET     0x024
#define TMC_MODE_OFFSET    0x028
#define TMC_LBUFLEVEL_OFFSET       0x02C
#define TMC_CBUFLEVEL_OFFSET       0x030
#define TMC_BUFWM_OFFSET   0x034
#define TMC_RRPHI_OFFSET   0x038
#define TMC_RWPHI_OFFSET   0x03C
#define TMC_AXICTL_OFFSET  0x110
#define TMC_DBALO_OFFSET   0x118
#define TMC_DBAHI_OFFSET   0x11C
#define TMC_FFSR_OFFSET    0x300
#define TMC_FFCR_OFFSET    0x304
#define TMC_PSCR_OFFSET    0x308
#define TMC_ITATBMDATA0_OFFSET     0xED0
#define TMC_ITATBMCTR2_OFFSET      0xED4
#define TMC_ITATBMCTR1_OFFSET      0xED8
#define TMC_ITATBMCTR0_OFFSET      0xEDC
#define TMC_ITMISCOP0_OFFSET       0xEE0
#define TMC_ITTRFLIN_OFFSET        0xEE8
#define TMC_ITATBDATA0_OFFSET      0xEEC
#define TMC_ITATBCTR2_OFFSET       0xEF0
#define TMC_ITATBCTR1_OFFSET       0xEF4
#define TMC_ITATBCTR0_OFFSET       0xEF8

// Legacy definitions
#define CS_MODE_OFFSET          TMC_MODE_OFFSET
#define CS_DBAHI_OFFSET         TMC_DBAHI_OFFSET
#define CS_DBALO_OFFSET         TMC_DBALO_OFFSET
#define CS_RSZ_OFFSET           TMC_RSZ_OFFSET
#define CS_AXICTL_OFFSET        TMC_AXICTL_OFFSET
/**********************************************************************/

// ETB function prototypes:
uint32_t css600_tmc_etb_configure(uint32_t baseaddr);
uint32_t css600_tmc_initialise_registers(uint32_t baseaddr);
uint32_t css600_tmc_etb_trc_enable(uint32_t baseaddr, uint32_t enable);
uint32_t css600_tmc_flush_and_stop(uint32_t baseaddr);
uint32_t css600_tmc_read_buffer(uint32_t baseaddr);

uint32_t CheckTraceCore_ETB(uint32_t baseaddr);
uint32_t ParseTrace(uint8_t new_byte);

// Structures which are used during checking the ETM trace captured by ETB
// (these are imported from debugdriver.c, which performs the same checks for the ETM trace
// captured by TPIU)
typedef enum  { ETM_NEW_PKT = 0,
                ETM_EXT_PKT = 1,
                ETM_TI__PKT = 2,
                ETM_TS__PKT = 3,
                ETM_EXP_PKT = 4,
                ETM_TSC_PKT = 5,
                ETM_LAR_PKT = 6,
                ETM_CAR_PKT = 7,
                ETM_SAR_PKT = 8,
                ETM_COM_PKT = 9,
                ETM_CAN_PKT = 10,
                ETM_CR__PKT = 11,
                ETM_CET_PKT = 12,
                ETM_CC__PKT = 13
              } etm_current_pkt;

typedef struct {
  etm_current_pkt pkt;
  uint32_t        bytes;
  uint32_t        address;
  uint32_t        H0_address;
  uint32_t        H1_address;
  uint32_t        H2_address;
  uint32_t        ts1;
  uint32_t        ts1_last;
  uint32_t        ts2;
  uint32_t        ti_payload;
  uint32_t        cc;
} etm_decomp_state_t;


typedef struct {
  uint32_t etm_p_headers;
  uint32_t trace_info;
  uint32_t etm_branches;
  uint32_t exceptions;
  uint32_t rfe;
  uint32_t timestamps;
  uint32_t triggers;
  uint32_t etm_ignore;
} etm_output_t;

/******************************************************************************/
/*          CoreSight and other Peripheral memory map                         */
/******************************************************************************/
/* Components */
#define TPIU_BASE                 0xE0040000UL
#define ETM_BASE                  0xE0041000UL
#define CTI_BASE                  0xE0042000UL
#define FPB_BASE                  0xE0002000UL
#define ETB_BASE                  0xE0045000UL
#define PMC_BASE                  0xE0046000UL
#define EWIC_MCU_BASE             0xE0047000UL
#define L1_BASE                   0xE001E000UL
#define IMPDEF_BASE               0xE0005000UL

#define DWT_CTRL_SYNCTAP24        1 << 10
#define DWT_CTRL_SYNCTAP26        2 << 10
#define DWT_CTRL_SYNCTAP28        3 << 10

#define DWT_CTRL_POSTPRESET_10    0xA

#define ITM_TER_STIM0             1 << 0
#define ITM_TER_STIM1             1 << 1
#define ITM_TER_STIM2             1 << 2

#define ITM_TCR_TS_GLOBAL_128     0x01 << ITM_TCR_GTSFREQ_Pos
#define ITM_TCR_TS_GLOBAL_8192    0x10 << ITM_TCR_GTSFREQ_Pos
#define ITM_TCR_TS_GLOBAL_ALL     0x11 << ITM_TCR_GTSFREQ_Pos

/* Peripheral and SRAM base address */
#define SRAM_BASE                 0x20000000UL
#define PERIPH_BASE               0x40000000UL

/* Peripheral memory map */
#define GPIO_BASE                 PERIPH_BASE

#define GPIO0_BASE                (GPIO_BASE)
#define GPIO1_BASE                (GPIO_BASE       + 0x0800UL)
#define GPIO2_BASE                (GPIO_BASE       + 0x1000UL)
#define GPIO3_BASE                (GPIO_BASE       + 0x1800UL)
#define BASEADDR_EXT_REG          (GPIO_BASE       + 0x2000UL)
#define DMAC_BASE                 (PERIPH_BASE     + 0x2800UL)
#define PMC_EXT_BASE              (PERIPH_BASE     + 0x3000UL)

/******************************************************************************/
/*                         Declaration                                        */
/******************************************************************************/
#define ETM                       ((ETM_Type     *) ETM_BASE)
#define TPIU                      ((TPIU_Type    *) TPIU_BASE)
#define CTI                       ((CTI_Type     *) CTI_BASE)
#define L1                        ((L1_Type      *) L1_BASE)
#define IMPDEF                    ((IMPDEF_Type  *) IMPDEF_BASE)

#define GPIO0                     ((GPIO_TypeDef *) GPIO0_BASE)
#define GPIO1                     ((GPIO_TypeDef *) GPIO1_BASE)
#define GPIO2                     ((GPIO_TypeDef *) GPIO2_BASE)
#define GPIO3                     ((GPIO_TypeDef *) GPIO3_BASE)

#endif



