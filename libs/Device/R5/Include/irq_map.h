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

#define NUM_SHD_INTR                    128
#define CNTH_INTR                       26
#define CNTV_INTR                       27
#define CNTPS_INTR                      29
#define CNTPNS_INTR                     30
//SPIs
#define DMA350_CH0_INTR                 32
#define DMA350_CH1_INTR                 33
#define DMA350_CH2_INTR                 34
#define DMA350_CH3_INTR                 35
#define DMA350_COM_INTR                 36

#define GPIO0_COMB_INTR                 37
#define GPIO1_COMB_INTR                 38

#define GPIO0_0_INTR                    39
#define GPIO0_1_INTR                    40
#define GPIO0_2_INTR                    41
#define GPIO0_3_INTR                    42
#define GPIO0_4_INTR                    43
#define GPIO0_5_INTR                    44
#define GPIO0_6_INTR                    45
#define GPIO0_7_INTR                    46
#define GPIO0_8_INTR                    47
#define GPIO0_9_INTR                    48
#define GPIO0_10_INTR                   49
#define GPIO0_11_INTR                   50
#define GPIO0_12_INTR                   51
#define GPIO0_13_INTR                   52
#define GPIO0_14_INTR                   53
#define GPIO0_15_INTR                   54

#define GPIO1_0_INTR                    55
#define GPIO1_1_INTR                    56
#define GPIO1_2_INTR                    57
#define GPIO1_3_INTR                    58
#define GPIO1_4_INTR                    59
#define GPIO1_5_INTR                    60
#define GPIO1_6_INTR                    61
#define GPIO1_7_INTR                    62
#define GPIO1_8_INTR                    63
#define GPIO1_9_INTR                    64
#define GPIO1_10_INTR                   65
#define GPIO1_11_INTR                   66
#define GPIO1_12_INTR                   67
#define GPIO1_13_INTR                   68
#define GPIO1_14_INTR                   69
#define GPIO1_15_INTR                   70

#define TIMER0_INTR                     71
#define TIMER1_INTR                     72

#define DUALTIMER1_INTR                 73
#define DUALTIMER2_INTR                 74
#define DUALTIMER_COM_INTR              75

#define USRT0_TX_INTR                   76
#define USRT0_RX_INTR                   77
#define USRT0_TX_OVR_INTR               78
#define USRT0_RX_OVR_INTR               79
#define USRT0_COMB_INTR                 80

#define USRT1_TX_INTR                   81
#define USRT1_RX_INTR                   82
#define USRT1_TX_OVR_INTR               83
#define USRT1_RX_OVR_INTR               84
#define USRT1_COMB_INTR                 85

#define UART0_TX_INTR                   86
#define UART0_RX_INTR                   87
#define UART0_TX_OVR_INTR               88
#define UART0_RX_OVR_INTR               89
#define UART0_COMB_INTR                 90

#define UART1_TX_INTR                   91
#define UART1_RX_INTR                   92
#define UART1_TX_OVR_INTR               93
#define UART1_RX_OVR_INTR               94
#define UART1_COMB_INTR                 95

#define WATCHDOG_INTR                   96
#define RTC_INTR                        97

#define SPI_COMB_INTR                   98
#define SPI_RX_INTR                     99
#define SPI_TX_INTR                     100
#define SPI_RX_OVERR_INTR               101
#define SPI_TX_TO_INTR                  102  

#define PL011_UARTMSINTR                103
#define PL011_UARTRXINTR                104
#define PL011_UARTTXINTR                105
#define PL011_UARTRTINTR                106
#define PL011_UARTEINTR                 107
#define PL011_UARTINTR                  108

#define QSPI_INTR                       109

#define EXP_DMA350_CH0_INTR             110
#define EXP_DMA350_CH1_INTR             111
#define EXP_DMA350_CH2_INTR             112
#define EXP_DMA350_CH3_INTR             113
#define EXP_DMA350_CMB_INTR             114
