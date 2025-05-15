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
    asdk_adc.h

    @path
    inc/asdk_adc.h

    @Created on
    Jun 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the ADC DAL module of ASDK.
*/

#ifndef ASDK_ADC_H
#define ASDK_ADC_H

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

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_adc_enum_group Enumerations
 *  Lists all the enumerations used by the ADC module.
 *  @{
 */

/*!
 * @brief ADC Resolution in bits.
 */
typedef enum
{
    ASDK_ADC_RESOLUTION_BITS_DEFAULT = 0u, /*!< Default value - depends on MCU. */

    ASDK_ADC_RESOLUTION_8_BITS,  /*!< ADC Resolution set to 08 bits. Value Range: 0-255 */
    ASDK_ADC_RESOLUTION_10_BITS, /*!< ADC Resolution set to 10 bits. Value Range: 0-1023 */
    ASDK_ADC_RESOLUTION_12_BITS, /*!< ADC Resolution set to 12 bits. Value Range: 0-4095 */
    ASDK_ADC_RESOLUTION_14_BITS, /*!< ADC Resolution set to 14 bits. Value Range: 0-16383 */
    ASDK_ADC_RESOLUTION_16_BITS, /*!< ADC Resolution set to 16 bits. Value Range: 0-65535 */
    ASDK_ADC_RESOLUTION_18_BITS, /*!< ADC Resolution set to 18 bits. Value Range: 0-262143 */
    ASDK_ADC_RESOLUTION_20_BITS, /*!< ADC Resolution set to 20 bits. Value Range: 0-1048575 */

    ASDK_ADC_RESOLUTION_MAX,
    ASDK_ADC_RESOLUTION_INVALID = ASDK_ADC_RESOLUTION_MAX
} asdk_adc_resolution_bits_t;

/*!
 * @brief Reference voltage selection options.
 */
typedef enum
{
    ASDK_ADC_REF_VOLTAGE_SRC_DEFAULT = 0u, /*!< Default value - depends on MCU. */

    ASDK_ADC_REF_VOLTAGE_SRC_INTERNAL, /*!< ADC Internal Reference Voltage Selected - Value depends on MCU. */
    ASDK_ADC_REF_VOLTAGE_SRC_EXTERNAL, /*!< ADC External Reference Voltage Selected - Value depends on ECU. */

    ASDK_ADC_REF_VOLTAGE_SRC_MAX,
    ASDK_ADC_REF_VOLTAGE_SRC_INVALID = ASDK_ADC_REF_VOLTAGE_SRC_MAX
} asdk_adc_ref_voltage_src_t;

/*!
 * @brief Transfer type selection options - Selects how converted value will be transferred to application.
 */
typedef enum
{
    ASDK_ADC_TRANSFER_TYPE_DEFAULT = 0u, /*!< Default value - depends on MCU. */

    ASDK_ADC_TRANSFER_TYPE_SOFTWARE, /*!< Converted value transferred using software, application needs to call read functions. */
    ASDK_ADC_TRANSFER_TYPE_DMA,      /*!< Converted value tranferred using DMA, after transfer application will receive callback (if set). */

    ASDK_ADC_TRANSFER_TYPE_MAX,
    ASDK_ADC_TRANSFER_TYPE_INVALID = ASDK_ADC_TRANSFER_TYPE_MAX
} asdk_adc_tranfer_type_t;

/*!
 * @brief Event for hardware trigger - only active when hardware trigger is selected.
 */
typedef enum
{
    ASDK_ADC_HW_TRG_SRC_PWM_INIT_EVENT,  /*!< Trigger Source set to PWM INIT Event (MOD == CNT). */
    ASDK_ADC_HW_TRG_SRC_PWM_RESET_EVENT, /*!< Tgger Source set to PWM RESET Event */
    ASDK_ADC_HW_TRG_SRC_PWM_HALF_EVENT,  /*!< Trigger Source set to PWM Half Event (MOD/2 == CNT). */

    ASDK_ADC_HW_TRG_SRC_GPIO_SET_EVENT,    /*!< Trigger Source set to GPIO Set Event. */
    ASDK_ADC_HW_TRG_SRC_GPIO_CLEAR_EVENT,  /*!< Trigger Source set to GPIO Clear Event. */
    ASDK_ADC_HW_TRG_SRC_GPIO_TOGGLE_EVENT, /*!< Trigger Source set to GPIO Toggle Event. */

    ASDK_ADC_HW_TRG_SRC_MAX,
    ASDK_ADC_HW_TRG_SRC_INVALID = ASDK_ADC_HW_TRG_SRC_MAX
} asdk_adc_hw_trig_src_t;

/*!
 * @brief Sample frequency options - Selects ADC sampling freq. in KHz.
 * 
 * @note This only sets sample time, all other time (conversion time,
 *       transfer time etc) are set to minimum and gets added to sample time.
 */
typedef enum
{
    ASDK_ADC_SAMPLING_FREQ_DEFAULT = 0u, /*!< Default value - depends on MCU. */

    ASDK_ADC_SAMPLING_FREQ_2000KHz, /*!< ADC Sample Freq is 2 MHz - 0.5 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_1000KHz, /*!< ADC Sample Freq is 1 MHz - 1 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_500KHz,  /*!< ADC Sample Freq is 500 KHz - 2 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_250KHz,  /*!< ADC Sample Freq is 250 KHz - 4 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_100KHz,  /*!< ADC Sample Freq is 100 KHz - 10 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_50KHz,   /*!< ADC Sample Freq is 50 KHz - 20 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_25KHz,   /*!< ADC Sample Freq is 25 KHz - 40 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_10KHz,   /*!< ADC Sample Freq is 10 KHz - 100 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_5KHz,    /*!< ADC Sample Freq is 5 KHz - 200 uS/sample. */
    ASDK_ADC_SAMPLING_FREQ_1KHz,    /*!< ADC Sample Freq is 1 KHz - 1000 uS/sample. */

    ASDK_ADC_SAMPLING_FREQ_MAX,
    ASDK_ADC_SAMPLING_FREQ_INVALID = ASDK_ADC_SAMPLING_FREQ_MAX
} asdk_adc_sampling_freq_t;

/*!
 * @brief Callback reasons list.
 */
typedef enum
{
    ASDK_ADC_CALLBACK_REASON_DEFAULT = 0u, /*!< Default value - depends on MCU. */

    ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE,      /*!< Conversion Complete Flag. */
    ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE_HALF, /*!< 50% Conversion Complete - in case of continuous conversion. */
    ASDK_ADC_CALLBACK_REASON_CONVERSION_ERROR,         /*!< Error in Conversion. */
    ASDK_ADC_CALLBACK_REASON_ADC_MOD_ERROR,            /*!< ADC Module Error. */

    ASDK_ADC_CALLBACK_REASON_MAX,
    ASDK_ADC_CALLBACK_REASON_INVALID = ASDK_ADC_CALLBACK_REASON_MAX
} asdk_adc_callback_reason_t;

/*!
 * @brief State/Conversion Status of ADC.
 */
typedef enum
{
    ASDK_ADC_CONVERSION_STATUS_RESET = 0, /*!< State Unknown or Not Initialized. */
    ASDK_ADC_CONVERSION_STATUS_INIT,      /*!< State Init - Conversion can be triggered. */
    ASDK_ADC_CONVERSION_STATUS_WAITING,   /*!< Conversion triggered but in wait state due to some reason (eg. another conversion active). */
    ASDK_ADC_CONVERSION_STATUS_ONGOING,   /*!< Conversion started. */
    ASDK_ADC_CONVERSION_STATUS_DONE,      /*!< Conversion done - value can be read from the registers. */

    ASDK_ADC_GROUP_STATE_MAX,
    ASDK_ADC_GROUP_STATE_INVALID = ASDK_ADC_GROUP_STATE_MAX
} asdk_adc_conversion_status_t;

/** @} */ // end of asdk_adc_enum_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_adc_ds_group Data structures
 *  Lists all the data structures used by the ADC module.
 *  @{
 */

/*!
 * @brief Structure to provide callback information to the application.
 */
typedef struct
{
    asdk_adc_callback_reason_t callback_reason; /*!< Callback reason - ex. Error or Conversion Complete. */
    asdk_mcu_pin_t adc_pin;                     /*!< ADC Pin on which ISR occured, in case of multiple Pins, first pin of seq will be passed. */
} asdk_adc_callback_t;

/** @defgroup asdk_adc_cb_group Callback function type
 *  Lists the callback functions from the ADC module.
 *  @{
 */

/*!
 * @brief ADC callback function type. Refer @ref asdk_adc_install_callback
   to install the callback.
 *
 * @param [in] info Structure to provide callback information to the application.
 */
typedef void (*asdk_adc_callback_fn_t)(asdk_adc_callback_t info);

/** @} */ // end of asdk_adc_cb_group

/*!
 * @brief Configuration for ADC (Analog Digital Conversion) Module Initialization.
 *
 * @note Avoid having two different configs for same ADC module - may not work for all MCU.
 * @note In case different channel of a module are initialized at two places, and there is a miss-match in ADC config, later one will be neglected.
 */
typedef struct
{
    /* Essential Input */
    asdk_mcu_pin_t *pin_nums; /*!< Array pointer to the list of ADC pins to be initialized with given configurations. */
    uint8_t pin_count;        /*!< Number/Count of ADC pins passed in mcu_pin_nums array.*/

    /* Basic Configs */
    asdk_adc_resolution_bits_t adc_resolution;        /*!< ADC module resolution in bits. */
    asdk_adc_ref_voltage_src_t reference_voltage_src; /*!< ADC reference voltage source - Internal or External. */
    asdk_adc_sampling_freq_t adc_sampling_freq;       /*!< ADC sampling frequency/sampling rate. */
    bool enable_adc_calibration;                      /*!< Enable internal calibration of ADC module - once for each module before channel initialization. */

    /* Interrupt configuration */
    bool enable_interrupt;      /*!< Enbale Interrupts for the ADC conversion complete - callback on common callback. */
    uint8_t interrupt_priority; /*!< Set interrupt priority. */

    /* Hardware Trigger Configs */
    bool enable_hw_trigger;                /*!< Hardware trigger enable for conversion start. */
    asdk_adc_hw_trig_src_t hw_trigger_src; /*!< Hardware source to trigger ADC conversion. */
    uint8_t hw_trigger_num;                /*!< Hardware source number to trigger ADC conversion.
                                                Ex - UART 0/1/2 or MCU PIN NUM for GPIO. */

    /* Group Configs */
    bool enable_group;                   /*!< Enable group conversion. */
    void *adc_group_instance;            /*!< Instance of the group conversion - used to trigger and read adc values. */
    asdk_adc_callback_fn_t grp_callback; /*!< Callback for ADC group events (Error/Conversion Complete). */

    /* DMA Configs */
    bool enable_dma;      /*!< DMA Enabled for data transfer - Additional Congigs in DMA module (If required). */
    uint32_t *dma_dest;   /*!< Destination address for DMA transfer - array of 32-bit. */
    uint8_t dma_dest_len; /*!< DMA destination buffer length. */
} asdk_adc_config_t;

/** @} */ // end of asdk_adc_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_adc_cb_group Callback function type
 *  Lists the callback functions from the ADC module.
 *  @{
 */

/*!
 * @brief ADC callback function type.
 *
 * @param [in] info Structure to provide callback information to the application.
 */
typedef void (*asdk_adc_callback_fn_t)(asdk_adc_callback_t info);

/** @} */ // end of asdk_adc_cb_group


/** @defgroup asdk_adc_fun_group Functions
 *  Lists the functions/APIs from the ADC module.
 *  @{
 */

/* ************************************************************************** *
 *                          Configuration APIs                                *
 * ************************************************************************** */

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_init */
/*----------------------------------------------------------------------------*/
/*!
    @brief
    This function Initializes an ADC module with all the required channel.

    @param[in] adc_config ADC configuration.

    @return
      - @ref ASDK_ADC_ERROR_HW_TRIG_NOT_SUPPORTED
      - @ref ASDK_ADC_ERROR_DMA_TRIG_NOT_SUPPORTED
      - @ref ASDK_ADC_ERROR_DMA_TRIG_NOT_SUPPORTED
      - @ref ASDK_ADC_ERROR_NULL_PTR
      - @ref ASDK_ADC_ERROR_INVALID_PIN_COUNT
      - @ref ASDK_ADC_ERROR_INVALID_PIN
      - @ref ASDK_ADC_ERROR_INVALID_RESOLUTION
      - @ref ASDK_ADC_ERROR_INVALID_REFERENCE_VOLTAGE_SRC
      - @ref ASDK_ADC_ERROR_INVALID_SAMPLE_FREQ
      - @ref ASDK_ADC_ERROR_INVALID_TRIGGER_SRC
      - @ref ASDK_ADC_ERROR_INVALID_GROUP_INSTANCE
      - @ref ASDK_ADC_ERROR_INVALID_DMA_DESTINATION
      - @ref ASDK_ADC_ERROR_INVALID_DMA_DESTINATION_LEN
      - @ref ASDK_ADC_ERROR_ISR_REQUIRED
      - @ref ASDK_SUCCESS

*/
asdk_errorcode_t asdk_adc_init(asdk_adc_config_t *adc_config);

/*! Install the global callback for ADC events (except group callback). Refer
    @ref asdk_adc_callback_fn_t to define the callback function.
 */
/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function registers the global callback for the ADC event.
  The following events are supported from the callback.
  - @ref ASDK_ADC_CALLBACK_REASON_DEFAULT
  - @ref ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE
  - @ref ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE_HALF
  - @ref ASDK_ADC_CALLBACK_REASON_CONVERSION_ERROR
  - @ref ASDK_ADC_CALLBACK_REASON_ADC_MOD_ERROR

  @param [in] adc_callback ADC callback function registerred for the callback event 

  @return
    - @ref ASDK_ADC_ERROR_INVALID_CALLBACK
    - @ref ASDK_SUCCESS
*/
asdk_errorcode_t asdk_adc_install_callback(asdk_adc_callback_fn_t adc_callback);

/*! This is the routine funtion of ADC, called from ASDK primary routine handler. */
asdk_errorcode_t asdk_adc_routine(uint32_t *global_tick_count);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_deinit */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function de-initializes an ADC module with all active channel.

    @param [in] pin_num any pin of that module.
    @param [in] pin_count pin count of the adc module

    @return
     - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_deinit(asdk_mcu_pin_t *pin_num, uint8_t pin_count);

/*! This function de-initializes the ADC Group. */
/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_group_deinit */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function de-initializes the adc channels of the ADC group

    @param [in] adc_group_instance Instance of the group conversion - used to trigger and read adc values.

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_group_deinit(void *adc_group_instance);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_read_configs */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function returns the current config of ADC Pin/Channel. .

    @param [in] pin_num Any pin of that module.
    @param [in] config ADC config data as per asdk_adc_config_t.

    @return
      - @ref  ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_read_configs(asdk_mcu_pin_t pin_num, asdk_adc_config_t *config);

/* ************************************************************************** *
 *                  Single channel/pin conversion APIs                        *
 * ************************************************************************** */

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_start_conversion_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function will sample one channel of ADC in blocking mode.

    @param [in] pin_num Any pin of that module.
    @param [out] adc_data Pointer to the data buffer where converted data will be stored.

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_start_conversion_blocking(asdk_mcu_pin_t pin_num, uint32_t *adc_data);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_start_conversion_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function will start conversion of one channel in non blocking mode.

    @param [in] pin_num Any pin of that module.

    @return
      - @ref ASDK_SUCCESS
      - @ref ASDK_ADC_ERROR_DAL_INVALID_PIN_MAP
      - @ref ASDK_ADC_ERROR_INVALID_PIN
*/
asdk_errorcode_t asdk_adc_start_conversion_non_blocking(asdk_mcu_pin_t pin_num);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_get_conversion_status_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief Get the status of ADC non blocking conversion for an ADC channel.

    @param [in] pin_num Any pin of that module.
    @param [out] status Pointer to the buffer in which the adc conversion status is returned.

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_get_conversion_status_non_blocking(asdk_mcu_pin_t pin_num, asdk_adc_conversion_status_t *status);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_read_conversion_value_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function read the data of the converted channel of ADC.

    @param [in] pin_num Any pin of that module.
    @param [out] adc_data Pointer to the data buffer where converted data will be stored.

    @return
      - @ref ASDK_SUCCESS
      - @ref ASDK_ADC_ERROR_DAL_INVALID_PIN_MAP
      - @ref ASDK_ADC_ERROR_INVALID_PIN
*/
asdk_errorcode_t asdk_adc_read_conversion_value_non_blocking(asdk_mcu_pin_t pin_num, uint32_t *data);

/* ************************************************************************** *
 *                       Group conversion APIs                                *
 * ************************************************************************** */

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_start_group_conversion_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function will sample all the channel of the ADC group in blocking mode.

    @param [in] adc_group_instance Instance of the group conversion - used to trigger and read adc values.
    @param [out] adc_data Pointer to the data buffers where converted data will be stored.
    @param [in] buffer_len Length of the data buffer

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED

*/
asdk_errorcode_t asdk_adc_start_group_conversion_blocking(void *adc_group_instance, uint32_t *adc_data, uint8_t buffer_len);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_start_group_conversion_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief This function will start group conversion of all channel of that module in non-blocking mio

    @param [in] adc_group_instance Instance of the group conversion - used to trigger and read adc values.

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED

*/
asdk_errorcode_t asdk_adc_start_group_conversion_non_blocking(void *adc_group_instance);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_get_group_conversion_status_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief Get the status of ADC non blocking group conversion for all ADC channel.

    @param [in] adc_group_instance Instance of the group conversion - used to trigger and read adc values.
    @param [out] status Pointer to the adc status buffer.

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED

*/
asdk_errorcode_t asdk_adc_get_group_conversion_status_non_blocking(void *adc_group_instance, asdk_adc_conversion_status_t *status);

/*----------------------------------------------------------------------------*/
/* Function : asdk_adc_read_group_conversion_value_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
    @brief Read the data of the all group channel of the ADC module.


    @param [in] adc_group_instance Instance of the group conversion - used to trigger and read adc values.
    @param [out] adc_data Pointer to the data buffers where converted data will be stored.
    @param [in] buffer_len Length of the data buffer

    @return
      - @ref ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_adc_read_group_conversion_value_non_blocking(void *adc_group_instance, uint32_t *adc_data, uint8_t buffer_len);

/** @} */ // end of asdk_adc_fun_group

#endif /* ASDK_ADC_H */
