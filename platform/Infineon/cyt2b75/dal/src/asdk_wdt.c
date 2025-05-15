/*
Disclaimer:

    The information/code provided by Ather Energy Limited (“Ather”) in
    this file is for informational purposes only. All information/code in this
    file is provided in good faith.  However, Ather makes no representation or
    warranty of any kind, express or implied, regarding the accuracy, adequacy,
    validity, reliability, availability or completeness of any information/code
    in the file. Under no circumstances shall Ather have any liability to you
    for any loss or damage of any kind incurred as a result of reliance on any
    information/code provided in the file. Your reliance on any Ather’s
    information/code in the file is solely at your own risk.

    @file
    asdk_wdt.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_wdt.c

    @Created on
    Aug 21, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the WDT Module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_wdt.h"

// sdk includes
#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "syswdt/cy_syswdt.h"  // CYT2B75 WDT driver APIs
#include "mcwdt/cy_mcwdt.h"
#include "sysint/cy_sysint.h" // CYT2B75 system Interrupt APIs

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
/* static array of function pointer to hold user callback function */
static asdk_wdt_callback_fun_t user_wdt_callback_fun_list[ASDK_WDT_MAX];

/* Callback for WDT Peripheral*/
void WDT0_ISR(void);
void WDT1_ISR(void);
void WDT2_ISR(void);

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/*Watchdog initialization function*/
asdk_errorcode_t asdk_watchdog_init(asdk_wdt_config_t *wdt_config_data)
{

    asdk_errorcode_t ret_value = ASDK_WDT_STATUS_SUCCESS;
    /*The source clock that can be selected for the basic WDT is fixed to the ILO0 clock: 32.768 kHz*/
    /*Logic to convert time value in ms to count for watchdog*/
    volatile uint32_t timeout_value_for_cyt2b7 = ((32768 * (wdt_config_data->wdt_timeout_value_in_ms)) / 1000);
    volatile uint32_t wwdt_lower_window_timeout_value = ((32768 * (wdt_config_data->wdt_wwdt_config.lower_window_timeout_value_in_ms)) / 1000);
    volatile uint32_t wwdt_upper_window_timeout_value = ((32768 * (wdt_config_data->wdt_wwdt_config.upper_window_timeout_value_in_ms)) / 1000);
    volatile uint32_t wdt_warn_timeout_value = ((32768 * (wdt_config_data->wdt_warn_timeout_value_in_ms)) / 1000);

    /* validate configuration parameters */

    if (NULL == wdt_config_data)
    {
        return ASDK_WDT_ERROR_NULL_PTR;
    }

    if (ASDK_WDT_MAX <= wdt_config_data->wdt_no)
    {
        return ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED;
    }

    if (0u != wdt_config_data->wdt_no)
    {
        return ASDK_WDT_ERROR_INVALID_WDT_NO;
    }

    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Disable();
    Cy_WDT_Unlock();

    /*Override the functions if the application requires*/
    if (ASDK_WDT_TYPE_WINDOW == wdt_config_data->wdt_type)
    {
        Cy_WDT_SetLowerLimit(wwdt_lower_window_timeout_value);
        Cy_WDT_SetUpperLimit(wwdt_upper_window_timeout_value);
        if (0 != wdt_config_data->wdt_warn_timeout_value_in_ms)
        {
            Cy_WDT_SetWarnLimit(wdt_warn_timeout_value);
        }
        Cy_WDT_SetLowerAction(wdt_config_data->wdt_timeout_action);
        Cy_WDT_SetUpperAction(wdt_config_data->wdt_timeout_action);
        Cy_WDT_SetWarnAction(wdt_config_data->wdt_warn_timeout_action);
    }
    else
    {
        if (0 != wdt_config_data->wdt_warn_timeout_value_in_ms)
        {
            Cy_WDT_SetWarnLimit(wdt_warn_timeout_value);
        }
        Cy_WDT_SetWarnAction(wdt_config_data->wdt_warn_timeout_action);
        Cy_WDT_SetLowerLimit(0ul);
        Cy_WDT_SetUpperLimit(timeout_value_for_cyt2b7); /* Upper Limit override*/
        Cy_WDT_SetLowerAction(CY_WDT_LOW_UPP_ACTION_NONE);
        Cy_WDT_SetUpperAction(wdt_config_data->wdt_timeout_action);
        Cy_WDT_SetAutoService(CY_WDT_DISABLE);
    }
    if (wdt_config_data->enable_wdt_in_debug)
    {
        Cy_WDT_SetDebugRun(CY_WDT_ENABLE); /* This is necessary when using debugger */
    }
    else
    {
        Cy_WDT_SetDebugRun(CY_WDT_DISABLE); /* This is necessary when using debugger */
    }

    Cy_WDT_Lock();

    
    Cy_WDT_MaskInterrupt();
    Cy_WDT_Enable();

    return ret_value;
}

/*Watchdog de-initialization function*/
asdk_errorcode_t asdk_watchdog_deinit(uint8_t wdt_no)
{

    asdk_errorcode_t ret_value = ASDK_WDT_STATUS_SUCCESS;

    if (ASDK_WDT_MAX <= wdt_no)
    {
        return ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED;
    }

    if (0u != wdt_no)
    {
        return ASDK_WDT_ERROR_INVALID_WDT_NO;
    }

    Cy_WDT_Disable();

    return ret_value;
}



/*Refresh the watchdog function*/
asdk_errorcode_t asdk_watchdog_refresh(uint8_t wdt_no)
{
    asdk_errorcode_t ret_value = ASDK_WDT_STATUS_SUCCESS;

    if (ASDK_WDT_MAX <= wdt_no)
    {
        return ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED;
    }

    if (0u != wdt_no)
    {
        return ASDK_WDT_ERROR_INVALID_WDT_NO;
    }

    /*Call the driver function to clear the watchdog*/
    Cy_WDT_ClearWatchdog();
    return ret_value;
}

/*Get the current watchdog time in ms */
asdk_errorcode_t asdk_watchdog_get_time_in_ms(uint8_t wdt_no, uint32_t *wdt_time_value_in_ms)
{
    asdk_errorcode_t ret_value = ASDK_WDT_STATUS_SUCCESS;
    uint32_t wdt_count = 0;
    uint32_t wdt_time_in_ms = 0;

    if (ASDK_WDT_MAX <= wdt_no)
    {
        return ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED;
    }

    if (0u != wdt_no)
    {
        return ASDK_WDT_ERROR_INVALID_WDT_NO;
    }

    wdt_count = Cy_WDT_GetCount();
    /*Convert wdt_count to time in ms and return the value*/
    wdt_time_in_ms = wdt_count * (float)(1/32.768); // LOgic to be implemented

    *wdt_time_value_in_ms = wdt_time_in_ms;

    return ret_value;
}


/*Register/Install the callback function*/
asdk_errorcode_t asdk_watchdog_install_callback(uint8_t wdt_no, asdk_wdt_callback_fun_t callback_fun)
{
    asdk_errorcode_t ret_value = ASDK_WDT_STATUS_SUCCESS;

    /* check for max wdt module */
    if (ASDK_WDT_MAX <= wdt_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    if (0u != wdt_no)
    {
        return ASDK_WDT_ERROR_INVALID_WDT_NO;
    }

    cy_stc_sysint_irq_t stc_sysint_irq_cfg_wdt =
        {
            .sysIntSrc = srss_interrupt_wdt_IRQn,
            .intIdx = CPUIntIdx2_IRQn,
            .isEnabled = true,
        };

    if (0 == wdt_no)
    {
        /*!<  15 [DeepSleep] Hardware Watchdog Timer interrupt */
        stc_sysint_irq_cfg_wdt.sysIntSrc = srss_interrupt_wdt_IRQn;
    }
    else if (1 == wdt_no)
    {
        /*!<  12[DeepSleep] Multi Counter Watchdog Timer interrupt */
        stc_sysint_irq_cfg_wdt.sysIntSrc = srss_interrupt_mcwdt_0_IRQn;
    }
    else if (2 == wdt_no)
    {
        /*!<  14 [DeepSleep] Multi Counter Watchdog Timer interrupt */
        stc_sysint_irq_cfg_wdt.sysIntSrc = srss_interrupt_mcwdt_1_IRQn;
    }

    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg_wdt);

    /* assign approprite ISR for the warn action of the respective watchdog module. */
    switch (wdt_no)
    {
    case 0:
        Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg_wdt.sysIntSrc, WDT0_ISR);
        break;
    case 1:
        Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg_wdt.sysIntSrc, WDT1_ISR);
        break;
    case 2:
        Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg_wdt.sysIntSrc, WDT2_ISR);
        break;
    default:
        break;
    }

    NVIC_ClearPendingIRQ(stc_sysint_irq_cfg_wdt.intIdx);
    NVIC_EnableIRQ(stc_sysint_irq_cfg_wdt.intIdx);

    /* store callback function */
    user_wdt_callback_fun_list[wdt_no] = callback_fun;

    return ret_value;
}

/*----------------------------------------------------------------------------*/
/* Function : WDT0_ISR*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the ISR function triggered when the warn is enabled for the WDT

  @param void

  @return void

*/

void WDT0_ISR()
{

    uint8_t wdt_no = 0;
    /*Clear the WDT0 interrupt*/
    Cy_WDT_ClearInterrupt();

    user_wdt_callback_fun_list[wdt_no](wdt_no);
}
/*----------------------------------------------------------------------------*/
/* Function : WDT1_ISR*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the ISR function triggered when the warn is enabled for the WDT

  @param void

  @return void

*/

void WDT1_ISR()
{
    uint32_t masked;
    uint8_t wdt_no = 1;
    masked = Cy_MCWDT_GetInterruptStatusMasked(MCWDT0);
    /*Clear the WDT0 interrupt*/
    Cy_MCWDT_ClearInterrupt(MCWDT0, masked);

    user_wdt_callback_fun_list[wdt_no](wdt_no);
}
/*----------------------------------------------------------------------------*/
/* Function : WDT2_ISR*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the ISR function triggered when the warn is enabled for the WDT

  @param void

  @return void

*/

void WDT2_ISR()
{
    uint32_t masked;
    uint8_t wdt_no = 2;
    masked = Cy_MCWDT_GetInterruptStatusMasked(MCWDT1);
    /*Clear the WDT0 interrupt*/
    Cy_MCWDT_ClearInterrupt(MCWDT1, masked);

    user_wdt_callback_fun_list[wdt_no](wdt_no);
}
