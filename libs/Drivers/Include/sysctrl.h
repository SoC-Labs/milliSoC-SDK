
#include <stdint.h>
#include "sys_memory_map.h"
typedef struct
{
    volatile uint32_t reset_info;
    volatile uint32_t cpu_mx_wait;
    volatile uint32_t reset_command;
} sysctrl_typedef;

#define SYSCTRL_RESET_COMMAND_R5       (1UL << 0)
#define SYSCTRL_RESET_COMMAND_M85      (1UL << 1)
#define SYSCTRL_RESET_COMMAND_SYSTEM   (1UL << 2)

#define SYSCTRL_RESET_CAUSE_SYSRESET   (1UL << 0)
#define SYSCTRL_RESET_CAUSE_WATCHDOG   (1UL << 1)
#define SYSCTRL_RESET_CAUSE_DAP        (1UL << 2)
#define SYSCTRL_RESET_CAUSE_SW_SYSTEM  (1UL << 3)
#define SYSCTRL_RESET_CAUSE_SW_R5      (1UL << 4)
#define SYSCTRL_RESET_CAUSE_SW_M85     (1UL << 5)
#define SYSCTRL_RESET_CAUSE_R5_DEBUG   (1UL << 6)

#define sysctrl ((sysctrl_typedef *)(SYSCTRL_BASE))