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
    asdk_pinmux.h

    @path
    inc/asdk_pinmux.h

    @Created on
    July 25, 2023

    @Author
    Ather Energy Pvt. Ltd.

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd. All rights reserved.

    @brief
    This file prototypes the PINMUX DAL module of ASDK.
    @note
    Peripherals with submodules and their channel number are given in the last
    argument as Channel Number and not as Module like in SCB blocks For ex:
*/

#ifndef ASDK_PINMUX_H
#define ASDK_PINMUX_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include "asdk_error.h"
#include "asdk_gpio.h"
#include "asdk_mcu_pins.h"
#include <stdbool.h>
#include <stdint.h>

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define ASDK_PINMUX_ALTFUN(Alt_Fn, Alt_Subfn, Peri_Module_no)                  \
  ((uint32_t)(((Alt_Fn << 24ul) | (Alt_Subfn << 16ul) |                        \
               (Peri_Module_no << 8ul)) &                                      \
              (0xFFFFFF00ul)))

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
/*!
 * @brief PinMux Alternate Functions
 *
 * Implements : asdk_pinmux_modules_t
 */
typedef enum {
  ASDK_PINMUX_FUN_USART = 1,
  ASDK_PINMUX_FUN_SPI = 2,
  ASDK_PINMUX_FUN_I2C = 3,
  ASDK_PINMUX_FUN_CAN = 4,
  ASDK_PINMUX_FUN_TIM = 5,
} asdk_pinmux_modules_t;

/*!
 * @brief PinMux Alternate SubFunction
 *
 * Implements : asdk_pinmux_subfunction_t
 */
typedef enum {
  ASDK_PINMUX_USART_SUBFUN_RX = 1,
  ASDK_PINMUX_USART_SUBFUN_TX,
  ASDK_PINMUX_USART_SUBFUN_RTS,
  ASDK_PINMUX_USART_SUBFUN_CTS,
  ASDK_PINMUX_USART_SUBFUN_CLK,

  ASDK_PINMUX_SPI_SUBFUN_MOSI,
  ASDK_PINMUX_SPI_SUBFUN_MISO,
  ASDK_PINMUX_SPI_SUBFUN_CLK,
  ASDK_PINMUX_SPI_SUBFUN_CS,

  ASDK_PINMUX_I2C_SUBFUN_SDA,
  ASDK_PINMUX_I2C_SUBFUN_SCL,

  ASDK_PINMUX_TIM1,
  ASDK_PINMUX_TIM2,
  ASDK_PINMUX_TIM3,
  ASDK_PINMUX_TIM4,
  ASDK_PINMUX_TIM5,
  ASDK_PINMUX_TIM8,
  ASDK_PINMUX_TIM9,
  ASDK_PINMUX_TIM10,
  ASDK_PINMUX_TIM11,
  ASDK_PINMUX_TIM12,
  ASDK_PINMUX_TIM13,
  ASDK_PINMUX_TIM14,

  ASDK_PINMUX_CAN_SUBFUN_RX,
  ASDK_PINMUX_CAN_SUBFUN_TX,
} asdk_pinmux_subfunction_t;

/*! GPIO pull-type configuration options. */
typedef enum {
  ASDK_GPIO_PULL_TYPE_ANALOG = 0x00ul,
  ASDK_GPIO_PULL_TYPE_HIGHZ = 0x01ul,
  ASDK_GPIO_PULL_TYPE_PULLUP = 0x02ul,
  ASDK_GPIO_PULL_TYPE_PULLDOWN = 0x03ul,
  ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW = 0x04ul,
  ASDK_GPIO_PULL_TYPE_OD_DRIVESHIGH = 0x05ul,
  ASDK_GPIO_PULL_TYPE_STRONG = 0x06ul,
  ASDK_GPIO_PULL_TYPE_PULLUP_DOWN = 0x07ul,
  ASDK_GPIO_PULL_TYPE_NO_PULL = 0x08ul,
  /*!< Indicates total number of available pull options. */
  ASDK_GPIO_PULL_TYPE_MAXIMUM,
  /*!< Values beyond ASDK_GPIO_PULL_TYPE_MAX are undefined. */
  ASDK_GPIO_PULL_TYPE_ALT_NOT_DEFINED = ASDK_GPIO_PULL_TYPE_MAXIMUM
} asdk_gpio_pull_altfn_t;

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*!
 * @brief Configuration for PinMux DAL
 *
 * @note Please ensure for group PinMux same Alternate Function is selected
 *
 * Implements : asdk_pinmux_config_t
 */
typedef struct {

  /*Alternate function enum*/
  uint32_t alternate_fun_id;
  /* Essential Input */
  asdk_mcu_pin_t MCU_pin_num; /*!< Array pointer to the list of ADC pins to be
                                 initialized with given configurations. */

  /*User will pass the pull configuration for the alternate function they want*/
  asdk_gpio_pull_altfn_t pull_configuration;

  /* User will pass the slew rate for the alternate function */
  asdk_gpio_speed_t slew_rate;

} asdk_pinmux_config_t;

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/* ************************************************************************** *
 *                          Configuration APIs                                *
 * ************************************************************************** */

/*! This function sets the pinmux alternate function for the GPIO pins */
asdk_errorcode_t asdk_set_pinmux(asdk_pinmux_config_t pinmux_config[],
                                 uint16_t pin_count);

// To do:
// API for getting the pinmux cofig incase of reintializing the pinmux with
// another alternate functionality.
// Relevance of this function is to be decided later
//  asdk_errorcode_t asdk_get_pinmux(asdk_mcu_pin_t mcu_pin_no, uint8_t
//  *alternate_fun);

/*! This function sets with the default pinmux configuration for the peripherals
 */
asdk_errorcode_t asdk_set_default_pinmux(asdk_pinmux_config_t pinmux_config);

/*! This function de-initializes the previously configured PinMux */
asdk_errorcode_t asdk_deinit_pinmux(asdk_mcu_pin_t pin);

#endif /* ASDK_PINMUX_H */
