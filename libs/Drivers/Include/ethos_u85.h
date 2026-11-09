#ifndef ETHOS_U85_H
#define ETHOS_U85_H

#include <stdint.h>

/*
 * Arm Ethos-U85 NPU register map, PMU, and command-stream encoding.
 *
 * Register offsets and bit fields follow the Arm Ethos-U85 Technical
 * Reference Manual (102685_0000_05_en).
 */

/* Allow the header to be used without sys_memory_map.h. */
#ifndef U85_CFG_BASE
#define U85_CFG_BASE    0x44032000UL
#endif

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

/* ------------------------------------------------------------------------- */
/* BASE.ID                                                                   */
/* ------------------------------------------------------------------------- */
#define U85_ID_ARCH_MINOR_Pos           20
#define U85_ID_ARCH_MINOR_Msk           (0xFUL << U85_ID_ARCH_MINOR_Pos)
#define U85_ID_ARCH_PATCH_Pos           16
#define U85_ID_ARCH_PATCH_Msk           (0xFUL << U85_ID_ARCH_PATCH_Pos)
#define U85_ID_PRODUCT_MAJOR_Pos        12
#define U85_ID_PRODUCT_MAJOR_Msk        (0xFUL << U85_ID_PRODUCT_MAJOR_Pos)
#define U85_ID_ARCH_MAJOR_Pos           28
#define U85_ID_ARCH_MAJOR_Msk           (0xFUL << U85_ID_ARCH_MAJOR_Pos)

#define U85_ID_ARCH_MAJOR_U85           2UL
#define U85_ID_RESET_VALUE              0x20007001UL

/* ------------------------------------------------------------------------- */
/* BASE.STATUS (read-only)                                                   */
/* ------------------------------------------------------------------------- */
#define U85_STATUS_STATE_Pos            0
#define U85_STATUS_STATE_Msk            (1UL << U85_STATUS_STATE_Pos)
#define U85_STATUS_STATE_STOPPED        (0UL << U85_STATUS_STATE_Pos)
#define U85_STATUS_STATE_RUNNING        (1UL << U85_STATUS_STATE_Pos)

#define U85_STATUS_IRQ_Pos              1
#define U85_STATUS_IRQ_Msk              (1UL << U85_STATUS_IRQ_Pos)

#define U85_STATUS_BUS_FAULT_Pos        2
#define U85_STATUS_BUS_FAULT_Msk        (1UL << U85_STATUS_BUS_FAULT_Pos)

#define U85_STATUS_RESET_Pos            3
#define U85_STATUS_RESET_Msk            (1UL << U85_STATUS_RESET_Pos)

#define U85_STATUS_CMD_PARSE_ERR_Pos    4
#define U85_STATUS_CMD_PARSE_ERR_Msk    (1UL << U85_STATUS_CMD_PARSE_ERR_Pos)

#define U85_STATUS_CMD_END_Pos          5
#define U85_STATUS_CMD_END_Msk          (1UL << U85_STATUS_CMD_END_Pos)

#define U85_STATUS_PMU_IRQ_Pos          6
#define U85_STATUS_PMU_IRQ_Msk          (1UL << U85_STATUS_PMU_IRQ_Pos)

#define U85_STATUS_ECC_FAULT_Pos        8
#define U85_STATUS_ECC_FAULT_Msk        (1UL << U85_STATUS_ECC_FAULT_Pos)

#define U85_STATUS_BRANCH_FAULT_Pos     9
#define U85_STATUS_BRANCH_FAULT_Msk     (1UL << U85_STATUS_BRANCH_FAULT_Pos)

#define U85_STATUS_FAULT_Msk                                                  \
    (U85_STATUS_BUS_FAULT_Msk | U85_STATUS_CMD_PARSE_ERR_Msk |                \
     U85_STATUS_ECC_FAULT_Msk | U85_STATUS_BRANCH_FAULT_Msk)

/* ------------------------------------------------------------------------- */
/* BASE.CMD                                                                  */
/* ------------------------------------------------------------------------- */
#define U85_CMD_RS_Pos                  0   /* transition to running state  */
#define U85_CMD_RS_Msk                  (1UL << U85_CMD_RS_Pos)
#define U85_CMD_CQ_Pos                  1   /* clear IRQ                    */
#define U85_CMD_CQ_Msk                  (1UL << U85_CMD_CQ_Pos)
#define U85_CMD_CE_Pos                  2   /* clock_q_enable               */
#define U85_CMD_CE_Msk                  (1UL << U85_CMD_CE_Pos)
#define U85_CMD_PE_Pos                  3   /* power_q_enable               */
#define U85_CMD_PE_Msk                  (1UL << U85_CMD_PE_Pos)
#define U85_CMD_SQ_Pos                  4   /* stop_request                 */
#define U85_CMD_SQ_Msk                  (1UL << U85_CMD_SQ_Pos)

/* ------------------------------------------------------------------------- */
/* BASE.QCONFIG / BASE.REGIONCFG (2 bits per region, index into MEM_ATTRn)    */
/* ------------------------------------------------------------------------- */
#define U85_QCONFIG_CMD_REGION0_Pos     0
#define U85_QCONFIG_CMD_REGION0_Msk     (0x3UL << U85_QCONFIG_CMD_REGION0_Pos)

#define U85_REGIONCFG_REGION_Pos(n)     (2UL * (n))
#define U85_REGIONCFG_REGION_Msk(n)     (0x3UL << U85_REGIONCFG_REGION_Pos(n))

/* ------------------------------------------------------------------------- */
/* BASE.MEM_ATTRn                                                            */
/* ------------------------------------------------------------------------- */
#define U85_MEM_ATTR_MEM_DOMAIN_Pos     0
#define U85_MEM_ATTR_MEM_DOMAIN_Msk     (0x3UL << U85_MEM_ATTR_MEM_DOMAIN_Pos)
#define U85_MEM_ATTR_AXI_PORT_Pos       2   /* 0 = SRAM port, 1 = EXT port   */
#define U85_MEM_ATTR_AXI_PORT_Msk       (1UL << U85_MEM_ATTR_AXI_PORT_Pos)
#define U85_MEM_ATTR_MEMTYPE_Pos        4
#define U85_MEM_ATTR_MEMTYPE_Msk        (0xFUL << U85_MEM_ATTR_MEMTYPE_Pos)

#define U85_MEM_ATTR_SRAM               0x00000000UL  /* non-sharable, device */
#define U85_MEM_ATTR_EXT                U85_MEM_ATTR_AXI_PORT_Msk

/* ------------------------------------------------------------------------- */
/* BASE.AXI_SRAM / BASE.AXI_EXT                                              */
/* ------------------------------------------------------------------------- */
#define U85_AXI_MAX_OUTSTANDING_READ_Pos     0
#define U85_AXI_MAX_OUTSTANDING_READ_Msk     (0x3FUL << 0)
#define U85_AXI_MAX_OUTSTANDING_WRITE_Pos    8
#define U85_AXI_MAX_OUTSTANDING_WRITE_Msk    (0x1FUL << 8)
#define U85_AXI_MAX_BEATS_Pos                16
#define U85_AXI_MAX_BEATS_Msk                (0x3UL << 16)

#define U85_AXI_SRAM_DEFAULT                                                  \
    ((11UL << U85_AXI_MAX_OUTSTANDING_READ_Pos) |                             \
     (15UL << U85_AXI_MAX_OUTSTANDING_WRITE_Pos) |                            \
     (2UL  << U85_AXI_MAX_BEATS_Pos))                       /* 0x00020F0B */

#define U85_AXI_EXT_DEFAULT                                                   \
    ((63UL << U85_AXI_MAX_OUTSTANDING_READ_Pos) |                             \
     (31UL << U85_AXI_MAX_OUTSTANDING_WRITE_Pos) |                            \
     (2UL  << U85_AXI_MAX_BEATS_Pos))                       /* 0x00021F3F */

/* ------------------------------------------------------------------------- */
/* PMU                                                                       */
/* ------------------------------------------------------------------------- */
#define U85_PMU_PMCR_CNT_EN_Pos         0
#define U85_PMU_PMCR_CNT_EN_Msk         (1UL << U85_PMU_PMCR_CNT_EN_Pos)
#define U85_PMU_PMCR_EVENT_CNT_RST_Pos  1
#define U85_PMU_PMCR_EVENT_CNT_RST_Msk  (1UL << U85_PMU_PMCR_EVENT_CNT_RST_Pos)
#define U85_PMU_PMCR_CYCLE_CNT_RST_Pos  2
#define U85_PMU_PMCR_CYCLE_CNT_RST_Msk  (1UL << U85_PMU_PMCR_CYCLE_CNT_RST_Pos)

#define U85_PMU_PMCNTENSET_CYCLE_CNT_Pos    31
#define U85_PMU_PMCNTENSET_CYCLE_CNT_Msk    (1UL << U85_PMU_PMCNTENSET_CYCLE_CNT_Pos)

/* ------------------------------------------------------------------------- */
/* Command stream encoding                                                   */
/* ------------------------------------------------------------------------- */
#define U85_CMD_CTRL_CMD0               0UL
#define U85_CMD_CTRL_CMD1               1UL

/* cmd0_opcode_t */
#define U85_OP_STOP                     0
#define U85_OP_DMA_START                16
#define U85_OP_DMA_WAIT                 17
#define U85_SET_DMA0_DST_REGION         305
#define U85_SET_DMA0_SRC_REGION         304
#define U85_SET_DMA0_SIZE0              306
#define U85_SET_DMA0_SIZE1              307

/* cmd1_opcode_t */
#define U85_SET_DMA0_SRC                48
#define U85_SET_DMA0_DST                49
#define U85_SET_DMA0_LEN                50

/* dma_region_mode_t */
#define U85_DMA_REGION_MODE_EXTERNAL    0UL
#define U85_DMA_REGION_MODE_INTERNAL    1UL

/* dma_stride_mode_t */
#define U85_DMA_STRIDE_MODE_D1          0UL
#define U85_DMA_STRIDE_MODE_D2          1UL
#define U85_DMA_STRIDE_MODE_D3          2UL

/* dma_idx_mode_t */
#define U85_DMA_IDX_MODE_DISABLED       0UL
#define U85_DMA_IDX_MODE_ENABLED        1UL

/*
 * CMD0 word: [9:0] opcode, [15:14] control = 0, [31:16] 16-bit payload.
 * Used for opcodes that take an immediate field (STOP, DMA_WAIT, DMA_START,
 * SET_DMA0_SRC_REGION, SET_DMA0_DST_REGION, SET_DMA0_SIZE0/1, ...).
 */
static inline uint32_t u85_cmd0(uint32_t opcode, uint32_t payload)
{
    return (opcode & 0x3FFUL) |
           ((U85_CMD_CTRL_CMD0 << 14) & 0xC000UL) |
           ((payload & 0xFFFFUL) << 16);
}

/*
 * CMD1 is a two-word encoding: a header word with [9:0] opcode,
 * [15:14] control = 1 and [23:16] the top 8 bits of a 64-bit value,
 * followed by a second word holding the low 32 bits.
 */
static inline uint32_t u85_cmd1_header(uint32_t opcode, uint32_t value_hi)
{
    return (opcode & 0x3FFUL) |
           ((U85_CMD_CTRL_CMD1 << 14) & 0xC000UL) |
           ((value_hi & 0xFFUL) << 16);
}

static inline uint32_t u85_cmd1_payload(uint32_t value_lo)
{
    return value_lo;
}

/* DMA0_SRC_REGION payload: [2:0] region, [8] region_mode, [10:9] stride_mode,
 * [11] idx_mode. */
static inline uint32_t u85_dma0_src_region(uint32_t region,
                                           uint32_t region_mode,
                                           uint32_t stride_mode,
                                           uint32_t idx_mode)
{
    return ((region      & 0x7UL) << 0)  |
           ((region_mode & 0x1UL) << 8)  |
           ((stride_mode & 0x3UL) << 9)  |
           ((idx_mode    & 0x1UL) << 11);
}

/* DMA0_DST_REGION payload: [2:0] region, [8] region_mode, [11] idx_mode. */
static inline uint32_t u85_dma0_dst_region(uint32_t region,
                                           uint32_t region_mode,
                                           uint32_t idx_mode)
{
    return ((region      & 0x7UL) << 0)  |
           ((region_mode & 0x1UL) << 8)  |
           ((idx_mode    & 0x1UL) << 11);
}

#endif /* ETHOS_U85_H */
