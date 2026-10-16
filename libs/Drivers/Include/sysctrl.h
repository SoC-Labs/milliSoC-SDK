
#include <stdint.h>
#include "sys_memory_map.h"
typedef struct
{
    volatile uint32_t reset_info;
    volatile uint32_t cpu_mx_wait;
} sysctrl_typedef;

static sysctrl_typedef * sysctrl = (sysctrl_typedef *)(SYSCTRL_BASE);