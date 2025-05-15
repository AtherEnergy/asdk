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
    asdk_system.h

    @path
    inc/asdk_system.h

    @Created on
    Sep 11, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the SYSTEM DAL module of ASDK.
*/

#ifndef ASDK_SYSTEM_H
#define ASDK_SYSTEM_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>

/* asdk includes ***************************** */

#include "asdk_error.h" // Defines ASDK CAN module error codes

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_sys_enum_group Enumerations
 *  Lists all the enumerations used by the System module.
 *  @{
 */

/*!
 * @brief An enumerator to represent system reset reason.
 */
typedef enum
{
    ASDK_SYS_RESET_DEFAULT = 0, /*!< Default value */
    ASDK_SYS_RESET_WDG,         /*!< Watchdog reset */
    ASDK_SYS_RESET_SEGV,        /*!< */
    ASDK_SYS_RESET_SW,          /*!< Software reset */
    ASDK_SYS_RESET_HOST,        /*!< Host reset */
    ASDK_SYS_RESET_LOW_VOLTAGE, /*!< Reset due to low voltage */
    ASDK_SYS_RESET_POWERON,     /*!< Power on reset */
    ASDK_SYS_RESET_EXT_PIN,     /*!< Reset due to external pin  */
    ASDK_SYS_RESET_CORE_LOCK,   /*!< Reset due to core lock */
    ASDK_SYS_RESET_CLOCK_LOSS,  /*!< Reset due to clock loss */
    ASDK_SYS_RESET_JTAG_RESET,  /*!< JTAG reset */
    ASDK_SYS_RESET_PMIC,        /*!< PMIC status triggered a reset */
    ASDK_SYS_RESET_OVDVDDD,     /*!< Reset due to over voltage detection on the external VDDD supply */
    ASDK_SYS_RESET_OVDVDDA,     /*!< Reset due to over voltage detection on the external VDDA supply */
    ASDK_SYS_RESET_OVDVCCD,     /*!< Reset due to over voltage detection on the internal core VCCD supply */
    ASDK_SYS_RESET_BODVDDA,     /*!< Reset due to external VDDA supply crossed the brown-out limit */
    ASDK_SYS_RESET_BODVCCD,     /*!< Reset due to internal VCCD core supply crossed the brown-out limit */
    ASDK_SYS_RESET_DPSLP_FAULT, /*!< he fault logging system requested a reset from its DeepSleep logic */
    ASDK_SYS_RESET_PXRES,       /*!< Programmable XRES triggered */
    ASDK_SYS_REST_STRUCT_XRES,  /*!< Structural reset was asserted */
    ASDK_SYS_RESET_UNKNOWN,     /*!< Other reason, please refer to platform. */
    ASDK_SYS_RESET_MAX          /*!< Total number of reset reasons */
} asdk_sys_reset_t;

/** @} */ // end of asdk_sys_enum_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_sys_ds_group Data structures
 *  Lists all the data structures used by the System module.
 *  @{
 */

/** @} */ // end of asdk_sys_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_sys_cb_group Callback function type
 *  Lists the callback functions from the System module.
 *  @{
 */

/** @} */ // end of asdk_sys_cb_group

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_sys_fun_group Functions
 *  Lists the functions/APIs from the System module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the system timer.

  @return @ref ASDK_SYS_SUCCESS
*/
asdk_errorcode_t asdk_sys_init(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_sw_reset */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function triggers software reset.
*/
void asdk_sys_sw_reset(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_enable_interrupts */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable all interrupts.
*/
void asdk_sys_enable_interrupts(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_disable_interrupts */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable all interrupts.
*/
void asdk_sys_disable_interrupts(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_get_core_clock_frequency */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets CPU core's clock frequency.

  @return Core clock frequency in Hz.
*/
uint32_t asdk_sys_get_core_clock_frequency(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_get_time_ms */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get system up time in milliseconds.

  @return System up time in milli seconds.
*/
uint64_t asdk_sys_get_time_ms(void);

/*----------------------------------------------------------------------------*/
/* Function : asdk_sys_get_reset_reason */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get reset reason.

  @return System reset reason.
*/
asdk_sys_reset_t asdk_sys_get_reset_reason(void);

/** @} */ // end of asdk_sys_fun_group

#endif /* ASDK_SYSTEM_H */
