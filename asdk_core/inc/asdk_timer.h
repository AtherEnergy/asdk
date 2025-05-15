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
    asdk_timer.h

    @path
    inc/asdk_timer.h

    @Created on
    Sep 22, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the TIMER DAL module of ASDK.
*/

#ifndef ASDK_TIMER_H
#define ASDK_TIMER_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>
#include <stdbool.h>

/* asdk includes ***************************** */

#include "asdk_error.h" // Defines ASDK Timer module error codes

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_platform.h" // Defines instances of timers

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_timer_enum_group Enumerations
 *  Lists all the enumerations used by the CAN module.
 *  @{
 */

/*!
 * @brief An enumerator to represent Timer types.
 */
typedef enum
{
    ASDK_TIMER_TYPE_ONE_SHOT = 0, /*!< One shot timer. Timer stops after it reaches terminal count. */
    ASDK_TIMER_TYPE_PERIODIC,     /*!< Periodic counter (Continous). Timer restarts on reaching terminal count. */

    ASDK_TIMER_TYPE_MAX /*!< Total number of timer types  */
} asdk_timer_type_t;

/*!
 * @brief An enumerator to timer count direction.
 */
typedef enum
{
    ASDK_TIMER_COUNT_DIRECTION_UP = 0, /*!< Count up. The counter starts with 0 and increments by 1. */
    ASDK_TIMER_COUNT_DIRECTION_DOWN,   /*!< Count down. The counter starts counting with MAX value and decrements by 1. */

    ASDK_TIMER_COUNT_DIRECTION_MAX /*!< Max count direction type */
} asdk_timer_count_direction_t;

/*!
 * @brief An enumerator to represent timer modes.
 */
typedef enum
{
    ASDK_TIMER_MODE_TIMER = 0, /*!< Timer mode. General purpose timer. */
    ASDK_TIMER_MODE_COMPARE,   /*!< Compare mode. */
    ASDK_TIMER_MODE_CAPTURE,   /*!< Capture mode. */

    ASDK_TIMER_MODE_MAX /*!< Total number of timer modes */
} asdk_timer_mode_type_t;

/*!
 * @brief An enumerator to represent timer capture edge.
 */
typedef enum
{
    ASDK_TIMER_CAPTURE_ON_RISING_EDGE = 0, /*!< Capture on rising edge */
    ASDK_TIMER_CAPTURE_ON_FALLING_EDGE,    /*!< Capture on falling edge */
    ASDK_TIMER_CAPTURE_ON_BOTH_EDGES,      /*!< Capture on both edges */
    ASDK_TIMER_CAPTURE_ON_NONE,            /*!< No capture */
} asdk_timer_capture_edge_t;

/*!
 * @brief An enumerator to represent timer events.
 */
typedef enum
{
    ASDK_TIMER_TERMINAL_COUNT_EVENT = 0x01, /*!< Counter reached terminal count value. */
    ASDK_TIMER_MATCH_EVENT = 0x02,          /*!< Counter value matched with compare value. */
    ASDK_TIMER_CAPTURE_EVENT = 0x04,        /*!< Counter value was captured in capture register. */
} asdk_timer_event_t;

/*!
 * @brief An enumerator to represent list of prescaler values.
 */
typedef enum
{
    ASDK_CLOCK_PRESCALER_1 = 1u,   /*!< Divide counter frequency by 1. */
    ASDK_CLOCK_PRESCALER_2 = 2u,   /*!< Divide counter frequency by 2. */
    ASDK_CLOCK_PRESCALER_4 = 3u,   /*!< Divide counter frequency by 4. */
    ASDK_CLOCK_PRESCALER_8 = 4u,   /*!< Divide counter frequency by 8. */
    ASDK_CLOCK_PRESCALER_16 = 5u,  /*!< Divide counter frequency by 16. */
    ASDK_CLOCK_PRESCALER_32 = 6u,  /*!< Divide counter frequency by 32. */
    ASDK_CLOCK_PRESCALER_64 = 7u,  /*!< Divide counter frequency by 64. */
    ASDK_CLOCK_PRESCALER_128 = 8u, /*!< Divide counter frequency by 128. */
    ASDK_CLOCK_PRESCALER_MAX,      /*!< Total number of pre-scalers. */
    ASDK_CLOCK_PRESCALER_UNDEFINED = ASDK_CLOCK_PRESCALER_MAX,
} asdk_clock_prescalers_t;

/** @} */ // end of asdk_timer_enum_group

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_timer_cb_group Callback function type
 *  Lists the callback functions from the Timer module.
 *  @{
 */

/*!
 * @brief Timer and Compare mode callback function type. Following timer
 * events are expected during the callback:
 * * @ref ASDK_TIMER_TERMINAL_COUNT_EVENT and
 * * @ref ASDK_TIMER_MATCH_EVENT
 */
typedef void (*asdk_timer_callback_t)(asdk_timer_event_t timer_event);

/*!
 * @brief Capture mode callback function type. Following timer events are
 * expected during the callback:
 * * @ref ASDK_TIMER_TERMINAL_COUNT_EVENT and
 * * @ref ASDK_TIMER_CAPTURE_EVENT
 */
typedef void (*asdk_capture_callback_t)(asdk_timer_event_t timer_event, asdk_timer_capture_edge_t edge, uint32_t captured_value);

/** @} */ // end of asdk_timer_cb_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_timer_ds_group Data structures
 *  Lists all the data structures used by the Timer module.
 *  @{
 */

/*!
 * @brief An data structure to represent timer interrupt.
 */
typedef struct
{
    bool enable;      /*!< Enable interrupt */
    uint8_t priority; /*!< Interrupt priority */
} asdk_timer_interrupt_t;

/*!
 * @brief An data structure to represent timer mode configuration.
 */
typedef struct
{
    uint32_t timer_period;          /*!< Period register count value */
    asdk_timer_callback_t callback; /*!< Timer mode callback */
} asdk_timer_mode_timer_config_t;

/*!
 * @brief An data structure to represent compare mode configuration.
 */
typedef struct
{
    uint32_t timer_period;          /*!< Period register count value */
    uint32_t compare_value;         /*!< Compare register count value */
    asdk_timer_callback_t callback; /*!< Compare mode callback */
} asdk_timer_mode_compare_config_t;

/*!
 * @brief An data structure to represent capture mode configuration.
 */
typedef struct
{
    uint32_t timer_period;            /*!< Period register count value */
    asdk_mcu_pin_t mcu_pin;           /*!< MCU pin for capture edge */
    asdk_timer_capture_edge_t edge;   /*!< Capture edge */
    asdk_capture_callback_t callback; /*!< Catpure mode callback */
} asdk_timer_mode_capture_config_t;

/*!
 * @brief A union to represent timer mode configurations.
 */
typedef union {
    asdk_timer_mode_timer_config_t timer;       /*!< Timer mode configuration. */
    asdk_timer_mode_compare_config_t compare;   /*!< Compare mode configuration. */
    asdk_timer_mode_capture_config_t capture;   /*!< Capture mode configuration. */
} asdk_timer_mode_config_t;

/*!
 * @brief An data structure to represent timer mode configuration.
 */
typedef struct
{
    asdk_timer_mode_type_t type; /*!< Timer mode type. */
    asdk_timer_mode_config_t config; /*!< Mode configuration. */
} asdk_timer_mode_t;

/*!
 * @brief An data structure to represent timer clock configuration.
 */
typedef struct
{
    uint32_t frequency; /*!< Timer count frequency */
    asdk_clock_prescalers_t prescaler;  /*!< Counter pre-scaler */
} asdk_timer_clock_t;

/*!
 * @brief An data structure to represent timer.
 */
typedef struct
{
    asdk_timer_type_t type;                 /*!< Timer type */
    asdk_timer_mode_t mode;                 /*!< Timer mode */
    asdk_timer_count_direction_t direction; /*!< Count direction */
    asdk_timer_interrupt_t interrupt;       /*!< Interrupt configuration */
    asdk_timer_clock_t counter_clock;       /*!< Timer clock configuration */
} asdk_timer_t;

/** @} */ // end of asdk_timer_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_timer_fun_group Functions
 *  Lists the functions/APIs from the Timer module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the given timer channel.

  The timer can be configured in 1 of the 3 modes. Refer @ref asdk_timer_mode_t
  to select and configure the desired mode. Each channel has designated callback
  function. Refer the corresponding mode configuration for reference. To enable
  interrupt refer @ref asdk_timer_interrupt_t and ensure that callback function
  is assigned.
  
  @param [in] timer_ch Timer channel number.
  @param [in] timer Timer configuration.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
    - @ref ASDK_TIMER_ERROR_INVALID_TYPE
    - @ref ASDK_TIMER_ERROR_INVALID_COUNT_DIRECTION
    - @ref ASDK_TIMER_ERROR_INVALID_PRESCALER
    - @ref ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ENABLED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ASSIGNED
    - @ref ASDK_TIMER_ERROR_INVALID_MODE
    - @ref ASDK_TIMER_ERROR_INVALID_PERIOD
    - @ref ASDK_TIMER_ERROR_INVALID_COMPARE_PERIOD
    - @ref ASDK_TIMER_ERROR_PINMUX
    - @ref ASDK_TIMER_INIT_FAILED
*/
asdk_errorcode_t asdk_timer_init(asdk_timer_channel_t timer_ch, asdk_timer_t *timer);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function deinitializes the given timer channel.

  Deinitializing a timer channel stops the timer and also clears the configuration 
  settings that were configured during init. Interrupt also gets disabled and the
  callback function gets unassigned.

  @param [in] timer_ch Timer channel number.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_timer_deinit(asdk_timer_channel_t timer_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_start */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function starts the given timer.

  @param [in] timer_ch Timer channel number.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_timer_start(asdk_timer_channel_t timer_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_stop */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function stops the given timer.

  @param [in] timer_ch Timer channel number.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_timer_stop(asdk_timer_channel_t timer_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_get_counter */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the counter value of the given timer.

  @param [in] timer_ch Timer channel number.
  @param [out] counter Count value.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_timer_get_counter(asdk_timer_channel_t timer_ch, uint32_t *counter);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_get_counter_in_us */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get counter value of the given timer in microseconds.

  @param [in] timer_ch Timer channel number.
  @param [in] counter_clock Counter clock frequency in hz.
  @param [out] counter_us Counter in microseconds.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_timer_get_counter_in_us(asdk_timer_channel_t timer_ch, asdk_timer_clock_t counter_clock, uint64_t *counter_us);

/*----------------------------------------------------------------------------*/
/* Function : asdk_timer_set_period */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set timer period value.

  @param [in] timer_ch Timer channel number.
  @param [out] period_count Period count.

  @return
    - @ref ASDK_TIMER_SUCCESS
    - @ref ASDK_TIMER_ERROR_INVALID_CHANNEL
    - @ref ASDK_TIMER_ERROR_INVALID_PERIOD
*/
asdk_errorcode_t asdk_timer_set_period(uint8_t timer_ch, uint32_t period_count);
/** @} */ // end of asdk_timer_fun_group

#endif /* ASDK_TIMER_H */
