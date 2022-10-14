/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************************************************************************************
 * Include
 ************************************************************************************/

#include "fwk_platform.h"

/************************************************************************************
 * Private memory declarations
 ************************************************************************************/

static volatile int timer_manager_initialized = 0;

/************************************************************************************
*************************************************************************************

* Public functions
*************************************************************************************
************************************************************************************/
timer_status_t PLATFORM_InitTimerManager(void)
{
    /* Initialize timer manager */
    timer_config_t timerConfig;
    timer_status_t status;

    if (timer_manager_initialized == 0)
    {
        timerConfig.instance    = PLATFORM_TM_INSTANCE;
        timerConfig.srcClock_Hz = CLOCK_GetFreq(kCLOCK_OscClk);

        status = TM_Init(&timerConfig);
        if (status == kStatus_TimerSuccess)
        {
            timer_manager_initialized = 1;
        }
    }
    return status;
}

void PLATFORM_DeinitTimerManager(void)
{
    if (timer_manager_initialized == 1)
    {
        TM_Deinit();
        timer_manager_initialized = 0;
    }
}
