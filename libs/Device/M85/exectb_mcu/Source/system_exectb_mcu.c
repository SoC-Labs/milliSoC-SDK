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

#include <stdint.h>
#include "exectb_mcu.h"
#include "EXECTB_Config.h"

/*----------------------------------------------------------------------------
  Define SYSCLK
 *----------------------------------------------------------------------------*/
#define __HSI (5000000UL)

/*----------------------------------------------------------------------------
  Externals
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  #if defined (__ARMCC_VERSION)
  extern uint32_t __Vectors;
  #else
  extern uint32_t __isr_vector;
  #endif
#endif

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

void SystemInit (void) __attribute__ ((noinline));
void SystemInit (void)
{
  // Fix the top of stack so that it does not contain Xs
    #if defined (__ARMCC_VERSION)
    extern unsigned int  Image$$ARM_LIB_STACK$$ZI$$Limit;
    uint32_t reg0 = (uint32_t)&Image$$ARM_LIB_STACK$$ZI$$Limit - 0x8;
    #else
    extern uint32_t __StackTop;
    uint32_t reg0 = (uint32_t)&__StackTop;
    #endif

  asm volatile (
  "MOV   r0,%[reg0]\n\t"
  "MOVS  r2, #0x0\n\t"
  "STR   r2, [r0]\n\t"
      :
      : [reg0] "r" (reg0)
      :
  );

  #if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    #if defined (__ARMCC_VERSION)
    SCB->VTOR = (uint32_t) &__Vectors;
    #else
    SCB->VTOR = (uint32_t) &__isr_vector;
    #endif
  #endif

  // Initialise all registers to zero
  asm volatile (

  "MOVS  r0, #0x0\n\t"
  "MOV   r1, r0\n\t"
  "MOV   r2, r0\n\t"
  "MOV   r3, r0\n\t"
  "MOV   r4, r0\n\t"
  "MOV   r5, r0\n\t"
  "MOV   r6, r0\n\t"
  "MOV   r7, r0\n\t"
  "MOV   r8, r0\n\t"
  "MOV   r9, r0\n\t"
  "MOV   r10,r0\n\t"
  "MOV   r11,r0\n\t"
  "MOV   r12,r0"
  );
}

#ifndef NS_IMAGE
void fpu_enable(void) __attribute__ ((noinline));
void fpu_enable(void)
{
  uint32_t value;
  uint32_t fpu_flag = 0;

#ifdef _FPU
  fpu_flag = 1;
#endif
  value = SCB->CPACR;
  SCB->CPACR = value | 0x00F00000;
  __ISB();

  if (fpu_flag == 1)
    {
      asm volatile (
      // Enable the co-processors in CPACR
      "MOV      r0,#0xF00000\n"
      "LDR      r1,=0xE000ED88\n"
      "LDR      r2,[r1]\n"
      "ORR      r0,r2,r0\n"
      "STR      r0,[r1]\n"
      // Set the ELPSTATE to ON
      "MOV      r0,0xFFFFFFCF\n"
      "LDR      r1,=0xE001E300\n"
      "LDR      r2,[r1]\n"
      "AND      r0,r2,r0\n"
      "STR      r0,[r1]\n"
      "ISB\n"
      // Enabled auto FP handling around exceptions and LTPSIZE==4
      "MOV      r0,#0x0000\n"
      "MOVT     r0,#0x0304\n"
      "VMSR     FPSCR,r0\n"
      // Enable low overhead loops
      "MOV      r0,#0x80000\n"
      "LDR      r1,=0xE000ED14\n"
      "LDR      r2, [r1]\n"
      "ORR      r0, r2, r0\n"
      "STR      r0, [r1]\n"
      // Initialise the Floating point registers to zero
      "MOV  r2, #0x0000\n\t"
      "MOVT r2, #0x0000\n\t"
      "VMOV.32 s0,  r2\n\t"
      "VMOV.32 s1,  r2\n\t"
      "VMOV.32 s2,  r2\n\t"
      "VMOV.32 s3,  r2\n\t"
      "VMOV.32 s4,  r2\n\t"
      "VMOV.32 s5,  r2\n\t"
      "VMOV.32 s6,  r2\n\t"
      "VMOV.32 s7,  r2\n\t"
      "VMOV.32 s8,  r2\n\t"
      "VMOV.32 s9,  r2\n\t"
      "VMOV.32 s10, r2\n\t"
      "VMOV.32 s11, r2\n\t"
      "VMOV.32 s12, r2\n\t"
      "VMOV.32 s13, r2\n\t"
      "VMOV.32 s14, r2\n\t"
      "VMOV.32 s15, r2\n\t"
      "VMOV.32 s16, r2\n\t"
      "VMOV.32 s17, r2\n\t"
      "VMOV.32 s18, r2\n\t"
      "VMOV.32 s19, r2\n\t"
      "VMOV.32 s20, r2\n\t"
      "VMOV.32 s21, r2\n\t"
      "VMOV.32 s22, r2\n\t"
      "VMOV.32 s23, r2\n\t"
      "VMOV.32 s24, r2\n\t"
      "VMOV.32 s25, r2\n\t"
      "VMOV.32 s26, r2\n\t"
      "VMOV.32 s27, r2\n\t"
      "VMOV.32 s28, r2\n\t"
      "VMOV.32 s29, r2\n\t"
      "VMOV.32 s30, r2\n\t"
      "VMOV.32 s31, r2\n\t"
      "ISB\n\t"
      :
      :
      : "r2", "s0"
      );
    }
}
#endif // ifndef NS_IMAGE

/* ##########################  Cache functions  #################################### */

#if ((defined (__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)) || \
     (defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)))

void cache_enable(void) __attribute__ ((noinline));
void cache_enable(void)
{
  SCB->CCR |=  (uint32_t)SCB_CCR_LOB_Msk;  // Enable loop and branch info
  SCB->CCR |=  (uint32_t)SCB_CCR_BP_Msk;   // Enable branch predication

  SCB_NS->CCR |=  (uint32_t)SCB_CCR_LOB_Msk;  // Enable loop and branch info
  SCB_NS->CCR |=  (uint32_t)SCB_CCR_BP_Msk;   // Enable branch predication

  SCB_EnableICache();
  SCB_EnableDCache();
}

void cache_invalidate(void) __attribute__ ((noinline));
void cache_invalidate(void)
{
  SCB_InvalidateICache();
  SCB_InvalidateDCache();
}

#endif /* #if Cache present */

void LPStateInit(void) __attribute__ ((noinline));
void LPStateInit(void)
{
  //
  //  Initialize Power Mode control registers based on retention support
  //  at the current execution testbench configuration (EXPECTED_RETENTION).
  //
  //  The CPDLPSTATE and DPDLPSTATE registers allow software to control the
  //  desired power mode of the functional and debug logic in the Olympus processor
  //  (details below)

  // CPDLPSTATE register specifies the desired low-power states for the PDCORE, PEDUP and PDRAMS power domains.
  // CPDLPSTATE encoding:
  // [31:10] --> RES0
  //   [9:8] --> RLPSTATE: type of low-power state for PDRAMS
  //                        0b00 = ON (Don't put the PDRAMS in a low-power state)
  //                        0b01 = ON, but clock off
  //                        0b10 = RET
  //                        0b11 = OFF (default value)
  //   [7:6] --> RES0
  //   [5:4] --> ELPSTATE: type of low-power state for PDEPU
  //                        0b00 = ON (Don't put the PDEPU in a low-power state)
  //                        0b01 = ON, but clock off
  //                        0b10 = RET
  //                        0b11 = OFF (default value)
  //   [3:2] --> RES0
  //   [1:0] --> CLPSTATE: type of low-power state for PDCORE
  //                        0b00 = ON (Don't put the PDCORE in a low-power state)
  //                        0b01 = ON, but clock off
  //                        0b10 = RET
  //                        0b11 = OFF (default value)

  // lpstate is used to set the fields of CPDLPSTATE
  // if EXPECTED_RETENTION=1, lpstate=0b10
  // if EXPECTED_RETENTION=0, lpstate=0b01
  // So, OFF state basically is not allowed for the power domains. If retention is supported, the lowest power
  // state for a domain is RET. If retention is not supported, the lowest power state is ON-with-clocks-off
  // Note: only in sleep.c test, RLPSTATE is overwritten before power-cycling PDRAMs, in such a way that
  // complete POWER OFF is allowed for it (please refer sleep.c, where PWRMODCTL->CPDLPSTATE  is overwritten).
  //

  uint8_t lpstate = EXPECTED_RETENTION<<1 | (1 ^ EXPECTED_RETENTION);

  PWRMODCTL->CPDLPSTATE =  (lpstate << PWRMODCTL_CPDLPSTATE_CLPSTATE_Pos) |
                           (lpstate << PWRMODCTL_CPDLPSTATE_ELPSTATE_Pos) |
                           (lpstate << PWRMODCTL_CPDLPSTATE_RLPSTATE_Pos);

  PWRMODCTL->DPDLPSTATE =  (0x3 << PWRMODCTL_DPDLPSTATE_DLPSTATE_Pos);  // Note that there is no retention support for PDDBG
                                                                  // Set set DPDLPSTATE to 2'b11 which indicates that
                                                                  // the desired low-power state for PDDBG is complete
                                                                  // power-off

  // optional initialisation of CPPWR (Coprocessor Power Control Register)
  //  SCnSCB->CPPWR = (SCnSCB->CPPWR) | (0x1<<20);
  // NOTE: The default setting for CPPWR register does not allow FPU to enter non-retentive power state.
  // If non-retentive power down for EPU is intended, please uncomment the above line together with modifying
  // the CPDLSTATE.ELPSTATE field accordingly
}

