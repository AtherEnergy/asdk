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
    asdk_pwm.c
    
    @path
    platform/Infineon/cyt2b75/dal/src/asdk_pwm.c

    @Created on
    Jan 5, 2024

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the PWM module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

#include "asdk_pwm.h"    // ASDK Timer APIs
#include "asdk_clock.h"  // CYT2B75 DAL clock APIs
#include "asdk_pinmux.h" // CYT2B75 DAL pinmux APIs
#include "asdk_system.h" // CYT2b75 DAL system APIs


/* sdk includes ****************************** */

#include "cy_device_headers.h"  // Defines reg. and variant of CYT2B7 series
#include "tcpwm/cy_tcpwm.h"     // CYT2B75 Timer module APIs
#include "tcpwm/cy_tcpwm_pwm.h" // CYT2B75 Timer-Counter driver APIs
#include "tcpwm/cy_tcpwm_counter.h"
#include "sysint/cy_sysint.h"   // CYT2B75 system Interrupt APIs

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

enum
{
    ASDK_CYT2B75_TIMER_GROUP_0 = 0,
    ASDK_CYT2B75_TIMER_GROUP_1 = 1,
    ASDK_CYT2B75_TIMER_GROUP_2 = 2,
};


/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static void _asdk_cyt2b75_get_timer_group_and_channel(uint8_t asdk_timer_channel, uint8_t *cyt2b75_timer_group, uint8_t *cyt2b75_timer_channel);
static bool _asdk_cyt2b75_is_period_valid(uint8_t timer_group, uint32_t period);
static asdk_errorcode_t _asdk_cyt2b75_init_pwm(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_pwm_config_t *pwm_config);

uint8_t channel_number = 0;
uint8_t timer_group_number = 0;
/* ISR handlers */

// ISR handler type
typedef void (*timer_isr_t)(void);

// general ISR handler
static inline void _asdk_timer_isr_handler(volatile stc_TCPWM_GRP_CNT_t *timer_base_reg, uint8_t asdk_timer_ch);

// group 0 ISR handlers
static void group0_ch_0_isr(void);
static void group0_ch_1_isr(void);
static void group0_ch_2_isr(void);
static void group0_ch_3_isr(void);
static void group0_ch_4_isr(void);
static void group0_ch_5_isr(void);
static void group0_ch_6_isr(void);
static void group0_ch_7_isr(void);
static void group0_ch_8_isr(void);
static void group0_ch_9_isr(void);
static void group0_ch_10_isr(void);
static void group0_ch_11_isr(void);
static void group0_ch_12_isr(void);
static void group0_ch_13_isr(void);
static void group0_ch_14_isr(void);
static void group0_ch_15_isr(void);
static void group0_ch_16_isr(void);
static void group0_ch_17_isr(void);
static void group0_ch_18_isr(void);
static void group0_ch_19_isr(void);
static void group0_ch_20_isr(void);
static void group0_ch_21_isr(void);
static void group0_ch_22_isr(void);
static void group0_ch_23_isr(void);
static void group0_ch_24_isr(void);
static void group0_ch_25_isr(void);
static void group0_ch_26_isr(void);
static void group0_ch_27_isr(void);
static void group0_ch_28_isr(void);
static void group0_ch_29_isr(void);
static void group0_ch_30_isr(void);
static void group0_ch_31_isr(void);
static void group0_ch_32_isr(void);
static void group0_ch_33_isr(void);
static void group0_ch_34_isr(void);
static void group0_ch_35_isr(void);
static void group0_ch_36_isr(void);
static void group0_ch_37_isr(void);
static void group0_ch_38_isr(void);
static void group0_ch_39_isr(void);
static void group0_ch_40_isr(void);
static void group0_ch_41_isr(void);
static void group0_ch_42_isr(void);
static void group0_ch_43_isr(void);
static void group0_ch_44_isr(void);
static void group0_ch_45_isr(void);
static void group0_ch_46_isr(void);
static void group0_ch_47_isr(void);
static void group0_ch_48_isr(void);
static void group0_ch_49_isr(void);
static void group0_ch_50_isr(void);
static void group0_ch_51_isr(void);
static void group0_ch_52_isr(void);
static void group0_ch_53_isr(void);
static void group0_ch_54_isr(void);
static void group0_ch_55_isr(void);
static void group0_ch_56_isr(void);
static void group0_ch_57_isr(void);
static void group0_ch_58_isr(void);
static void group0_ch_59_isr(void);
static void group0_ch_60_isr(void);
static void group0_ch_61_isr(void);
static void group0_ch_62_isr(void);

// group 1 ISR handlers
static void group1_ch_0_isr(void);
static void group1_ch_1_isr(void);
static void group1_ch_2_isr(void);
static void group1_ch_3_isr(void);
static void group1_ch_4_isr(void);
static void group1_ch_5_isr(void);
static void group1_ch_6_isr(void);
static void group1_ch_7_isr(void);
static void group1_ch_8_isr(void);
static void group1_ch_9_isr(void);
static void group1_ch_10_isr(void);
static void group1_ch_11_isr(void);

// group 2 ISR handlers
static void group2_ch_0_isr(void);
static void group2_ch_1_isr(void);
static void group2_ch_2_isr(void);
static void group2_ch_3_isr(void);

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

static cy_stc_sysint_irq_t timer_irq_cfg = {0};
static cy_en_intr_t timer_isr_src = disconnected_IRQn;
static asdk_pwm_callback_t pwm_callback[ASDK_PWM_MODULE_CH_MAX] = {NULL};

static timer_isr_t timer_isr[ASDK_PWM_MODULE_CH_MAX] = {
    group0_ch_0_isr,
    group0_ch_1_isr,
    group0_ch_2_isr,
    group0_ch_3_isr,
    group0_ch_4_isr,
    group0_ch_5_isr,
    group0_ch_6_isr,
    group0_ch_7_isr,
    group0_ch_8_isr,
    group0_ch_9_isr,
    group0_ch_10_isr,
    group0_ch_11_isr,
    group0_ch_12_isr,
    group0_ch_13_isr,
    group0_ch_14_isr,
    group0_ch_15_isr,
    group0_ch_16_isr,
    group0_ch_17_isr,
    group0_ch_18_isr,
    group0_ch_19_isr,
    group0_ch_20_isr,
    group0_ch_21_isr,
    group0_ch_22_isr,
    group0_ch_23_isr,
    group0_ch_24_isr,
    group0_ch_25_isr,
    group0_ch_26_isr,
    group0_ch_27_isr,
    group0_ch_28_isr,
    group0_ch_29_isr,
    group0_ch_30_isr,
    group0_ch_31_isr,
    group0_ch_32_isr,
    group0_ch_33_isr,
    group0_ch_34_isr,
    group0_ch_35_isr,
    group0_ch_36_isr,
    group0_ch_37_isr,
    group0_ch_38_isr,
    group0_ch_39_isr,
    group0_ch_40_isr,
    group0_ch_41_isr,
    group0_ch_42_isr,
    group0_ch_43_isr,
    group0_ch_44_isr,
    group0_ch_45_isr,
    group0_ch_46_isr,
    group0_ch_47_isr,
    group0_ch_48_isr,
    group0_ch_49_isr,
    group0_ch_50_isr,
    group0_ch_51_isr,
    group0_ch_52_isr,
    group0_ch_53_isr,
    group0_ch_54_isr,
    group0_ch_55_isr,
    group0_ch_56_isr,
    group0_ch_57_isr,
    group0_ch_58_isr,
    group0_ch_59_isr,
    group0_ch_60_isr,
    group0_ch_61_isr,
    group0_ch_62_isr,

    group1_ch_0_isr,
    group1_ch_1_isr,
    group1_ch_2_isr,
    group1_ch_3_isr,
    group1_ch_4_isr,
    group1_ch_5_isr,
    group1_ch_6_isr,
    group1_ch_7_isr,
    group1_ch_8_isr,
    group1_ch_9_isr,
    group1_ch_10_isr,
    group1_ch_11_isr,

    group2_ch_0_isr,
    group2_ch_1_isr,
    group2_ch_2_isr,
    group2_ch_3_isr,
};

// only unused settings have been initialized
cy_stc_tcpwm_pwm_config_t cyt2b75_pwm_config =
    {
        .pwmMode = CY_TCPWM_PWM_MODE_PWM,
        .clockPrescaler = CY_TCPWM_PRESCALER_DIVBY_1,
        .debug_pause = false,
        .cc0MatchMode = CY_TCPWM_PWM_TR_CTRL2_CLEAR,
        .overflowMode = CY_TCPWM_PWM_TR_CTRL2_SET,
        .underflowMode = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
        .cc1MatchMode = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
        .deadTime = 0ul,
        .deadTimeComp = 0ul,
        .runMode = CY_TCPWM_PWM_CONTINUOUS,
        .period = 1000 - 1ul,
        .period_buff = 0ul,
        .enablePeriodSwap = false,
        .compare0 = 49,
        .compare1 = 0ul,
        .enableCompare0Swap = false,
        .enableCompare1Swap = false,
        .interruptSources = CY_TCPWM_INT_NONE,
        .invertPWMOut = 0ul,
        .invertPWMOutN = 0ul,
        .killMode = CY_TCPWM_PWM_STOP_ON_KILL,
        .switchInputMode = CY_TCPWM_INPUT_LEVEL,
        .switchInput = 0ul,
        .reloadInputMode = CY_TCPWM_INPUT_LEVEL,
        .reloadInput = 0ul,
        .startInputMode = CY_TCPWM_INPUT_LEVEL,
        .startInput = 0ul,
        .kill0InputMode = CY_TCPWM_INPUT_LEVEL,
        .kill0Input = 0ul,
        .kill1InputMode = CY_TCPWM_INPUT_LEVEL,
        .kill1Input = 0ul,
        .countInputMode = CY_TCPWM_INPUT_LEVEL,
        .countInput = 1ul,
};

asdk_pinmux_config_t pwm_pin_cfg[] = {
    {
        // .alternate_fun_id = ASDK_PINMUX_FUN_TCPWM_TRIG,
        .alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_PWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE0, 2),
        .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF,
    },
};

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

asdk_errorcode_t asdk_pwm_init(asdk_pwm_channel_t pwm_ch, asdk_pwm_config_t *pwm_config)
{
    asdk_errorcode_t pwm_status = ASDK_PWM_SUCCESS;
    static asdk_clock_peripheral_t pwm_clock_config = {0};
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    if (NULL == pwm_config)
    {
        return ASDK_PWM_ERROR_NULL_PTR;
    }

    /* validate channel */

    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    /* validate direction */

    if (ASDK_PWM_COUNT_DIRECTION_MAX <= pwm_config->direction)
    {
        return ASDK_PWM_ERROR_INVALID_COUNT_DIRECTION;
    }

    /* validate prescaler */

    /* PWM general settings*/
    pwm_clock_config.peripheral_type = ASDK_PWM;
    pwm_clock_config.target_frequency = pwm_config->pwm_clock.clock_frequency;
    pwm_clock_config.module_no = cyt2b75_timer_group;
    pwm_clock_config.ch_no = cyt2b75_timer_channel;

    pwm_status = asdk_clock_enable(&pwm_clock_config, NULL);

    if (ASDK_CLOCK_SUCCESS != pwm_status)
    {
        // ASDK_CLOCK_ERROR
        return pwm_status;
    }

    /* validate mode and initialize timer */
    pwm_status = _asdk_cyt2b75_init_pwm(pwm_ch, cyt2b75_timer_group, cyt2b75_timer_channel, pwm_config);

    return pwm_status;
}

/*This function de-initializes the PWM channel passed as parameter*/
asdk_errorcode_t asdk_pwm_deinit(asdk_pwm_channel_t pwm_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    channel_number = cyt2b75_timer_channel;
    timer_group_number = cyt2b75_timer_group;

    Cy_Tcpwm_Pwm_DeInit(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    pwm_callback[pwm_ch] = NULL;

    return ASDK_PWM_SUCCESS;
}

/*This function starts the PWM for the channel passed as parameter
Note:It is Software Based trigger start of the PWM*/
asdk_errorcode_t asdk_pwm_start(asdk_pwm_channel_t pwm_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    Cy_Tcpwm_TriggerStart(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_PWM_SUCCESS;
}

/*This function stops the PWM of the PWM channel passed as parameter*/
asdk_errorcode_t asdk_pwm_stop(asdk_pwm_channel_t pwm_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    Cy_Tcpwm_TriggerStopOrKill(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_PWM_SUCCESS;
}

/*This function updates the duty cycle of the PWM channel during runtime*/
asdk_errorcode_t asdk_pwm_set_duty(asdk_pwm_channel_t pwm_ch, uint8_t duty_cycle)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;
    uint32_t compare_val;
    uint32_t period_val;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    period_val = Cy_Tcpwm_Pwm_GetPeriod(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    if ((MAX_DUTY_CYCLE < duty_cycle) || (MIN_DUTY_CYCLE > duty_cycle))
    {
        return ASDK_PWM_ERROR_INVALID_DUTY_CYCLE;
    }
    /*Get the compare value from the duty cycle passed as parameter*/
    compare_val = (uint32_t)(((duty_cycle)*period_val) / 100);

    Cy_Tcpwm_Pwm_SetCompare0(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], compare_val);

    return ASDK_PWM_SUCCESS;
}

/*This function gives the user the current count value of the running PWM*/
asdk_errorcode_t asdk_pwm_get_counter(asdk_pwm_channel_t pwm_ch, uint32_t *counter)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    *counter = Cy_Tcpwm_Pwm_GetCounter(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_PWM_SUCCESS;
}

/*Updates the PWM Output Alignment during run time*/
asdk_errorcode_t asdk_pwm_set_output_alignment(asdk_pwm_channel_t pwm_ch, asdk_pwm_output_align_t pwm_output_align)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    /*Change the PWM output alignment as per the configuration the user has passed*/
    if (ASDK_PWM_OUTPUT_RIGHT_ALIGN == pwm_output_align)
    {
        Cy_Tcpwm_Pwm_RightAlign(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else if (ASDK_PWM_OUTPUT_RIGHT_ALIGN == pwm_output_align)
    {
        Cy_Tcpwm_Pwm_LeftAlign(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else if (ASDK_PWM_OUTPUT_RIGHT_ALIGN == pwm_output_align)
    {
        Cy_Tcpwm_Pwm_CentreAlign(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else
    {
        return ASDK_PWM_ERROR_INVALID_OUTPUT_ALIGN;
    }

    return ASDK_PWM_SUCCESS;
}

/*Updates the PWM frequency during run time*/
asdk_errorcode_t asdk_pwm_set_frequency(asdk_pwm_channel_t pwm_ch, asdk_pwm_clock_t pwm_clock, uint32_t pwm_freq_in_Hz)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;
    uint32_t pwm_period;
    bool is_valid = false;

    /* validate channel */
    if (ASDK_PWM_MODULE_CH_MAX <= pwm_ch)
    {
        return ASDK_PWM_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(pwm_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    switch (pwm_clock.prescaler)
    {
    case ASDK_PWM_PRESCALER_DIVBY_1:
        pwm_period = ((pwm_clock.clock_frequency) / (1u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_2:
        pwm_period = ((pwm_clock.clock_frequency) / (2u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_4:
        pwm_period = ((pwm_clock.clock_frequency) / (4u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_8:
        pwm_period = ((pwm_clock.clock_frequency) / (8u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_16:
        pwm_period = ((pwm_clock.clock_frequency) / (16u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_32:
        pwm_period = ((pwm_clock.clock_frequency) / (32u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_64:
        pwm_period = ((pwm_clock.clock_frequency) / (64u)) / (pwm_freq_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_128:
        pwm_period = ((pwm_clock.clock_frequency) / (128u)) / (pwm_freq_in_Hz);
        break;
    default:
        return ASDK_PWM_ERROR_INVALID_PRESCALER;
        break;
    }

    /* validate period against counter resolution */

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, pwm_period);

    if (!is_valid)
    {
        return ASDK_PWM_ERROR_INVALID_FREQUENCY;
    }

    Cy_Tcpwm_Pwm_SetPeriod(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], pwm_period);

    return ASDK_PWM_SUCCESS;
}

/* static functions ************************** */
static void _asdk_cyt2b75_get_timer_group_and_channel(asdk_pwm_channel_t asdk_timer_channel, uint8_t *cyt2b75_timer_group, uint8_t *cyt2b75_timer_channel)
{
    if (ASDK_PWM_MODULE_CH_62 >= asdk_timer_channel)
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_0;
        *cyt2b75_timer_channel = asdk_timer_channel; // 0 to 62
    }
    else if (ASDK_PWM_MODULE_CH_74 >= asdk_timer_channel)
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_1;
        *cyt2b75_timer_channel = ASDK_PWM_MODULE_CH_63 - asdk_timer_channel; // 0 to 11
    }
    else
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_2;
        *cyt2b75_timer_channel = ASDK_PWM_MODULE_CH_75 - asdk_timer_channel; // 0 to 3
    }
}

static bool _asdk_cyt2b75_is_period_valid(uint8_t timer_group, uint32_t period)
{

    // group 0 and group 1
    if (ASDK_CYT2B75_TIMER_GROUP_1 >= timer_group)
    {
        // 16-bit timer, range: 0 to 0xFFFF
        if (period <= 0xFFFFu)
            return true;
        else
            return false;
    }
    else // group 2
    {
        // 32-bit timer, range: 0 to 0xFFFFFFFF
        return true;
    }
}

static asdk_errorcode_t _asdk_cyt2b75_init_pwm(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_pwm_config_t *pwm_config)
{
    bool is_valid = false;
    uint32_t cyt_status = CY_RET_SUCCESS;
    asdk_errorcode_t pinmux_status = ASDK_PINMUX_SUCCESS;
    uint32_t compare_val = 0;
    uint32_t pwm_period = 0;

    switch (pwm_config->pwm_clock.prescaler)
    {
    case ASDK_PWM_PRESCALER_DIVBY_1:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (1u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_2:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (2u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_4:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (4u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_8:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (8u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_16:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (16u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_32:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (32u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_64:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (64u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    case ASDK_PWM_PRESCALER_DIVBY_128:
        pwm_period = ((pwm_config->pwm_clock.clock_frequency) / (128u)) / (pwm_config->pwm_frequency_in_Hz);
        break;
    default:
        return ASDK_PWM_ERROR_INVALID_PRESCALER;
        break;
    }

    /* validate period against counter resolution */

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, pwm_period);

    if (!is_valid)
    {
        return ASDK_PWM_ERROR_INVALID_FREQUENCY;
    }

    if (ASDK_PINMUX_SUCCESS != pinmux_status)
    {
        return ASDK_TIMER_ERROR_PINMUX;
    }
    /* enable interrupt */

    if (pwm_config->interrupt.enable)
    {
        timer_isr_src = tcpwm_0_interrupts_0_IRQn + asdk_timer_channel;

        // enable terminal-count interrupt
        cyt2b75_pwm_config.interruptSources = CY_TCPWM_INT_ON_TC;

        // interrupt configuration
        timer_irq_cfg.sysIntSrc = timer_isr_src;
        timer_irq_cfg.intIdx = CPUIntIdx3_IRQn;
        timer_irq_cfg.isEnabled = true;

        // initialize timer interrupt
        Cy_SysInt_InitIRQ(&timer_irq_cfg);
        Cy_SysInt_SetSystemIrqVector(timer_isr_src, timer_isr[asdk_timer_channel]);
        NVIC_SetPriority(CPUIntIdx3_IRQn, pwm_config->interrupt.priority);
        NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
        NVIC_EnableIRQ(CPUIntIdx3_IRQn);

        // assign callback handler
        if (NULL != pwm_config->callback)
        {
            pwm_callback[asdk_timer_channel] = pwm_config->callback;
        }
    }
    else
        // {
        //     cyt2b75_pwm_config.interruptSources = CY_TCPWM_INT_NONE;
        // }

        /* init timer in timer mode */

        // general settings
        cyt2b75_pwm_config.clockPrescaler = pwm_config->pwm_clock.prescaler;
    cyt2b75_pwm_config.period = pwm_period - 1;

    /*Get the compare value from the duty cycle passed as parameter*/
    compare_val = (uint32_t)(((pwm_config->pwm_duty_cycle_in_percent) * pwm_period) / 100);

    cyt2b75_pwm_config.compare0 = compare_val;

    /*Set the PWM output run mode */
    if (ASDK_PWM_RUN_MODE_ONE_SHOT == pwm_config->pwm_run_mode)
    {
        cyt2b75_pwm_config.runMode = CY_TCPWM_PWM_ONESHOT;
    }
    else if (ASDK_PWM_RUN_MODE_CONTINUOUS == pwm_config->pwm_run_mode)
    {
        cyt2b75_pwm_config.runMode = CY_TCPWM_PWM_CONTINUOUS;
    }
    else
    {
        return ASDK_PWM_ERROR_INVALID_RUN_MODE;
    }

    pwm_pin_cfg[0].pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF;

    /*Change the PWM output alignment as per the configuration the user has passed*/
    if (ASDK_PWM_OUTPUT_RIGHT_ALIGN == pwm_config->pwm_output_align)
    {
        cyt2b75_pwm_config.cc0MatchMode = CY_TCPWM_PWM_TR_CTRL2_SET;
        cyt2b75_pwm_config.overflowMode = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE;
        cyt2b75_pwm_config.underflowMode = CY_TCPWM_PWM_TR_CTRL2_CLEAR;
    }
    else if (ASDK_PWM_OUTPUT_LEFT_ALIGN == pwm_config->pwm_output_align)
    {
        cyt2b75_pwm_config.cc0MatchMode = CY_TCPWM_PWM_TR_CTRL2_CLEAR;
        cyt2b75_pwm_config.overflowMode = CY_TCPWM_PWM_TR_CTRL2_SET;
        cyt2b75_pwm_config.underflowMode = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE;
    }
    else if (ASDK_PWM_OUTPUT_CENTRE_ALIGN == pwm_config->pwm_output_align)
    {
        cyt2b75_pwm_config.cc0MatchMode = CY_TCPWM_PWM_TR_CTRL2_INVERT;
        cyt2b75_pwm_config.overflowMode = CY_TCPWM_PWM_TR_CTRL2_SET;
        cyt2b75_pwm_config.underflowMode = CY_TCPWM_PWM_TR_CTRL2_CLEAR;
    }
    else
    {
        return ASDK_PWM_ERROR_INVALID_OUTPUT_ALIGN;
    }

    if (ASDK_PWM_MODE_DEADTIME == pwm_config->pwm_mode)
    {
        cyt2b75_pwm_config.pwmMode = CY_TCPWM_PWM_MODE_DEADTIME;

        if ((ASDK_CYT2B75_TIMER_GROUP_0 == cyt2b75_timer_group) || (ASDK_CYT2B75_TIMER_GROUP_2 == cyt2b75_timer_group))
        {
            // 8-bit timer, range: 0 to 0xFF
            if ((0xFFu >= pwm_config->deadtime_config.left_side_deadtime_clocks) || (0xFFu >= pwm_config->deadtime_config.right_side_deadtime_clocks))
            {
                cyt2b75_pwm_config.deadTime = pwm_config->deadtime_config.left_side_deadtime_clocks;
                cyt2b75_pwm_config.deadTimeComp = pwm_config->deadtime_config.right_side_deadtime_clocks;
            }
            else
            {
                return ASDK_PWM_ERROR_INVALID_DEADTIME; // Return invalid deadtime
            }
        }
        else if ((ASDK_CYT2B75_TIMER_GROUP_1 == cyt2b75_timer_group))
        {
            cyt2b75_pwm_config.deadTime = pwm_config->deadtime_config.left_side_deadtime_clocks;
            cyt2b75_pwm_config.deadTimeComp = pwm_config->deadtime_config.right_side_deadtime_clocks;
        }

        /* Set the PinMux for the PWM output */
        pwm_pin_cfg[0].MCU_pin_num = pwm_config->deadtime_config.mcu_pin_compl;
        if (ASDK_CYT2B75_TIMER_GROUP_0 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE0, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_1 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE1, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_2 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE2, cyt2b75_timer_channel);
        }

        pinmux_status = asdk_set_pinmux(pwm_pin_cfg, 1);

        pwm_pin_cfg[0].MCU_pin_num = pwm_config->mcu_pin;
        if (ASDK_CYT2B75_TIMER_GROUP_0 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE0, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_1 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE1, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_2 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE2, cyt2b75_timer_channel);
        }

        pinmux_status = asdk_set_pinmux(pwm_pin_cfg, 1);
    }
    else if (ASDK_PWM_MODE_PWM == pwm_config->pwm_mode)
    {
        cyt2b75_pwm_config.pwmMode = CY_TCPWM_PWM_MODE_PWM;
        /* Set the PinMux for the PWM output */
        pwm_pin_cfg[0].MCU_pin_num = pwm_config->mcu_pin;
        if (ASDK_CYT2B75_TIMER_GROUP_0 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE0, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_1 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE1, cyt2b75_timer_channel);
        }
        else if (ASDK_CYT2B75_TIMER_GROUP_2 == cyt2b75_timer_group)
        {
            pwm_pin_cfg[0].alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM, ASDK_PINMUX_TCPWM_SUBFUN_MODULE2, cyt2b75_timer_channel);
        }
        pwm_pin_cfg[0].pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF;

        pinmux_status = asdk_set_pinmux(pwm_pin_cfg, 1);
    }
    else
    {
        return ASDK_PWM_ERROR_INVALID_MODE;
    }

    cyt_status = Cy_Tcpwm_Pwm_Init(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], &cyt2b75_pwm_config);

    if (CY_RET_SUCCESS == cyt_status)
    {
        Cy_Tcpwm_Pwm_Enable(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else
    {
        return ASDK_PWM_ERROR_INIT_FAILED;
    }

    return ASDK_PWM_SUCCESS;
}

/* ISR handlers */

static inline void _asdk_timer_isr_handler(volatile stc_TCPWM_GRP_CNT_t *timer_base_reg, uint8_t asdk_timer_ch)
{

    // terminal count event
    if (timer_base_reg->unINTR_MASKED.stcField.u1TC)
    {
        Cy_Tcpwm_Counter_ClearTC_Intr(timer_base_reg);

        if (NULL != pwm_callback[asdk_timer_ch])
        {
            pwm_callback[asdk_timer_ch](ASDK_PWM_TERMINAL_COUNT_EVENT, Cy_Tcpwm_Pwm_GetCounter(timer_base_reg));
        }
    }

    // match or rising-edge capture event
    if (timer_base_reg->unINTR_MASKED.stcField.u1CC0_MATCH)
    {
        Cy_Tcpwm_Counter_ClearCC0_Intr(timer_base_reg);

        if (NULL != pwm_callback[asdk_timer_ch])
        {
            pwm_callback[asdk_timer_ch](ASDK_PWM_MATCH_EVENT, Cy_Tcpwm_Pwm_GetCounter(timer_base_reg));
        }

        // match or rising-edge capture event
    }
}

// group 0 ISR handlers
static void group0_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT0, ASDK_PWM_MODULE_CH_0);
}

static void group0_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT1, ASDK_PWM_MODULE_CH_1);
}

static void group0_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT2, ASDK_PWM_MODULE_CH_2);
}

static void group0_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT3, ASDK_PWM_MODULE_CH_3);
}

static void group0_ch_4_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT4, ASDK_PWM_MODULE_CH_4);
}

static void group0_ch_5_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT5, ASDK_PWM_MODULE_CH_5);
}

static void group0_ch_6_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT6, ASDK_PWM_MODULE_CH_6);
}

static void group0_ch_7_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT7, ASDK_PWM_MODULE_CH_7);
}

static void group0_ch_8_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT8, ASDK_PWM_MODULE_CH_8);
}

static void group0_ch_9_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT9, ASDK_PWM_MODULE_CH_9);
}

static void group0_ch_10_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT10, ASDK_PWM_MODULE_CH_10);
}

static void group0_ch_11_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT11, ASDK_PWM_MODULE_CH_11);
}

static void group0_ch_12_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT12, ASDK_PWM_MODULE_CH_12);
}

static void group0_ch_13_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT13, ASDK_PWM_MODULE_CH_13);
}

static void group0_ch_14_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT14, ASDK_PWM_MODULE_CH_14);
}

static void group0_ch_15_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT15, ASDK_PWM_MODULE_CH_15);
}

static void group0_ch_16_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT16, ASDK_PWM_MODULE_CH_16);
}

static void group0_ch_17_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT17, ASDK_PWM_MODULE_CH_17);
}

static void group0_ch_18_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT18, ASDK_PWM_MODULE_CH_18);
}

static void group0_ch_19_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT19, ASDK_PWM_MODULE_CH_19);
}

static void group0_ch_20_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT20, ASDK_PWM_MODULE_CH_20);
}

static void group0_ch_21_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT21, ASDK_PWM_MODULE_CH_21);
}

static void group0_ch_22_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT22, ASDK_PWM_MODULE_CH_22);
}

static void group0_ch_23_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT23, ASDK_PWM_MODULE_CH_23);
}

static void group0_ch_24_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT24, ASDK_PWM_MODULE_CH_24);
}

static void group0_ch_25_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT25, ASDK_PWM_MODULE_CH_25);
}

static void group0_ch_26_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT26, ASDK_PWM_MODULE_CH_26);
}

static void group0_ch_27_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT27, ASDK_PWM_MODULE_CH_27);
}

static void group0_ch_28_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT28, ASDK_PWM_MODULE_CH_28);
}

static void group0_ch_29_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT29, ASDK_PWM_MODULE_CH_29);
}

static void group0_ch_30_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT30, ASDK_PWM_MODULE_CH_30);
}

static void group0_ch_31_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT31, ASDK_PWM_MODULE_CH_31);
}

static void group0_ch_32_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT32, ASDK_PWM_MODULE_CH_32);
}

static void group0_ch_33_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT33, ASDK_PWM_MODULE_CH_33);
}

static void group0_ch_34_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT34, ASDK_PWM_MODULE_CH_34);
}

static void group0_ch_35_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT35, ASDK_PWM_MODULE_CH_35);
}

static void group0_ch_36_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT36, ASDK_PWM_MODULE_CH_36);
}

static void group0_ch_37_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT37, ASDK_PWM_MODULE_CH_37);
}

static void group0_ch_38_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT38, ASDK_PWM_MODULE_CH_38);
}

static void group0_ch_39_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT39, ASDK_PWM_MODULE_CH_39);
}

static void group0_ch_40_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT40, ASDK_PWM_MODULE_CH_40);
}

static void group0_ch_41_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT41, ASDK_PWM_MODULE_CH_41);
}

static void group0_ch_42_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT42, ASDK_PWM_MODULE_CH_42);
}

static void group0_ch_43_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT43, ASDK_PWM_MODULE_CH_43);
}

static void group0_ch_44_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT44, ASDK_PWM_MODULE_CH_44);
}

static void group0_ch_45_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT45, ASDK_PWM_MODULE_CH_45);
}

static void group0_ch_46_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT46, ASDK_PWM_MODULE_CH_46);
}

static void group0_ch_47_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT47, ASDK_PWM_MODULE_CH_47);
}

static void group0_ch_48_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT48, ASDK_PWM_MODULE_CH_48);
}

static void group0_ch_49_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT49, ASDK_PWM_MODULE_CH_49);
}

static void group0_ch_50_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT50, ASDK_PWM_MODULE_CH_50);
}

static void group0_ch_51_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT51, ASDK_PWM_MODULE_CH_51);
}

static void group0_ch_52_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT52, ASDK_PWM_MODULE_CH_52);
}

static void group0_ch_53_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT53, ASDK_PWM_MODULE_CH_53);
}

static void group0_ch_54_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT54, ASDK_PWM_MODULE_CH_54);
}

static void group0_ch_55_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT55, ASDK_PWM_MODULE_CH_55);
}

static void group0_ch_56_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT56, ASDK_PWM_MODULE_CH_56);
}

static void group0_ch_57_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT57, ASDK_PWM_MODULE_CH_57);
}

static void group0_ch_58_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT58, ASDK_PWM_MODULE_CH_58);
}

static void group0_ch_59_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT59, ASDK_PWM_MODULE_CH_59);
}

static void group0_ch_60_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT60, ASDK_PWM_MODULE_CH_60);
}

static void group0_ch_61_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT61, ASDK_PWM_MODULE_CH_61);
}

static void group0_ch_62_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT62, ASDK_PWM_MODULE_CH_62);
}

// group 1 ISR handlers
static void group1_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT0, ASDK_PWM_MODULE_CH_63);
}

static void group1_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT1, ASDK_PWM_MODULE_CH_64);
}

static void group1_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT2, ASDK_PWM_MODULE_CH_65);
}

static void group1_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT3, ASDK_PWM_MODULE_CH_66);
}

static void group1_ch_4_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT4, ASDK_PWM_MODULE_CH_67);
}

static void group1_ch_5_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT5, ASDK_PWM_MODULE_CH_68);
}

static void group1_ch_6_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT6, ASDK_PWM_MODULE_CH_69);
}

static void group1_ch_7_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT7, ASDK_PWM_MODULE_CH_70);
}

static void group1_ch_8_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT8, ASDK_PWM_MODULE_CH_71);
}

static void group1_ch_9_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT9, ASDK_PWM_MODULE_CH_72);
}

static void group1_ch_10_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT10, ASDK_PWM_MODULE_CH_73);
}

static void group1_ch_11_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT11, ASDK_PWM_MODULE_CH_74);
}

// group 2 ISR handlers
static void group2_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT0, ASDK_PWM_MODULE_CH_75);
}

static void group2_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT1, ASDK_PWM_MODULE_CH_76);
}

static void group2_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT2, ASDK_PWM_MODULE_CH_77);
}

static void group2_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT3, ASDK_PWM_MODULE_CH_78);
}
