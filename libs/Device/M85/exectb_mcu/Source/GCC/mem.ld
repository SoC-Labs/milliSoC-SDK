/* Linker script to configure memory regions.
 * Need modifying for a specific board.
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 */
GROUP(libgcc.a libc.a libm.a librdimon.a)

MEMORY
{
  FLASH_S   (rx)  : ORIGIN = 0x00000000, LENGTH = 0x40000   /* 256k */
  FLASH_NSC (rx)  : ORIGIN = 0x00100000, LENGTH = 0x40000   /* 256k */
  FLASH_NS  (rx)  : ORIGIN = 0x00180000, LENGTH = 0x40000   /* 256k */
  RAM_S     (rwx) : ORIGIN = 0x20000000, LENGTH = 0x100000  /*   1M */
  RAM_NS    (rwx) : ORIGIN = 0x20100000, LENGTH = 0x08000   /*  32k */
}

INCLUDE "./Device/ARM/exectb_mcu/Source/GCC/sections.ld"
