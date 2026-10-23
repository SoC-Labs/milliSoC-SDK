#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>

#define MILLI_SOC_TPIU_BASE       (0xE00F1000UL)
#define MILLI_SOC_ATB_FUNNEL_BASE (0xE00F2000UL)
#define MILLI_SOC_R5_ETM_BASE     (0xE00F3000UL)
#define TPIU_CURRENT_PORT_SIZE    (0x004UL)
#define TPIU_FORMATTER_CONTROL    (0x304UL)
#define ATB_FUNNEL_LAR            (0xFB0UL)
#define ATB_FUNNEL_CONTROL        (0x000UL)
#define M85_ETM_TRACE_ID          (0x22U)
#define R5_ETM_TRACE_ID           (0x23U)
#define ETMR5_PERIPHERAL_ID0      (0xFE0UL)
#define ETMR5_COMPONENT_ID0       (0xFF0UL)
#define ETMR5_CONTROL             (0x000UL)
#define ETMR5_STATUS              (0x010UL)
#define ETMR5_TRACE_ENABLE_EVENT  (0x020UL)
#define ETMR5_ATID                (0x200UL)
#define ETMR5_LAR                 (0xFB0UL)
#define ETMR5_ETMCR_PORTSIZE_32   (1UL << 6)
#define ETMR5_ETMCR_PROGRAMMING   (1UL << 10)
#define ETMR5_ETMCR_ENABLE        (1UL << 11)
#define ETMR5_STATUS_PROGRAMMING  (1UL << 1)
#define ETMR5_ALWAYS_EVENT        (0x6FUL)
#define ETM_STATUS_POLL_LIMIT     (128UL)

static void trace_init(void)
{
    volatile uint32_t * const tpiu = (volatile uint32_t *)MILLI_SOC_TPIU_BASE;
    volatile uint32_t * const funnel =
        (volatile uint32_t *)MILLI_SOC_ATB_FUNNEL_BASE;

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    funnel[ATB_FUNNEL_LAR / sizeof(uint32_t)] = 0xC5ACCE55U;
    funnel[ATB_FUNNEL_CONTROL / sizeof(uint32_t)] = 0x7U;
    tpiu[TPIU_CURRENT_PORT_SIZE / sizeof(uint32_t)] = 0x8U;
    tpiu[TPIU_FORMATTER_CONTROL / sizeof(uint32_t)] = 0x2U;
    ITM->TER = 1U;
    ITM->TCR = ITM_TCR_ITMENA_Msk;
    __DSB();
    __ISB();
}

static int m85_etm_init(void)
{
    ETM->TRCLAR = 0xC5ACCE55U;
    ETM->TRCOSLAR = 0U;
    ETM->TRCPRGCTLR = 0U;

    uint32_t timeout = ETM_STATUS_POLL_LIMIT;
    while ((ETM->TRCSTATR & ETM_TRCSTATR_IDLE_Msk) == 0U && timeout > 0U)
        --timeout;
    if (timeout == 0U)
        return 0;

    ETM->TRCCONFIGR =
        (1UL << ETM_TRCCONFIGR_TS_Pos) | (1UL << ETM_TRCCONFIGR_RS_Pos);
    ETM->TRCEVENTCTL0R = 2U;
    ETM->TRCEVENTCTL1R = 1U;
    ETM->TRCRSCTLR2 = 0x00010001U;
    ETM->TRCSTALLCTLR = 0U;
    ETM->TRCSYNCPR = 0x0CU;
    ETM->TRCTRACEIDR = M85_ETM_TRACE_ID;
    ETM->TRCTSCTLR = 0U;
    ETM->TRCVICTLR =
        (1UL << ETM_TRCVICTLR_SSSTATUS_Pos) |
        (1UL << ETM_TRCVICTLR_EVENT_Pos);
    ETM->TRCVIIECTLR = 0U;
    ETM->TRCVISSCTLR = 0U;
    ETM->TRCCNTRLDVR0 = 0U;
    ETM->TRCSSCCR0 = 0U;
    ETM->TRCSSCSR0 = 0U;
    ETM->TRCSSPCICR0 = 0U;
    ETM->TRCPRGCTLR = ETM_TRCPRGCTLR_EN_Msk;

    __DSB();
    __ISB();
    return ETM->TRCTRACEIDR == M85_ETM_TRACE_ID &&
           (ETM->TRCPRGCTLR & ETM_TRCPRGCTLR_EN_Msk) != 0U;
}

static int r5_etm_init(void)
{
    volatile uint32_t * const etm =
        (volatile uint32_t *)MILLI_SOC_R5_ETM_BASE;
    const uint32_t programming_control =
        ETMR5_ETMCR_PORTSIZE_32 | ETMR5_ETMCR_PROGRAMMING;
    const uint32_t enabled_control =
        ETMR5_ETMCR_PORTSIZE_32 | ETMR5_ETMCR_ENABLE;

    etm[ETMR5_LAR / sizeof(uint32_t)] = 0xC5ACCE55U;
    etm[ETMR5_CONTROL / sizeof(uint32_t)] = programming_control;

    const uint32_t status = etm[ETMR5_STATUS / sizeof(uint32_t)];
    if ((status & ETMR5_STATUS_PROGRAMMING) == 0U)
    {
        printf("R5 ETM programming acknowledgement missing (0x%08lx)\n",
               (unsigned long)status);
        return 0;
    }

    etm[ETMR5_TRACE_ENABLE_EVENT / sizeof(uint32_t)] = ETMR5_ALWAYS_EVENT;
    etm[ETMR5_ATID / sizeof(uint32_t)] = R5_ETM_TRACE_ID;
    etm[ETMR5_CONTROL / sizeof(uint32_t)] = enabled_control;
    __DSB();
    __ISB();

    return 1;
}

static void trace_send_test_marker(void)
{
    static const char marker[] = "M85 ITM trace";

    for (size_t i = 0; i < sizeof(marker) - 1U; ++i)
        ITM_SendChar((uint32_t)marker[i]);
}

static int r5_etm_apb_probe(void)
{
    volatile uint32_t * const etm =
        (volatile uint32_t *)MILLI_SOC_R5_ETM_BASE;

    return etm[ETMR5_PERIPHERAL_ID0 / sizeof(uint32_t)] == 0x31U &&
           etm[ETMR5_COMPONENT_ID0 / sizeof(uint32_t)] == 0x0DU;
}

int main(void){
    UartStdOutInit();

    printf("R5 ETMR5 APB %s\n", r5_etm_apb_probe() ? "PASS" : "FAIL");
    trace_init();
    printf("M85 ETM config %s\n", m85_etm_init() ? "PASS" : "FAIL");
    printf("R5 ETM programming writes %s\n", r5_etm_init() ? "PASS" : "FAIL");
    trace_send_test_marker();

    printf("Hello SoCLabs MilliSoC\n");
    printf("M85 ITM trace marker sent\n");

    UartEndSimulation();
}
