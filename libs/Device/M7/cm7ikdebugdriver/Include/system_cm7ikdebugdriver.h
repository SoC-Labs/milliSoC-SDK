/*
 *-----------------------------------------------------------------------------
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 *            (C) COPYRIGHT 2011-2014  ARM Limited.
 *                ALL RIGHTS RESERVED
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 *
 *      SVN Information
 *
 *      Checked In          : $Date: 2014-01-24 16:41:40 +0000 (Fri, 24 Jan 2014) $
 *
 *      Revision            : $Revision: 268403 $
 *
 *      Release Information : CORTEX-M7-r1p2-00rel0
 *-----------------------------------------------------------------------------
 */

#ifndef __SYSTEM_CM7IKDEBUGDRIVER_H
#define __SYSTEM_CM7IKDEBUGDRIVER_H

extern uint32_t SystemFrequency;                   /*!< System Clock Frequency (Core Clock) */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system
 *         Initialise GPIO directions and values
 */
extern                   void SystemInit     (void);

/**
 * Enable Caches
 *
 * @param none
 * @return none
 *
 * @brief Enable the Caches
 *
 */
extern                   void cache_enable     (void);

/**
 * Invalidate Caches
 *
 * @param none
 * @return none
 *
 * @brief Invalidate the Caches
 *
 */
extern                   void cache_invalidate     (void);

#endif
