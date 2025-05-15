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
    asdk_platform.h

    @path
    platform/Infineon/cyt2b75/dal/inc/asdk_platform.h

    @Created on
    Mar 23, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    Platform abstraction for CYT2B75 microcontroller.
*/

#ifndef ASDK_PLATFORM_H
#define ASDK_PLATFORM_H

/* MCU SDK includes required for application */

#include "cy_device_headers.h"
#include "asdk_mcu_pins.h"
#include "systick/cy_systick.h"

#define ASDK_DELAY_MS(DELAY_MS) Cy_SysTick_DelayInUs((DELAY_MS) * 1000) /*!< Delay in milliseconds */

#define ASDK_CYT2B75_PIN_MAX MCU_PIN_MAX /*!< This microcontroller has 100 pins. Refer @ref MCU_PIN_MAX */

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
    ASDK_CAN_MODULE_CAN_CH_1,     /*!< Use CANFD0 Channel-1 as CAN Channel #1 */
    ASDK_CAN_MODULE_CAN_CH_2,     /*!< Use CANFD0 Channel-2 as CAN Channel #2 */
    ASDK_CAN_MODULE_CAN_CH_3,     /*!< Use CANFD1 Channel-0 as CAN Channel #3 */
    ASDK_CAN_MODULE_CAN_CH_4,     /*!< Use CANFD1 Channel-1 as CAN Channel #4 */
    ASDK_CAN_MODULE_CAN_CH_5,     /*!< Use CANFD1 Channel-2 as CAN Channel #5 */
    ASDK_CAN_MODULE_CAN_CH_MAX,   /*!< Total number of CAN channels: 6*/
    ASDK_CAN_MODULE_NOT_DEFINED = ASDK_CAN_MODULE_CAN_CH_MAX, /*!< Channels beyond @ref ASDK_CAN_MODULE_CAN_CH_MAX is undefined. */
} asdk_can_channel_t;

/*!
 * @brief An enumerator to represent Timer channels. The Timer peripheral is 
 * abstracted based on channel number regardless of the number of available 
 * Timer modules or their underlying channels. Refer the TRM of the 
 * microcontroller for more information.
 * 
 * @note
 * Following groups of timers are available in this microcontrollers:
 * * Group 0 has 63 timers of 16-bit type.
 * * Group 1 has 12 timers of 16-bit type.
 * * Group 2 has 4 timers of 32-bit type.
 */
typedef enum
{
    // Group 0, 63 timers
    ASDK_TIMER_MODULE_CH_0 = 0, /*!< 16-bit Timer, Use Group0 Channel-0 as Timer Channel #0 */
    ASDK_TIMER_MODULE_CH_1,     /*!< 16-bit Timer, Use Group0 Channel-1 as Timer Channel #1 */
    ASDK_TIMER_MODULE_CH_2,     /*!< 16-bit Timer, Use Group0 Channel-2 as Timer Channel #2 */
    ASDK_TIMER_MODULE_CH_3,     /*!< 16-bit Timer, Use Group0 Channel-3 as Timer Channel #3 */
    ASDK_TIMER_MODULE_CH_4,     /*!< 16-bit Timer, Use Group0 Channel-4 as Timer Channel #4 */
    ASDK_TIMER_MODULE_CH_5,     /*!< 16-bit Timer, Use Group0 Channel-5 as Timer Channel #5 */
    ASDK_TIMER_MODULE_CH_6,     /*!< 16-bit Timer, Use Group0 Channel-6 as Timer Channel #6 */
    ASDK_TIMER_MODULE_CH_7,     /*!< 16-bit Timer, Use Group0 Channel-7 as Timer Channel #7 */
    ASDK_TIMER_MODULE_CH_8,     /*!< 16-bit Timer, Use Group0 Channel-8 as Timer Channel #8 */
    ASDK_TIMER_MODULE_CH_9,     /*!< 16-bit Timer, Use Group0 Channel-9 as Timer Channel #9 */
    ASDK_TIMER_MODULE_CH_10,    /*!< 16-bit Timer, Use Group0 Channel-10 as Timer Channel #10 */
    ASDK_TIMER_MODULE_CH_11,    /*!< 16-bit Timer, Use Group0 Channel-11 as Timer Channel #11 */
    ASDK_TIMER_MODULE_CH_12,    /*!< 16-bit Timer, Use Group0 Channel-12 as Timer Channel #12 */
    ASDK_TIMER_MODULE_CH_13,    /*!< 16-bit Timer, Use Group0 Channel-13 as Timer Channel #13 */
    ASDK_TIMER_MODULE_CH_14,    /*!< 16-bit Timer, Use Group0 Channel-14 as Timer Channel #14 */
    ASDK_TIMER_MODULE_CH_15,    /*!< 16-bit Timer, Use Group0 Channel-15 as Timer Channel #15 */
    ASDK_TIMER_MODULE_CH_16,    /*!< 16-bit Timer, Use Group0 Channel-16 as Timer Channel #16 */
    ASDK_TIMER_MODULE_CH_17,    /*!< 16-bit Timer, Use Group0 Channel-17 as Timer Channel #17 */
    ASDK_TIMER_MODULE_CH_18,    /*!< 16-bit Timer, Use Group0 Channel-18 as Timer Channel #18 */
    ASDK_TIMER_MODULE_CH_19,    /*!< 16-bit Timer, Use Group0 Channel-19 as Timer Channel #19 */
    ASDK_TIMER_MODULE_CH_20,    /*!< 16-bit Timer, Use Group0 Channel-20 as Timer Channel #20 */
    ASDK_TIMER_MODULE_CH_21,    /*!< 16-bit Timer, Use Group0 Channel-21 as Timer Channel #21 */
    ASDK_TIMER_MODULE_CH_22,    /*!< 16-bit Timer, Use Group0 Channel-22 as Timer Channel #22 */
    ASDK_TIMER_MODULE_CH_23,    /*!< 16-bit Timer, Use Group0 Channel-23 as Timer Channel #23 */
    ASDK_TIMER_MODULE_CH_24,    /*!< 16-bit Timer, Use Group0 Channel-24 as Timer Channel #24 */
    ASDK_TIMER_MODULE_CH_25,    /*!< 16-bit Timer, Use Group0 Channel-25 as Timer Channel #25 */
    ASDK_TIMER_MODULE_CH_26,    /*!< 16-bit Timer, Use Group0 Channel-26 as Timer Channel #26 */
    ASDK_TIMER_MODULE_CH_27,    /*!< 16-bit Timer, Use Group0 Channel-27 as Timer Channel #27 */
    ASDK_TIMER_MODULE_CH_28,    /*!< 16-bit Timer, Use Group0 Channel-28 as Timer Channel #28 */
    ASDK_TIMER_MODULE_CH_29,    /*!< 16-bit Timer, Use Group0 Channel-29 as Timer Channel #29 */
    ASDK_TIMER_MODULE_CH_30,    /*!< 16-bit Timer, Use Group0 Channel-30 as Timer Channel #30 */
    ASDK_TIMER_MODULE_CH_31,    /*!< 16-bit Timer, Use Group0 Channel-31 as Timer Channel #31 */
    ASDK_TIMER_MODULE_CH_32,    /*!< 16-bit Timer, Use Group0 Channel-32 as Timer Channel #32 */
    ASDK_TIMER_MODULE_CH_33,    /*!< 16-bit Timer, Use Group0 Channel-33 as Timer Channel #33 */
    ASDK_TIMER_MODULE_CH_34,    /*!< 16-bit Timer, Use Group0 Channel-34 as Timer Channel #34 */
    ASDK_TIMER_MODULE_CH_35,    /*!< 16-bit Timer, Use Group0 Channel-35 as Timer Channel #35 */
    ASDK_TIMER_MODULE_CH_36,    /*!< 16-bit Timer, Use Group0 Channel-36 as Timer Channel #36 */
    ASDK_TIMER_MODULE_CH_37,    /*!< 16-bit Timer, Use Group0 Channel-37 as Timer Channel #37 */
    ASDK_TIMER_MODULE_CH_38,    /*!< 16-bit Timer, Use Group0 Channel-38 as Timer Channel #38 */
    ASDK_TIMER_MODULE_CH_39,    /*!< 16-bit Timer, Use Group0 Channel-39 as Timer Channel #39 */
    ASDK_TIMER_MODULE_CH_40,    /*!< 16-bit Timer, Use Group0 Channel-40 as Timer Channel #40 */
    ASDK_TIMER_MODULE_CH_41,    /*!< 16-bit Timer, Use Group0 Channel-41 as Timer Channel #41 */
    ASDK_TIMER_MODULE_CH_42,    /*!< 16-bit Timer, Use Group0 Channel-42 as Timer Channel #42 */
    ASDK_TIMER_MODULE_CH_43,    /*!< 16-bit Timer, Use Group0 Channel-43 as Timer Channel #43 */
    ASDK_TIMER_MODULE_CH_44,    /*!< 16-bit Timer, Use Group0 Channel-44 as Timer Channel #44 */
    ASDK_TIMER_MODULE_CH_45,    /*!< 16-bit Timer, Use Group0 Channel-45 as Timer Channel #45 */
    ASDK_TIMER_MODULE_CH_46,    /*!< 16-bit Timer, Use Group0 Channel-46 as Timer Channel #46 */
    ASDK_TIMER_MODULE_CH_47,    /*!< 16-bit Timer, Use Group0 Channel-47 as Timer Channel #47 */
    ASDK_TIMER_MODULE_CH_48,    /*!< 16-bit Timer, Use Group0 Channel-48 as Timer Channel #48 */
    ASDK_TIMER_MODULE_CH_49,    /*!< 16-bit Timer, Use Group0 Channel-49 as Timer Channel #49 */
    ASDK_TIMER_MODULE_CH_50,    /*!< 16-bit Timer, Use Group0 Channel-50 as Timer Channel #50 */
    ASDK_TIMER_MODULE_CH_51,    /*!< 16-bit Timer, Use Group0 Channel-51 as Timer Channel #51 */
    ASDK_TIMER_MODULE_CH_52,    /*!< 16-bit Timer, Use Group0 Channel-52 as Timer Channel #52 */
    ASDK_TIMER_MODULE_CH_53,    /*!< 16-bit Timer, Use Group0 Channel-53 as Timer Channel #53 */
    ASDK_TIMER_MODULE_CH_54,    /*!< 16-bit Timer, Use Group0 Channel-54 as Timer Channel #54 */
    ASDK_TIMER_MODULE_CH_55,    /*!< 16-bit Timer, Use Group0 Channel-55 as Timer Channel #55 */
    ASDK_TIMER_MODULE_CH_56,    /*!< 16-bit Timer, Use Group0 Channel-56 as Timer Channel #56 */
    ASDK_TIMER_MODULE_CH_57,    /*!< 16-bit Timer, Use Group0 Channel-57 as Timer Channel #57 */
    ASDK_TIMER_MODULE_CH_58,    /*!< 16-bit Timer, Use Group0 Channel-58 as Timer Channel #58 */
    ASDK_TIMER_MODULE_CH_59,    /*!< 16-bit Timer, Use Group0 Channel-59 as Timer Channel #59 */
    ASDK_TIMER_MODULE_CH_60,    /*!< 16-bit Timer, Use Group0 Channel-60 as Timer Channel #60 */
    ASDK_TIMER_MODULE_CH_61,    /*!< 16-bit Timer, Use Group0 Channel-61 as Timer Channel #61 */
    ASDK_TIMER_MODULE_CH_62,    /*!< 16-bit Timer, Use Group0 Channel-62 as Timer Channel #62 */

    // Group 1, 12 timers
    ASDK_TIMER_MODULE_CH_63, /*!< 16-bit Timer, Use Group1 Channel-0 as Timer Channel #63 */
    ASDK_TIMER_MODULE_CH_64, /*!< 16-bit Timer, Use Group1 Channel-1 as Timer Channel #64 */
    ASDK_TIMER_MODULE_CH_65, /*!< 16-bit Timer, Use Group1 Channel-2 as Timer Channel #65 */
    ASDK_TIMER_MODULE_CH_66, /*!< 16-bit Timer, Use Group1 Channel-3 as Timer Channel #66 */
    ASDK_TIMER_MODULE_CH_67, /*!< 16-bit Timer, Use Group1 Channel-4 as Timer Channel #67 */
    ASDK_TIMER_MODULE_CH_68, /*!< 16-bit Timer, Use Group1 Channel-5 as Timer Channel #68 */
    ASDK_TIMER_MODULE_CH_69, /*!< 16-bit Timer, Use Group1 Channel-6 as Timer Channel #69 */
    ASDK_TIMER_MODULE_CH_70, /*!< 16-bit Timer, Use Group1 Channel-7 as Timer Channel #70 */
    ASDK_TIMER_MODULE_CH_71, /*!< 16-bit Timer, Use Group1 Channel-8 as Timer Channel #71 */
    ASDK_TIMER_MODULE_CH_72, /*!< 16-bit Timer, Use Group1 Channel-9 as Timer Channel #72 */
    ASDK_TIMER_MODULE_CH_73, /*!< 16-bit Timer, Use Group1 Channel-10 as Timer Channel #73 */
    ASDK_TIMER_MODULE_CH_74, /*!< 16-bit Timer, Use Group1 Channel-11 as Timer Channel #74 */

    // Group 2, 4 timers
    ASDK_TIMER_MODULE_CH_75, /*!< 32-bit Timer, Use Group2 Channel-0 as Timer Channel #75 */
    ASDK_TIMER_MODULE_CH_76, /*!< 32-bit Timer, Use Group2 Channel-1 as Timer Channel #76 */
    ASDK_TIMER_MODULE_CH_77, /*!< 32-bit Timer, Use Group2 Channel-2 as Timer Channel #77 */
    ASDK_TIMER_MODULE_CH_78, /*!< 32-bit Timer, Use Group2 Channel-3 as Timer Channel #78 */

    ASDK_TIMER_MODULE_CH_MAX /*!< Total number of timer channels. */
} asdk_timer_channel_t;

/*!
 * @brief An enumerator to represent PWM channels.
 *
 * The PWM peripheral is abstracted based on channel number regardless of the
 * number of available PWM modules or their underlying channels. Refer the TRM
 * of the microcontroller for more information.
 * 
 * @note
 *  Following groups of timers are available as PWM in this microcontrollers:
 * * Group 0 has 63 timers of 16-bit type.
 * * Group 1 has 12 timers of 16-bit type. Dedicated for Motor Control.
 * * Group 2 has 4 timers of 32-bit type.
 */
typedef enum
{
    // Group 0, 63 PWM Channels
    ASDK_PWM_MODULE_CH_0 = 0, /*!< 16-bit Timer, Use Group0 Channel-0 as PWM Channel #0 */
    ASDK_PWM_MODULE_CH_1,     /*!< 16-bit Timer, Use Group0 Channel-1 as PWM Channel #1 */
    ASDK_PWM_MODULE_CH_2,     /*!< 16-bit Timer, Use Group0 Channel-2 as PWM Channel #2 */
    ASDK_PWM_MODULE_CH_3,     /*!< 16-bit Timer, Use Group0 Channel-3 as PWM Channel #3 */
    ASDK_PWM_MODULE_CH_4,     /*!< 16-bit Timer, Use Group0 Channel-4 as PWM Channel #4 */
    ASDK_PWM_MODULE_CH_5,     /*!< 16-bit Timer, Use Group0 Channel-5 as PWM Channel #5 */
    ASDK_PWM_MODULE_CH_6,     /*!< 16-bit Timer, Use Group0 Channel-6 as PWM Channel #6 */
    ASDK_PWM_MODULE_CH_7,     /*!< 16-bit Timer, Use Group0 Channel-7 as PWM Channel #7 */
    ASDK_PWM_MODULE_CH_8,     /*!< 16-bit Timer, Use Group0 Channel-8 as PWM Channel #8 */
    ASDK_PWM_MODULE_CH_9,     /*!< 16-bit Timer, Use Group0 Channel-9 as PWM Channel #9 */
    ASDK_PWM_MODULE_CH_10,    /*!< 16-bit Timer, Use Group0 Channel-10 as PWM Channel #10 */
    ASDK_PWM_MODULE_CH_11,    /*!< 16-bit Timer, Use Group0 Channel-11 as PWM Channel #11 */
    ASDK_PWM_MODULE_CH_12,    /*!< 16-bit Timer, Use Group0 Channel-12 as PWM Channel #12 */
    ASDK_PWM_MODULE_CH_13,    /*!< 16-bit Timer, Use Group0 Channel-13 as PWM Channel #13 */
    ASDK_PWM_MODULE_CH_14,    /*!< 16-bit Timer, Use Group0 Channel-14 as PWM Channel #14 */
    ASDK_PWM_MODULE_CH_15,    /*!< 16-bit Timer, Use Group0 Channel-15 as PWM Channel #15 */
    ASDK_PWM_MODULE_CH_16,    /*!< 16-bit Timer, Use Group0 Channel-16 as PWM Channel #16 */
    ASDK_PWM_MODULE_CH_17,    /*!< 16-bit Timer, Use Group0 Channel-17 as PWM Channel #17 */
    ASDK_PWM_MODULE_CH_18,    /*!< 16-bit Timer, Use Group0 Channel-18 as PWM Channel #18 */
    ASDK_PWM_MODULE_CH_19,    /*!< 16-bit Timer, Use Group0 Channel-19 as PWM Channel #19 */
    ASDK_PWM_MODULE_CH_20,    /*!< 16-bit Timer, Use Group0 Channel-20 as PWM Channel #20 */
    ASDK_PWM_MODULE_CH_21,    /*!< 16-bit Timer, Use Group0 Channel-21 as PWM Channel #21 */
    ASDK_PWM_MODULE_CH_22,    /*!< 16-bit Timer, Use Group0 Channel-22 as PWM Channel #22 */
    ASDK_PWM_MODULE_CH_23,    /*!< 16-bit Timer, Use Group0 Channel-23 as PWM Channel #23 */
    ASDK_PWM_MODULE_CH_24,    /*!< 16-bit Timer, Use Group0 Channel-24 as PWM Channel #24 */
    ASDK_PWM_MODULE_CH_25,    /*!< 16-bit Timer, Use Group0 Channel-25 as PWM Channel #25 */
    ASDK_PWM_MODULE_CH_26,    /*!< 16-bit Timer, Use Group0 Channel-26 as PWM Channel #26 */
    ASDK_PWM_MODULE_CH_27,    /*!< 16-bit Timer, Use Group0 Channel-27 as PWM Channel #27 */
    ASDK_PWM_MODULE_CH_28,    /*!< 16-bit Timer, Use Group0 Channel-28 as PWM Channel #28 */
    ASDK_PWM_MODULE_CH_29,    /*!< 16-bit Timer, Use Group0 Channel-29 as PWM Channel #29 */
    ASDK_PWM_MODULE_CH_30,    /*!< 16-bit Timer, Use Group0 Channel-30 as PWM Channel #30 */
    ASDK_PWM_MODULE_CH_31,    /*!< 16-bit Timer, Use Group0 Channel-31 as PWM Channel #31 */
    ASDK_PWM_MODULE_CH_32,    /*!< 16-bit Timer, Use Group0 Channel-32 as PWM Channel #32 */
    ASDK_PWM_MODULE_CH_33,    /*!< 16-bit Timer, Use Group0 Channel-33 as PWM Channel #33 */
    ASDK_PWM_MODULE_CH_34,    /*!< 16-bit Timer, Use Group0 Channel-34 as PWM Channel #34 */
    ASDK_PWM_MODULE_CH_35,    /*!< 16-bit Timer, Use Group0 Channel-35 as PWM Channel #35 */
    ASDK_PWM_MODULE_CH_36,    /*!< 16-bit Timer, Use Group0 Channel-36 as PWM Channel #36 */
    ASDK_PWM_MODULE_CH_37,    /*!< 16-bit Timer, Use Group0 Channel-37 as PWM Channel #37 */
    ASDK_PWM_MODULE_CH_38,    /*!< 16-bit Timer, Use Group0 Channel-38 as PWM Channel #38 */
    ASDK_PWM_MODULE_CH_39,    /*!< 16-bit Timer, Use Group0 Channel-39 as PWM Channel #39 */
    ASDK_PWM_MODULE_CH_40,    /*!< 16-bit Timer, Use Group0 Channel-40 as PWM Channel #40 */
    ASDK_PWM_MODULE_CH_41,    /*!< 16-bit Timer, Use Group0 Channel-41 as PWM Channel #41 */
    ASDK_PWM_MODULE_CH_42,    /*!< 16-bit Timer, Use Group0 Channel-42 as PWM Channel #42 */
    ASDK_PWM_MODULE_CH_43,    /*!< 16-bit Timer, Use Group0 Channel-43 as PWM Channel #43 */
    ASDK_PWM_MODULE_CH_44,    /*!< 16-bit Timer, Use Group0 Channel-44 as PWM Channel #44 */
    ASDK_PWM_MODULE_CH_45,    /*!< 16-bit Timer, Use Group0 Channel-45 as PWM Channel #45 */
    ASDK_PWM_MODULE_CH_46,    /*!< 16-bit Timer, Use Group0 Channel-46 as PWM Channel #46 */
    ASDK_PWM_MODULE_CH_47,    /*!< 16-bit Timer, Use Group0 Channel-47 as PWM Channel #47 */
    ASDK_PWM_MODULE_CH_48,    /*!< 16-bit Timer, Use Group0 Channel-48 as PWM Channel #48 */
    ASDK_PWM_MODULE_CH_49,    /*!< 16-bit Timer, Use Group0 Channel-49 as PWM Channel #49 */
    ASDK_PWM_MODULE_CH_50,    /*!< 16-bit Timer, Use Group0 Channel-50 as PWM Channel #50 */
    ASDK_PWM_MODULE_CH_51,    /*!< 16-bit Timer, Use Group0 Channel-51 as PWM Channel #51 */
    ASDK_PWM_MODULE_CH_52,    /*!< 16-bit Timer, Use Group0 Channel-52 as PWM Channel #52 */
    ASDK_PWM_MODULE_CH_53,    /*!< 16-bit Timer, Use Group0 Channel-53 as PWM Channel #53 */
    ASDK_PWM_MODULE_CH_54,    /*!< 16-bit Timer, Use Group0 Channel-54 as PWM Channel #54 */
    ASDK_PWM_MODULE_CH_55,    /*!< 16-bit Timer, Use Group0 Channel-55 as PWM Channel #55 */
    ASDK_PWM_MODULE_CH_56,    /*!< 16-bit Timer, Use Group0 Channel-56 as PWM Channel #56 */
    ASDK_PWM_MODULE_CH_57,    /*!< 16-bit Timer, Use Group0 Channel-57 as PWM Channel #57 */
    ASDK_PWM_MODULE_CH_58,    /*!< 16-bit Timer, Use Group0 Channel-58 as PWM Channel #58 */
    ASDK_PWM_MODULE_CH_59,    /*!< 16-bit Timer, Use Group0 Channel-59 as PWM Channel #59 */
    ASDK_PWM_MODULE_CH_60,    /*!< 16-bit Timer, Use Group0 Channel-60 as PWM Channel #60 */
    ASDK_PWM_MODULE_CH_61,    /*!< 16-bit Timer, Use Group0 Channel-61 as PWM Channel #61 */
    ASDK_PWM_MODULE_CH_62,    /*!< 16-bit Timer, Use Group0 Channel-62 as PWM Channel #62 */

    // Group 1, 12 timers
    ASDK_PWM_MODULE_CH_63, /*!< 16-bit Timer, Use Group1 Channel-0 as PWM Channel #63 */
    ASDK_PWM_MODULE_CH_64, /*!< 16-bit Timer, Use Group1 Channel-1 as PWM Channel #64 */
    ASDK_PWM_MODULE_CH_65, /*!< 16-bit Timer, Use Group1 Channel-2 as PWM Channel #65 */
    ASDK_PWM_MODULE_CH_66, /*!< 16-bit Timer, Use Group1 Channel-3 as PWM Channel #66 */
    ASDK_PWM_MODULE_CH_67, /*!< 16-bit Timer, Use Group1 Channel-4 as PWM Channel #67 */
    ASDK_PWM_MODULE_CH_68, /*!< 16-bit Timer, Use Group1 Channel-5 as PWM Channel #68 */
    ASDK_PWM_MODULE_CH_69, /*!< 16-bit Timer, Use Group1 Channel-6 as PWM Channel #69 */
    ASDK_PWM_MODULE_CH_70, /*!< 16-bit Timer, Use Group1 Channel-7 as PWM Channel #70 */
    ASDK_PWM_MODULE_CH_71, /*!< 16-bit Timer, Use Group1 Channel-8 as PWM Channel #71 */
    ASDK_PWM_MODULE_CH_72, /*!< 16-bit Timer, Use Group1 Channel-9 as PWM Channel #72 */
    ASDK_PWM_MODULE_CH_73, /*!< 16-bit Timer, Use Group1 Channel-10 as PWM Channel #73 */
    ASDK_PWM_MODULE_CH_74, /*!< 16-bit Timer, Use Group1 Channel-11 as PWM Channel #74 */

    // Group 2, 4 timers
    ASDK_PWM_MODULE_CH_75, /*!< 32-bit Timer, Use Group2 Channel-0 as PWM Channel #75 */
    ASDK_PWM_MODULE_CH_76, /*!< 32-bit Timer, Use Group2 Channel-1 as PWM Channel #76 */
    ASDK_PWM_MODULE_CH_77, /*!< 32-bit Timer, Use Group2 Channel-2 as PWM Channel #77 */
    ASDK_PWM_MODULE_CH_78, /*!< 32-bit Timer, Use Group2 Channel-3 as PWM Channel #78 */

    ASDK_PWM_MODULE_CH_MAX /*!< Total number of PWM channels. */
}asdk_pwm_channel_t;

/*!
 * @brief An enumerator to represent number of I2C modules.
 *
 * @note: Maximum of 8 I2C modules are supported.
 */
typedef enum
{
    ASDK_I2C_0 = 0, /*!< Use SCB0 as I2C #0 */
    ASDK_I2C_1,     /*!< Use SCB1 as I2C #1 */
    ASDK_I2C_2,     /*!< Use SCB2 as I2C #2 */
    ASDK_I2C_3,     /*!< Use SCB3 as I2C #3 */
    ASDK_I2C_4,     /*!< Use SCB4 as I2C #4 */
    ASDK_I2C_5,     /*!< Use SCB5 as I2C #5 */
    ASDK_I2C_6,     /*!< Use SCB6 as I2C #6 */
    ASDK_I2C_7,     /*!< Use SCB7 as I2C #7 */
    ASDK_I2C_MAX,   /*!< Max number of I2C channels */
    ASDK_I2C_UNDEFINED = ASDK_I2C_MAX, /*!< Values beyond @ref ASDK_I2C_MAX is undefined. */
}asdk_i2c_num_t;

/*!
 * @brief An enumerator to represent number of SPI modules.
 *
 */
typedef enum
{
    ASDK_SPI_0 = 0, /*!< Use SCB0 as SPI #0 */
    ASDK_SPI_1,     /*!< Use SCB1 as SPI #1 */
    ASDK_SPI_2,     /*!< Use SCB2 as SPI #2 */
    ASDK_SPI_3,     /*!< Use SCB3 as SPI #3 */
    ASDK_SPI_4,     /*!< Use SCB4 as SPI #4 */
    ASDK_SPI_5,     /*!< Use SCB5 as SPI #5 */
    ASDK_SPI_6,     /*!< Use SCB6 as SPI #6 */
    ASDK_SPI_7,     /*!< Use SCB6 as SPI #7 */
    ASDK_SPI_MAX,   /*!< Max number of SPI channels */
    ASDK_SPI_UNDEFINED = ASDK_SPI_MAX,  /*!< Values beyond @ref ASDK_SPI_MAX is undefined. */
} asdk_spi_num_t;

/*!
 * @brief An enumerator to represent number of UART modules.
 *
 * @note: Maximum of 8 UART modules are supported.
 */
typedef enum
{
    ASDK_UART_0 = 0,    /*!< Use SCB0 as UART #0 */
    ASDK_UART_1,        /*!< Use SCB1 as UART #1 */
    ASDK_UART_2,        /*!< Use SCB2 as UART #2 */
    ASDK_UART_3,        /*!< Use SCB3 as UART #3 */
    ASDK_UART_4,        /*!< Use SCB4 as UART #4 */
    ASDK_UART_5,        /*!< Use SCB5 as UART #5 */
    ASDK_UART_6,        /*!< Use SCB6 as UART #6 */
    ASDK_UART_7,        /*!< Use SCB7 as UART #7 */
    ASDK_UART_MAX,      /*!< Max number of UART channels */
    ASDK_UART_UNDEFINED = ASDK_UART_MAX, /*!< Values beyond @ref ASDK_UART_MAX is undefined. */
}asdk_uart_num_t;

#endif
