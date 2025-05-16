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
    asdk_platform.h

    @path
    platform/stm32f4/dal/inc/asdk_platform.h

    @Created on
    Mar 23, 2023

    @Author
    Ather Energy Pvt. Ltd.

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    Platform abstraction for STM32F411 microcontroller.
*/

#ifndef ASDK_PLATFORM_H
#define ASDK_PLATFORM_H

/* MCU SDK includes required for application */

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include <stdbool.h>

#include "asdk_mcu_pins.h"

#define ASDK_STM32F411_PIN_MAX MCU_PIN_MAX /*!< This microcontroller has 100 pins. Refer @ref MCU_PIN_MAX */

#define ASDK_DELAY_MS(delay_ms) HAL_Delay(delay_ms)

/*!
 * @brief An enumerator to represent CAN channels.
 *
 * The CAN peripheral is abstracted based on channel number regardless of the
 * number of available CAN modules or their underlying channels. Refer the TRM
 * of the microcontroller for more information.
 * 
 * @note
 * * The TRM lists combination of CAN peripheral clock with total time-quanta.
 * From the table, we picked clock of 40MHz because it covers the entire range of
 * baudrate within the specified time-quanta limits.
 * * As consequence of setting clock to 40MHz the pre-scaler can be adjusted
 * to maintain a constant time quanta. Hence the DAL of CYT2B75 platform
 * adjusts the pre-scaler to maintain a constant time quanta of 40 for
 * all baudrates.
 * 
 */
typedef enum
{
    ASDK_CAN_MODULE_CAN_CH_0 = 0, /*!< Use CANFD0 Channel-0 as CAN Channel #0 */
    ASDK_CAN_MODULE_CAN_CH_1,     /*!< Use CANFD1 Channel-1 as CAN Channel #1 */
    ASDK_CAN_MODULE_CAN_CH_MAX,   /*!< Total number of CAN channels: 6*/
    ASDK_CAN_MODULE_NOT_DEFINED = ASDK_CAN_MODULE_CAN_CH_MAX, /*!< Channels beyond @ref ASDK_CAN_MODULE_CAN_CH_MAX is undefined. */
} asdk_can_channel_t;

typedef enum
{
    // Group 1, 4 Channels
    ASDK_TIMER_MODULE_CH_0 = 0,
    ASDK_TIMER_MODULE_CH_1, 
    ASDK_TIMER_MODULE_CH_2,
    ASDK_TIMER_MODULE_CH_3, 

    // Group 2, 4 Channels
    ASDK_TIMER_MODULE_CH_4,
    ASDK_TIMER_MODULE_CH_5,
    ASDK_TIMER_MODULE_CH_6,
    ASDK_TIMER_MODULE_CH_7,

    //Group 3, 4 Channels
    ASDK_TIMER_MODULE_CH_8,
    ASDK_TIMER_MODULE_CH_9,
    ASDK_TIMER_MODULE_CH_10,
    ASDK_TIMER_MODULE_CH_11,

    //Group 4, 4 Channels
    ASDK_TIMER_MODULE_CH_12,
    ASDK_TIMER_MODULE_CH_13,
    ASDK_TIMER_MODULE_CH_14,
    ASDK_TIMER_MODULE_CH_15,

    //Group 5, 4 Channels
    ASDK_TIMER_MODULE_CH_16,
    ASDK_TIMER_MODULE_CH_17,
    ASDK_TIMER_MODULE_CH_18,
    ASDK_TIMER_MODULE_CH_19,

    //Group 6, 1 Channels
    ASDK_TIMER_MODULE_CH_20,

    //Group 7, 1 Channels
    ASDK_TIMER_MODULE_CH_21,

    //Group 8, 4 Channels
    ASDK_TIMER_MODULE_CH_22,
    ASDK_TIMER_MODULE_CH_23,
    ASDK_TIMER_MODULE_CH_24,
    ASDK_TIMER_MODULE_CH_25,

    //Group 9, 2 Channels
    ASDK_TIMER_MODULE_CH_26,
    ASDK_TIMER_MODULE_CH_27,

    //Group 10, 1 Channels
    ASDK_TIMER_MODULE_CH_28,

    //Group 11, 1 Channels
    ASDK_TIMER_MODULE_CH_29,

    //Group 12, 2 Channels
    ASDK_TIMER_MODULE_CH_30,
    ASDK_TIMER_MODULE_CH_31,

    //Group 13, 1 Channels
    ASDK_TIMER_MODULE_CH_32,

    //Group 14, 1 Channels
    ASDK_TIMER_MODULE_CH_33,

    ASDK_TIMER_MODULE_CH_MAX
}asdk_timer_channel_t;

typedef enum {
  // Group 1 (TIM1), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_1 = 0, // TIM1_CH1
  ASDK_PWM_MODULE_CH_2,     // TIM1_CH2
  ASDK_PWM_MODULE_CH_3,     // TIM1_CH3
  ASDK_PWM_MODULE_CH_4,     // TIM1_CH4

  // Group 2 (TIM2), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_5, // TIM2_CH1
  ASDK_PWM_MODULE_CH_6, // TIM2_CH2
  ASDK_PWM_MODULE_CH_7, // TIM2_CH3
  ASDK_PWM_MODULE_CH_8, // TIM2_CH4

  // Group 3 (TIM3), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_9,  // TIM3_CH1
  ASDK_PWM_MODULE_CH_10, // TIM3_CH2
  ASDK_PWM_MODULE_CH_11, // TIM3_CH3
  ASDK_PWM_MODULE_CH_12, // TIM3_CH4

  // Group 4 (TIM4), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_13, // TIM4_CH1
  ASDK_PWM_MODULE_CH_14, // TIM4_CH2
  ASDK_PWM_MODULE_CH_15, // TIM4_CH3
  ASDK_PWM_MODULE_CH_16, // TIM4_CH4

  // Group 5 (TIM5), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_17, // TIM5_CH1
  ASDK_PWM_MODULE_CH_18, // TIM5_CH2
  ASDK_PWM_MODULE_CH_19, // TIM5_CH3
  ASDK_PWM_MODULE_CH_20, // TIM5_CH4

  // Group 8 (TIM8), 4 channels, supports edge and center align
  ASDK_PWM_MODULE_CH_21, // TIM8_CH1
  ASDK_PWM_MODULE_CH_22, // TIM8_CH2
  ASDK_PWM_MODULE_CH_23, // TIM8_CH3
  ASDK_PWM_MODULE_CH_24, // TIM8_CH4

  // Group 9 (TIM9), 2 channels, supports edge align
  ASDK_PWM_MODULE_CH_25, // TIM9_CH1
  ASDK_PWM_MODULE_CH_26, // TIM9_CH2

  // Group 10 (TIM10), 1 channels, supports edge align
  ASDK_PWM_MODULE_CH_27, // TIM10_CH1

  // Group 11 (TIM11), 1 channels, supports edge align
  ASDK_PWM_MODULE_CH_28, // TIM11_CH1

  // Group 12 (TIM12), 2 channels, supports edge align
  ASDK_PWM_MODULE_CH_29, // TIM12_CH1
  ASDK_PWM_MODULE_CH_30, // TIM12_CH2

  // Group 13 (TIM13), 1 channels, supports edge align
  ASDK_PWM_MODULE_CH_31, // TIM13_CH1

  // Group 14 (TIM14), 1 channels, supports edge align
  ASDK_PWM_MODULE_CH_32, // TIM14_CH1

  ASDK_PWM_MODULE_CH_MAX = ASDK_PWM_MODULE_CH_32
} asdk_pwm_channel_t;

#endif /* ASDK_PLATFORM_H */