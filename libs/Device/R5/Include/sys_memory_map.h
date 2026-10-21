//------------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from Arm Limited or its affiliates.
//
//        (C) COPYRIGHT 2018-2021 Arm Limited or its affiliates.
//            ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Arm Limited or its affiliates.
//
//      Release Information : SSE710-r0p0-00rel0
//
// -----------------------------------------------------------------------------

#define ADA_DMA_S_BASE  0x44000000UL
#define ADA_DMA_NS_BASE  0x44000000UL
#define QSPI_CFG_BASE   0x44010000UL

#define FLASH_BASE      0x60000000UL
#define ITCM_BASE       0x00000000UL
#define ITCM_SIZE       0x10000UL

#define PERIPHERAL_BASE     0x40000000UL
#define TIMER0_BASE         PERIPHERAL_BASE
#define TIMER1_BASE         (PERIPHERAL_BASE + 0x1000UL)
#define DUALTIMER_BASE      (PERIPHERAL_BASE + 0x2000UL)

#define SYS_UART0_BASE      (PERIPHERAL_BASE + 0x5000UL)
#define SYS_UART1_BASE      (PERIPHERAL_BASE + 0x6000UL)
#define SYS_WATCHDOG_BASE   (PERIPHERAL_BASE + 0x7000UL)
#define SYS_RTC_BASE        (PERIPHERAL_BASE + 0x8000UL)
#define SYS_SPI_BASE        (PERIPHERAL_BASE + 0x9000UL)
#define SYS_PL011_UART      (PERIPHERAL_BASE + 0xA000UL)
#define GPIO_0_BASE         0x40010000UL
#define GPIO_1_BASE         0x40020000UL
#define MHU_SND             0x40040000UL // to 0x4005FFFF
#define MHU_REC             0x40060000UL // to 0x4007FFFF

#define VIC_BASE            0x44020000UL

#define SYSCTRL_BASE        0x44030000UL

#define PASS_CODE 0xBEEF1AC0
#define FAIL_CODE 0xDEADBEEF


