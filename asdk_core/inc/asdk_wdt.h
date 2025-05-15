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
    asdk_wdt.h

    @path
    inc/asdk_wdt.h

    @Created on
    Aug 21, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the WDT Module for ASDK.

*/

#ifndef ASDK_WDT_H_
#define ASDK_WDT_H_

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_error.h"

/*==============================================================================

                        DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define ASDK_WDT_MAX 3

/*==============================================================================

DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_wdt_enum_group Enumerations
 *  Lists all the enumerations used by the Watchdog module.
 *  @{
 */

/*!
 * @brief WDT type whether it is IWDT or WWDT
 */
typedef enum
{
    ASDK_WDT_TYPE_INDEPENDENT = 0, /*!< Independent watchdog. */
    ASDK_WDT_TYPE_WINDOW,   /*!< Watchdog window type. */
    ASDK_WDT_TYPE_MAX,
    ASDK_WDT_TYPE_UNDEFINED = ASDK_WDT_TYPE_MAX,
} asdk_wdt_type_t;

/*!
 * @brief WDT timeout action
 */

typedef enum
{
    ASDK_WDT_TIMEOUT_ACTION_NONE = 0,   /*!< No action. */
    ASDK_WDT_TIMEOUT_ACTION_RESET,      /*!< Reset on timeout. */
    ASDK_WDT_TIMEOUT_ACTION_MAX,
    ASDK_WDT_TIMEOUT_ACTION_UNDEFINED = ASDK_WDT_TIMEOUT_ACTION_MAX,
} asdk_wdt_timeout_action_t;

/*!
 * @brief WDT WARN timeout action
 */

typedef enum
{

    ASDK_WDT_WARN_TIMEOUT_ACTION_NONE = 0,  /*!< No action. */
    ASDK_WDT_WARN_TIMEOUT_ACTION_INTERRUPT, /*!< Reset on timeout. */
    ASDK_WDT_WARN_TIMEOUT_ACTION_MAX,
    ASDK_WDT_WARN_TIMEOUT_ACTION_UNDEFINED = ASDK_WDT_WARN_TIMEOUT_ACTION_MAX,
} asdk_wdt_warn_timeout_action_t;

/** @} */ // end of asdk_wdt_enum_group

/*==============================================================================

DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_wdt_ds_group Data structures
 *  Lists all the data structures used by the Watchdog module.
 *  @{
 */

/*!
 * @brief WWDT config parameters incase of WDT type is WWDT
 */
typedef struct
{
    uint32_t lower_window_timeout_value_in_ms;  /*!< Window wathchdog lower timeout value in milliseconds. */
    uint32_t upper_window_timeout_value_in_ms;  /*!< Window wathchdog upper timeout value in milliseconds. */
} asdk_wdt_type_wwdt_config_t;

/*!
 * @brief WDT Configuration structure
 */

typedef struct
{
    uint8_t wdt_no;                                         /*!< Watchdog number(WDT module) */
    asdk_wdt_type_t wdt_type;                               /*!< Watchdog type: IWDT or WWDT */
    uint32_t wdt_timeout_value_in_ms;                       /*!< Watchdog timeout value in ms */
    asdk_wdt_type_wwdt_config_t wdt_wwdt_config;            /*!< Windowed Watchdog configuration parameters */
    asdk_wdt_timeout_action_t wdt_timeout_action;           /*!< Watchdog timeout action to be taken */
    uint32_t wdt_warn_timeout_value_in_ms;                  /*!< Watchdog warning timeout value */
    asdk_wdt_warn_timeout_action_t wdt_warn_timeout_action; /*!< Watchdog warn action to taken */
    /*To do callback for warning interrupt*/
    bool enable_wdt_in_debug; /*!< Enable or disable WDT in DEBUG mode */
    bool enable_wdt;          /*!< WDT enable/disable */
} asdk_wdt_config_t;

/** @} */ // end of asdk_wdt_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_wdt_cb_group Callback function type
 *  Lists the callback functions from the Watchdog module.
 *  @{
 */

/*!
 * @brief The CAN module's callback function type. For the supported CAN events
   refer @ref asdk_can_install_callback function.

   @param wdt_no Watchdog number
 */
typedef void (*asdk_wdt_callback_fun_t)(uint8_t wdt_no);

/** @} */ // end of asdk_wdt_cb_group

/*==============================================================================

    FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_wdt_fun_group Functions
 *  Lists the functions/APIs from the Watchdog module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_watchdog_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the WDT Module based on the config structure passed
  as parameter.

  @param [in] wdt_config_data ASDK WDT configuration

  @return
    - @ref ASDK_WDT_STATUS_SUCCESS
    - @ref ASDK_WDT_STATUS_ERROR
    - @ref ASDK_WDT_ERROR_NOT_INITIALIZED
    - @ref ASDK_WDT_ERROR_INVALID_HANDLER
    - @ref ASDK_WDT_ERROR_INVALID_WDT_NO

  @note Please use the wdt_no as 0 for the syswdt to be enabled.
        Other wdt_no is not supported for this cyt version

*/
asdk_errorcode_t asdk_watchdog_init(asdk_wdt_config_t *wdt_config_data);

/*----------------------------------------------------------------------------*/
/* Function : asdk_wdt_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the WDT Module based on the config structure
  passed as parameter.

  @param [in] wdt_no Watchdog number

  @return
    - @ref ASDK_WDT_STATUS_SUCCESS
    - @ref ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED
*/
asdk_errorcode_t asdk_watchdog_deinit(uint8_t wdt_no);

/*----------------------------------------------------------------------------*/
/* Function : asdk_watchdog_refresh */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function clears ("feeds") the watchdog, to prevent a reset.

  @param [in] wdt_no Watchdog number

  @return
    - @ref ASDK_WDT_STATUS_SUCCESS
    - @ref ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED
*/
asdk_errorcode_t asdk_watchdog_refresh(uint8_t wdt_no);

/*----------------------------------------------------------------------------*/
/* Function : asdk_watchdog_get_time_in_ms */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the current watchdog time value in ms

  @param [in] wdt_no Watchdog number
  @param [out] wdt_time_value_in_ms Current time value in ms to be returned
  to the user.

  @return
    - @ref ASDK_WDT_STATUS_SUCCESS
    - @ref ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED
*/
asdk_errorcode_t asdk_watchdog_get_time_in_ms(uint8_t wdt_no, uint32_t *wdt_time_value_in_ms);

/*----------------------------------------------------------------------------*/
/* Function : asdk_watchdog_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function installs/registers the callback function for the user callback.

  @param [in] wdt_no Watchdog number
  @param [in] callback_fun Callback function to the user

  @return
    - @ref ASDK_WDT_STATUS_SUCCESS
    - @ref ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED
*/
asdk_errorcode_t asdk_watchdog_install_callback(uint8_t wdt_no, asdk_wdt_callback_fun_t callback_fun);

/** @} */ // end of asdk_wdt_fun_group

#endif /* ASDK_WDT_H_ */
