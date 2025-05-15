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
    asdk_clock.h

    @path
    inc/asdk_clock.h

    @Created on
    Aug 01, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the CLOCK DAL module of ASDK

    @example
    source frequency = 80,000,000 Hz
    target frequency = 921,600 Hz

    The required fractional divider value = source / target = 86.8056
    which is the desired value.

    However, the SDK driver for 24.5 bit fractional divider yields 86.78125
    because it calculates the value after decimal point to be 25 (25/32 = 0.78125)
    for the 5-bit divider.

    Hence, the effective divider value is 86.78125 and the effective frequency
    is now 921858.1203 Hz.
*/

#ifndef ASDK_CLOCK_H
#define ASDK_CLOCK_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>

/* asdk includes ***************************** */

#include "asdk_error.h"       // Defines ASDK CLOCK module error codes
#include "asdk_peripherals.h" // Defines ASDK peripheral types

/* dal includes ****************************** */

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_clock_enum_group Enumerations
 *  Lists all the enumerations used by the Clock module.
 *  @{
 */

/*! Clock source options. */
typedef enum
{
    ASDK_CLOCK_SRC_INT = 0, /*!< Use internal oscillator. */
    ASDK_CLOCK_SRC_EXT,     /*!< Use external oscillator. */

    ASDK_CLOCK_SRC_MAX,                             /*!< Indicates total number of available clock sources. */
    ASDK_CLOCK_SRC_NOT_DEFINED = ASDK_CLOCK_SRC_MAX /*!< Values beyond ASDK_CLOCK_SRC_MAX are undefined. */
} asdk_clock_source_t;

/** @} */ // end of asdk_clock_enum_group

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_clock_ds_group Data structures
 *  Lists all the data structures used by the Clock module.
 *  @{
 */

/*!
 * @brief Configuration for PLL (Phase Locked Loop) settings.
 */
typedef struct
{
    uint32_t output_frequency; /*!< The desired PLL output frequency (Hz) */
    uint32_t input_frequency;  /*!< The input frequency (Hz) to PLL circuit. */
} asdk_clock_pll_t;

/*!
 * @brief Configuration for external crystal (XTAL) if available. Refer the Techinical Reference Manual (TRM) of the microcontroller for more information.
 * Quoted from CYT2B75 TRM for your reference only:
 * "The crystal manufacturer typically provides numerical values for parameters, namely the maximum drive
 *  level (Dl), the equivalent series resistance (ESR), the ideal shunt capacitance (C0) and the parallel load capacitance (CL).
 *  These parameters can be used to calculate the transconductance (gm) and the maximum peak oscillation voltage across the
 *  crystal (Vp)."
 */
typedef struct
{
    uint8_t watchdog_trim;          /*!< Sets the crystal drive level when automatic gain control (AGC) is enabled. Refer the TRM of the microcontroller." */
    uint8_t amplitude_trim;         /*!< Amplitude trim (ATRIM) sets the crystal drive level when AGC is enabled. Refer the TRM of the microcontroller." */
    uint8_t filter_trim;            /*!< Tunes the low-pass filter between the ECO_IN pin and the amplifier, which is used to prevent amplification of harmonics of the intended crystal frequency. Refer the TRM of the microcontroller." */
    uint8_t feedback_resistor_trim; /*!< Oscillator feedback resistor. Refer the TRM of the microcontroller. */
    uint8_t gain_trim;              /*!< sets up the trim for amplifier gain based on the calculated gm. Refer the TRM of the microcontroller. */
} xtal_trim_config_t;

/*!
 * @brief The clock configuration data structure holds the necessary
 * information alone which is required to setup and enable system
 * (core-cpu) clock.
 */
typedef struct
{
    asdk_clock_source_t clk_source; /*!< The clock source. */
    asdk_clock_pll_t pll;           /*!< PLL configuration to derive High Frequency (HF) clock from the selected clock source. */
    xtal_trim_config_t xtal_trim;   /*!< Settings associated with XTAL. Refer the TRM of the microcontoller. */
} asdk_clock_config_t;

/** @} */ // end of asdk_clock_ds_group

/*!
 * @brief Peripheral clock parameters. These parameters are used by
 * asdk_clock_enable function to configure the clock for a given
 * peripheral.
 */
typedef struct
{
    asdk_peripheral_t peripheral_type;  /*!< The peripheral for which clock must be set. */
    uint8_t module_no;                  /*!< The H/W module or instance of the given peripheral. Refer the TRM of the microcontroller. */
    uint8_t ch_no;                      /*!< The channel number of the given peripheral. Refer the TRM of the microcontroller. */
    uint32_t target_frequency;          /*!< The target frequency at which the peripheral must operate. Refer the TRM of the microcontroller. */
} asdk_clock_peripheral_t;

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_clock_fun_group Functions
 *  Lists the functions/APIs from the Clock module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_clock_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the core CPU clock source and PLL.

  @param [in] clk_config Clock configuration settings.
*/
void asdk_clock_init(asdk_clock_config_t *clk_config);

/** @} */ // end of asdk_clock_fun_group

/*----------------------------------------------------------------------------*/
/* Function : asdk_clock_enable */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the peripheral clock.
  
  It internally setups the clocks tree for the given peripheral and derives the closest value
  for the clock divider. The effective frequency may vary from the desired
  target frequency hence effective_frequency is given as output parameter.

  @param [in] params Clock configuration settings.
  @param [out] effective_frequency outputs the effective clock frequency based on the given params.

  @return
    - @ref ASDK_CLOCK_SUCCESS
    - @ref ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ENABLED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ASSIGNED

  @note
  The developer must ensure that all periperal clocks yield ASDK_CLOCK_SUCCESS
  return value and verify the effective_frequency. Internally the dividers may be shared if the divider value required by two
  or more peripherals are same.

  @note
  The fractional divider is implemented by hardware in such a away that it
  rounds of the value after the decimal point in steps of 0.03125. It's
  because the 5-bit divider takes fixed values in the range of 0-31
  and the hardware divides this value by 32.
*/
asdk_errorcode_t asdk_clock_enable(asdk_clock_peripheral_t *params, double *effective_frequency_hz);

#endif /* ASDK_CLOCK_H */
