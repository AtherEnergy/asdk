/*
Disclaimer:

    The information/code provided by Ather Energy Private Limited (“Ather”) in
    this file is for informational purposes only. All information/code in this
    file is provided in good faith.  However, Ather makes no representation or
    warranty of any kind, express or implied, regarding the accuracy, adequacy,
    validity, reliability, availability or completeness of any information/code
    in the file. Under no circumstances shall Ather have any liability to you
    for any loss or damage of any kind incurred as a result of reliance on any
    information/code provided in the file. Your reliance on any Ather’s
    information/code in the file is solely at your own risk.

    @file
    asdk_system.c

    @path
    platform/stm32/f4/dal/src/asdk_system.c

    @Created on
    Sep 11, 2023

    @Author
    Ather Energy Pvt. Ltd.

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file implements the SYSTEM module for STM32F4 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

#include "asdk_platform.h"

/* dal includes ****************************** */

#include "asdk_system.h"
#include "asdk_clock.h"

/* sdk includes ****************************** */


/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

#ifdef USE_RTOS
extern uint64_t asdk_os_get_time_in_ms(void);
#endif

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/* static functions ************************** */

asdk_errorcode_t asdk_sys_init()
{
#ifdef SYS_TIMER
    asdk_timer_init(SYS_TIMER, sys_timer_config);
    asdk_timer_start(SYS_TIMER);
#endif

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
    {
        return ASDK_SYS_ERROR_TIMER_INIT_FAILED;
    }

    return ASDK_SYS_SUCCESS;
}

void asdk_sys_sw_reset(void)
{
    HAL_NVIC_SystemReset();
}

void asdk_sys_enable_interrupts(void)
{
    __enable_irq();
}

void asdk_sys_disable_interrupts(void)
{
    __disable_irq();
}

inline uint32_t asdk_sys_get_core_clock_frequency(void)
{
    return SystemCoreClock;
}

uint64_t asdk_sys_get_time_ms(void)
{
#ifdef USE_RTOS
    return asdk_os_get_time_in_ms();
#endif

#ifdef SYS_TIMER
    return asdk_timer_get_time(SYS_TIMER);
#else
    return HAL_GetTick();
#endif
}

asdk_sys_reset_t asdk_sys_get_reset_reason(void)
{
    asdk_sys_reset_t reason = ASDK_SYS_RESET_UNKNOWN;

    /* Low power management reset was generated */
    if (READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF))
    {
        reason = ASDK_SYS_RESET_LOW_POWER_MGMT;
    }
    /* Window WDG reset was triggered, but categorizing as WDG reset */
    else if (READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF))
    {
        reason = ASDK_SYS_RESET_WDG;
    }
    /* Watchdog reset was triggered */
    else if (READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF))
    {
        reason = ASDK_SYS_RESET_WDG;
    }
    /* Software reset generated */
    else if (READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF))
    {
        reason = ASDK_SYS_RESET_SW;
    }
    /* Power On Reset generated */
    else if (READ_BIT(RCC->CSR, RCC_CSR_PORRSTF))
    {
        reason = ASDK_SYS_RESET_POWERON;
    }
    /* Pin Reset generated */
    else if (READ_BIT(RCC->CSR, RCC_CSR_PINRSTF))
    {
        reason = ASDK_SYS_RESET_EXT_PIN;
    }
    /* Brown-out reset triggered */
    else if (READ_BIT(RCC->CSR, RCC_CSR_BORRSTF))
    {
        reason = ASDK_SYS_RESET_LOW_VOLTAGE;
    }

    return reason;
}

void asdk_sys_clear_reset_reason(void)
{
    /* Writing RMVF bit clears all the reset flags */
    SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}
