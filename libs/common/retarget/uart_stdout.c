/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from Arm Limited or its affiliates.
 *
 *            (C) COPYRIGHT 2010-2013 Arm Limited or its affiliates.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from Arm Limited or its affiliates.
 *
 *      SVN Information
 *
 *      Checked In          : $Date: 2017-10-10 15:55:38 +0100 (Tue, 10 Oct 2017) $
 *
 *      Revision            : $Revision: 371321 $
 *
 *      Release Information : Cortex-M System Design Kit-r1p1-00rel0
 *-----------------------------------------------------------------------------
 */

 /*

 UART functions for retargetting

 */

#include "uart_stdout.h"
#include "CMSDK.h"
#define CLKFREQ    100000000
#define BAUDRATE   3125000
#define BAUDCLKDIV (CLKFREQ / BAUDRATE)

void UartStdOutInit(void)
{
  CMSDK_UART0->CTRL    = 0x00;       // disable whie reprogramming

  CMSDK_USRT0->CTRL    = 0x00;       // RX+TX, FT1248 USRT
  CMSDK_USRT0->BAUDDIV = 0x00;       // (prescaler value)
  CMSDK_USRT0->CTRL    = 0x03;       // RX+TX, FT1248 USRT
  return;
}

// Output a character
unsigned char UartPutc(unsigned char my_ch)
{
  while (CMSDK_USRT0->STATE & 1); // Wait if Transmit Holding register full
  CMSDK_USRT0->DATA = my_ch; // write to transmit holding register
  return (my_ch);
}
// Get a character
unsigned char UartGetc(void)
{
  while (((CMSDK_UART0->STATE & 2)==0) & ((CMSDK_USRT0->STATE & 2)==0));
  if ((CMSDK_UART0->STATE & 2)==2) return (CMSDK_UART0->DATA);
  if ((CMSDK_USRT0->STATE & 2)==2) return (CMSDK_USRT0->DATA);
  else{return 0;}
}

void UartEndSimulation(void)
{
  UartPutc((char) 0x4); // End of simulation
  while(1);
}

