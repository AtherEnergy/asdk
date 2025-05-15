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
    asdk_pwm.h

    @path
    inc/pwm.h

    @Created on
    Jan 5, 2024

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the PWM DAL module of ASDK.
*/

#ifndef ASDK_PWM_H
#define ASDK_PWM_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>
#include <stdbool.h>

/* asdk includes ***************************** */

#include "asdk_error.h" // Defines ASDK PWM module error codes

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_platform.h"

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define MAX_DUTY_CYCLE 100U
#define MIN_DUTY_CYCLE 0U

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_pwm_enum_group Enumerations
 *  Lists all the enumerations used by the PWM module.
 *  @{
 */

/*!
 * @brief An enumerator to set the Run Mode for the PWM.
 *
 * Implements : asdk_pwm_run_mode_t
 */
typedef enum
{
  ASDK_PWM_RUN_MODE_ONE_SHOT = 0, /*!< The PWM runs once on a trigger event */
  ASDK_PWM_RUN_MODE_CONTINUOUS,   /*!< The PWM runs forever on a trigger event */
  ASDK_PWM_RUN_MODE_MAX,          /*!< Max number of Run Mode for PWM*/
  ASDK_PWM_RUN_MODE_INVALID = ASDK_PWM_RUN_MODE_MAX,
} asdk_pwm_run_mode_t;

/*!
 * @brief An enumerator to set the pwm count direction.
 *
 * Implements : asdk_pwm_count_direction_t
 */
typedef enum
{
  ASDK_PWM_COUNT_DIRECTION_UP = 0, /*!< Count up */
  ASDK_PWM_COUNT_DIRECTION_DOWN,   /*!< Count down */
  ASDK_PWM_COUNT_DIRECTION_MAX,    /*!< Max count direction type */
  ASDK_PWM_COUNT_DIRECTION_INVALID = ASDK_PWM_COUNT_DIRECTION_MAX,
} asdk_pwm_count_direction_t;

/*!
 * @brief An enumerator to set the output alignment of the  PWM.
 *
 * Implements : asdk_pwm_output_align_t
 */
typedef enum
{
  ASDK_PWM_OUTPUT_RIGHT_ALIGN = 0x00, /*!< The output is right aligned. */
  ASDK_PWM_OUTPUT_LEFT_ALIGN,         /*!< The output is left aligned. */
  ASDK_PWM_OUTPUT_CENTRE_ALIGN,       /*!< The output is centre aligned. */
  ASDK_PWM_OUTPUT_ALIGN_MAX,
  ASDK_PWM_OUTPUT_ALIGN_INVALID = ASDK_PWM_OUTPUT_ALIGN_MAX,
} asdk_pwm_output_align_t;

/*!
 * @brief An enumerator to set the pwm prescaler.
 *
 * Implements : asdk_prescaler_config_t
 */
typedef enum
{
  ASDK_PWM_PRESCALER_DIVBY_1 = 0u,   /**< Divide by 1 */
  ASDK_PWM_PRESCALER_DIVBY_2 = 1u,   /**< Divide by 2 */
  ASDK_PWM_PRESCALER_DIVBY_4 = 2u,   /**< Divide by 4 */
  ASDK_PWM_PRESCALER_DIVBY_8 = 3u,   /**< Divide by 8 */
  ASDK_PWM_PRESCALER_DIVBY_16 = 4u,  /**< Divide by 16 */
  ASDK_PWM_PRESCALER_DIVBY_32 = 5u,  /**< Divide by 32 */
  ASDK_PWM_PRESCALER_DIVBY_64 = 6u,  /**< Divide by 64 */
  ASDK_PWM_PRESCALER_DIVBY_128 = 7u, /**< Divide by 128 */
  ASDK_PWM_PRESCALER_DIVBY_MAX,
  ASDK_PWM_PRESCALER_DIVBY_INVALID = ASDK_PWM_PRESCALER_DIVBY_MAX,
} asdk_prescaler_config_t;

/*!
 * @brief An enum to set the PWM modes..
 *
 * Implements : asdk_pwm_modes_t
 */
typedef enum
{
  ASDK_PWM_MODE_PWM = 0,  /*!< Standard PWM Mode*/
  ASDK_PWM_MODE_DEADTIME, /*!< PWM with deadtime mode*/
  ASDK_PWM_MODE_MAX,
  ASDK_PWM_MODE_INVALID = ASDK_PWM_MODE_MAX,
} asdk_pwm_modes_t;

/*!
 * @brief An enumerator to set the PWM  event.
 *
 * Implements : asdk_pwm_event_t
 */
typedef enum
{
  ASDK_PWM_TERMINAL_COUNT_EVENT = 0x01, /*!< Counter reached terminal count value. */
  ASDK_PWM_MATCH_EVENT = 0x02,          /*!< Counter value matched with compare value. */
} asdk_pwm_event_t;

/*!
 * @brief An enumerator to set the PWM  interrupt source.
 *
 * Implements : asdk_pwm_interrupt_src
 */
typedef enum
{
  ASDK_PWM_INTERRUPT_TERMINAL_COUNT = 0x01, /*!< Counter reached terminal count value. */
  ASDK_PWM_INTERRUPT_MATCH = 0x02,          /*!< Counter value matched with compare value. */
} asdk_pwm_interrupt_src;

/** @} */ // end of asdk_pwm_enum_group

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_pwm_cb_group Callback function type
 *  Lists the callback functions from the PWM module.
 *  @{
 */

/*!
 * @brief Pwm mode callback function type.  The following events are 
 * expected during the callback.
 * * @ref ASDK_PWM_TERMINAL_COUNT_EVENT and
 * * @ref ASDK_PWM_MATCH_EVENT
 *
 * @param [in] pwm_event PWM event which trigger the callback function.
 * @param [in] count_value counter value at the time of event trigger.
 */
typedef void (*asdk_pwm_callback_t)(asdk_pwm_event_t pwm_event, uint32_t count_value);

/** @} */ // end of asdk_pwm_cb_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_pwm_ds_group Data structures
 *  Lists all the data structures used by the PWM module.
 *  @{
 */

/*!
 * @brief An data structure to represent PWM interrupt.
 *
 * Implements : asdk_pwm_interrupt_t
 */
typedef struct
{
  bool enable;                          /*!< Enable interrupt */
  uint8_t priority;                     /*!< Interrupt priority */
  asdk_pwm_interrupt_src interrupt_src; /*!<Interrupt Source*/
} asdk_pwm_interrupt_t;

/*!
 * @brief An data structure to represent pwm clock configuration.
 *
 * Implements : asdk_pwm_clock_t
 */

typedef struct
{
  uint32_t clock_frequency;          /*!< PWM IP Block frequency */
  asdk_prescaler_config_t prescaler; /*!< PWM pre-scaler */
} asdk_pwm_clock_t;

/*!
 * @brief An data structure to represent pwm clock configuration.
 *
 * Implements : asdk_deadtime_config_t
 * 
 * @note For the PWM channels between (ASDK_PWM_MODULE_CH_0 to ASDK_PWM_MODULE_CH_62) & (ASDK_PWM_MODULE_CH_75 & ASDK_PWM_MODULE_CH_78) deadtime 
 *       is same for both sides as the left side deadtime_clocks. For the PWM channels between (ASDK_PWM_MODULE_CH_63 & ASDK_PWM_MODULE_CH_74)
 *       deadtime can be different for both sides as given in the configuration.
 */
typedef struct
{
  asdk_mcu_pin_t mcu_pin_compl;        /*!< This PIN will be used for the Complementary PWM*/
  uint16_t right_side_deadtime_clocks; /*!< Right side dead time */
  uint16_t left_side_deadtime_clocks;  /*!< Left side dead time*/
} asdk_deadtime_config_t;

/*!
 * @brief An data structure to represent the PWM configuration.
 *
 * Implements : asdk_pwm_config_t
 */
typedef struct
{
  asdk_pwm_modes_t pwm_mode;                /*!<PWM Mode*/
  asdk_deadtime_config_t deadtime_config;   /*!<The number of dead time-clocks in PWM in PWM_DT mode*/
  asdk_pwm_run_mode_t pwm_run_mode;         /*!<Pwm mode */
  asdk_pwm_count_direction_t direction;     /*!<Count direction */
  asdk_pwm_interrupt_t interrupt;           /*!<Interrupt configuration */
  asdk_pwm_clock_t pwm_clock;               /*!<Pwm clock configuration */
  uint32_t pwm_frequency_in_Hz;             /*!<Period register count value */
  uint8_t pwm_duty_cycle_in_percent;        /*!<PWM compare value for duty cycle*/
  asdk_mcu_pin_t mcu_pin;                   /*!<MCU pin for PWM MOde */
  asdk_pwm_output_align_t pwm_output_align; /*!<Align the PWM output*/

  /*PWM callback function asigned if configured for the interrupt mode*/
  asdk_pwm_callback_t callback; /*!<PWM mode callback */

} asdk_pwm_config_t;

/** @} */ // end of asdk_pwm_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_pwm_fun_group Functions
 *  Lists the functions/APIs from the PWM module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the given pwm channel with given pwm configuration.

  @param [in] pwm_ch PWM channel number.
  @param [in] pwm_config PWM configuration.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
    - @ref ASDK_PWM_ERROR_INVALID_COUNT_DIRECTION
    - @ref ASDK_PWM_ERROR_INVALID_PRESCALER
    - @ref ASDK_PWM_ERROR_INVALID_MODE
    - @ref ASDK_PWM_ERROR_PINMUX
    - @ref ASDK_PWM_ERROR_INIT_FAILED
*/
asdk_errorcode_t asdk_pwm_init(asdk_pwm_channel_t pwm_ch, asdk_pwm_config_t *pwm_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function deinitializes the given pwm channel

  @param [in] pwm_ch PWM channel number.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_deinit(asdk_pwm_channel_t pwm_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_start */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function starts the given pwm channel.

  @param [in] pwm_ch PWM channel number.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_start(asdk_pwm_channel_t pwm_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_stop */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function stops/disables the given pwm channel.

  @param [in] pwm_ch - PWM channel number.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_stop(asdk_pwm_channel_t pwm_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_get_counter */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the PWM current counter value.

  @param [in] pwm_ch PWM channel number.
  @param [out] counter Count value.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_get_counter(asdk_pwm_channel_t pwm_ch, uint32_t *counter);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_set_output_alignment */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets the PWM output alignment.

  @param [in] pwm_ch PWM channel number.
  @param [in] pwm_output_align PWM output alignment.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_set_output_alignment(asdk_pwm_channel_t pwm_ch, asdk_pwm_output_align_t pwm_output_align);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_set_duty */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the duty cycle of the PWM.

  @param [in] pwm_ch PWM channel number.
  @param [in] duty_cycle_in_percent Duty cycle in percentage (1-100).

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_pwm_set_duty(asdk_pwm_channel_t pwm_ch, uint8_t duty_cycle_in_percent);

/*----------------------------------------------------------------------------*/
/* Function : asdk_pwm_set_frequency */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the desired PWM frequency.

  @param [in] pwm_ch PWM channel number.
  @param [in] pwm_clock PWM clock settings.
  @param [in] pwm_freq_in_Hz PWM frequency in Hz.

  @return
    - @ref ASDK_PWM_SUCCESS
    - @ref ASDK_PWM_ERROR_INVALID_CHANNEL
    - @ref ASDK_PWM_INVALID_FREQUENCY
*/
asdk_errorcode_t asdk_pwm_set_frequency(asdk_pwm_channel_t pwm_ch, asdk_pwm_clock_t pwm_clock, uint32_t pwm_freq_in_Hz);

/** @} */ // end of asdk_pwm_fun_group

#endif /* ASDK_PWM_H */
