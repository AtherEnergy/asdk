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
    platform/stm32f4/dal/inc/asdk_clock_values.h

    @Created on
    Mar 23, 2023

    @Author
    Ather Energy Pvt. Ltd.

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    Choice of input and output clock frequencies for STM32F411 microcontroller.
*/

#ifndef ASDK_CLOCK_VALUES_H
#define ASDK_CLOCK_VALUES_H

/**
 *  NOTE: to the ASDK user
 *  1) Only few specific frequencies are supported for input and output of the PLL
 *  2) This is done to meet a tradeoff between space and time complexity of 
 *     calculating the PLL dividers and multipliers.
 */

/**
 *  NOTE: when external clock is chosen, pass an value from this enum to 
 *  asdk_clock_config_t.asdk_clock_pll_t.input_frequency in asdk_clock_init()
 */
typedef enum
{
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_4MHZ  = 4000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_5MHZ  = 5000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_6MHZ  = 6000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_8MHZ  = 8000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_10MHZ = 10000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_12MHZ = 12000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_16MHZ = 16000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_20MHZ = 20000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_24MHZ = 24000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_25MHZ = 25000000,
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_26MHZ = 26000000,

    /*!< Indicates total number of choices in external clock frequencies available. */
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_MAX,
    /*!< Values beyond ASDK_CLOCK_HSE_CRYSTAL_FREQ_MAX are undefined. */
    ASDK_CLOCK_HSE_CRYSTAL_FREQ_NOT_DEFINED = ASDK_CLOCK_HSE_CRYSTAL_FREQ_MAX
} asdk_hse_crystal_freq_t;

/**
 *  NOTE: when internal clock is chosen, pass an value from this enum to 
 *  asdk_clock_config_t.asdk_clock_pll_t.output_frequency in asdk_clock_init()
 */
typedef enum {
    ASDK_CLOCK_OUTPUT_FREQ_168MHZ = 168000000,

    /*!< Indicates total number of choices in output clock frequencies available. */
    ASDK_CLOCK_OUTPUT_FREQ_MAX,
    /*!< Values beyond ASDK_CLOCK_OUTPUT_FREQ_MAX are undefined. */
    ASDK_CLOCK_OUTPUT_FREQ_NOT_DEFINED = ASDK_CLOCK_OUTPUT_FREQ_MAX
} asdk_output_clock_freq_t;

#endif /* ASDK_CLOCK_VALUES_H */