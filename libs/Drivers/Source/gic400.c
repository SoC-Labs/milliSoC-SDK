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

#include <stdio.h>
#include "gic400.h"
#include "uart_stdout.h"
#include "CMSDK.h"
#include "cmsis_armclang.h"
#include "sys_memory_map.h"
// #ifdef AARCH_V8
// # include "ipc.h"
// # include "cpu_asm_codes.h"
// #else
// # include "helper_functions.h"
// #endif

#define MAX_NO_OF_INTERRUPTS_IMPLEMENTED  512
#define SPURIOUS_INTERRUPT                1023
#define INTERRUPT_MASK                    0x000003FF
#define CPUID_SHIFT                       10


/* DEFAULT ADDRESSES IN MEMORY MAP FOR INTERRUPT CONTROLLER */
static interrupt_distributor    * id = (interrupt_distributor *)(GIC_DISTRIBUTOR_BASE);
static cpu_interface            * ci = (cpu_interface *)(GIC_CPU_INTERFACE_BASE);
static virtual_interface_ctrl *vctrl = (virtual_interface_ctrl *)(GIC_VIRTUAL_INTERFACE_CONTROL_BASE);
static virtual_cpu_interface    *vci = (virtual_cpu_interface *)(GIC_VIRTUAL_CPU_INTERFACE_BASE);


static IntHandler IntHandlers[MAX_NO_OF_INTERRUPTS_IMPLEMENTED] __attribute__((section ("RW")));

/* Following function routes the interrupt to targeted CPU */
void intr_target_cpu(unsigned int interrupt, unsigned int cpu, unsigned int set)
{
  unsigned int word, bit_shift, temp;

  /* There are 4 interrupt target registers per word */
  word = (interrupt / 4);
  bit_shift = (interrupt % 4) * 8;
  cpu = (1 << cpu) << bit_shift;

  temp = id->target[word];
  if (set)
    {
      temp |= cpu;
    }
  else
    {
      temp &= ~cpu;
    }
  id->target[word] = temp;

#ifdef DEBUG
  printf("intr_target_cpu: interrupt = %d addr=%x val=%x vtbw=%x\n",interrupt, &id->target[word], id->target[word], temp);
#endif

  __ISB();
}

/* Following function sets the security of interrupt */
void intr_security_set(unsigned int interrupt,  unsigned int set)
{
  unsigned int word, bit_shift, temp;

  /* There are 4 interrupt target registers per word */
  word = interrupt / 32;
  bit_shift = (interrupt % 32);

  /* Non-secure interrupt */
  temp = id->group[word];
  if (set)
    {
      temp |= (1U << bit_shift);
    }
  /* Secure interrupt */
  else
    {
      temp &= ~(1U << bit_shift);
    }

  //  while(id->group[word] ==0)
    id->group[word] = temp;

#ifdef DEBUG
  printf("intr_security_set: interrupt = %d addr=%x val=%x vtbw=%x\n",interrupt, &id->group[word], id->group[word], temp);
#endif

  __ISB();
}

/* Following function enables the interrupt */
void gic_enable_interrupt (unsigned int i)
{
  unsigned int word;

  word  = i / 32;
  i    %= 32;
  i     = 1 << i;

  //  while(id->enable.set[word] ==0)
  id->enable.set[word] = i;

#ifdef DEBUG
  printf("Enable_intr: addr=%x val=%x vtbw=%x\n",&id->enable.set[word],id->enable.set[word],i);
#endif
}

/* Following function disables the interrupt */
void gic_disable_interrupt (unsigned int i)
{
  unsigned int word;

  word  = i / 32;
  i    %= 32;
  i     = 1 << i;

  //  while(id->enable.clear[word]==0)
  id->enable.clear[word] = i;
}

/* Set correct interrupt configuration (level sensitive, 1-N) */
void set_intr_conf(unsigned int int_num, unsigned int int_val)
{
  unsigned int reg_num = int_num / 16;
  unsigned int bit_shift = (int_num % 16) * 2;

  //  while(id->configuration[reg_num] ==0)
  id->configuration[reg_num] |= (int_val << (bit_shift));
#ifdef DEBUG
  printf("set_intr_conf: interrupt = %d addr=%x val=%x vtbw=%x\n",int_num, &id->configuration[reg_num], id->configuration[reg_num], (int_val << (bit_shift)) );
#endif

}

/* Following function sets the priority and priority masks */
void set_intr_priority_mask(unsigned int intr_num, unsigned int intr_pri, unsigned int pri_mask)
{
  unsigned int reg_num = (intr_num / 4);
  unsigned int bit_shift = (intr_num % 4) * 8;

  if (intr_pri > pri_mask)
    printf("Interrupt priority should be lesser than priority mask\n");

  //  while(ci->priority_mask == 0)
  ci->priority_mask = pri_mask;

  //  while(id->priority[reg_num] == 0)
  id->priority[reg_num] |= (intr_pri << (bit_shift));

#ifdef DEBUG
  printf("priority mask: intr num=%d addr=%x val=%x vtbw=%x\n",intr_num, &ci->priority_mask, ci->priority_mask, pri_mask);
  printf("priority mask: addr=%x val=%x vtbw=%x\n", &id->priority[reg_num], id->priority[reg_num], (intr_pri << ((bit_shift))) );
#endif
}

/* Following function sets the cpu interface and distributor */
void gic_init_ci_di(unsigned int fiq_enable)
{
  if(fiq_enable){
    ci->control = 0x9;

  } else {
    //    while(ci->control ==0)
    ci->control = 0x1;
  }

  //while(id->control == 0)
  id->control = 0x1;

#ifdef DEBUG
  printf("gic_init_ci_di: addr=%x val=%x addr=%x val=%x\n",&ci->control, ci->control, &id->control, id->control);
#endif

}

/* Following function configures the GIC, targets the interrupt to cpu and configures priority */
void gic_initialise_intr(unsigned int interrupt, unsigned int cpu, unsigned int set, unsigned int fiq_enable)
{
  /* Install the handler for v7 code */

  id->control = 0x00000000;
  /* Interrupt target cpu */
  intr_target_cpu(interrupt, cpu, set);

  /* Interrupt (1-N model, Level sensitive) -> 0x1  */
  /* Interrupt (N-N model, edge sensitive)  -> 0x2 */
  /* Refer to Interrupt configuration register GICD_ICFGRn */
  set_intr_conf(interrupt, 0x1);

  /* Set priority mask and priority */
  set_intr_priority_mask(interrupt, 0x00, 0xF0);

  /* Set security */
  intr_security_set(interrupt, 1);

  /* Enable gic distributor and CPU interface */
  gic_init_ci_di(fiq_enable);

 }


/* Following function installs the handler */
void gic_install_handler(unsigned int interrupt, IntHandler handler)
{
  if (interrupt > MAX_NO_OF_INTERRUPTS_IMPLEMENTED) {
    return;
  }
  IntHandlers[interrupt] = handler;
}


/* Following function calls the interrupt handler */
void irq_handler(void)
{
  int source, interrupt, raw_interrupt;

  /* service all pending interrupts */
  while (1)
    {
      /* Get the highest priority interrupt */
      raw_interrupt = ci->interrupt_ack;
      source        = raw_interrupt >> CPUID_SHIFT;
      interrupt     = raw_interrupt & INTERRUPT_MASK;

      if (interrupt == SPURIOUS_INTERRUPT) {
        break;
      }

      /* Call the handler function */
      if (IntHandlers[interrupt]) {
        IntHandlers[interrupt](interrupt, source);
      }

      //      data_sync_barrier();

      /* Clear the interrupt */
      ci->end_of_interrupt = raw_interrupt;
    }

}

/* Following function calls the virtual interrupt handler */
void call_handler_virq(unsigned int interrupt, unsigned int source)
{
  if (IntHandlers[interrupt]) {
    IntHandlers[interrupt](interrupt, source);
  }
}

/* Following function identifies the interrupt type */
void gic_interrupt_type(unsigned int interrupt, unsigned int edge_type) {

  unsigned int tmp_config;
  unsigned int config_reg;
  unsigned int bit_pos;

  //check interrupt is in range
  if(interrupt > MAX_NO_OF_INTERRUPTS_IMPLEMENTED) {
    return;
  }

  //calculate which GICD_ICFGRn the interrupt is in
  config_reg = interrupt / 16;

  //calculate bit position in the GICD_ICFGRn register
  //Add 1 as each interrupt has two bits [X:Y]
  //Y - is readonly
  //X - selects the edge type
  bit_pos = (interrupt - (config_reg * 16)) * 2 + 1;

  tmp_config = id->configuration[config_reg];

  tmp_config = tmp_config | (edge_type << bit_pos);

  id->configuration[config_reg] = tmp_config;

}

void gic_read_pids(){
  printf("Addr=%x val=%x\n",&id->pid4, id->pid4);
  for (int i=0;i<4;i++){
    printf("Addr=%x val=%x\n",&id->pid[i], id->pid[i]);
  }
  for (int i=0;i<4;i++){
    printf("Addr=%x val=%x\n",&id->cid[i], id->cid[i]);
  }
}

void gic_set_active_interrupt(unsigned int i){
  unsigned int word;

  word  = i / 32;
  i    %= 32;
  i     = 1 << i;
  id->active.set[word] = i;

}

void gic_disable_cpu_interface(void)
{
    /* Disable the CPU Interface */
    ci->control &= ~(ENABLE_GROUP0 | ENABLE_GROUP1);
}