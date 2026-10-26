#include <stdint.h>

typedef struct{
    volatile uint32_t   ID;         // 0x000
    volatile uint32_t   Status;     // 0x004
    volatile uint32_t   CMD;        // 0x008
    volatile uint32_t   RESET;      // 0x00C
    volatile uint32_t   QBASE[2];   // 0x010 - 0x014
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




#define U85_Base    ((U85_Base_Typedef *) U85_CFG_BASE)