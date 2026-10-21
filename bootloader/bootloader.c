//#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "ada_dma_command_lib.h"
#include "SL_QSPI.h"
#include <inttypes.h>
#include "irq_map.h"
#include "sysctrl.h"
#include "vic.h"
#include <stdint.h>

int FlashInit(void);
int DmaMemcpy(uint32_t addr, uint32_t size, uint32_t dest);
void start_main_code(void);
extern void enable_caches(void);
void DMA0_IRQHandler(void);

int FlashInit(void){
  if(SPI_STARTUP(SL_QSPI)==0){
    printf("Flash started...\n");
    CACHE_STARTUP(CG092);
  } else {
    printf("ERROR FLASH NOT STARTED\n");
    return 1;
  }
  return 0;
}


int main (void)
{
  uint32_t M_image_start, M_image_size;
  uint32_t R_image_start, R_image_size;
  // STDOUT init
  UartStdOutInit();

  printf("\nSoCLabs MilliSoC'26 ARM-M85+R5\n");
  enable_caches();
  printf("Bootloader starting...\n");

// Enable vectored interrupts
  __asm volatile("MRC     p15, 0, r0, c1, c0, 0");   // Read SCTLR
  __asm volatile("ORR     r0, r0, #(0x1 << 24) ");   // Enable VE bit 24 to enable VIC
  __asm volatile("MCR     p15, 0, r0, c1, c0, 0");   // Write SCTLR
  __asm volatile("ISB ");


  FlashInit();
  printf("Preload ITCM...\n");
  M_image_start=*(volatile uint32_t *) FLASH_BASE;
  M_image_size =*(volatile uint32_t *) (FLASH_BASE+0x04);
  DmaMemcpy(M_image_start + FLASH_BASE, M_image_size, 0x00000000);
  R_image_start=*(volatile uint32_t *) (FLASH_BASE+0x08);
  R_image_size =*(volatile uint32_t *) (FLASH_BASE+0x0C);
  DmaMemcpy(R_image_start + FLASH_BASE, R_image_size, 0x80000000);

  printf("Start main program\n\n");
  printf("***********************\n");

  sysctrl->cpu_mx_wait=0;

  //EnableITCM();
  __DSB();
  __ISB();

  start_main_code();

  UartEndSimulation();
  return 0;
}


void start_main_code(void){
  asm volatile(
  "CPSID   if \n\t" // Mask IRQ and FIQ
  "DSB  \n\t"       // Complete prior memory accesses
  "MOV     r0, #0\n\t"
  "MCR     p15, 0, r0, c7, c5, 0\n\t"//   ; Invalidate I-cache
  "DSB\n\t"
  "ISB\n\t"
  "LDR     r0, =0x00000000\n\t"
  "BX      r0\n\t"
  "B       . \n\t"
  );
}

int DmaMemcpy(uint32_t addr, uint32_t size, uint32_t dest){
// Define structures for the tests
  // Note: Command and channel parameters can be modified to the desired settings

  // Set the DMA channel related parameters
  // Channel Priority is '0' - Lowest Priority
  // Cleaning DMA Channel registers at the end of the command is enabled
  // Reloading initial register values at the end of the command is disabled
  // STATUS_DONE status flag is asserted at the end of the command.
  // Automatic pause request at the end of the command is disabled
  // Maximum source and destination burst lengths sent by the DMA are 15+1=16 (Can be limited by FIFO size)
  AdaChannelSettingsType ch_settings = {
    .CHPRIO         = 15,
    .CLEARCMD       = 1,
    .REGRELOADTYPE  = RELOAD_DISABLED,
    .DONETYPE       = DONETYPE_EOF_CMD,
    .DONEPAUSEEN    = 0,
    .SRCMAXBURSTLEN = 15,
    .DESMAXBURSTLEN = 15
  };

  // Set the attributes of the read AXI transactions sent by the DMA
  // These settings affects the ARINNER[3:0], AWINNER[3:0], ARCACHE[3:0], AWCACHE[3:0] and AxDOMAIN[1:0] attributes
  // of the AXI transactions
  // This command uses Normal memory, Non-Cacheable, Bufferable, System-shareable AXI attributes
  // ARINNER[3:0] = 4'b0011, AWINNER[3:0] = 4'b0011, ARCACHE[3:0] = 4'b0011, AWCACHE[3:0] = 4'b0011, AxDOMAIN[1:0] = 2'b11
  // More information about the AXI attribute settings can be found in the TRM
  // The command uses non-secure, non-privileged AXI read transactions
  AdaChannelSrcAttrType ch_srcattr = {
    .SRCMEMATTRLO  = 4,
    .SRCMEMATTRHI  = 4,
    .SRCSHAREATTR  = 0,
    .SRCNONSECATTR = 1,
    .SRCPRIVATTR   = 0
  };

  // Set the attributes of the write AXI transactions sent by the DMA
  // These settings affects the ARINNER[3:0], AWINNER[3:0], ARCACHE[3:0], AWCACHE[3:0] and AxDOMAIN[1:0] attributes
  // of the AXI transactions
  // This command uses Normal memory, Non-Cacheable, Bufferable, System-shareable AXI attributes
  // ARINNER[3:0] = 4'b0011, AWINNER[3:0] = 4'b0011, ARCACHE[3:0] = 4'b0011, AWCACHE[3:0] = 4'b0011, AxDOMAIN[1:0] = 2'b11
  // More information about the AXI attribute settings can be found in the TRM
  // The command uses non-secure, non-privileged AXI write transactions
  AdaChannelDesAttrType ch_desattr = {
    .DESMEMATTRLO  = 4,
    .DESMEMATTRHI  = 4,
    .DESSHAREATTR  = 0,
    .DESNONSECATTR = 1,
    .DESPRIVATTR   = 0
  };

  // Set the attributes of the read AXI transactions used for the command link related transfers
  AdaChannelLinkAttrType ch_linkattr = {
    .LINKMEMATTRLO = 4,
    .LINKMEMATTRHI = 4,
    .LINKSHAREATTR = 0
  };

  // Set the basic command parameters
  // The command uses the defined FLASH_BASE as source address, ITCM_BASE as destination address
  // the defined DATA_SIZE as source and destination data (X)size and 64 bit transaction size
  AdaBaseCommandType command_base = {
    .SRCADDR  = addr, // Read from M0 interface
    .DESADDR  = dest, // Write to M0 interface
    .SRCXSIZE = size,
    .DESXSIZE = size,
    .TRANSIZE = BITS_32
  };

  // Set the increment of source and destination addresses
  // The source and destination address increments are 1
  Ada1DIncrCommandType command_1d_incr = {
    .SRCXADDRINC = 1,           // Autoincrement by transaction size
    .DESXADDRINC = 1            // Autoincrement by transaction size
  };

  // Enable/disable the interupts of the channel
  // Error and done interrupts are enabled
  AdaIrqEnType ch_irqs = {
    .INTREN_DONE     = 1,
    .INTREN_ERR      = 1,
    .INTREN_DISABLED = 0,
    .INTREN_STOPPED  = 0
  };


  AdaChannelInit(ch_settings, ch_srcattr, ch_desattr, 0, SECURE);
  Ada1DIncrCommand(command_base, command_1d_incr, 0, SECURE);
  AdaSetIntEn(ch_irqs, 0, SECURE);

  __enable_irq();
  vic_initialisation(DMA350_CH0_INTR,&DMA0_IRQHandler);

  AdaEnable(0, SECURE);
  __WFI();

  __disable_irq();

  return 0;

}

void DMA0_IRQHandler(void)
{
	__asm volatile("stmdb	sp!,{r0-r3,r12,lr}");		// state save from compiled code
  __disable_irq();
  // Check the source of the interrupt and clear interrupts
  AdaStatType ST = AdaReadStatus(0, SECURE);
  if (ST.STAT_DONE == 1) {
    AdaClearChDone(0, SECURE);
  } else {
    printf("Unknown IRQ on CH0!\n");
  }
  __enable_irq();
  VIC_AD->VICAddress=0x0; // clear interrupt in VIC
	__asm volatile("ldmia	sp!,{r0-r3,r12,lr}	");	// state restore from compiled code
	__asm volatile("subs	pc, lr, #4");			// adjust return
  return;
}
