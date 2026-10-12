#include <stdint.h>
#include "sys_memory_map.h"
#include "exectb_mcu.h"

#define CG092_BASEADDR (QSPI_CFG_BASE + 0x1000UL)


typedef struct{
    volatile    uint32_t    CTRL;           // Offset: 0x00
    volatile    uint32_t    STATUS;         // Offset: 0x04
    volatile    uint32_t    SPI_CMD;        // Offset: 0x08
    volatile    uint32_t    SPI_ADDR;       // Offset: 0x0C
    volatile    uint32_t    READ_DATA[4];   // Offset: 0x10-1C
    volatile    uint32_t    WRITE_DATA[4];  // Offset: 0x20-2C
    volatile    uint32_t    AHB_CMD;        // Offset: 0x30
    volatile    uint32_t    CLK_DIV;        // Offset: 0x34
} SL_AHB_QSPI_TypeDef;

#define SL_AHB_QSPI_CTRL_QIO_Pos        0
#define SL_AHB_QSPI_CTRL_QIO_Msk        (0x1UL << SL_AHB_QSPI_CTRL_QIO_Pos)

#define SL_AHB_QSPI_CTRL_XiP_Pos        8
#define SL_AHB_QSPI_CTRL_XiP_Msk        (0x1UL << SL_AHB_QSPI_CTRL_XiP_Pos)

#define SL_AHB_QSPI_CTRL_Mode_Code_Pos  16
#define SL_AHB_QSPI_CTRL_Mode_Code_Msk  (0xFFUL << SL_AHB_QSPI_CTRL_Mode_Code_Pos)

#define SL_AHB_QSPI_CTRL_Cont_Rd_Pos    24
#define SL_AHB_QSPI_CTRL_Cont_Rd_Msk    (0x1UL << SL_AHB_QSPI_CTRL_Cont_Rd_Pos)

#define SL_AHB_QSPI_CTRL_No_CMD_Pos     25
#define SL_AHB_QSPI_CTRL_No_CMD_Msk     (0x1UL << SL_AHB_QSPI_CTRL_No_CMD_Pos)

#define SL_AHB_QSPI_STATUS_Busy_Pos     0
#define SL_AHB_QSPI_STATUS_Busy_Msk     (0x1UL << SL_AHB_QSPI_STATUS_Busy_Pos)

#define SL_AHB_QSPI_SPI_CMD_Pos         0
#define SL_AHB_QSPI_SPI_CMD_Msk         (0xFFUL << SL_AHB_QSPI_SPI_CMD_Pos)

#define SL_AHB_QSPI_SPI_CMD_Enable_Pos  8
#define SL_AHB_QSPI_SPI_CMD_Enable_Msk  (0x1UL << SL_AHB_QSPI_SPI_CMD_Enable_Pos)

#define SL_AHB_QSPI_SPI_CMD_Rd_Enable_Pos  9
#define SL_AHB_QSPI_SPI_CMD_Rd_Enable_Msk  (0x1UL << SL_AHB_QSPI_SPI_CMD_Rd_Enable_Pos)

#define SL_AHB_QSPI_SPI_CMD_Wr_Enable_Pos  10
#define SL_AHB_QSPI_SPI_CMD_Wr_Enable_Msk  (0x1UL << SL_AHB_QSPI_SPI_CMD_Wr_Enable_Pos)

#define SL_AHB_QSPI_SPI_CMD_Addr_Enable_Pos  11
#define SL_AHB_QSPI_SPI_CMD_Addr_Enable_Msk  (0x1UL << SL_AHB_QSPI_SPI_CMD_Addr_Enable_Pos)

#define SL_AHB_QSPI_SPI_CMD_N_Dummy_Pos  12
#define SL_AHB_QSPI_SPI_CMD_N_Dummy_Msk  (0xFUL << SL_AHB_QSPI_SPI_CMD_N_Dummy_Pos)

#define SL_AHB_QSPI_SPI_CMD_N_RW_Bytes_Pos  16
#define SL_AHB_QSPI_SPI_CMD_N_RW_Bytes_Msk  (0xFUL << SL_AHB_QSPI_SPI_CMD_N_RW_Bytes_Pos)


typedef struct{
    volatile    uint32_t    CCR;
    volatile    uint32_t    SR;
    volatile    uint32_t    IRQMASK;
    volatile    uint32_t    IRQSTAT;
    volatile    uint32_t    HWPARAMS;
    volatile    uint32_t    CSHR;
    volatile    uint32_t    CSMR;
} CG092_TypeDef;


#define SL_QSPI         ((SL_AHB_QSPI_TypeDef *) QSPI_CFG_BASE)
#define CG092           ((CG092_TypeDef *) CG092_BASEADDR)

extern uint32_t SPI_READ_JEDIC(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern int SPI_STARTUP(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void SPI_WAIT_BUSY(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void SPI_RESET(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void SET_QPI_MODE(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void QPI_READ_WORDS_no_data(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI, uint32_t addr, uint8_t n_words, uint32_t n_dummy, uint8_t cont_read);
extern void QPI_SET_CONT_READ(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void QPI_SET_CONT_READ_MICRON(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);
extern void QPI_SET_AHB_MODE(SL_AHB_QSPI_TypeDef *SL_AHB_QSPI);

extern void CACHE_STARTUP(CG092_TypeDef *CACHE);
