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
    asdk_pinmux.c

    @path
    platform/stm32/f4/dal/src/asdk_pinmux.c

    @Created on
    July 25, 2023

    @Author
    Ather Energy Pvt Ltd.

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file implements the PINMUX module for the STM32F407 microcontroller.

*/
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "asdk_pinmux.h"
#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_platform.h"

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

typedef enum {
  STM32F4_GPIO_PORT_A = 0,
  STM32F4_GPIO_PORT_B,
  STM32F4_GPIO_PORT_C,
  STM32F4_GPIO_PORT_D,
  STM32F4_GPIO_PORT_E,
  STM32F4_GPIO_PORT_H,

  STM32F4_GPIO_PORT_MAX,
  STM32F4_GPIO_PORT_NOT_DEFINED = STM32F4_GPIO_PORT_MAX,
} stm32f4_port_t;

typedef enum {
  STM32F4_GPIO_PIN_0 = 0,
  STM32F4_GPIO_PIN_1,
  STM32F4_GPIO_PIN_2,
  STM32F4_GPIO_PIN_3,
  STM32F4_GPIO_PIN_4,
  STM32F4_GPIO_PIN_5,
  STM32F4_GPIO_PIN_6,
  STM32F4_GPIO_PIN_7,
  STM32F4_GPIO_PIN_8,
  STM32F4_GPIO_PIN_9,
  STM32F4_GPIO_PIN_10,
  STM32F4_GPIO_PIN_11,
  STM32F4_GPIO_PIN_12,
  STM32F4_GPIO_PIN_13,
  STM32F4_GPIO_PIN_14,
  STM32F4_GPIO_PIN_15,

  STM32F4_GPIO_PIN_MAX,
  STM32F4_GPIO_PIN_NOT_DEFINED = STM32F4_GPIO_PIN_MAX,
} stm32f4_pin_t;

typedef struct {
  asdk_mcu_pin_t mcu_pin; /* Actual pin number of the MCU */
  stm32f4_port_t port;    /* Actual port of the given GPIO */
  stm32f4_pin_t pin;      /* Actual pin of a the given GPIO Port */
} dal_pin_t;

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

extern const dal_pin_t pin_map[];
extern volatile GPIO_TypeDef *const gpio_port[];

typedef struct {
  uint32_t alt_fn_e;
  uint16_t st_alt_fn;
} asdk_pinmux_t;

/* Static constant array of the alternate functions for the STM32F4 platform */
/**
 * NOTE: Not all alternate functions are mapped here.
         Less common peripherals have been left out.

         Alternate functions in this file are not tested fully.
         DAL developer will have to verify the pin mux for the specific
         peripheral that he/she intends to write.
 */
static const asdk_pinmux_t pinmux_arr[100][16] = {
    // E5
    [3] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM9, 1),
             .st_alt_fn = GPIO_AF3_TIM9},
        },
    // E6
    [4] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM9, 2),
             .st_alt_fn = GPIO_AF3_TIM9},
        },
    // C2
    [16] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SDA, 2),
             .st_alt_fn = GPIO_AF6_I2S2ext},
        },
    // C3
    [17] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MOSI, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SDA, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
        },
    // A0
    [22] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 1),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM5, 1),
             .st_alt_fn = GPIO_AF2_TIM5},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CTS, 2),
             .st_alt_fn = GPIO_AF7_USART2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 4),
             .st_alt_fn = GPIO_AF8_UART4},
        },
    // A1
    [23] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 2),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM5, 2),
             .st_alt_fn = GPIO_AF2_TIM5},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RTS, 2),
             .st_alt_fn = GPIO_AF7_USART2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 4),
             .st_alt_fn = GPIO_AF8_UART4},
        },
    // A2
    [24] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 3),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM5, 3),
             .st_alt_fn = GPIO_AF2_TIM5},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM9, 1),
             .st_alt_fn = GPIO_AF3_TIM9},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // A3
    [25] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 4),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM5, 4),
             .st_alt_fn = GPIO_AF2_TIM5},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM9, 2),
             .st_alt_fn = GPIO_AF3_TIM9},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // A4
    [28] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // A5
    [29] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 1),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 1),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 2),
             .st_alt_fn = GPIO_AF5_SPI1},
        },
    // A6
    [30] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 1),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM13, 1),
             .st_alt_fn = GPIO_AF9_TIM13},
        },
    // A7
    [31] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 1),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 2),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 1),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MOSI, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM14, 1),
             .st_alt_fn = GPIO_AF9_TIM14},
        },
    // B0
    [34] = {{.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 2),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 3),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 2),
             .st_alt_fn = GPIO_AF3_TIM8}},
    // B1
    [35] = {{.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 4),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 3),
             .st_alt_fn = GPIO_AF3_TIM8}},
    // E8
    [38] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 1),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E9
    [39] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 1),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E10
    [40] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 2),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E11
    [41] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 2),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E12
    [42] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E13
    [43] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // E14
    [44] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 4),
             .st_alt_fn = GPIO_AF1_TIM1},
        },
    // B10
    [46] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SCL, 2),
             .st_alt_fn = GPIO_AF4_I2C2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // B12
    [50] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_RX, 2),
             .st_alt_fn = GPIO_AF9_CAN2},
        },
    // B13
    [51] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 1),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CTS, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_TX, 2),
             .st_alt_fn = GPIO_AF9_CAN2},
        },
    // B14
    [52] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 2),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 2),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RTS, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM12, 1),
             .st_alt_fn = GPIO_AF9_TIM12},
        },
    // B15
    [53] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 3),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM12, 2),
             .st_alt_fn = GPIO_AF9_TIM12},
        },
    // D8
    [54] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // D9
    [55] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // D10
    [56] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // D11
    [57] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CTS, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // D12
    [58] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 1),
             .st_alt_fn = GPIO_AF2_TIM4},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RTS, 3),
             .st_alt_fn = GPIO_AF7_USART3},
        },
    // D13
    [59] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 2),
             .st_alt_fn = GPIO_AF2_TIM4},
        },
    // D14
    [60] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 3),
             .st_alt_fn = GPIO_AF2_TIM4},
        },
    // D15
    [61] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 4),
             .st_alt_fn = GPIO_AF2_TIM4},
        },
    // C6
    [62] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 1),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 1),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 6),
             .st_alt_fn = GPIO_AF8_USART6},
        },
    // C7
    [63] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 2),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 5),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 6),
             .st_alt_fn = GPIO_AF8_USART6},
        },
    // C8
    [64] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 3),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 3),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 6),
             .st_alt_fn = GPIO_AF8_USART6},
        },
    // C9
    [65] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 4),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM8, 5),
             .st_alt_fn = GPIO_AF3_TIM8},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SDA, 3),
             .st_alt_fn = GPIO_AF4_I2C3},
        },
    // A8
    [66] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 1),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SCL, 3),
             .st_alt_fn = GPIO_AF4_I2C3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 1),
             .st_alt_fn = GPIO_AF7_USART1},
        },
    // A9
    [67] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 2),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 1),
             .st_alt_fn = GPIO_AF7_USART1},
        },
    // A10
    [68] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 3),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 1),
             .st_alt_fn = GPIO_AF7_USART1},
        },
    // A11
    [69] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM1, 4),
             .st_alt_fn = GPIO_AF1_TIM1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CTS, 3),
             .st_alt_fn = GPIO_AF7_USART1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_RX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
    // A12
    [70] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RTS, 3),
             .st_alt_fn = GPIO_AF7_USART1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_TX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
    // A15
    [76] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 1),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
        },

    // C10
    [77] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 4),
             .st_alt_fn = GPIO_AF8_UART4},
        },
    // C11
    [78] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 4),
             .st_alt_fn = GPIO_AF8_UART4},
        },
    // C12
    [79] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MOSI, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 3),
             .st_alt_fn = GPIO_AF7_USART3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 4),
             .st_alt_fn = GPIO_AF8_UART5},
        },
    // D0
    [80] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_RX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
    // D1
    [81] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_TX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
    // D2
    [82] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 5),
             .st_alt_fn = GPIO_AF8_UART5},
        },
    // D3
    [83] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CTS, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // D4
    [84] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RTS, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // D5
    [85] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // D6
    [86] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // D7
    [87] =
        {
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_CLK, 2),
             .st_alt_fn = GPIO_AF7_USART2},
        },
    // B3
    [88] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM2, 2),
             .st_alt_fn = GPIO_AF1_TIM2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CLK, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
        },
    // B4
    [89] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 1),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MISO, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
        },
    // B5
    [90] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM3, 2),
             .st_alt_fn = GPIO_AF2_TIM3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MOSI, 1),
             .st_alt_fn = GPIO_AF5_SPI1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_MOSI, 3),
             .st_alt_fn = GPIO_AF6_SPI3},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_RX, 2),
             .st_alt_fn = GPIO_AF9_CAN2},
        },
    // B6
    [91] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 1),
             .st_alt_fn = GPIO_AF2_TIM4},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SCL, 1),
             .st_alt_fn = GPIO_AF4_I2C1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_TX, 1),
             .st_alt_fn = GPIO_AF7_USART1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_TX, 2),
             .st_alt_fn = GPIO_AF9_CAN2},
        },
    // B7
    [92] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 2),
             .st_alt_fn = GPIO_AF2_TIM4},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SDA, 1),
             .st_alt_fn = GPIO_AF4_I2C1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_USART,
                                            ASDK_PINMUX_USART_SUBFUN_RX, 1),
             .st_alt_fn = GPIO_AF7_USART1},
        },
    // B8
    [94] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 3),
             .st_alt_fn = GPIO_AF2_TIM4},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM10, 1),
             .st_alt_fn = GPIO_AF3_TIM10},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SCL, 1),
             .st_alt_fn = GPIO_AF4_I2C1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_RX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
    // B9
    [95] =
        {
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM4, 4),
             .st_alt_fn = GPIO_AF2_TIM4},
            {.alt_fn_e =
                 ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TIM, ASDK_PINMUX_TIM11, 1),
             .st_alt_fn = GPIO_AF3_TIM11},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C,
                                            ASDK_PINMUX_I2C_SUBFUN_SDA, 1),
             .st_alt_fn = GPIO_AF4_I2C1},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI,
                                            ASDK_PINMUX_SPI_SUBFUN_CS, 2),
             .st_alt_fn = GPIO_AF5_SPI2},
            {.alt_fn_e = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN,
                                            ASDK_PINMUX_CAN_SUBFUN_TX, 1),
             .st_alt_fn = GPIO_AF9_CAN1},
        },
};

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define PIN_MUX_SUCCESS 0
#define REQUESTED_ALT_FUN_NOT_SUPPORTED 1
#define MAX_ALT_FN 16

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
static inline bool __asdk_set_pinmux_config_for_alternate_function(
    asdk_pinmux_config_t pinmux_config, GPIO_InitTypeDef *stm32_pin_cfg);
static inline void __enable_gpio_clock(stm32f4_port_t port);

/*----------------------------------------------------------------------------*/
/* Function : asdk_set_pinmux */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the pinmux for the given pin

  @param pinmux_config - ASDK PINMUX configuration.
  @param pin_count - Number of pins to be ste the Alternate Function

  @return asdk_errorcode_t
  @retval ASDK_PINMUX_SUCCESS
  @retval ASDK_PINMUX_ERROR_NULL_PTR
  @retval ASDK_PINMUX_ERROR_INVALID_MCU_PIN
  @retval ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION
  @retval ASDK_PINMUX_ERROR_INIT_CONFIG_ERROR
  @retval ASDK_PINMUX_ERROR_DEINIT_CONFIG_ERROR

*/

asdk_errorcode_t asdk_set_pinmux(asdk_pinmux_config_t pinmux_config[],
                                 uint16_t pin_count) {
  const dal_pin_t *stm32f4_gpio = NULL;
  asdk_errorcode_t ret_value = ASDK_PINMUX_SUCCESS;
  bool pinmux_status_ret_val = PIN_MUX_SUCCESS;

  GPIO_InitTypeDef GPIO_Type = {
      .Alternate = 0, .Mode = 0, .Pin = 0, .Pull = 0, .Speed = 0};

  /* validate configuration parameters */
  if (NULL == pinmux_config) {
    return ASDK_PINMUX_ERROR_NULL_PTR;
  }

  // de-intializing pinmux incase of failure
  // Initialize the pinmux one-by-one i.e. one at a time
  for (uint8_t pin_traverse = 0; pin_traverse < pin_count; pin_traverse++) {
    if (MCU_PIN_NOT_DEFINED <= pinmux_config[pin_traverse].MCU_pin_num) {
      return ASDK_PINMUX_ERROR_INVALID_MCU_PIN;
    }

    if (ASDK_GPIO_SPEED_TYPE_MAX <= pinmux_config[pin_traverse].slew_rate) {
      return ASDK_PINMUX_ERROR_INVALID_GPIO_SLEW_RATE;
    }

    stm32f4_gpio = &pin_map[(pinmux_config[pin_traverse].MCU_pin_num)];

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port) {
      return ASDK_PINMUX_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin) {
      return ASDK_PINMUX_ERROR_INVALID_GPIO_PIN;
    }

    GPIO_Type.Pin = (1 << stm32f4_gpio->pin);

    pinmux_status_ret_val = __asdk_set_pinmux_config_for_alternate_function(
        pinmux_config[pin_traverse], &GPIO_Type);

    /* init clock gating */
    __enable_gpio_clock(stm32f4_gpio->port);

    if (PIN_MUX_SUCCESS == pinmux_status_ret_val) {
      /* initialize pin as GPIO */
      HAL_GPIO_Init(gpio_port[stm32f4_gpio->port], &GPIO_Type);
    } else {
      ret_value = ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION;
    }
  }
  return ret_value;
}

/*----------------------------------------------------------------------------*/
/* Function : asdk_set_default_pinmux */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the default pinmux for the given alternate function

  @param pin - MCU pin for which the alternate function is set.
  @param pinmux_config - ASDK PinMux config structure to get the Alternate
  Function

  @return asdk_errorcode_t
  @retval ASDK_PINMUX_SUCCESS
  @retval ASDK_PINMUX_ERROR_INVALID_MCU_PIN
  @retval ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION
  @retval ASDK_PINMUX_ERROR_INIT_CONFIG_ERROR
  @retval ASDK_PINMUX_ERROR_DEINIT_CONFIG_ERROR

*/
asdk_errorcode_t asdk_set_default_pinmux(asdk_pinmux_config_t pinmux_config) {
  asdk_errorcode_t ret_value = ASDK_PINMUX_SUCCESS;
  bool pinmux_status_ret_val = PIN_MUX_SUCCESS;
  const dal_pin_t *stm32f4_gpio = NULL;

  GPIO_InitTypeDef GPIO_Type = {
      .Alternate = 0, .Mode = 0, .Pin = 0, .Pull = 0, .Speed = 0};

  if (MCU_PIN_NOT_DEFINED <= pinmux_config.MCU_pin_num) {
    return ASDK_PINMUX_ERROR_INVALID_MCU_PIN;
  }

  stm32f4_gpio = &pin_map[pinmux_config.MCU_pin_num];

  if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port) {
    return ASDK_PINMUX_ERROR_INVALID_GPIO_PORT;
  }

  if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin) {
    return ASDK_PINMUX_ERROR_INVALID_GPIO_PIN;
  }

  GPIO_Type.Pin = stm32f4_gpio->pin;

  pinmux_status_ret_val = __asdk_set_pinmux_config_for_alternate_function(
      pinmux_config, &GPIO_Type);

  if (PIN_MUX_SUCCESS == pinmux_status_ret_val) {
    /* initialize pin as GPIO */
    HAL_GPIO_Init(gpio_port[stm32f4_gpio->port], &GPIO_Type);
  } else {
    ret_value = ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION;
  }
  return ret_value;
}
/*----------------------------------------------------------------------------*/
/* Function : asdk_gpio_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes GPIO pin based on ECU input pin name.

  @param asdk_mcu_pin_t pin_num ( input ) - Pin number of the MCU.

  @return asdk_errorcode_t
  @retval ASDK_PINMUX_SUCCESS
  @retval ASDK_PINMUX_ERROR_INVALID_MCU_PIN
  @retval ASDK_PINMUX_ERROR_INVALID_GPIO_PORT
  @retval ASDK_PINMUX_ERROR_INVALID_GPIO_PIN
*/
asdk_errorcode_t asdk_deinit_pinmux(asdk_mcu_pin_t pin_num) {
  const dal_pin_t *stm32f4_gpio = NULL;

  if (MCU_PIN_MAX <= pin_num) {
    return ASDK_PINMUX_ERROR_INVALID_MCU_PIN;
  }

  // derive actual PORT.PIN from MCU pin
  stm32f4_gpio = &pin_map[pin_num];

  if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port) {
    return ASDK_PINMUX_ERROR_INVALID_GPIO_PORT;
  }

  if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin) {
    return ASDK_PINMUX_ERROR_INVALID_GPIO_PIN;
  }

  /* Calling De-init will reset all the gpio related configs to default values
   */
  HAL_GPIO_DeInit(gpio_port[stm32f4_gpio->port], stm32f4_gpio->pin);

  return ASDK_PINMUX_SUCCESS;
}

volatile uint32_t pinmux_id = 0; // TODO why is this volatile?
static inline bool __asdk_set_pinmux_config_for_alternate_function(
    asdk_pinmux_config_t pinmux_config, GPIO_InitTypeDef *stm32_pin_cfg) {
  bool ret_value = PIN_MUX_SUCCESS;

  if ((pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW) ||
      (pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_OD_DRIVESHIGH)) {
    stm32_pin_cfg->Mode = GPIO_MODE_AF_OD;
  } else if (pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_ANALOG) {
    stm32_pin_cfg->Mode = GPIO_MODE_ANALOG;
  } else if (pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_HIGHZ) {
    stm32_pin_cfg->Mode = GPIO_MODE_INPUT;
  } else {
    stm32_pin_cfg->Mode = GPIO_MODE_AF_PP;
  }

  if ((pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_PULLUP) ||
      (pinmux_config.pull_configuration == ASDK_GPIO_PULL_TYPE_OD_DRIVESHIGH)) {
    stm32_pin_cfg->Pull = GPIO_PULLUP;
  } else if ((pinmux_config.pull_configuration ==
              ASDK_GPIO_PULL_TYPE_PULLDOWN) ||
             (pinmux_config.pull_configuration ==
              ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW)) {
    stm32_pin_cfg->Pull = GPIO_PULLDOWN;
  } else {
    stm32_pin_cfg->Pull = GPIO_NOPULL;
  }

  stm32_pin_cfg->Speed = pinmux_config.slew_rate;

  for (uint8_t alt_fn = 0; alt_fn < MAX_ALT_FN; alt_fn++) {
    pinmux_id = pinmux_arr[pinmux_config.MCU_pin_num][alt_fn].alt_fn_e;

    if (pinmux_id == pinmux_config.alternate_fun_id) {
      stm32_pin_cfg->Alternate =
          pinmux_arr[pinmux_config.MCU_pin_num][alt_fn].st_alt_fn;
      ret_value = PIN_MUX_SUCCESS;

      break;
    } else {
      ret_value = REQUESTED_ALT_FUN_NOT_SUPPORTED;
    }
  }

  return ret_value;
}

static inline void __enable_gpio_clock(stm32f4_port_t port) {
  switch (port) {
  case STM32F4_GPIO_PORT_A:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    break;

  case STM32F4_GPIO_PORT_B:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    break;

  case STM32F4_GPIO_PORT_C:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    break;

  case STM32F4_GPIO_PORT_D:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    break;

  case STM32F4_GPIO_PORT_E:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    break;

  case STM32F4_GPIO_PORT_H:
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    break;

  default:
    break;
  }
}
