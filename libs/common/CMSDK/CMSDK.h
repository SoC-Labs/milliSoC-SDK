#include "sys_memory_map.h"
#include <stdint.h>
#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
/** @addtogroup CMSDK_UART CMSDK Universal Asynchronous Receiver/Transmitter
  memory mapped structure for CMSDK_UART
  @{
*/
typedef struct
{
  volatile uint32_t  DATA;          /*!< Offset: 0x000 Data Register    (R/W) */
  volatile uint32_t  STATE;         /*!< Offset: 0x004 Status Register  (R/W) */
  volatile uint32_t  CTRL;          /*!< Offset: 0x008 Control Register (R/W) */
  union {
    volatile    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
    volatile    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };
  volatile uint32_t  BAUDDIV;       /*!< Offset: 0x010 Baudrate Divider Register (R/W) */

} CMSDK_UART_TypeDef;

/* CMSDK_UART DATA Register Definitions */

#define CMSDK_UART_DATA_Pos               0                                             /*!< CMSDK_UART_DATA_Pos: DATA Position */
#define CMSDK_UART_DATA_Msk              (0xFFul << CMSDK_UART_DATA_Pos)                /*!< CMSDK_UART DATA: DATA Mask */

#define CMSDK_UART_STATE_RXOR_Pos         3                                             /*!< CMSDK_UART STATE: RXOR Position */
#define CMSDK_UART_STATE_RXOR_Msk         (0x1ul << CMSDK_UART_STATE_RXOR_Pos)          /*!< CMSDK_UART STATE: RXOR Mask */

#define CMSDK_UART_STATE_TXOR_Pos         2                                             /*!< CMSDK_UART STATE: TXOR Position */
#define CMSDK_UART_STATE_TXOR_Msk         (0x1ul << CMSDK_UART_STATE_TXOR_Pos)          /*!< CMSDK_UART STATE: TXOR Mask */

#define CMSDK_UART_STATE_RXBF_Pos         1                                             /*!< CMSDK_UART STATE: RXBF Position */
#define CMSDK_UART_STATE_RXBF_Msk         (0x1ul << CMSDK_UART_STATE_RXBF_Pos)          /*!< CMSDK_UART STATE: RXBF Mask */

#define CMSDK_UART_STATE_TXBF_Pos         0                                             /*!< CMSDK_UART STATE: TXBF Position */
#define CMSDK_UART_STATE_TXBF_Msk         (0x1ul << CMSDK_UART_STATE_TXBF_Pos )         /*!< CMSDK_UART STATE: TXBF Mask */

#define CMSDK_UART_CTRL_HSTM_Pos          6                                             /*!< CMSDK_UART CTRL: HSTM Position */
#define CMSDK_UART_CTRL_HSTM_Msk          (0x01ul << CMSDK_UART_CTRL_HSTM_Pos)          /*!< CMSDK_UART CTRL: HSTM Mask */

#define CMSDK_UART_CTRL_RXORIRQEN_Pos     5                                             /*!< CMSDK_UART CTRL: RXORIRQEN Position */
#define CMSDK_UART_CTRL_RXORIRQEN_Msk     (0x01ul << CMSDK_UART_CTRL_RXORIRQEN_Pos)     /*!< CMSDK_UART CTRL: RXORIRQEN Mask */

#define CMSDK_UART_CTRL_TXORIRQEN_Pos     4                                             /*!< CMSDK_UART CTRL: TXORIRQEN Position */
#define CMSDK_UART_CTRL_TXORIRQEN_Msk     (0x01ul << CMSDK_UART_CTRL_TXORIRQEN_Pos)     /*!< CMSDK_UART CTRL: TXORIRQEN Mask */

#define CMSDK_UART_CTRL_RXIRQEN_Pos       3                                             /*!< CMSDK_UART CTRL: RXIRQEN Position */
#define CMSDK_UART_CTRL_RXIRQEN_Msk       (0x01ul << CMSDK_UART_CTRL_RXIRQEN_Pos)       /*!< CMSDK_UART CTRL: RXIRQEN Mask */

#define CMSDK_UART_CTRL_TXIRQEN_Pos       2                                             /*!< CMSDK_UART CTRL: TXIRQEN Position */
#define CMSDK_UART_CTRL_TXIRQEN_Msk       (0x01ul << CMSDK_UART_CTRL_TXIRQEN_Pos)       /*!< CMSDK_UART CTRL: TXIRQEN Mask */

#define CMSDK_UART_CTRL_RXEN_Pos          1                                             /*!< CMSDK_UART CTRL: RXEN Position */
#define CMSDK_UART_CTRL_RXEN_Msk          (0x01ul << CMSDK_UART_CTRL_RXEN_Pos)          /*!< CMSDK_UART CTRL: RXEN Mask */

#define CMSDK_UART_CTRL_TXEN_Pos          0                                             /*!< CMSDK_UART CTRL: TXEN Position */
#define CMSDK_UART_CTRL_TXEN_Msk          (0x01ul << CMSDK_UART_CTRL_TXEN_Pos)          /*!< CMSDK_UART CTRL: TXEN Mask */

#define CMSDK_UART_INTSTATUS_RXORIRQ_Pos  3                                             /*!< CMSDK_UART CTRL: RXORIRQ Position */
#define CMSDK_UART_CTRL_RXORIRQ_Msk       (0x01ul << CMSDK_UART_INTSTATUS_RXORIRQ_Pos)  /*!< CMSDK_UART CTRL: RXORIRQ Mask */

#define CMSDK_UART_CTRL_TXORIRQ_Pos       2                                             /*!< CMSDK_UART CTRL: TXORIRQ Position */
#define CMSDK_UART_CTRL_TXORIRQ_Msk       (0x01ul << CMSDK_UART_CTRL_TXORIRQ_Pos)       /*!< CMSDK_UART CTRL: TXORIRQ Mask */

#define CMSDK_UART_CTRL_RXIRQ_Pos         1                                             /*!< CMSDK_UART CTRL: RXIRQ Position */
#define CMSDK_UART_CTRL_RXIRQ_Msk         (0x01ul << CMSDK_UART_CTRL_RXIRQ_Pos)         /*!< CMSDK_UART CTRL: RXIRQ Mask */

#define CMSDK_UART_CTRL_TXIRQ_Pos         0                                             /*!< CMSDK_UART CTRL: TXIRQ Position */
#define CMSDK_UART_CTRL_TXIRQ_Msk         (0x01ul << CMSDK_UART_CTRL_TXIRQ_Pos)         /*!< CMSDK_UART CTRL: TXIRQ Mask */

#define CMSDK_UART_BAUDDIV_Pos            0                                             /*!< CMSDK_UART BAUDDIV: BAUDDIV Position */
#define CMSDK_UART_BAUDDIV_Msk           (0xFFFFFul << CMSDK_UART_BAUDDIV_Pos)          /*!< CMSDK_UART BAUDDIV: BAUDDIV Mask */

/*@}*/ /* end of group CMSDK_UART */


#define CMSDK_UART0             ((CMSDK_UART_TypeDef   *) SYS_UART0_BASE   )
#define CMSDK_UART1             ((CMSDK_UART_TypeDef   *) SYS_UART1_BASE   )

/*----------------------------- Timer (TIMER) -------------------------------*/
/** @addtogroup CMSDK_TIMER CMSDK Timer
  @{
*/
typedef struct
{
    volatile   uint32_t  CTRL;          /*!< Offset: 0x000 Control Register (R/W) */
    volatile   uint32_t  VALUE;         /*!< Offset: 0x004 Current Value Register (R/W) */
    volatile   uint32_t  RELOAD;        /*!< Offset: 0x008 Reload Value Register  (R/W) */
    union {
        const volatile    uint32_t  INTSTATUS;   /*!< Offset: 0x00C Interrupt Status Register (R/ ) */
        volatile    uint32_t  INTCLEAR;    /*!< Offset: 0x00C Interrupt Clear Register ( /W) */
    };
            uint32_t RESERVED[1008]; // 0x010 - 0xFD0
    const volatile     uint32_t PID4;
    const volatile     uint32_t PID5;
    const volatile     uint32_t PID6;
    const volatile     uint32_t PID7;
    const volatile     uint32_t PID0;
    const volatile     uint32_t PID1;
    const volatile     uint32_t PID2;
    const volatile     uint32_t PID3;
    const volatile     uint32_t CID0;
    const volatile     uint32_t CID1;
    const volatile     uint32_t CID2;
    const volatile     uint32_t CID3;
} CMSDK_TIMER_TypeDef;

/* CMSDK_TIMER CTRL Register Definitions */

#define CMSDK_TIMER_CTRL_IRQEN_Pos          3                                              /*!< CMSDK_TIMER CTRL: IRQEN Position */
#define CMSDK_TIMER_CTRL_IRQEN_Msk          (0x01ul << CMSDK_TIMER_CTRL_IRQEN_Pos)         /*!< CMSDK_TIMER CTRL: IRQEN Mask */

#define CMSDK_TIMER_CTRL_SELEXTCLK_Pos      2                                              /*!< CMSDK_TIMER CTRL: SELEXTCLK Position */
#define CMSDK_TIMER_CTRL_SELEXTCLK_Msk      (0x01ul << CMSDK_TIMER_CTRL_SELEXTCLK_Pos)     /*!< CMSDK_TIMER CTRL: SELEXTCLK Mask */

#define CMSDK_TIMER_CTRL_SELEXTEN_Pos       1                                              /*!< CMSDK_TIMER CTRL: SELEXTEN Position */
#define CMSDK_TIMER_CTRL_SELEXTEN_Msk       (0x01ul << CMSDK_TIMER_CTRL_SELEXTEN_Pos)      /*!< CMSDK_TIMER CTRL: SELEXTEN Mask */

#define CMSDK_TIMER_CTRL_EN_Pos             0                                              /*!< CMSDK_TIMER CTRL: EN Position */
#define CMSDK_TIMER_CTRL_EN_Msk             (0x01ul << CMSDK_TIMER_CTRL_EN_Pos)            /*!< CMSDK_TIMER CTRL: EN Mask */

#define CMSDK_TIMER_VAL_CURRENT_Pos         0                                              /*!< CMSDK_TIMER VALUE: CURRENT Position */
#define CMSDK_TIMER_VAL_CURRENT_Msk         (0xFFFFFFFFul << CMSDK_TIMER_VAL_CURRENT_Pos)  /*!< CMSDK_TIMER VALUE: CURRENT Mask */

#define CMSDK_TIMER_RELOAD_VAL_Pos          0                                              /*!< CMSDK_TIMER RELOAD: RELOAD Position */
#define CMSDK_TIMER_RELOAD_VAL_Msk          (0xFFFFFFFFul << CMSDK_TIMER_RELOAD_VAL_Pos)   /*!< CMSDK_TIMER RELOAD: RELOAD Mask */

#define CMSDK_TIMER_INTSTATUS_Pos           0                                              /*!< CMSDK_TIMER INTSTATUS: INTSTATUSPosition */
#define CMSDK_TIMER_INTSTATUS_Msk           (0x01ul << CMSDK_TIMER_INTSTATUS_Pos)          /*!< CMSDK_TIMER INTSTATUS: INTSTATUSMask */

#define CMSDK_TIMER_INTCLEAR_Pos            0                                              /*!< CMSDK_TIMER INTCLEAR: INTCLEAR Position */
#define CMSDK_TIMER_INTCLEAR_Msk            (0x01ul << CMSDK_TIMER_INTCLEAR_Pos)           /*!< CMSDK_TIMER INTCLEAR: INTCLEAR Mask */

/*@}*/ /* end of group CMSDK_TIMER */

#define CMSDK_TIMER0            ((CMSDK_TIMER_TypeDef  *) TIMER0_BASE  )
#define CMSDK_TIMER1            ((CMSDK_TIMER_TypeDef  *) TIMER1_BASE  )

/*-------------------------- Dual Timer (SP804) -----------------------------*/
/** @addtogroup CMSDK_DUALTIMER CMSDK Dual Timer
  memory mapped structure for the CMSDK/SP804 dual timer
  @{
*/

/* One half (one of the two independent 32-bit timers) of the dual timer. */
typedef struct
{
    volatile          uint32_t  LOAD;       /*!< Offset: 0x00 Load Register (R/W) */
    volatile          uint32_t  VALUE;      /*!< Offset: 0x04 Current Value Register (R/W) */
    volatile          uint32_t  CONTROL;    /*!< Offset: 0x08 Control Register (R/W) */
    volatile          uint32_t  INTCLR;     /*!< Offset: 0x0C Interrupt Clear Register ( /W) */
    const volatile    uint32_t  RIS;        /*!< Offset: 0x10 Raw Interrupt Status (R/ ) */
    const volatile    uint32_t  MIS;        /*!< Offset: 0x14 Masked Interrupt Status (R/ ) */
    volatile          uint32_t  BGLOAD;     /*!< Offset: 0x18 Background Load Register (R/W) */
    const volatile    uint32_t  RESERVED;   /*!< Offset: 0x1C Reserved */
} CMSDK_DUALTIMER_CHANNEL_TypeDef;

typedef struct
{
    CMSDK_DUALTIMER_CHANNEL_TypeDef TIMER[2]; /*!< Offset: 0x00 Timer 1, Offset: 0x20 Timer 2 */
                uint32_t RESERVED0[996];      /*!< Offset: 0x040 - 0xFCC */
    const volatile     uint32_t PID4;
    const volatile     uint32_t PID5;
    const volatile     uint32_t PID6;
    const volatile     uint32_t PID7;
    const volatile     uint32_t PID0;
    const volatile     uint32_t PID1;
    const volatile     uint32_t PID2;
    const volatile     uint32_t PID3;
    const volatile     uint32_t CID0;
    const volatile     uint32_t CID1;
    const volatile     uint32_t CID2;
    const volatile     uint32_t CID3;
} CMSDK_DUALTIMER_TypeDef;

/* CMSDK_DUALTIMER CONTROL Register Definitions (ARM DDI 0479, CMSDK APB
 * dual-input timer).  Reset value is 0x20 (interrupt enable set). */

#define CMSDK_DUALTIMER_CTRL_ONESHOT_Pos      0                                              /*!< 1: one-shot, 0: wrapping/periodic */
#define CMSDK_DUALTIMER_CTRL_ONESHOT_Msk      (0x01ul << CMSDK_DUALTIMER_CTRL_ONESHOT_Pos)

#define CMSDK_DUALTIMER_CTRL_SIZE_Pos         1                                              /*!< 1: 32-bit counter, 0: 16-bit counter */
#define CMSDK_DUALTIMER_CTRL_SIZE_Msk         (0x01ul << CMSDK_DUALTIMER_CTRL_SIZE_Pos)

#define CMSDK_DUALTIMER_CTRL_PRESCALE_Pos     2                                              /*!< 2-bit prescale field */
#define CMSDK_DUALTIMER_CTRL_PRESCALE_Msk     (0x03ul << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos)
#define CMSDK_DUALTIMER_CTRL_PRESCALE_DIV1    (0x00ul << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos)  /*!< divide by 1 */
#define CMSDK_DUALTIMER_CTRL_PRESCALE_DIV16   (0x01ul << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos)  /*!< divide by 16 */
#define CMSDK_DUALTIMER_CTRL_PRESCALE_DIV256  (0x02ul << CMSDK_DUALTIMER_CTRL_PRESCALE_Pos)  /*!< divide by 256 */

#define CMSDK_DUALTIMER_CTRL_IRQEN_Pos        5                                              /*!< 1: interrupt enabled */
#define CMSDK_DUALTIMER_CTRL_IRQEN_Msk        (0x01ul << CMSDK_DUALTIMER_CTRL_IRQEN_Pos)

#define CMSDK_DUALTIMER_CTRL_PERIODIC_Pos     6                                              /*!< 1: periodic, 0: free-running */
#define CMSDK_DUALTIMER_CTRL_PERIODIC_Msk     (0x01ul << CMSDK_DUALTIMER_CTRL_PERIODIC_Pos)

#define CMSDK_DUALTIMER_CTRL_EN_Pos           7                                              /*!< 1: timer enabled */
#define CMSDK_DUALTIMER_CTRL_EN_Msk           (0x01ul << CMSDK_DUALTIMER_CTRL_EN_Pos)

/* RIS/MIS are per channel and only use bit 0. */
#define CMSDK_DUALTIMER_INT_Pos               0
#define CMSDK_DUALTIMER_INT_Msk               (0x01ul << CMSDK_DUALTIMER_INT_Pos)

/*@}*/ /* end of group CMSDK_DUALTIMER */

#define CMSDK_DUALTIMER         ((CMSDK_DUALTIMER_TypeDef *) DUALTIMER_BASE)

/*------------------------- Real Time Clock (PL031) -------------------------*/
/** @addtogroup CMSDK_RTC CMSDK/PL031 Real Time Clock
  memory mapped structure for the Arm PL031 RTC
  @{
*/
typedef struct
{
    volatile          uint32_t  DR;         /*!< Offset: 0x000 Data Register (R/W) */
    volatile          uint32_t  MR;         /*!< Offset: 0x004 Match Register (R/W) */
    volatile          uint32_t  LR;         /*!< Offset: 0x008 Load Register (R/W) */
    volatile          uint32_t  CR;         /*!< Offset: 0x00C Control Register (R/W) */
    volatile          uint32_t  IMSC;       /*!< Offset: 0x010 Interrupt Mask Set/Clear (R/W) */
    const volatile    uint32_t  RIS;        /*!< Offset: 0x014 Raw Interrupt Status (R/ ) */
    const volatile    uint32_t  MIS;        /*!< Offset: 0x018 Masked Interrupt Status (R/ ) */
    volatile          uint32_t  ICR;        /*!< Offset: 0x01C Interrupt Clear Register ( /W) */
                uint32_t  RESERVED0[1004];  /*!< Offset: 0x020 - 0xFCC */
    const volatile     uint32_t PID4;
    const volatile     uint32_t PID5;
    const volatile     uint32_t PID6;
    const volatile     uint32_t PID7;
    const volatile     uint32_t PID0;
    const volatile     uint32_t PID1;
    const volatile     uint32_t PID2;
    const volatile     uint32_t PID3;
    const volatile     uint32_t CID0;
    const volatile     uint32_t CID1;
    const volatile     uint32_t CID2;
    const volatile     uint32_t CID3;
} CMSDK_RTC_TypeDef;

/* CMSDK_RTC CR Register Definitions */
#define CMSDK_RTC_CR_RTCEN_Pos                0
#define CMSDK_RTC_CR_RTCEN_Msk                (0x01ul << CMSDK_RTC_CR_RTCEN_Pos)  /*!< 1: counter enabled */

/* CMSDK_RTC IMSC/RIS/MIS/ICR Register Definitions */
#define CMSDK_RTC_INT_MATCH_Pos               0
#define CMSDK_RTC_INT_MATCH_Msk               (0x01ul << CMSDK_RTC_INT_MATCH_Pos) /*!< Match interrupt */

/*@}*/ /* end of group CMSDK_RTC */

#define CMSDK_RTC               ((CMSDK_RTC_TypeDef     *) SYS_RTC_BASE )
