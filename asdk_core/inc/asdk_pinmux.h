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
    asdk_pinmux.h

    @path
    inc/asdk_pinmux.h

    @Created on
    July 25, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the PINMUX DAL module of ASDK.
    @note
    Peripherals with submodules and their channel number are given in the last argument as Channel Number 
    and not as Module like in SCB blocks
    For ex: 
*/

#ifndef ASDK_PINMUX_H
#define ASDK_PINMUX_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_mcu_pins.h"
#include "asdk_error.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define ASDK_PINMUX_ALTFUN(Alt_Fn, Alt_Subfn, Peri_Module_no) ((uint32_t)(((Alt_Fn << 24ul) | (Alt_Subfn << 16ul) | (Peri_Module_no << 8ul)) & (0xFFFFFF00ul)))

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
/*!
 * @brief PinMux Alternate Functions
 *
 * Implements : asdk_pinmux_modules_t
 */
typedef enum
{
    ASDK_PINMUX_FUN_UART = 1,
    ASDK_PINMUX_FUN_SPI = 2,
    ASDK_PINMUX_FUN_I2C = 3,
    ASDK_PINMUX_FUN_PWM = 4,
    ASDK_PINMUX_FUN_ADC = 5,
    ASDK_PINMUX_FUN_LIN = 6,
    ASDK_PINMUX_FUN_CAN = 7,
    ASDK_PINMUX_FUN_TCPWM = 8,
    ASDK_PINMUX_FUN_TCPWM_TRIG = 10,

} asdk_pinmux_modules_t;
/*!
 * @brief PinMux Alternate SubFunction
 *
 * Implements : asdk_pinmux_subfunction_t
 */

typedef enum
{
    ASDK_PINMUX_UART_SUBFUN_RX = 1,
    ASDK_PINMUX_UART_SUBFUN_TX,
    ASDK_PINMUX_UART_SUBFUN_RTS,
    ASDK_PINMUX_UART_SUBFUN_CTS,
    ASDK_PINMUX_SPI_SUBFUN_MOSI,
    ASDK_PINMUX_SPI_SUBFUN_MISO,
    ASDK_PINMUX_SPI_SUBFUN_CLK,
    ASDK_PINMUX_SPI_SUBFUN_CS0,
    ASDK_PINMUX_SPI_SUBFUN_CS1,
    ASDK_PINMUX_SPI_SUBFUN_CS2,
    ASDK_PINMUX_SPI_SUBFUN_CS3,
    ASDK_PINMUX_I2C_SUBFUN_SDA,
    ASDK_PINMUX_I2C_SUBFUN_SCL,
    ASDK_PINMUX_TCPWM_SUBFUN_MODULE0,
    ASDK_PINMUX_TCPWM_SUBFUN_MODULE1,
    ASDK_PINMUX_TCPWM_SUBFUN_MODULE2,
    ASDK_PINMUX_LIN_SUBFUN_RX,
    ASDK_PINMUX_LIN_SUBFUN_TX,
    ASDK_PINMUX_LIN_SUBFUN_EN,
    ASDK_PINMUX_CAN_SUBFUN_RX,
    ASDK_PINMUX_CAN_SUBFUN_TX,
    ASDK_PINMUX_ADC_SUBFUN_MODULE0,
    ASDK_PINMUX_ADC_SUBFUN_MODULE1,
    ASDK_PINMUX_ADC_SUBFUN_MODULE2,
    ASDK_PINMUX_TCPWM0_CHX_SUBFUN_TRIG0,
    ASDK_PINMUX_TCPWM0_CHX_SUBFUN_TRIG1,
    ASDK_PINMUX_TCPWM1_CHX_SUBFUN_TRIG0,
    ASDK_PINMUX_TCPWM1_CHX_SUBFUN_TRIG1,
    ASDK_PINMUX_TCPWM2_CHX_SUBFUN_TRIG0,
    ASDK_PINMUX_TCPWM2_CHX_SUBFUN_TRIG1,

} asdk_pinmux_subfunction_t;

/*! GPIO pull-type configuration options. */
typedef enum
{
    ASDK_GPIO_PULL_TYPE_ANALOG = 0x00ul,
    ASDK_GPIO_PULL_TYPE_PULLUP_IN_OFF = 0x02ul,
    ASDK_GPIO_PULL_TYPE_PULLDOWN_IN_OFF = 0x03ul,
    ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW_IN_OFF = 0x04ul,
    ASDK_GPIO_PULL_TYPE_OD_DRIVESHIGH_IN_OFF = 0x05ul,
    ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF = 0x06ul,
    ASDK_GPIO_PULL_TYPE_PULLUP_DOWN_IN_OFF = 0x07ul,
    ASDK_GPIO_PULL_TYPE_HIGHZ = 0x08ul,
    ASDK_GPIO_PULL_TYPE_PULLUP = 0x0Aul,
    ASDK_GPIO_PULL_TYPE_PULLDOWN = 0x0Bul,
    ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW = 0x0Cul,
    ASDK_GPIO_PULL_TYPE_OD_DRIVESHIGH = 0x0Dul,
    ASDK_GPIO_PULL_TYPE_STRONG = 0x0Eul,
    ASDK_GPIO_PULL_TYPE_PULLUP_DOWN = 0x0Ful,
    ASDK_GPIO_PULL_TYPE_MAXIMUM,                                  /*!< Indicates total number of available pull options. */
    ASDK_GPIO_PULL_TYPE_ALT_NOT_DEFINED = ASDK_GPIO_PULL_TYPE_MAXIMUM /*!< Values beyond ASDK_GPIO_PULL_TYPE_MAX are undefined. */
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
typedef struct
{

    /*Alternate function enum*/
    uint32_t alternate_fun_id;
    /* Essential Input */
    asdk_mcu_pin_t MCU_pin_num; /*!< Array pointer to the list of ADC pins to be initialized with given configurations. */

    /*User will pass the pull configuration for the alternate function they want*/
    asdk_gpio_pull_altfn_t pull_configuration;



} asdk_pinmux_config_t;



typedef enum
{
    CYT2B75_GPIO_PORT_0 = 0,
    CYT2B75_GPIO_PORT_1,
    CYT2B75_GPIO_PORT_2,
    CYT2B75_GPIO_PORT_3,
    CYT2B75_GPIO_PORT_4,
    CYT2B75_GPIO_PORT_5,
    CYT2B75_GPIO_PORT_6,
    CYT2B75_GPIO_PORT_7,
    CYT2B75_GPIO_PORT_8,
    CYT2B75_GPIO_PORT_9,
    CYT2B75_GPIO_PORT_10,
    CYT2B75_GPIO_PORT_11,
    CYT2B75_GPIO_PORT_12,
    CYT2B75_GPIO_PORT_13,
    CYT2B75_GPIO_PORT_14,
    CYT2B75_GPIO_PORT_15,
    CYT2B75_GPIO_PORT_16,
    CYT2B75_GPIO_PORT_17,
    CYT2B75_GPIO_PORT_18,
    CYT2B75_GPIO_PORT_19,
    CYT2B75_GPIO_PORT_20,
    CYT2B75_GPIO_PORT_21,
    CYT2B75_GPIO_PORT_22,
    CYT2B75_GPIO_PORT_23,

    CYT2B75_GPIO_PORT_MAX,
    CYT2B75_GPIO_PORT_NOT_DEFINED = CYT2B75_GPIO_PORT_MAX,
} cyt2b75_port_t;

typedef enum
{
    CYT2B75_GPIO_PIN_0 = 0,
    CYT2B75_GPIO_PIN_1,
    CYT2B75_GPIO_PIN_2,
    CYT2B75_GPIO_PIN_3,
    CYT2B75_GPIO_PIN_4,
    CYT2B75_GPIO_PIN_5,
    CYT2B75_GPIO_PIN_6,
    CYT2B75_GPIO_PIN_7,

    CYT2B75_GPIO_PIN_MAX,
    CYT2B75_GPIO_PIN_NOT_DEFINED = CYT2B75_GPIO_PIN_MAX,
} cyt2b75_pin_t;

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

typedef struct
{
    asdk_mcu_pin_t mcu_pin; /* Actual pin number of the MCU */
    cyt2b75_port_t port;    /* Actual port of the given GPIO */
    cyt2b75_pin_t pin;      /* Actual pin of a the given GPIO Port */
} dal_pin_t;

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
asdk_errorcode_t asdk_set_pinmux(asdk_pinmux_config_t pinmux_config[], uint16_t pin_count);

// To do:
// API for getting the pinmux cofig incase of reintializing the pinmux with another alternate functionality
// Relevance of this function is to be decided later
//  asdk_errorcode_t asdk_get_pinmux(asdk_mcu_pin_t mcu_pin_no, uint8_t *alternate_fun);

/*! This function sets with the default pinmux configuration for the peripherals */
asdk_errorcode_t asdk_set_default_pinmux(asdk_pinmux_config_t pinmux_config);

/*! This function de-initializes the previously configured PinMux */
asdk_errorcode_t asdk_deinit_pinmux(asdk_mcu_pin_t pin);

#endif /* ASDK_PINMUX_H */
