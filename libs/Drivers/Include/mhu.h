#include <stdint.h>

typedef struct {
    volatile uint32_t   ST;     //0x00
    volatile uint32_t   RSVD[2];//0x04-0x08
    volatile uint32_t   SET;    //0x0C
    volatile uint32_t   INT_ST; //0x10
    volatile uint32_t   INT_CLR;//0x14
    volatile uint32_t   INT_EN; //0x18
    volatile uint32_t   CTRL;   //0x1C
} PDBCW_str ;

typedef struct {
    volatile uint32_t   ST;     //0x00
    volatile uint32_t   ST_MSK; //0x04
    volatile uint32_t   CLR;    //0x08
    volatile uint32_t   RSVD;   //0x0C
    volatile uint32_t   MSK_ST; //0x10
    volatile uint32_t   MSK_SET;//0x14
    volatile uint32_t   MSK_CLR;//0x18
    volatile uint32_t   CTRL;   //0x1C
} MDBCW_str ;


typedef struct {
    union {
        volatile uint64_t PAY64;
        struct{
            volatile uint32_t PAY32_l;
            volatile uint32_t PAY32_h;
        };
    };
    volatile uint32_t   FLG;    // 0x08
    volatile uint32_t   RSVD0;  // 0x0C
    volatile uint32_t   INT_ST; // 0x10
    volatile uint32_t   INT_CLR;// 0x14
    volatile uint32_t   INT_EN; // 0x18
    volatile uint32_t   RSVD1;  // 0x1C
    volatile uint32_t   CTRL;   // 0x20
    volatile uint32_t   ST;     // 0x24
    volatile uint32_t   ACK_CNT;// 0x28
    volatile uint32_t   TIDE;   // 0x2C
    volatile uint32_t   RSVD2[4];// 0x30 - 0x3C
} PFFCW_str ;

typedef struct {
    union {
        volatile uint64_t PAY64;
        struct{
            volatile uint32_t PAY32_l;
            volatile uint32_t PAY32_h;
        };
    };
    union {
        volatile uint64_t FLG64;
        struct{
            volatile uint32_t   FLG32_l;
            volatile uint32_t   FLG32_h;
        };
    };
    volatile uint32_t   INT_ST; // 0x10
    volatile uint32_t   INT_CLR;// 0x14
    volatile uint32_t   INT_EN; // 0x18
    volatile uint32_t   RSVD1;  // 0x1C
    volatile uint32_t   CTRL;   // 0x20
    volatile uint32_t   ST;     // 0x24
    volatile uint32_t   FIFO_POP;// 0x28
    volatile uint32_t   TIDE;   // 0x2C
    volatile uint32_t   RSVD2[4];// 0x30 - 0x3C
} MFFCW_str ;

typedef struct {
    union{
        volatile uint64_t   PAY64;
        struct{
            volatile uint32_t PAY32_l;
            volatile uint32_t PAY32_h;
        };
    };  // 0x00 - 0x04
} PFCW_str ;

typedef struct{
    volatile uint32_t   PBX_BLK_ID;     // 0x0000   Postbox Block Identifier
    volatile uint32_t   RSVD0[3];       // 0x0004 - 0x000C  
    volatile uint32_t   FEAT_SPT0;      // 0x0010   Postbox Feature Support 0
    volatile uint32_t   FEAT_SPT1;      // 0x0014   Postbox Feature Support 1
    volatile uint32_t   RSVD1[2];       // 0x0018 - 0x001C
    volatile uint32_t   DBCH_CFG0;      // 0x0020   Postbox Doorbell Channel Configuration 0 
    volatile uint32_t   RSVD2[3];       // 0x0024 - 0x002C
    volatile uint32_t   FFCH_CFG0;      // 0x0030   Postbox FIFO Channel Configuration 0
    volatile uint32_t   RSVD3[3];       // 0x0034 - 0x003C
    volatile uint32_t   FCH_CFG0;       // 0x0040   Postbox Fast Channel Configuration 0
    volatile uint32_t   RSVD4[47];      // 0x0044 - 0x00FC
    volatile uint32_t   CTRL;           // 0x0100   Postbox Control Reg
    volatile uint32_t   RSVD5[191];     // 0x0104 - 0x03FC
    volatile uint32_t   DBCH_INT_ST[4]; // 0x0400 - 0x040C Postbox Doorbell Channel Interrupt Status n Reg
    volatile uint32_t   FFCH_INT_ST[2]; // 0x0410 - 0x0414  
    volatile uint32_t   RSVD6[748];     // 0x0418 - 0x0FC4
    volatile uint32_t   IIDR;           // 0x0FC8   
    volatile uint32_t   AAIDR;          // 0x0FCC
    volatile uint32_t   PIDR4;          // 0x0FD0
    volatile uint32_t   PIDR5;          // 0x0FD4
    volatile uint32_t   PIDR6;          // 0x0FD8
    volatile uint32_t   PIDR7;          // 0x0FDC
    volatile uint32_t   PIDR0;          // 0x0FE0
    volatile uint32_t   PIDR1;          // 0x0FE4
    volatile uint32_t   PIDR2;          // 0x0FE8
    volatile uint32_t   PIDR3;          // 0x0FEC
    volatile uint32_t   CIDR0;          // 0x0FF0
    volatile uint32_t   CIDR1;          // 0x0FF4
    volatile uint32_t   CIDR2;          // 0x0FF8
    volatile uint32_t   CIDR3;          // 0x0FFC
    volatile PDBCW_str  PDBCW[128];     // 0x1000 
    volatile PFFCW_str  PFFCW[64];      // 0x2000
    volatile PFCW_str   PFCW[512];      // 0x3000 
    volatile uint32_t   RSVD7[11264];   // 0x4000 - 0xEFFC
    volatile uint32_t   FCTRL;          // 0xF000
    volatile uint32_t   RSVD8[3];       // 0xF004 - 0xF00C
    volatile uint32_t   FIFO_ERRINS;    // 0xF010
} MHUS_PBX_typedef;

typedef struct{
    volatile uint32_t   PBX_BLK_ID;     // 0x0000
    volatile uint32_t   RSVD0[3];       // 0x0004 - 0x000C
    volatile uint32_t   FEAT_SPT0;      // 0x0010
    volatile uint32_t   FEAT_SPT1;      // 0x0014
    volatile uint32_t   RSVD1[2];       // 0x0018 - 0x001C
    volatile uint32_t   DBCH_CFG0;      // 0x0020
    volatile uint32_t   RSVD2[3];       // 0x0024 - 0x002C
    volatile uint32_t   FFCH_CFG0;      // 0x0030
    volatile uint32_t   RSVD3[3];       // 0x0034 - 0x003C
    volatile uint32_t   FCH_CFG0;       // 0x0040
    volatile uint32_t   RSVD4[47];      // 0x0044 - 0x00FC
    volatile uint32_t   CTRL;           // 0x0100
    volatile uint32_t   RSVD5[15];      // 0x0104 - 0x013C
    volatile uint32_t   FCH_CTRL;       // 0x0140
    volatile uint32_t   FCG_INT_EN;     // 0x0144
    volatile uint32_t   RSVD6[174];     // 0x0148 - 0x03FC
    volatile uint32_t   DBCH_INT_ST[4]; // 0x0400 - 0x040C
    volatile uint32_t   FFCH_INT_ST[2]; // 0x0410 - 0x0414
    volatile uint32_t   RSVD7[22];      // 0x0418 - 0x046C
    volatile uint32_t   FCG_INT_ST;     // 0x0470
    volatile uint32_t   RSVD8[3];       // 0x0474 - 0x047C
    volatile uint32_t   FCH_GRP_INT_ST[32]; // 0x0480 -0x04FC
    volatile uint32_t   RSVD9[690];     // 0x0500 - 0x0FC4
    volatile uint32_t   IIDR;           // 0x0FC8
    volatile uint32_t   AAIDR;          // 0x0FCC
    volatile uint32_t   PIDR4;          // 0x0FD0
    volatile uint32_t   PIDR5;          // 0x0FD4
    volatile uint32_t   PIDR6;          // 0x0FD8
    volatile uint32_t   PIDR7;          // 0x0FDC
    volatile uint32_t   PIDR0;          // 0x0FE0
    volatile uint32_t   PIDR1;          // 0x0FE4
    volatile uint32_t   PIDR2;          // 0x0FE8
    volatile uint32_t   PIDR3;          // 0x0FEC
    volatile uint32_t   CIDR0;          // 0x0FF0
    volatile uint32_t   CIDR1;          // 0x0FF4
    volatile uint32_t   CIDR2;          // 0x0FF8
    volatile uint32_t   CIDR3;          // 0x0FFC
    volatile MDBCW_str  MDBCW[128];     // 0x1000 
    volatile MFFCW_str  MFFCW[64];      // 0x2000
    volatile PFCW_str   MFCW[512];      // 0x3000 
    volatile uint32_t   RSVD10[11264];  // 0x4000 - 0xEFFC
    volatile uint32_t   FCTRL;          // 0xF000
    volatile uint32_t   RSVD11[3];      // 0xF004 - 0xF00C
    volatile uint64_t   DB_ERRINS;      // 0xF010
    volatile uint64_t   FST_ERRINS;     // 0xF018
    volatile uint64_t   FCFG_ERRINS;    // 0xF020
    volatile uint64_t   FDATA_ERRINS;   // 0xF028
} MHUR_MBX_typedef;



#define MHUS_PBX  ((MHUS_PBX_typedef *) MHU_SND)

#define MHUR_MBX  ((MHUR_MBX_typedef *) MHU_REC)
