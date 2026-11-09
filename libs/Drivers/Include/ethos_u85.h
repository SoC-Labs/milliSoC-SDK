#include <stdint.h>

typedef struct{
    volatile uint32_t   ID;         // 0x000
    volatile uint32_t   Status;     // 0x004
    volatile uint32_t   CMD;        // 0x008
    volatile uint32_t   RESET;      // 0x00C
    volatile uint64_t   QBASE;      // 0x010 - 0x014
    volatile uint32_t   QREAD;      // 0x018
    volatile uint32_t   QCONFIG;    // 0x01C
    volatile uint32_t   QSIZE;      // 0x020
    volatile uint32_t   PROT;       // 0x024
    volatile uint32_t   CONFIG;     // 0x028
    volatile uint32_t   RSVD0;      // 0x02C
    volatile uint32_t   COND_STATUS;// 0x030
    volatile uint32_t   RSVD1;      // 0x034
    volatile uint32_t   POWER_CTRL; // 0x038
    volatile uint32_t   REGIONCFG;  // 0x03C
    volatile uint32_t   MEM_ATTR0;  // 0x040
    volatile uint32_t   MEM_ATTR1;  // 0x044
    volatile uint32_t   MEM_ATTR2;  // 0x048
    volatile uint32_t   MEM_ATTR3;  // 0x04C
    volatile uint32_t   AXI_SRAM;   // 0x050
    volatile uint32_t   AXI_EXT;    // 0x054
    volatile uint32_t   RSVD2[2];   // 0x058 - 0x05C
    volatile uint32_t   CFG_SRAM_CAP;   // 0x060
    volatile uint32_t   CFG_EXT_CAP;// 0x064
    volatile uint32_t   CFG_SRAM_HASH0[2]; // 0x068 - 0x06C
    volatile uint32_t   CFG_SRAM_HASH1[2]; // 0x070 - 0x074
    volatile uint32_t   CFG_EXT_HASH0[2];  // 0x078 - 0x07C
} U85_Base_Typedef;

typedef struct{
    volatile uint64_t   BASEP_ARRAY[8];   // 0x000 - 0x03C   
} U85_Base_Pointers_Typedef;

typedef struct{
    volatile uint32_t    RSVD0[52];    // 0x0000 - 0x00CC
    volatile uint32_t    PID4;  // 0xd0
    volatile uint32_t    PID5;  // 0xd4
    volatile uint32_t    PID6;  // 0xd8
    volatile uint32_t    PID7;  // 0xdc
    volatile uint32_t    PID0;  // 0xe0
    volatile uint32_t    PID1;  // 0xe4
    volatile uint32_t    PID2;  // 0xe8
    volatile uint32_t    PID3;  // 0xec
    volatile uint32_t    CID0;  // 0xf0
    volatile uint32_t    CID1;  // 0xf4
    volatile uint32_t    CID2;  // 0xf8
    volatile uint32_t    CID3;  // 0xfc
} U85_ID_Typedef;

typedef struct{
    volatile uint32_t   PMCR;
    volatile uint32_t   PMCNTENSET;
    volatile uint32_t   PMCNTENCLR;
    volatile uint32_t   PMOVSSET;
    volatile uint32_t   PMOVSCLR;
    volatile uint32_t   PMINTSET;
    volatile uint32_t   PMINTCLR;
    volatile uint32_t   RSVD0;
    volatile uint64_t   PMCCNTR;
    volatile uint32_t   PMCCNTR_CFG;
    volatile uint32_t   PMCAXI_CHAN;
    volatile uint32_t   PMCLUT;
} U85_PMU_Typedef;

typedef struct{
    volatile uint32_t   PMEVCNTR0;
    volatile uint32_t   PMEVCNTR1;
    volatile uint32_t   PMEVCNTR2;
    volatile uint32_t   PMEVCNTR3;
    volatile uint32_t   PMEVCNTR4;
    volatile uint32_t   PMEVCNTR5;
    volatile uint32_t   PMEVCNTR6;
    volatile uint32_t   PMEVCNTR7;
    volatile uint32_t   RSVD[24]; // 0x0020 - 0x007C
    volatile uint32_t   PMEVTYPER0;
    volatile uint32_t   PMEVTYPER1;
    volatile uint32_t   PMEVTYPER2;
    volatile uint32_t   PMEVTYPER3;
    volatile uint32_t   PMEVTYPER4;
    volatile uint32_t   PMEVTYPER5;
    volatile uint32_t   PMEVTYPER6;
    volatile uint32_t   PMEVTYPER7;
} U85_PMU_COUNTERS_Typedef;

#define U85_Base            ((U85_Base_Typedef *) U85_CFG_BASE)
#define U85_Base_Pointers   ((U85_Base_Pointers_Typedef *) (U85_CFG_BASE+0x80))
#define U85_ID              ((U85_ID_Typedef *) (U85_CFG_BASE+0x0f00))
#define U85_PMU             ((U85_PMU_Typedef* ) (U85_CFG_BASE+0x1180))
#define U85_PMU_COUNTERS    ((U85_PMU_COUNTERS_Typedef *) (U85_CFG_BASE+0x1300))