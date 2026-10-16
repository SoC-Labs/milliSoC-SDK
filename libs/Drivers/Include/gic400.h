//------------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from Arm Limited or its affiliates.
//
//        (C) COPYRIGHT 2018-2021 Arm Limited or its affiliates.
//            ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Arm Limited or its affiliates.
//
//      Release Information : SSE710-r0p0-00rel0
//
// -----------------------------------------------------------------------------

#ifndef __GIC400_H__
#define __GIC400_H__

#include <stdio.h>

#define ENABLE_GROUP0   1U << 0
#define ENABLE_GROUP1   1U << 1

/* Register Definitions*/

struct set_and_clear_regs
{
  volatile unsigned int set[32], clear[32];
};

typedef struct
{
  volatile unsigned int control;                  /* 0x000 */ 
  volatile unsigned const int controller_type;    /* 0x004 */ 
  volatile unsigned int iidr;                     /* 0x008 */ 
  const unsigned int padding1[29];                /* 0x010 */
  unsigned int group[32];                         /* 0x080 */
  struct set_and_clear_regs enable;               /* 0x100 */ 
  struct set_and_clear_regs pending;              /* 0x200 */ 
  struct set_and_clear_regs active;               /* 0x300 */ 
  volatile unsigned int priority[256];            /* 0x400 */ 
  volatile unsigned int target[256];              /* 0x800 */ 
  volatile unsigned int configuration[64];        /* 0xC00 */ 
  const char padding2[512];                       /* 0xD00 */ 
  volatile unsigned int software_interrupt;       /* 0xF00 */ 
  const char padding3[207];                                   
  volatile unsigned const int pid4;               /* 0xFD0 */ 
  const char padding4[12];                                    
  volatile unsigned const int pid[4];             /* 0xFE0 */ 
  volatile unsigned const int cid[4];             /* 0xFF0 */ 
} interrupt_distributor;

typedef struct
{
  volatile unsigned int control;                    /* 0x00 */
  volatile unsigned int priority_mask;              /* 0x04 */
  volatile unsigned int binary_point;               /* 0x08 */
  volatile unsigned const int interrupt_ack;        /* 0x0c */
  volatile unsigned int end_of_interrupt;           /* 0x10 */
  volatile unsigned const int running_priority;     /* 0x14 */
  volatile unsigned const int highest_pending;      /* 0x18 */
  volatile unsigned int aliased_binary_point;       /* 0x1C */
  volatile unsigned int aliased_interrupt_ack;      /* 0x20 */
  volatile unsigned int aliased_end_of_interript;   /* 0x24 */    
 
} cpu_interface;


typedef struct
{
  volatile unsigned int hcr;            /* 0x00  */
  volatile unsigned int vtr;            /* 0x04  */
  volatile unsigned int vmcr;           /* 0x08  */
  volatile unsigned int padding1;       /* 0x0c  */
  volatile unsigned int misr;           /* 0x10  */
  volatile unsigned int padding2[3];               
  volatile unsigned int eisr0;          /* 0x20  */
  volatile unsigned int eisr1;          /* 0x24  */
  volatile unsigned int padding3[2];               
  volatile unsigned int elsr0;          /* 0x30  */
  volatile unsigned int elsr1;          /* 0x34  */
  volatile unsigned int padding4[2];               
  volatile unsigned int padding5[48];              
  volatile unsigned int lr[64];         /* 0x100 */
}virtual_interface_ctrl;


typedef struct 
{
   volatile unsigned int ctlr;              /* 0x00  */ 
   volatile unsigned int pmr;               /* 0x04  */ 
   volatile unsigned int bpr;               /* 0x08  */ 
   volatile unsigned int iar;               /* 0x0c  */ 
   volatile unsigned int eoir;              /* 0x10  */ 
   volatile unsigned int rpr;               /* 0x14  */ 
   volatile unsigned int hppir;             /* 0x18  */ 
   volatile unsigned int abpr;              /* 0x1c  */ 
   volatile unsigned int aiar;              /* 0x20  */ 
   volatile unsigned int aeoir;             /* 0x24  */ 
   volatile unsigned int ahppir;            /* 0x28  */ 
   volatile unsigned int padding1;          /* 0x2C  */ 
   volatile unsigned int padding2[1012];                
   volatile unsigned int dir;               /* 0x1000 */

} virtual_cpu_interface;

extern void (*wrapped_irq_vector[][4])(void);

void irq_handler(void);


/* Function prototype - interrupt handler function prototype */
typedef void (*IntHandler)(int, int);

/* Function prototype - legacy gic initialise function */
void gic_initialise (void);

/* Function prototype - install interrupt handler */
void gic_install_handler (unsigned int, IntHandler);

/* Function prototype - enable interrupt */
void gic_enable_interrupt (unsigned int);

/* Function prototype - disable interrupt */
void gic_disable_interrupt (unsigned int);

/* Function prototype - virtual irq handler */
void call_handler_virq(unsigned int, unsigned int);

/* Function prototype - returns interrupt type */
void gic_interrupt_type(unsigned int, unsigned int);

/* Function prototype - interrupt handler */
void irq_handler(void); 

/* Function prototype - target the cpu in multi-cluster multi-cpu environment */
void intr_target_cpu(unsigned int interrupt, unsigned int cpu, unsigned int set);

/* Function prototype - set the interrupt security */
void intr_security_set(unsigned int interrupt,  unsigned int set);

/* Function prototype - set the interrupt security */
void set_intr_conf(unsigned int int_num, unsigned int int_val);

/* Function prototype - set the interrupt priority and priority mask */
void set_intr_priority_mask(unsigned int intr_num, unsigned int intr_pri, unsigned int pri_mask);

/* Function prototype - set the cpu interface and distributor */
void gic_init_ci_di(unsigned int fiq_enable);

/* Function prototype - Gic improved initialise method */
void gic_initialise_intr(unsigned int interrupt, unsigned int cpu, unsigned int set, unsigned int fiq_enable);

void gic_read_pids();
void gic_set_active_interrupt(unsigned int);
void gic_disable_cpu_interface(void);


#endif