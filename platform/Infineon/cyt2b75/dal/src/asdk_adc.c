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
    asdk_adc.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_adc.c

    @Created on
    Jun 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the ADC module for CYT2B75 microcontroller.

*/
#define CYT2B75CAE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
// dal includes
#include "asdk_mcu_pins.h"
#include "asdk_error.h"
#include "asdk_adc.h"
#include "asdk_clock.h"
#include "asdk_pinmux.h"

// sdk includes
#include "cy_device_headers.h"
#include "adc/cy_adc.h"
#include "sysint/cy_sysint.h"
#include "cy_device_headers.h"
#include "cy_project.h"

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

#ifndef ASDK_ADC_MAX_TOTAL_CHANNELS
#define ASDK_ADC_MAX_TOTAL_CHANNELS 32
#endif

#ifndef ASDK_ADC_MAX_GROUP_COUNT
#define ASDK_ADC_MAX_GROUP_COUNT 5
#endif

#define ASDK_ADC_INVALID_CHANNEL (cy_en_adc_pin_address_t)0xFFu
#define ASDK_ADC_INVALID_VALUE 0xFFFFu
#define ADC_MAX_CH_NO 32u
#define MAX_ADC_MOD 3u

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
typedef enum
{
    ASDK_ADC_CYT2B7_MODULE_0 = 0u,
    ASDK_ADC_CYT2B7_MODULE_1,
    ASDK_ADC_CYT2B7_MODULE_2,

    ASDK_ADC_CYT2B7_MODULE_MAX,
    ASDK_ADC_CYT2B7_MODULE_INVALID = ASDK_ADC_CYT2B7_MODULE_MAX
} asdk_adc_module_t;

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/
typedef struct
{
    asdk_mcu_pin_t mcu_pin;
    asdk_adc_module_t adc_module;
    cy_en_adc_pin_address_t adc_channel;
} asdk_adc_pin_map_t;

typedef struct
{
    asdk_mcu_pin_t *grp_pins;
    uint8_t pin_count;
    asdk_adc_conversion_status_t grp_status;
    asdk_adc_callback_fn_t grp_callback;
} asdk_adc_grp_private_handle_t;

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

static asdk_mcu_pin_t asdk_adc_active_pins[ASDK_ADC_MAX_TOTAL_CHANNELS];
static bool is_adc_params_init = false;
static double asdk_adc_module_clock_hz = 0;

static asdk_adc_callback_fn_t asdk_adc_application_callback = NULL;

// Base pointer for the SAR ADC Modules
volatile stc_PASS_SAR_t *const asdk_adc_module_cyt2b7[] = {PASS0_SAR0, PASS0_SAR1, PASS0_SAR2};

static const asdk_adc_pin_map_t adc_pin_map[] = {
    {MCU_PIN_0, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_1, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_2, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_3, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_4, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_5, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_6, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_7, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_8, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_9, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_10, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_11, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_12, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_13, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_14, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_15, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_16, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_17, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_18, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN0},
    {MCU_PIN_19, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN1},
    {MCU_PIN_20, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN2},
    {MCU_PIN_21, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN3},
    {MCU_PIN_22, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN4},
    {MCU_PIN_23, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN5},

    {MCU_PIN_24, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_25, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_26, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_27, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_28, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_29, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN8},
    {MCU_PIN_30, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN9},
    {MCU_PIN_31, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN10},
    {MCU_PIN_32, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN11},
    {MCU_PIN_33, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN12},
    {MCU_PIN_34, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN13},

    {MCU_PIN_35, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_36, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN16},
    {MCU_PIN_37, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_AN17},

    {MCU_PIN_38, ASDK_ADC_CYT2B7_MODULE_0, CY_ADC_PIN_ADDRESS_VMOTOR},
    {MCU_PIN_39, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_VMOTOR},
    {MCU_PIN_40, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_VMOTOR},

    {MCU_PIN_41, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_42, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_43, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_44, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_45, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN4},
    {MCU_PIN_46, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN5},
    {MCU_PIN_47, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN6},
    {MCU_PIN_48, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN7},
    {MCU_PIN_49, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN8},

    {MCU_PIN_50, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_51, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_52, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN12},
    {MCU_PIN_53, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN13},
    {MCU_PIN_54, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN14},
    {MCU_PIN_55, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN15},
    {MCU_PIN_56, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN16},
    {MCU_PIN_57, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN17},
    {MCU_PIN_58, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN18},
    {MCU_PIN_59, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN19},
    {MCU_PIN_60, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN20},
    {MCU_PIN_61, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN21},
    {MCU_PIN_62, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN22},
    {MCU_PIN_63, ASDK_ADC_CYT2B7_MODULE_1, CY_ADC_PIN_ADDRESS_AN23},

    {MCU_PIN_64, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_65, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_66, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},

    {MCU_PIN_67, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN0},
    {MCU_PIN_68, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN1},
    {MCU_PIN_69, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN2},
    {MCU_PIN_70, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN3},
    {MCU_PIN_71, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN4},
    {MCU_PIN_72, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN5},
    {MCU_PIN_73, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN6},
    {MCU_PIN_74, ASDK_ADC_CYT2B7_MODULE_2, CY_ADC_PIN_ADDRESS_AN7},

    {MCU_PIN_75, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_76, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_77, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_78, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_79, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_80, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_81, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_82, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_83, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_84, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_85, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_86, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_87, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_88, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_89, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_90, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_91, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_92, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_93, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_94, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_95, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_96, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_97, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_98, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL},
    {MCU_PIN_99, ASDK_ADC_CYT2B7_MODULE_INVALID, ASDK_ADC_INVALID_CHANNEL}};

static asdk_adc_grp_private_handle_t adc_grp_handle[ASDK_ADC_MAX_GROUP_COUNT];

/* Initilization of the config structure of an ADC like SAR0 */
/* Configuration structure of a ADC */
cy_stc_adc_config_t adcConfig = {
    .preconditionTime = 0u,
    .powerupTime = 0u,
    .enableIdlePowerDown = false,
    .msbStretchMode = CY_ADC_MSB_STRETCH_MODE_1CYCLE,
    .enableHalfLsbConv = 0u,
    .sarMuxEnable = true,
    .adcEnable = true,
    .sarIpEnable = true,
};

/* Configuration of ADC channel */
cy_stc_adc_channel_config_t adcChannelConfig = {
    .triggerSelection = CY_ADC_TRIGGER_OFF,
    .channelPriority = 0u,
    .preenptionType = CY_ADC_PREEMPTION_FINISH_RESUME,
    .isGroupEnd = true,
    .doneLevel = CY_ADC_DONE_LEVEL_PULSE,
    .pinAddress = ((cy_en_adc_pin_address_t)13), // BB_POTI_ANALOG_INPUT_NO,
    .portAddress = CY_ADC_PORT_ADDRESS_SARMUX0,
    .extMuxSelect = 0u,
    .extMuxEnable = true,
    .preconditionMode = CY_ADC_PRECONDITION_MODE_OFF,
    .overlapDiagMode = CY_ADC_OVERLAP_DIAG_MODE_OFF,
    .sampleTime = 120, // 13, // ADC_CH_SAMPLE_TIME,//11,//112,//12,//8,//ADC_CH_SAMPLE_TIME,
    .calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_REGULAR,
    .postProcessingMode = CY_ADC_POST_PROCESSING_MODE_NONE,
    .resultAlignment = CY_ADC_RESULT_ALIGNMENT_RIGHT,
    .signExtention = CY_ADC_SIGN_EXTENTION_UNSIGNED,
    .averageCount = 0u,
    .rightShift = 0u,
    .rangeDetectionMode = CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,
    .rangeDetectionLoThreshold = 0x0000u,
    .rangeDetectionHiThreshold = 0x0FFFu,
    .mask.grpDone = true,
    .mask.grpCancelled = false,
    .mask.grpOverflow = false,
    .mask.chRange = false,
    .mask.chPulse = false,
    .mask.chOverflow = false,
};

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static asdk_errorcode_t asdk_adc_config_input_check(asdk_adc_config_t *adc_config);

static asdk_errorcode_t asdk_adc_get_module_info(asdk_mcu_pin_t pin_num,
                                                 asdk_adc_module_t *adc_module,
                                                 cy_en_adc_pin_address_t *adc_channel);

static asdk_errorcode_t asdk_adc_group_init(asdk_adc_config_t *adc_config);

static asdk_errorcode_t asdk_adc_single_init(asdk_adc_config_t *adc_config);

static asdk_errorcode_t asdk_adc_module_init(asdk_adc_module_t adc_module_num);

static asdk_errorcode_t asdk_adc_channel_init(asdk_adc_module_t adc_module_num,
                                              cy_en_adc_pin_address_t channel_num,
                                              bool enable_group);

static asdk_errorcode_t asdk_adc_irq_init(asdk_adc_module_t adc_module_num,
                                          cy_en_adc_pin_address_t channel_num,
                                          uint8_t isr_priority,
                                          bool enable_group);

static asdk_errorcode_t asdk_adc_channel_enable(asdk_adc_module_t adc_module_num,
                                                cy_en_adc_pin_address_t channel_num);

/* ISR */
static void ADC0_ISR(void);
static void ADC1_ISR(void);
static void ADC2_ISR(void);

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/


asdk_errorcode_t asdk_adc_init(asdk_adc_config_t *adc_config)
{
    asdk_errorcode_t error_code = ASDK_SUCCESS;

    /* check inputs */
    error_code = asdk_adc_config_input_check(adc_config);
    if (ASDK_SUCCESS != error_code)
    {
        return error_code;
    }

    /* check if unsupported feature requested - hw trigger */
    if (true == adc_config->enable_hw_trigger)
    {
        return ASDK_ADC_ERROR_HW_TRIG_NOT_SUPPORTED;
    }

    /* check if unsupported feature requested - dma */
    if (true == adc_config->enable_dma)
    {
        return ASDK_ADC_ERROR_DMA_TRIG_NOT_SUPPORTED;
    }

    /* check if group conversion enabled */
    if (true == adc_config->enable_group)
    {
        error_code = asdk_adc_group_init(adc_config);
    }
    else
    {
        error_code = asdk_adc_single_init(adc_config);
    }

    return error_code;
}

/*! Install the global callback for ADC events (except group callback). */
asdk_errorcode_t asdk_adc_install_callback(asdk_adc_callback_fn_t adc_callback)
{
    if (NULL == adc_callback)
    {
        return ASDK_ADC_ERROR_INVALID_CALLBACK;
    }

    /* assign callback */
    asdk_adc_application_callback = adc_callback;

    return ASDK_SUCCESS;
}

/*! This is the routine funtion of ADC, called from ASDK primary routine handler. */
asdk_errorcode_t asdk_adc_routine(uint32_t *global_tick_count)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}



asdk_errorcode_t asdk_adc_deinit(asdk_mcu_pin_t *pin_num, uint8_t pin_count)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! This function de-initializes the ADC Group. */
asdk_errorcode_t asdk_adc_group_deinit(void *adc_group_instance)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! This function returns the current config of ADC Pin/Channel. */
asdk_errorcode_t asdk_adc_read_configs(asdk_mcu_pin_t pin_num, asdk_adc_config_t *config)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/* ************************************************************************** *
 *                  Single channel/pin conversion APIs                        *
 * ************************************************************************** */

/*! This function will sample one channel of ADC in blocking mode. */
asdk_errorcode_t asdk_adc_start_conversion_blocking(asdk_mcu_pin_t pin_num, uint32_t *adc_data)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! This function will start conversion of one channel in non blocking mode. */
asdk_errorcode_t asdk_adc_start_conversion_non_blocking(asdk_mcu_pin_t pin_num)
{
    asdk_errorcode_t error_code = ASDK_SUCCESS;
    cy_en_adc_status_t cy_error_code = CY_ADC_SUCCESS;
    asdk_adc_module_t adc_module_num = ASDK_ADC_CYT2B7_MODULE_INVALID;
    cy_en_adc_pin_address_t adc_channel_num = ASDK_ADC_INVALID_CHANNEL;
    cy_stc_adc_group_status_t asdk_adc_grpstatus_cyt2b7;

    /* get channel number and adc number */
    error_code = asdk_adc_get_module_info(pin_num, &adc_module_num, &adc_channel_num);
    if (ASDK_SUCCESS != error_code)
    {
        return error_code;
    }

    cy_error_code = Cy_Adc_Channel_GetGroupStatus(&asdk_adc_module_cyt2b7[adc_module_num]->CH[adc_channel_num], &asdk_adc_grpstatus_cyt2b7);
    if ((CY_ADC_SUCCESS != cy_error_code) || (false != asdk_adc_grpstatus_cyt2b7.grpBusy))
    {
        return ASDK_ADC_ERROR_CONVERSION_FAIL;
    }

    /* Issue SW trigger */
    Cy_Adc_Channel_SoftwareTrigger(&asdk_adc_module_cyt2b7[adc_module_num]->CH[adc_channel_num]);

    return ASDK_SUCCESS;
}

/*! Get the status of ADC non blocking conversion for an ADC channel. */
asdk_errorcode_t asdk_adc_get_conversion_status_non_blocking(asdk_mcu_pin_t pin_num, asdk_adc_conversion_status_t *status)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! Read the data of the converted channel of ADC. */
asdk_errorcode_t asdk_adc_read_conversion_value_non_blocking(asdk_mcu_pin_t pin_num, uint32_t *data)
{
    asdk_errorcode_t error_code = ASDK_SUCCESS;
    asdk_adc_module_t adc_module_num = ASDK_ADC_CYT2B7_MODULE_INVALID;
    cy_en_adc_pin_address_t adc_channel_num = ASDK_ADC_INVALID_CHANNEL;
    cy_stc_adc_ch_status_t asdk_adc_channel_status_cyt2b7;

    /* get channel number and adc number */
    error_code = asdk_adc_get_module_info(pin_num, &adc_module_num, &adc_channel_num);
    if (ASDK_SUCCESS != error_code)
    {
        return error_code;
    }

    /* get conversion data */
    Cy_Adc_Channel_GetResult(&asdk_adc_module_cyt2b7[adc_module_num]->CH[adc_channel_num], (uint16_t *)data, &asdk_adc_channel_status_cyt2b7);

    return ASDK_SUCCESS;
}

/* ************************************************************************** *
 *                       Group conversion APIs                                *
 * ************************************************************************** */

/*! This function will sample all the channel of the ADC group in blocking mode. */
asdk_errorcode_t asdk_adc_start_group_conversion_blocking(void *adc_group_instance, uint32_t *adc_data, uint8_t buffer_len)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! This function will start group conversion of all channel of that module. */
asdk_errorcode_t asdk_adc_start_group_conversion_non_blocking(void *adc_group_instance)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! Get the status of ADC non blocking group conversion for all ADC channel. */
asdk_errorcode_t asdk_adc_get_group_conversion_status_non_blocking(void *adc_group_instance, asdk_adc_conversion_status_t *status)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*! Read the data of the all group channel of the ADC module. */
asdk_errorcode_t asdk_adc_read_group_conversion_value_non_blocking(void *adc_group_instance, uint32_t *adc_data, uint8_t buffer_len)
{
    return ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED;
}

/*==============================================================================

                            STATIC FUNCTION DEFINITIONS

==============================================================================*/

static asdk_errorcode_t asdk_adc_config_input_check(asdk_adc_config_t *adc_config)
{
    if (NULL == adc_config)
    {
        return ASDK_ADC_ERROR_NULL_PTR;
    }

    if (0u == adc_config->pin_count)
    {
        return ASDK_ADC_ERROR_INVALID_PIN_COUNT;
    }

    if (NULL == adc_config->pin_nums)
    {
        return ASDK_ADC_ERROR_INVALID_PIN;
    }

    if (ASDK_ADC_RESOLUTION_MAX <= adc_config->adc_resolution)
    {
        return ASDK_ADC_ERROR_INVALID_RESOLUTION;
    }

    if (ASDK_ADC_REF_VOLTAGE_SRC_MAX <= adc_config->reference_voltage_src)
    {
        return ASDK_ADC_ERROR_INVALID_REFERENCE_VOLTAGE_SRC;
    }

    if (ASDK_ADC_SAMPLING_FREQ_MAX <= adc_config->adc_sampling_freq)
    {
        return ASDK_ADC_ERROR_INVALID_SAMPLE_FREQ;
    }

    if (ASDK_ADC_HW_TRG_SRC_MAX <= adc_config->hw_trigger_src)
    {
        return ASDK_ADC_ERROR_INVALID_TRIGGER_SRC;
    }

    if (NULL != adc_config->adc_group_instance)
    {
        return ASDK_ADC_ERROR_INVALID_GROUP_INSTANCE;
    }

    if ((NULL == adc_config->dma_dest) &&
        (true == adc_config->enable_dma))
    {
        return ASDK_ADC_ERROR_INVALID_DMA_DESTINATION;
    }

    if ((0U == adc_config->dma_dest_len) &&
        (true == adc_config->enable_dma))
    {
        return ASDK_ADC_ERROR_INVALID_DMA_DESTINATION_LEN;
    }

    if (true != adc_config->enable_interrupt)
    {
        return ASDK_ADC_ERROR_ISR_REQUIRED;
    }

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_get_module_info(asdk_mcu_pin_t pin_num,
                                                 asdk_adc_module_t *adc_module,
                                                 cy_en_adc_pin_address_t *adc_channel)
{
    /* check inputs */
    if (adc_pin_map[pin_num].mcu_pin != pin_num)
    {
        return ASDK_ADC_ERROR_DAL_INVALID_PIN_MAP;
    }
    if (ASDK_ADC_CYT2B7_MODULE_INVALID == adc_pin_map[pin_num].adc_module)
    {
        return ASDK_ADC_ERROR_INVALID_PIN;
    }

    *adc_module = adc_pin_map[pin_num].adc_module;
    *adc_channel = adc_pin_map[pin_num].adc_channel;

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_group_init(asdk_adc_config_t *adc_config)
{
    uint8_t free_group_handle = UINT8_MAX;
    asdk_mcu_pin_t empty_pin_index = ASDK_ADC_MAX_TOTAL_CHANNELS;

    /* get free group handle */
    for (uint8_t index = 0; index < ASDK_ADC_MAX_GROUP_COUNT; index++)
    {
        if (NULL == adc_grp_handle[index].grp_pins)
        {
            free_group_handle = index;
            break;
        }
    }

    /* check if valid group instance location not found */
    if (UINT8_MAX == free_group_handle)
    {
        return ASDK_ADC_ERROR_GROUP_HANDLE_NOT_AVAILABLE;
    }

    /* get available pin/channel index */
    for (uint8_t index = 0; index < ASDK_ADC_MAX_TOTAL_CHANNELS; index++)
    {
        if (MCU_PIN_NOT_DEFINED == asdk_adc_active_pins[index])
        {
            empty_pin_index = index;
            break;
        }
    }

    /* check if buffer space is available */
    if ((ASDK_ADC_MAX_TOTAL_CHANNELS - empty_pin_index) < adc_config->pin_count)
    {
        return ASDK_ADC_ERROR_PIN_BUFFER_NOT_AVAILABLE;
    }

    /* copy group info in the group instance */
    adc_grp_handle[free_group_handle].grp_pins = &asdk_adc_active_pins[empty_pin_index];
    adc_grp_handle[free_group_handle].pin_count = adc_config->pin_count;
    adc_grp_handle[free_group_handle].grp_callback = adc_config->grp_callback;
    adc_grp_handle[free_group_handle].grp_status = ASDK_ADC_CONVERSION_STATUS_RESET;

    /* return assigned instance */
    adc_config->adc_group_instance = &adc_grp_handle[free_group_handle];

    /* copy pin data into buffer */
    for (uint8_t index = 0; index < adc_config->pin_count; index++)
    {
        asdk_adc_active_pins[empty_pin_index] = adc_config->pin_nums[index];
        empty_pin_index++;
    }

    /* initialize adc hardware for continuous conversion */
    for (uint8_t index = 0; index <= adc_config->pin_count; index++)
    {
    }

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_single_init(asdk_adc_config_t *adc_config)
{
    asdk_errorcode_t error_code = ASDK_SUCCESS;
    asdk_adc_module_t adc_module_num = ASDK_ADC_CYT2B7_MODULE_INVALID;
    cy_en_adc_pin_address_t adc_channel_num = ASDK_ADC_INVALID_CHANNEL;
    asdk_mcu_pin_t empty_pin_index = ASDK_ADC_MAX_TOTAL_CHANNELS;

    /* check if init running for first time - reset the global variables */
    if (false == is_adc_params_init)
    {
        is_adc_params_init = true;

        /* reset all the pin num to invalid */
        for (uint8_t index = 0; index < ASDK_ADC_MAX_TOTAL_CHANNELS; index++)
        {
            asdk_adc_active_pins[index] = MCU_PIN_NOT_DEFINED;
        }
    }

    /* get available pin/channel index */
    for (uint8_t index = 0; index < ASDK_ADC_MAX_TOTAL_CHANNELS; index++)
    {
        if (MCU_PIN_NOT_DEFINED == asdk_adc_active_pins[index])
        {
            empty_pin_index = index;
            break;
        }
    }

    /* check if buffer space is available */
    if ((ASDK_ADC_MAX_TOTAL_CHANNELS - empty_pin_index) < adc_config->pin_count)
    {
        return ASDK_ADC_ERROR_PIN_BUFFER_NOT_AVAILABLE;
    }

    /* start init each channel */
    for (uint8_t index = 0; index < adc_config->pin_count; index++)
    {
        /* get channel number and adc number */
        error_code = asdk_adc_get_module_info(adc_config->pin_nums[index], &adc_module_num, &adc_channel_num);
        if (ASDK_SUCCESS != error_code)
        {
            return error_code;
        }

        /* check if pin already not initialized */
        for (uint8_t index_l = 0; index_l < ASDK_ADC_MAX_TOTAL_CHANNELS; index_l++)
        {
            /* if any of the pin already initialized, stop the init process */
            if (adc_config->pin_nums[index] == asdk_adc_active_pins[index_l])
                return ASDK_ADC_ERROR_PIN_ALREADY_INITIALIZED;
        }

        /* initialize module */
        error_code = asdk_adc_module_init(adc_module_num);
        if (ASDK_SUCCESS != error_code)
        {
            return error_code;
        }

        /* initialize channel */
        error_code = asdk_adc_channel_init(adc_module_num,
                                           adc_channel_num,
                                           adc_config->enable_group);
        if (ASDK_SUCCESS != error_code)
        {
            return error_code;
        }

        /* initialize irq */
        error_code = asdk_adc_irq_init(adc_module_num,
                                       adc_channel_num,
                                       adc_config->interrupt_priority,
                                       adc_config->enable_group);
        if (ASDK_SUCCESS != error_code)
        {
            return error_code;
        }

        /* pin mux settings */
        asdk_pinmux_config_t adc_pinmux_pins[] = {
            {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_ADC, (ASDK_PINMUX_ADC_SUBFUN_MODULE0 + adc_module_num), adc_channel_num),
             .MCU_pin_num = adc_config->pin_nums[index],
             .pull_configuration = ASDK_GPIO_PULL_TYPE_ANALOG}};

        error_code = asdk_set_pinmux(adc_pinmux_pins, (uint16_t)1U);

        if (ASDK_PINMUX_SUCCESS != error_code)
        {
            // pin_mux error
            return error_code;
        }

        /* enable channel */
        error_code = asdk_adc_channel_enable(adc_module_num,
                                             adc_channel_num);

        /* add pin number in the active pin list */
        asdk_adc_active_pins[empty_pin_index++] = adc_config->pin_nums[index];
    }

    return error_code;
}

static asdk_errorcode_t asdk_adc_module_init(asdk_adc_module_t adc_module_num)
{
    asdk_errorcode_t error_code = ASDK_SUCCESS;
    cy_en_adc_status_t cyt2b7_error_code = CY_ADC_SUCCESS;
    un_PASS_SAR_CTL_t unSarCtl = {0};
    asdk_clock_peripheral_t adc_clock_cfg = {
        .peripheral_type = ASDK_ADC,
        .module_no = adc_module_num,
        .target_frequency = 20000000,
    };

    /* check if module is already initialized */
    unSarCtl.u32Register = asdk_adc_module_cyt2b7[adc_module_num]->unCTL.u32Register;
    if (true == unSarCtl.stcField.u1ADC_EN)
    {
        /* do nothing - return success */
        return ASDK_SUCCESS;
    }

    /* configure adc module clock */
    error_code = asdk_clock_enable(&adc_clock_cfg, &asdk_adc_module_clock_hz);
    if ((ASDK_SUCCESS != error_code) && (ASDK_CLOCK_SUCCESS != error_code))
    {
        /* return failure */
        return error_code;
    }

    /* enable adc module with dafault configuration */
    cyt2b7_error_code = Cy_Adc_Init(asdk_adc_module_cyt2b7[adc_module_num], &adcConfig);
    if (CY_ADC_SUCCESS != cyt2b7_error_code)
    {
        /* return failure */
        return ASDK_ADC_ERROR_INIT_FAIL;
    }

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_channel_init(asdk_adc_module_t adc_module_num,
                                              cy_en_adc_pin_address_t channel_num,
                                              bool enable_group)
{
    cy_en_adc_status_t cyt2b7_error_code = CY_ADC_SUCCESS;

    /* config channel number */
    adcChannelConfig.pinAddress = ((cy_en_adc_pin_address_t)channel_num);

    /* initialize channel */
    cyt2b7_error_code = Cy_Adc_Channel_Init(&asdk_adc_module_cyt2b7[adc_module_num]->CH[channel_num],
                                            &adcChannelConfig);
    if (CY_ADC_SUCCESS != cyt2b7_error_code)
    {
        /* return failure */
        return ASDK_ADC_ERROR_CHANNEL_INIT_FAIL;
    }

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_irq_init(asdk_adc_module_t adc_module_num,
                                          cy_en_adc_pin_address_t channel_num,
                                          uint8_t isr_priority,
                                          bool enable_group)
{
    /* Register ADC interrupt handler and enable interrupt */
    cy_stc_sysint_irq_t irq_cfg;
    irq_cfg = (cy_stc_sysint_irq_t){
        .sysIntSrc = (cy_en_intr_t)((uint32_t)pass_0_interrupts_sar_0_IRQn + channel_num), // channel_num + 25),
        .intIdx = CPUIntIdx4_IRQn,
        .isEnabled = true,
    };

    if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_0)
    {
        irq_cfg.sysIntSrc = (cy_en_intr_t)((uint32_t)pass_0_interrupts_sar_0_IRQn + channel_num);
    }
    else if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_1)
    {
        irq_cfg.sysIntSrc = (cy_en_intr_t)((uint32_t)pass_0_interrupts_sar_32_IRQn + channel_num);
    }
    else if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_2)
    {
        irq_cfg.sysIntSrc = (cy_en_intr_t)((uint32_t)pass_0_interrupts_sar_64_IRQn + channel_num);
    }

    if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_0)
    {
        Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, ADC0_ISR);
    }
    else if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_1)
    {
        Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, ADC1_ISR);
    }
    else if (adc_module_num == ASDK_ADC_CYT2B7_MODULE_2)
    {
        Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, ADC2_ISR);
    }

    Cy_SysInt_InitIRQ(&irq_cfg);
    NVIC_SetPriority(irq_cfg.intIdx, isr_priority);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    return ASDK_SUCCESS;
}

static asdk_errorcode_t asdk_adc_channel_enable(asdk_adc_module_t adc_module_num,
                                                cy_en_adc_pin_address_t channel_num)
{
    /* Enable ADC ch. */
    Cy_Adc_Channel_Enable(&asdk_adc_module_cyt2b7[adc_module_num]->CH[channel_num]);

    return ASDK_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Function : ADC_ISR */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets ADC channel conversion value using non-blocking.

  @param void

  @return void

*/
static void ADC0_ISR(void)
{
    uint32_t channel_no;
    cy_stc_adc_interrupt_source_t intrSource = {false};
    asdk_adc_callback_t callback_params = {
        .adc_pin = MCU_PIN_NOT_DEFINED,
        .callback_reason = ASDK_ADC_CALLBACK_REASON_INVALID};

    channel_no = asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_0]->unSTATUS.stcField.u5CUR_CHAN;

    /* Get interrupt source */
    Cy_Adc_Channel_GetInterruptMaskedStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_0]->CH[channel_no], &intrSource);

    if (intrSource.grpDone)
    {
        if (asdk_adc_application_callback != NULL)
        {
            for (asdk_mcu_pin_t pin_index = MCU_PIN_0; pin_index < MCU_PIN_MAX; pin_index++)
            {
                if ((ASDK_ADC_CYT2B7_MODULE_0 == adc_pin_map[pin_index].adc_module) &&
                    (channel_no == adc_pin_map[pin_index].adc_channel))
                {
                    callback_params.adc_pin = adc_pin_map[pin_index].mcu_pin;
                    callback_params.callback_reason = ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE;
                }
            }
            asdk_adc_application_callback(callback_params);
        }
    }

    /* Clear interrupt source */
    Cy_Adc_Channel_ClearInterruptStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_0]->CH[channel_no], &intrSource);
}

static void ADC1_ISR(void)
{
    uint32_t channel_no;
    cy_stc_adc_interrupt_source_t intrSource = {false};
    asdk_adc_callback_t callback_params = {
        .adc_pin = MCU_PIN_NOT_DEFINED,
        .callback_reason = ASDK_ADC_CALLBACK_REASON_INVALID};

    channel_no = asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_1]->unSTATUS.stcField.u5CUR_CHAN;

    /* Get interrupt source */
    Cy_Adc_Channel_GetInterruptMaskedStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_1]->CH[channel_no], &intrSource);

    if (intrSource.grpDone)
    {
        if (asdk_adc_application_callback != NULL)
        {
            for (asdk_mcu_pin_t pin_index = MCU_PIN_0; pin_index < MCU_PIN_MAX; pin_index++)
            {
                if ((ASDK_ADC_CYT2B7_MODULE_1 == adc_pin_map[pin_index].adc_module) &&
                    (channel_no == adc_pin_map[pin_index].adc_channel))
                {
                    callback_params.adc_pin = adc_pin_map[pin_index].mcu_pin;
                    callback_params.callback_reason = ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE;
                }
            }
            asdk_adc_application_callback(callback_params);
        }
    }

    /* Clear interrupt source */
    Cy_Adc_Channel_ClearInterruptStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_1]->CH[channel_no], &intrSource);
}

static void ADC2_ISR(void)
{
    uint32_t channel_no;
    cy_stc_adc_interrupt_source_t intrSource = {false};
    asdk_adc_callback_t callback_params = {
        .adc_pin = MCU_PIN_NOT_DEFINED,
        .callback_reason = ASDK_ADC_CALLBACK_REASON_INVALID};

    channel_no = asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_2]->unSTATUS.stcField.u5CUR_CHAN;

    /* Get interrupt source */
    Cy_Adc_Channel_GetInterruptMaskedStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_2]->CH[channel_no], &intrSource);

    if (intrSource.grpDone)
    {

        if (asdk_adc_application_callback != NULL)
        {
            for (asdk_mcu_pin_t pin_index = MCU_PIN_0; pin_index < MCU_PIN_MAX; pin_index++)
            {
                if ((ASDK_ADC_CYT2B7_MODULE_2 == adc_pin_map[pin_index].adc_module) &&
                    (channel_no == adc_pin_map[pin_index].adc_channel))
                {
                    callback_params.adc_pin = adc_pin_map[pin_index].mcu_pin;
                    callback_params.callback_reason = ASDK_ADC_CALLBACK_REASON_CONVERSION_COMPLETE;
                }
            }
            asdk_adc_application_callback(callback_params);
        }
    }

    /* Clear interrupt source */
    Cy_Adc_Channel_ClearInterruptStatus(&asdk_adc_module_cyt2b7[ASDK_ADC_CYT2B7_MODULE_2]->CH[channel_no], &intrSource);
}
