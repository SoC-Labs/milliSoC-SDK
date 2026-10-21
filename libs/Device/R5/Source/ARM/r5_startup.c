//----------------------------------------------------------------
// Cortex-R5(F) Embedded example - Startup Code
//
// Copyright (c) 2006-2024 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such license agreement.
//----------------------------------------------------------------


// MPU region defines

#define Region_32B  0b00100
#define Region_64B  0b00101
#define Region_128B 0b00110
#define Region_256B 0b00111
#define Region_512B 0b01000
#define Region_1K   0b01001
#define Region_2K   0b01010
#define Region_4K   0b01011
#define Region_8K   0b01100
#define Region_16K  0b01101
#define Region_32K  0b01110
#define Region_64K  0b01111
#define Region_128K 0b10000
#define Region_256K 0b10001
#define Region_512K 0b10010
#define Region_1M   0b10011
#define Region_2M   0b10100
#define Region_4M   0b10101
#define Region_8M   0b10110
#define Region_16M  0b10111
#define Region_32M  0b11000
#define Region_64M  0b11001
#define Region_128M 0b11010
#define Region_256M 0b11011
#define Region_512M 0b11100
#define Region_1G   0b11101
#define Region_2G   0b11110
#define Region_4G   0b11111

#define Region_Enable 0b1

#define Execute_Never 0x1000 // Bit 12

#define Normal_nShared 0x0B // Outer and Inner Write-Back, Write-Allocate
#define Device_nShared 0x10

#define Full_Access 0b011
#define Read_Only   0b110

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Vectors        (void) __attribute__ ((naked, section("RESET")));
void Reset_Handler  (void) __attribute__ ((naked));
void Default_Handler(void) __attribute__ ((noreturn));

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
void Undef_Handler (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void PAbt_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void DAbt_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void FIQ_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/
void Vectors (void)
{
  __asm volatile(
  "LDR    PC, =Reset_Handler                        \n"
  "LDR    PC, =Undef_Handler                        \n"
  "LDR    PC, =SVC_Handler                          \n"
  "LDR    PC, =PAbt_Handler                         \n"
  "LDR    PC, =DAbt_Handler                         \n"
  "NOP                                              \n"
  "LDR    PC, =IRQ_Handler                          \n"
  "LDR    PC, =FIQ_Handler                          \n"
  );
}


/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler (void)
{
  __asm volatile(
        "MOV     r0, #0         \n"
        "MOV     r1, #0         \n"
        "MOV     r2, #0         \n"
        "MOV     r3, #0         \n"
        "MOV     r4, #0         \n"
        "MOV     r5, #0         \n"
        "MOV     r6, #0         \n"
        "MOV     r7, #0         \n"
        "MOV     r8, #0         \n"
        "MOV     r9, #0         \n"
        "MOV     r10, #0        \n"
        "MOV     r11, #0        \n"
        "MOV     r12, #0        \n"
        "MOV     r14, #0        \n"

        // Mask interrupts
        "CPSID   if     \n"


        // In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction.
        // Some control bits in the ACTLR control the program flow and prefetch features instead.
        // These are enabled by default, but are shown here for completeness.
        "MRC     p15, 0, R0, c1, c0, 0                   \n"  // Read CP15 System Control register
        "BIC     R0, R0, #(0x1 << 12)                    \n"  // Clear I bit 12 to disable I Cache
        "BIC     R0, R0, #(0x1 <<  2)                    \n"  // Clear C bit  2 to disable D Cache
        "BIC     R0, R0, #0x1                            \n"  // Clear M bit  0 to disable MPU
        "DSB                                             \n"  // Ensure all previous loads/stores have completed
        "MCR     p15, 0, R0, c1, c0, 0                   \n"  // Write value back to CP15 System Control register
        "ISB                                             \n"  // Ensure subsequent insts execute with new MPU settings

        // Configure ACTLR
        "MRC     p15, 0, r0, c1, c0, 1                   \n"  // Read CP15 Auxiliary Control Register
        "BIC     r0, r0, #(0x1 << 17)                    \n"  // Clear RSDIS bit 17 to enable return stack
        "BIC     r0, r0, #(0x1 << 16)                    \n"  // Clear BP bit 15 and BP bit 16:
        "BIC     r0, r0, #(0x1 << 15)                    \n"  // Normal operation, BP is taken from the global history table.
        "MCR     p15, 0, r0, c1, c0, 1                   \n"  // Write ACTLR
        "ISB                                             \n"  

//----------------------------------------------------------------
// Initialize Supervisor Mode Stack using Linker symbol from scatter file.
// Stacks must be 8 byte aligned.
//----------------------------------------------------------------


        // Setup Stack for each exceptional mode
        "CPS    #0x11                                    \n"
        "LDR    SP, =Image$$FIQ_STACK$$ZI$$Limit         \n"
        "CPS    #0x12                                    \n"
        "LDR    SP, =Image$$IRQ_STACK$$ZI$$Limit         \n"
        "CPS    #0x13                                    \n"
        "LDR    SP, =Image$$SVC_STACK$$ZI$$Limit         \n"
        "CPS    #0x17                                    \n"
        "LDR    SP, =Image$$ABT_STACK$$ZI$$Limit         \n"
        "CPS    #0x1B                                    \n"
        "LDR    SP, =Image$$UND_STACK$$ZI$$Limit         \n"
        "CPS    #0x1F                                    \n"
        "LDR    SP, =Image$$ARM_LIB_STACK$$ZI$$Limit     \n"

//----------------------------------------------------------------
// Cache invalidation
//----------------------------------------------------------------

        "DSB                            \n"     // Complete all outstanding explicit memory operations
        "MOV     r0, #0                 \n"
        "MCR     p15, 0, r0, c7, c5, 0  \n"     // Invalidate entire instruction cache
        "MCR     p15, 0, r0, c15, c5, 0 \n"     // Invalidate entire data cache


//----------------------------------------------------------------
// TCM Configuration
//----------------------------------------------------------------

// Cortex-R5 optionally provides two Tightly-Coupled Memory (TCM) blocks (ATCM and BTCM) for fast access to code or data.
// ATCM typically holds interrupt or exception code that must be accessed at high speed,
//    without any potential delay resulting from a cache miss.
// BTCM typically holds a block of data for intensive processing, such as audio or video data.
// In the Cortex-R5 processor, both ATCM and BTCM support both instruction and data accesses.

// The following illustrates basic TCM configuration, as the basis for exploration by the user

//#ifdef TCM
//    .global  Image$$ATCM$$Base
//    .global  Image$$BTCM$$Base
//
//        MRC     p15, 0, r0, c0, c0, 2       // Read TCM Type Register
//        // r0 now contains ATCM & BTCM availability
//
//        MRC     p15, 0, r0, c9, c1, 1       // Read ATCM Region Register
//        // r0 now contains ATCM size in bits [6:2]
//
//        MRC     p15, 0, r0, c9, c1, 0       // Read BTCM Region Register
//        // r0 now contains BTCM size in bits [6:2]
//
//// The Cortex-R5 Logic Tile on Versatile Express has
//// 64K ATCM from 0x40000000 to 0x4000FFFF
//// 64K BTCM from 0xE0FE0000 to 0xE0FEFFFF
//
//        LDR     r0, =Image$$ATCM$$Base      // Set ATCM base address
//        ORR     r0, r0, #1                  // Enable it
//        MCR     p15, 0, r0, c9, c1, 1       // Write ATCM Region Register
//
//        LDR     r0, =Image$$BTCM$$Base      // Set BTCM base address
//        ORR     r0, r0, #1                  // Enable it
//        MCR     p15, 0, r0, c9, c1, 0       // Write BTCM Region Register
//
//#endif


//----------------------------------------------------------------
// MPU Configuration
//----------------------------------------------------------------

// Notes:
// * Regions apply to both instruction and data accesses.
// * Each region base address must be a multiple of its size
// * Any address range not covered by an enabled region will abort
// * The region at 0x0 over the Vector table is needed to support semihosting

// Region 0: Code          Base = 0x48000000  Size = 32KB   Normal  Non-shared  Read-only    Executable
// Region 1: Data          Base = 0x48008000  Size = 16KB   Normal  Non-shared  Full access  Not Executable
// Region 2: Stack/Heap    Base = 0x4800C000  Size = 16KB   Normal  Non-shared  Full access  Not Executable
// Region 3: ITCM          Base = 0x00000000  Size = 16KB   Normal  Non-shared  Full access  Executable
// Region 4: Peripheral    Base = 0x40000000  Size = 512Kb  Device  Non-Shared  Full access  Not Excecutable
// Region 5: SYS config    Base = 0x44000000  Size = 256Kb  Device  Non-Shared  Full access  Not Executable
// Region 6: Flash         Base = 0x60000000  Size = 2MB    Normal  Non-shared  Read-only    Excecutable

        // Import linker symbols to get region base addresses
        ".global  Image$$CODE$$Base \n"
        ".global  Image$$RW_DATA$$Base \n"

        // Region 0 - Code
        "MOV     r1, #0 \n"
        "MCR     p15, 0, r1, c6, c2, 0  \n"     // Set memory region number register
        "ISB                            \n"     // Ensure subsequent insts execute wrt this region
        "LDR     r2, =Image$$CODE$$Base \n"
        "MCR     p15, 0, r2, c6, c1, 0      \n" // Set region base address register
        "LDR     r2, =0x0  |  (0b01110 << 1)  |  0b1 \n"
        "MCR     p15, 0, r2, c6, c1, 2      \n" // Set region size & enable register
        "LDR     r2, =0x0  |  (0b110  << 8)  |  0x0B \n"
        "MCR     p15, 0, r2, c6, c1, 4      \n" // Set region access control register

        // Region 1 - Data 
        "ADD     r1, r1, #1 \n"
        "MCR     p15, 0, r1, c6, c2, 0        \n"
        "ISB                                  \n"
        "LDR     r2, =Image$$RW_DATA$$Base \n"
        "MCR     p15, 0, r2, c6, c1, 0       \n"
        "LDR     r2, =0x0  |  (0b01101 << 1)  |  0b1 \n"
        "MCR     p15, 0, r2, c6, c1, 2       \n" // Set region size & enable registe
        "LDR     r2, =0x0  |  (0b011 << 8)  |  0x0B  |  0x1000 \n"
        "MCR     p15, 0, r2, c6, c1, 4       \n"  

        // Region 2 - Stack/Heap
        "ADD     r1, r1, #1 \n"
        "MCR     p15, 0, r1, c6, c2, 0        \n"
        "ISB                                  \n"
        "LDR     r2, =0x20000000 \n"
        "MCR     p15, 0, r2, c6, c1, 0       \n"
        "LDR     r2, =0x0  |  (0b01101 << 1)  |  0b1 \n"
        "MCR     p15, 0, r2, c6, c1, 2        \n"
        "LDR     r2, =0x0  |  (0b011 << 8)  |  0x0B  |  0x1000 \n"
        "MCR     p15, 0, r2, c6, c1, 4     \n"   

        // Region 3 - Vectors
        "ADD     r1, r1, #1  \n"
        "MCR     p15, 0, r1, c6, c2, 0         \n"
        "ISB                                   \n"
        "LDR     r2, =0  \n"
        "MCR     p15, 0, r2, c6, c1, 0        \n"
        "LDR     r2, =0x0  |  (0b01101  << 1)  |  0b1  \n"
        "MCR     p15, 0, r2, c6, c1, 2         \n"
        "LDR     r2, =0x0  |  (0b011 << 8)  |  0x0B  \n"
        "MCR     p15, 0, r2, c6, c1, 4         \n"

        // Region 4 - peripheral
        "ADD     r1, r1, #1  \n"
        "MCR     p15, 0, r1, c6, c2, 0  \n"
        "ISB  \n"
        "LDR     r2, =0x40000000  \n"
        "MCR     p15, 0, r2, c6, c1, 0  \n"
        "LDR     r2, =0x0  |  (0b10010  << 1)  |  0b1  \n"
        "MCR     p15, 0, r2, c6, c1, 2         \n"
        "LDR     r2, =0x0  |  (0b011 << 8)  |  0x10  |  0x1000  \n"
        "MCR     p15, 0, r2, c6, c1, 4         \n"

        // Region 5 - Sys config
        "ADD     r1, r1, #1  \n"
        "MCR     p15, 0, r1, c6, c2, 0  \n"
        "ISB  \n"
        "LDR     r2, =0x44000000  \n"
        "MCR     p15, 0, r2, c6, c1, 0  \n"
        "LDR     r2, =0x0  |  (0b10010  << 1)  |  0b1  \n"
        "MCR     p15, 0, r2, c6, c1, 2         \n"
        "LDR     r2, =0x0  |  (0b011 << 8)  |  0x10  |  0x1000  \n"
        "MCR     p15, 0, r2, c6, c1, 4         \n"

        // Region 6 - Flash 
        "ADD     r1, r1, #1 \n"
        "MCR     p15, 0, r1, c6, c2, 0 \n"
        "ISB \n"
        "LDR     r2, =0x60000000 \n"
        "MCR     p15, 0, r2, c6, c1, 0 \n"
        "LDR     r2, =0x0  |  (0b10100  << 1)  |  0b1 \n"
        "MCR     p15, 0, r2, c6, c1, 2        \n"
        "LDR     r2, =0x0  |  (0b110 << 8)  |  0x0B \n"
        "MCR     p15, 0, r2, c6, c1, 4        \n"

        // Disable all higher priority regions (assumes unified regions, which is always true for Cortex-R5)
        "MRC     p15, 0, r0, c0, c0, 4     \n"   // Read MPU Type register (MPUIR)
        "LSR     r0, r0, #8 \n"
        "AND     r0, r0, #0xff          \n"      // r0 = Number of MPU regions (0, 12, or 16 for Cortex-R5)
        "MOV     r2, #0               \n"        // Value to write to disable region
"region_loop:  \n"
        "ADD     r1, r1, #1 \n"
        "CMP     r0, r1 \n"
        "BLS     regions_done \n"
        "MCR     p15, 0, r1, c6, c2, 0        \n"// Set memory region number register (RGNR)
        "MCR     p15, 0, r2, c6, c1, 2     \n"   // Set region size & enable register (DRSR)
        "B       region_loop \n"
"regions_done:  \n"


//----------------------------------------------------------------
// Enable Branch prediction
//----------------------------------------------------------------

// In the Cortex-R5, the Z-bit of the SCTLR does not control the program flow prediction. 
// Some control bits in the ACTLR control the program flow and prefetch features instead.
// These are enabled by default, but are shown here for completeness.

        "MRC     p15, 0, r0, c1, c0, 1     \n"   // Read ACTLR
        "BIC     r0, r0, #(0x1 << 17)      \n"   // Clear RSDIS bit 17 to enable return stack
        "BIC     r0, r0, #(0x1 << 16)      \n"   // Clear BP bit 15 and BP bit 16:
        "BIC     r0, r0, #(0x1 << 15)      \n"   // Normal operation, BP is taken from the global history table.
        "MCR     p15, 0, r0, c1, c0, 1     \n"   // Write ACTLR
        "ISB  \n"

//----------------------------------------------------------------
// Enable MPU and branch to C library init
// Leaving the caches disabled until after scatter loading.
//----------------------------------------------------------------

        "MRC     p15, 0, r0, c1, c0, 0      \n"  // Read System Control Register
        "ORR     r0, r0, #0x01              \n"  // Set M bit to enable MPU
        "DSB                                \n"  // Ensure all previous loads/stores have completed
        "MCR     p15, 0, r0, c1, c0, 0      \n"  // Write System Control Register
        "ISB                                \n"  // Ensure subsequent insts execute wrt new MPU settings

        "BL       __main  \n"
  );
}


//----------------------------------------------------------------
// Global Enable for Instruction and Data Caching
//----------------------------------------------------------------

void enable_caches(void){
        __asm volatile(
        "MRC     p15, 0, r0, c1, c0, 0    \n"   // Read System Control Register
        "ORR     r0, r0, #(0x1 << 12)     \n"   // enable I Cache
        "ORR     r0, r0, #(0x1 << 2)      \n"   // enable D Cache
        "MCR     p15, 0, r0, c1, c0, 0    \n"   // Write System Control Register
        "ISB\n"
        "BX    lr\n"
        );
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler (void)
{
  while(1);
}
