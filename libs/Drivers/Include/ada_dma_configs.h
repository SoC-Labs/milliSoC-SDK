//----------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from Arm Limited or its affiliates.
//
// (C) COPYRIGHT 2021-2022 Arm Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Arm Limited or its affiliates.
//----------------------------------------------------------------------------
//
// Release Information : DMA350-r0p0-00rel0
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Purpose : ADA DMA config definition file
#ifndef ADA_OOB_RTL_PARAM_DEFINES
#define ADA_OOB_RTL_PARAM_DEFINES


#define CH__GPO__MASK                   (uint32_t)   0
#define CH__STREAM__MASK                (uint32_t)   0
#define CH__0__FIFO__DEPTH              (uint32_t)   8
#define CH__1__FIFO__DEPTH              (uint32_t)   8
#define CH__2__FIFO__DEPTH              (uint32_t)   2
#define CH__3__FIFO__DEPTH              (uint32_t)   8
#define CH__4__FIFO__DEPTH              (uint32_t)   8
#define CH__5__FIFO__DEPTH              (uint32_t)   2
#define CH__6__FIFO__DEPTH              (uint32_t)   2
#define CH__7__FIFO__DEPTH              (uint32_t)   2
#define CH__EXT__FEAT__MASK             (uint32_t)   3
#define TRIG__IN__SYNC__EN__MASK        (uint32_t)   0
#define TRIG__OUT__SYNC__EN__MASK       (uint32_t)   0
#define AXI5__M1__PRESENT               (uint32_t)   0
#define ID__WIDTH                       (uint32_t)   2
#define STRB__WIDTH                     (uint32_t)   8
#define POIS__WIDTH                     (uint32_t)   1
#define ADDR__WIDTH                     (uint32_t)   32
#define DATA__WIDTH                     (uint32_t)   64
#define CHID__WIDTH                     (uint32_t)   0
#define GPO__WIDTH                      (uint32_t)   1
#define NUM__CHANNELS                   (uint32_t)   4
#define NUM__TRIGGER__IN                (uint32_t)   4
#define NUM__TRIGGER__OUT               (uint32_t)   4
#define SECEXT__PRESENT                 (uint32_t)   0

#endif
