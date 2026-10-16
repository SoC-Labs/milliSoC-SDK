//#include "exectb_mcu.h"
#include "CMSDK.h"
#include "uart_stdout.h"
#include <stdio.h>
#include "ada_dma_command_lib.h"
#include "SL_QSPI.h"
#include <inttypes.h>
#include "gic400.h"
#include "irq_map.h"
#include "sysctrl.h"

int FlashInit(void);
int DmaMemcpy(void);
void start_main_code(void);
extern void enable_caches(void);
void DMA0_IRQHandler(int num, int src);

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
  // STDOUT init
  UartStdOutInit();

  printf("\nSoCLabs MilliSoC'26 ARM-M85+R5\n");
  enable_caches();
  printf("Bootloader starting...\n");

  FlashInit();
  printf("Preload ITCM...\n");
  DmaMemcpy();

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
  "MOVS  R0,#0\n\t"
  "LDR   R1,[R0]\n\t"     //; Get initial MSP value
  "MOV   SP, R1\n\t"
  "LDR   R1,[R0, #4]\n\t" //; Get initial PC value
  "BX    R1\n\t"
  );
}

int DmaMemcpy(void){
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
    .SRCADDR  = FLASH_BASE, // Read from M0 interface
    .DESADDR  = ITCM_BASE, // Write to M0 interface
    .SRCXSIZE = ITCM_SIZE/8,
    .DESXSIZE = ITCM_SIZE/8,
    .TRANSIZE = BITS_64
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

  gic_initialise_intr(DMA350_CH0_INTR,0,1,0);
  gic_install_handler(DMA350_CH0_INTR, &DMA0_IRQHandler);
  gic_enable_interrupt(DMA350_CH0_INTR);
  __enable_irq();
  AdaEnable(0, SECURE);
  __WFI();

  __disable_irq();
  gic_disable_interrupt(DMA350_CH0_INTR);

  return 0;

}

void DMA0_IRQHandler(int num, int src)
{
  __disable_irq();
  // Check the source of the interrupt and clear interrupts
  AdaStatType ST = AdaReadStatus(0, SECURE);
  if (ST.STAT_DONE == 1) {
    AdaClearChDone(0, SECURE);
  } else {
    printf("Unknown IRQ on CH0!\n");
  }
}
