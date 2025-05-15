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
  asdk_gpio.h

  @path
  inc/asdk_gpio.h

  @Created on
  Jun 26, 2023

  @Author
  Ather Energy Limited

  @Copyright
  Copyright (c) Ather Energy Limited. All rights reserved.

  @brief
  This file prototypes the GPIO DAL module of ASDK
*/

#ifndef ASDK_GPIO_H
#define ASDK_GPIO_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>

/* asdk includes ***************************** */

#include "asdk_error.h" // Defines ASDK GPIO module error codes

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_gpio_enum_group Enumerations
 *  Lists all the enumerations used by the GPIO module.
 *  @{
 */

/*! GPIO mode configuration options. */
typedef enum
{
    ASDK_GPIO_MODE_INPUT = 0, /*!< Use GPIO as input. */
    ASDK_GPIO_MODE_OUTPUT,    /*!< Use GPIO as output. */

    ASDK_GPIO_MODE_TYPE_MAX,                                  /*!< Indicates total number of available GPIO modes. */
    ASDK_GPIO_MODE_TYPE_NOT_DEFINED = ASDK_GPIO_MODE_TYPE_MAX /*!< Values beyond ASDK_GPIO_MODE_TYPE_MAX are undefined. */
} asdk_gpio_mode_t;

/*! GPIO pull-type configuration options. */
typedef enum
{
    ASDK_GPIO_HIGH_Z = 0,             /*!< High impedance, normally most of the pins of MCU are in this state. Refere the datasheet for more information. */
    ASDK_GPIO_PUSH_PULL,              /*!< When GPIO should function as output. */
    ASDK_GPIO_PULL_UP,                /*!< When GPIO should function as input with internal pull-up. */
    ASDK_GPIO_PULL_DOWN,              /*!< When GPIO should function as input with internal pull-down. */

    ASDK_GPIO_PULL_TYPE_MAX,                                  /*!< Indicates total number of available pull options. */
    ASDK_GPIO_PULL_TYPE_NOT_DEFINED = ASDK_GPIO_PULL_TYPE_MAX /*!< Values beyond ASDK_GPIO_PULL_TYPE_MAX are undefined. */
} asdk_gpio_pull_t;

/*! GPIO slew-rate control configuration options. Generally, low-frequency option is used by most applications
    but if you are expecting the GPIO pin to respond as qucik as it can then use the high-frequency option.
 */
typedef enum
{
    ASDK_GPIO_SPEED_HIGH_FREQ = 0, /*!< Switch speed GPIO at high-frequency. */
    ASDK_GPIO_SPEED_LOW_FREQ,      /*!< Switch speed GPIO at low-frequency. */

    ASDK_GPIO_SPEED_TYPE_MAX,                                   /*!< Indicates the total number of available speed options. */
    ASDK_GPIO_SPEED_TYPE_NOT_DEFINED = ASDK_GPIO_SPEED_TYPE_MAX /*!< Values beyond ASDK_GPIO_SPEED_TYPE_MAX aren't defined. */
} asdk_gpio_speed_t;

/*! GPIO state configuration options. */
typedef enum
{
    ASDK_GPIO_STATE_LOW = 0, /*!< The logical-low (0) state for the pin. */
    ASDK_GPIO_STATE_HIGH,    /*!< The logical-high (1) state for the pin. */
    ASDK_GPIO_STATE_INVALID, /*!< Invalid state should never be used to configure the GPIO but is useful when reading the pin state. */

    ASDK_GPIO_STATE_MAX,                                   /*!< Indicates the total number of states. */
    ASDK_GPIO_STATE_TYPE_NOT_DEFINED = ASDK_GPIO_STATE_MAX /*!< Values beyond ASDK_GPIO_STATE_MAX are undefined. */
} asdk_gpio_state_t;

/*! GPIO interrupt configuration options. */
typedef enum
{
    ASDK_GPIO_INTERRUPT_DISABLED = 0, /*!< Disables the interrupt of the given pin. */
    ASDK_GPIO_INTERRUPT_RISING_EDGE,  /*!< Generate interrupt at rising-edge of the given pin. */
    ASDK_GPIO_INTERRUPT_FALLING_EDGE, /*!< Generate interrupt at falling-edge of the given pin. */
    ASDK_GPIO_INTERRUPT_BOTH_EDGES,   /*!< Generate interrupt at both rising-edge & falling-edge of the given pin. */

    ASDK_GPIO_INTERRUPT_MAX,                                       /*!< Indicates the total number of available interrupt options. */
    ASDK_GPIO_INTERRUPT_TYPE_NOT_DEFINED = ASDK_GPIO_INTERRUPT_MAX /*!< Values beyond ASDK_GPIO_INTERRUPT_MAX are undefined. */
} asdk_gpio_interrupt_type_t;

/** @} */ // end of asdk_gpio_enum_group

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_gpio_cb_group Callback function type
 *  Lists the callback functions from the GPIO module.
 *  @{
 */

/*! @brief The user callback function type.

  The callback is module level callback and passes following parameters
  with the callback which helps the user identify the source of GPIO
  interrupt. Refer @ref asdk_gpio_input_callback_t to install the
  callback function.

  @param gpio_pin Indicates the pin on which GPIO interrupt was generated.
  @param pin_state Indicates the current-state of the GPIO pin that
  caused the interrupt.
*/
typedef void (*asdk_gpio_input_callback_t)(asdk_mcu_pin_t mcu_pin, uint32_t pin_state);

/** @} */ // end of asdk_gpio_cb_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_gpio_ds_group Data structures
 *  Lists all the data structures used by the GPIO module.
 *  @{
 */

typedef struct {
    asdk_gpio_interrupt_type_t type; /*!< GPIO interrupt type. */
    uint8_t priority;                /*!< Priority of the GPIO interrupt. */
} asdk_gpio_interrupt_t;

/*!
 * @brief Configuration for GPIO (General Purpose Input Output) module initialization.
 */
typedef struct
{
    asdk_mcu_pin_t mcu_pin; /*!< The actual MCU-pin */

    /* Common configuration */
    asdk_gpio_mode_t gpio_mode;   /*!< GPIO mode as input/output */
    asdk_gpio_speed_t gpio_speed; /*!< The required slew-rate of the GPIO pin */

    /* GPIO output */
    asdk_gpio_state_t gpio_init_state; /*!< The required initial state of the output GPIO pin */

    /* GPIO input */
    asdk_gpio_pull_t gpio_pull;             /*!< The required pull configuration of the input GPIO pin */
    asdk_gpio_interrupt_t interrupt_config; /*!< The required interrupt configuration of the input GPIO pin */
} asdk_gpio_config_t;

/** @} */ // end of asdk_gpio_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_gpio_fun_group Functions
 *  Lists the functions/APIs from the GPIO module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the given MCU pin either as General Purpose Output
  or as General Purpose Input. Use @ref asdk_gpio_mode_t to confgure the
  direction of the pin.

  To enable GPIO interrupt on a given input pin. First, configure "interrupt_config"
  with @ref asdk_gpio_interrupt_type_t in the configuration parameter
  before calling this API. Then, define the GPIO callback function by referring
  @ref asdk_gpio_input_callback_t and install it with
  @ref asdk_gpio_install_callback API after initializing the pin.

  When the pin is configured as output. You may set the initial state on the
  output pin by setting the "gpio_init_state" parameter in the configuration
  to one of the valid output state. Refer @ref asdk_gpio_state_t for valid
  pin states.

  @note
  By default, all GPIO interrupts are disabled.

  @param [in] gpio_cfg GPIO configuration.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_NULL_PTR
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_MODE
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_STATE
    - @ref ASDK_GPIO_ERROR_INIT
*/
asdk_errorcode_t asdk_gpio_init(asdk_gpio_config_t *gpio_cfg);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the pin from acting as GPIO. It clears out the
  GPIO configuration settings. After deinitializing the pin to re-use it as
  GPIO again the user must re-initialize with @ref asdk_gpio_init function.

  When the pin is configured as input deinitializing the pin only disables the
  interrupt request if enabled for the given pin. However, the callback function
  remains assigned but the callback will not happen for all the deintialized
  pins.

  @param [in] pin_num Pin number of the MCU.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_deinit(asdk_mcu_pin_t gpio_pin);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function assigns user callback function to the GPIO module. Refer @ref
  asdk_gpio_input_callback_t function type to define your callback function. 

  The callback is made whenver the GPIO interrupt occurs and is made in the
  context of the GPIO interrupt handler. The interrupt must be enabled, refer
  @ref asdk_gpio_init function to enable interrupt.

  @note
  The callback is implemented as module level callback.

  @param [in] callback User callback function.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_NULL_PTR
*/
asdk_errorcode_t asdk_gpio_install_callback(asdk_gpio_input_callback_t callback);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_output_set */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the output pin state the given MCU pin to a logical high state
  @ref ASDK_GPIO_STATE_HIGH.

  @param [in] pin_num Pin number of the MCU.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_output_set(asdk_mcu_pin_t gpio_pin);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_output_clear */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function clears the output pin state the given MCU pin to a logical low state
  @ref ASDK_GPIO_STATE_LOW.

  @param [in] pin_num Pin number of the MCU.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_output_clear(asdk_mcu_pin_t gpio_pin);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_output_toggle */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function toggles current output state of the given MCU pin. If the current
  state is high it will change to low, similarly if the pin state is low it
  will change to high.

  @param [in] pin_num Pin number of the MCU.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_output_toggle(asdk_mcu_pin_t gpio_pin);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_get_output_state */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the current output state of the given MCU pin.

  @param [in] gpio_pin Pin number of the MCU.
  @param [out] gpio_state The current state of the GPIO.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_get_output_state(asdk_mcu_pin_t gpio_pin, asdk_gpio_state_t *gpio_state);

/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_get_input_state */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the current input state of the given MCU pin.

  @param [in] gpio_pin Pin number of the MCU.
  @param [out] gpio_state The current state of the GPIO.

  @return
    - @ref ASDK_GPIO_SUCCESS
    - @ref ASDK_GPIO_ERROR_INVALID_MCU_PIN
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PORT
    - @ref ASDK_GPIO_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_gpio_get_input_state(asdk_mcu_pin_t gpio_pin, asdk_gpio_state_t *gpio_state);

/** @} */ // end of asdk_gpio_fun_group

#endif /* ASDK_GPIO_H */
