#include <stdint.h>
#include "sys_memory_map.h"

typedef struct{
    volatile uint32_t   IRQStatus;      // 0x000
    volatile uint32_t   FIQStatus;      // 0x004
    volatile uint32_t   RawIntr;        // 0x008
    volatile uint32_t   IntSelect;      // 0x00C
    volatile uint32_t   IntEnable;      // 0x010
    volatile uint32_t   IntEnClear;     // 0x014
    volatile uint32_t   SoftInt;        // 0x018
    volatile uint32_t   SoftIntClear;   // 0x01C
    volatile uint32_t   Protection;     // 0x020
    volatile uint32_t   SWPriorityMask; // 0x024
    volatile uint32_t   PriorityDaisy;  // 0x028
} VIC_typedef;

typedef struct{
    volatile uint32_t   VectAddr[32];
} VIC_vectaddr_typedef;

typedef struct{
    volatile uint32_t   VectPriority[32];
} VIC_vectpriority_typedef;

typedef struct{
    volatile uint32_t   VICAddress;
} VIC_address_typedef;

#define VIC     ((VIC_typedef *) VIC_BASE)
#define VIC_VA  ((VIC_vectaddr_typedef *) (VIC_BASE+0x100))
#define VIC_VP  ((VIC_vectpriority_typedef *) (VIC_BASE+0x200))
#define VIC_AD  ((VIC_address_typedef *) (VIC_BASE+0xF00))

extern void vic_enable_irq(uint32_t irq);
extern void vic_disable_irq(uint32_t irq);
void vic_initialisation(int irq, void (*vect_addr));