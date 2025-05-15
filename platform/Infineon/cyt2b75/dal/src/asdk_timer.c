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
    asdk_timer.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_timer.c

    @Created on
    Sep 22, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the TIMER module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

#include "asdk_timer.h"  // ASDK Timer APIs
#include "asdk_clock.h"  // CYT2B75 DAL clock APIs
#include "asdk_pinmux.h" // CYT2B75 DAL pinmux APIs
#include "asdk_system.h" // CYT2b75 DAL system APIs

/* sdk includes ****************************** */

#include "cy_device_headers.h"      // Defines reg. and variant of CYT2B7 series
#include "tcpwm/cy_tcpwm.h"         // CYT2B75 Timer module APIs
#include "tcpwm/cy_tcpwm_counter.h" // CYT2B75 Timer-Counter driver APIs
#include "sysint/cy_sysint.h"       // CYT2B75 system Interrupt APIs

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
static bool _asdk_cyt2b75_is_prescaler_valid(asdk_clock_prescalers_t prescaler);
static bool _asdk_cyt2b75_is_period_valid(uint8_t timer_group, uint32_t period);
static asdk_errorcode_t _asdk_cyt2b75_init_timer(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer);
static asdk_errorcode_t _asdk_cyt2b75_init_timer_compare(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer);
static asdk_errorcode_t _asdk_cyt2b75_init_capture(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer);

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
static asdk_timer_callback_t timer_callbacks[ASDK_TIMER_MODULE_CH_MAX] = {NULL};
static asdk_capture_callback_t capture_callbacks[ASDK_TIMER_MODULE_CH_MAX] = {NULL};

static timer_isr_t timer_isr[ASDK_TIMER_MODULE_CH_MAX] = {
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
static cy_stc_tcpwm_counter_config_t cyt2b75_counter_config = {
    .debug_pause = 0uL,
    .compare0_buff = 0ul,
    .compare1 = 0ul,
    .compare1_buff = 0ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .reloadInputMode = CY_TCPWM_INPUT_LEVEL,
    .reloadInput = 0ul,
    .startInputMode = CY_TCPWM_INPUT_LEVEL,
    .startInput = 0ul,
    .stopInputMode = CY_TCPWM_INPUT_LEVEL,
    .stopInput = 0ul,
    .countInputMode = CY_TCPWM_INPUT_LEVEL,
    .countInput = 1ul,
    .trigger0EventCfg = CY_TCPWM_COUNTER_DISABLED,
    .trigger1EventCfg = CY_TCPWM_COUNTER_DISABLED,
};

asdk_pinmux_config_t capture_pin_cfg[] = {
    {
        // .alternate_fun_id = ASDK_PINMUX_FUN_TCPWM_TRIG,
        .alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_TCPWM_TRIG, ASDK_PINMUX_TCPWM1_CHX_SUBFUN_TRIG0, 2),
        .pull_configuration = ASDK_GPIO_PULL_TYPE_HIGHZ,
    },
};

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

asdk_errorcode_t asdk_timer_init(uint8_t timer_ch, asdk_timer_t *timer)
{
    asdk_errorcode_t timer_status = ASDK_TIMER_SUCCESS;
    static asdk_clock_peripheral_t timer_clock_config = {0};
    bool is_valid = false;
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */

    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    /* validate type */

    if (ASDK_TIMER_TYPE_MAX <= timer->type)
    {
        return ASDK_TIMER_ERROR_INVALID_TYPE;
    }

    /* validate direction */

    if (ASDK_TIMER_COUNT_DIRECTION_MAX <= timer->direction)
    {
        return ASDK_TIMER_ERROR_INVALID_COUNT_DIRECTION;
    }

    /* validate prescaler */

    is_valid = _asdk_cyt2b75_is_prescaler_valid(timer->counter_clock.prescaler);

    if (false == is_valid)
    {
        return ASDK_TIMER_ERROR_INVALID_PRESCALER;
    }

    /* assign cyt2b75 clock divider and enable it */

    timer_clock_config.peripheral_type = ASDK_TIMER;
    timer_clock_config.target_frequency = timer->counter_clock.frequency;
    timer_clock_config.module_no = cyt2b75_timer_group;
    timer_clock_config.ch_no = cyt2b75_timer_channel;

    timer_status = asdk_clock_enable(&timer_clock_config, NULL);

    if (ASDK_CLOCK_SUCCESS != timer_status)
    {
        // ASDK_CLOCK_ERROR
        return timer_status;
    }

    /* validate mode and initialize timer */

    switch (timer->mode.type)
    {
    case ASDK_TIMER_MODE_TIMER:
        timer_status = _asdk_cyt2b75_init_timer(timer_ch, cyt2b75_timer_group, cyt2b75_timer_channel, timer);
        break;

    case ASDK_TIMER_MODE_COMPARE:
        timer_status = _asdk_cyt2b75_init_timer_compare(timer_ch, cyt2b75_timer_group, cyt2b75_timer_channel, timer);
        break;

    case ASDK_TIMER_MODE_CAPTURE:
        timer_status = _asdk_cyt2b75_init_capture(timer_ch, cyt2b75_timer_group, cyt2b75_timer_channel, timer);
        break;

    default:
        timer_status = ASDK_TIMER_ERROR_INVALID_MODE;
        break;
    }

    return timer_status;
}

asdk_errorcode_t asdk_timer_deinit(uint8_t timer_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    Cy_Tcpwm_Counter_DeInit(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    capture_callbacks[timer_ch] = NULL;
    timer_callbacks[timer_ch] = NULL;

    return ASDK_TIMER_SUCCESS;
}

asdk_errorcode_t asdk_timer_start(uint8_t timer_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    Cy_Tcpwm_TriggerStart(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_TIMER_SUCCESS;
}

asdk_errorcode_t asdk_timer_stop(uint8_t timer_ch)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    Cy_Tcpwm_TriggerStopOrKill(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_TIMER_SUCCESS;
}

asdk_errorcode_t asdk_timer_get_counter(uint8_t timer_ch, uint32_t *counter)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;

    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    *counter = Cy_Tcpwm_Counter_GetCounter(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    return ASDK_TIMER_SUCCESS;
}

asdk_errorcode_t asdk_timer_get_counter_in_us(uint8_t timer_ch, asdk_timer_clock_t counter_clock, uint64_t *counter_us)
{
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;
    uint64_t count_wo_prescaler;
    uint32_t count;

    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }

    count = Cy_Tcpwm_Counter_GetCounter(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);

    count_wo_prescaler = ((uint64_t)count) << (counter_clock.prescaler - 1);

    // count_in_us = count_wo_prescaler * 1MHz / counter clk
    *counter_us = (count_wo_prescaler * 1000000ull) / (uint64_t)counter_clock.frequency;

    return ASDK_TIMER_SUCCESS;
}

asdk_errorcode_t asdk_timer_set_period(uint8_t timer_ch, uint32_t period_count){
    uint8_t cyt2b75_timer_group;
    uint8_t cyt2b75_timer_channel;
    
    /* validate channel */
    if (ASDK_TIMER_MODULE_CH_MAX <= timer_ch)
    {
        return ASDK_TIMER_ERROR_INVALID_CHANNEL;
    }
    else
    {
        // derive timer group and corresponding channel
        _asdk_cyt2b75_get_timer_group_and_channel(timer_ch, &cyt2b75_timer_group, &cyt2b75_timer_channel);
    }
    bool period_validity = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, period_count);
    if(period_validity == true){
        Cy_Tcpwm_Counter_SetPeriod(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel],period_count);
        return ASDK_TIMER_SUCCESS;
    }
    else{
        return ASDK_TIMER_ERROR_INVALID_PERIOD;
    }
}

/* static functions ************************** */

static void _asdk_cyt2b75_get_timer_group_and_channel(uint8_t asdk_timer_channel, uint8_t *cyt2b75_timer_group, uint8_t *cyt2b75_timer_channel)
{
    if (ASDK_TIMER_MODULE_CH_62 >= asdk_timer_channel)
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_0;
        *cyt2b75_timer_channel = asdk_timer_channel; // 0 to 62
    }
    else if (ASDK_TIMER_MODULE_CH_74 >= asdk_timer_channel)
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_1;
        *cyt2b75_timer_channel = asdk_timer_channel - ASDK_TIMER_MODULE_CH_63; // 0 to 11
    }
    else
    {
        *cyt2b75_timer_group = ASDK_CYT2B75_TIMER_GROUP_2;
        *cyt2b75_timer_channel = asdk_timer_channel - ASDK_TIMER_MODULE_CH_75; // 0 to 3
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

static bool _asdk_cyt2b75_is_prescaler_valid(asdk_clock_prescalers_t prescaler)
{
    if ((ASDK_CLOCK_PRESCALER_1 <= prescaler) && (ASDK_CLOCK_PRESCALER_MAX > prescaler))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static asdk_errorcode_t _asdk_cyt2b75_init_timer(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer)
{
    bool is_valid = false;
    asdk_timer_mode_timer_config_t *timer_config = &timer->mode.config.timer;
    uint32_t cyt_status = CY_RET_SUCCESS;

    /* validate period against counter resolution */

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, timer_config->timer_period);

    if (!is_valid)
    {
        return ASDK_TIMER_ERROR_INVALID_PERIOD;
    }

    /* enable interrupt */

    if (timer->interrupt.enable)
    {
        timer_isr_src = tcpwm_0_interrupts_0_IRQn + asdk_timer_channel;

        // enable terminal-count interrupt
        cyt2b75_counter_config.interruptSources = CY_TCPWM_INT_ON_TC;

        // interrupt configuration
        timer_irq_cfg.sysIntSrc = timer_isr_src;
        timer_irq_cfg.intIdx = CPUIntIdx3_IRQn;
        timer_irq_cfg.isEnabled = true;

        // initialize timer interrupt
        Cy_SysInt_InitIRQ(&timer_irq_cfg);
        Cy_SysInt_SetSystemIrqVector(timer_isr_src, timer_isr[asdk_timer_channel]);
        NVIC_SetPriority(CPUIntIdx3_IRQn, timer->interrupt.priority);
        NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
        NVIC_EnableIRQ(CPUIntIdx3_IRQn);

        // assign callback handler
        if (NULL != timer_config->callback)
        {
            timer_callbacks[asdk_timer_channel] = timer_config->callback;
        }
    }
    else
    {
        cyt2b75_counter_config.interruptSources = CY_TCPWM_INT_NONE;
    }

    /* init timer in timer mode */

    // general settings
    cyt2b75_counter_config.clockPrescaler = timer->counter_clock.prescaler - 1;
    cyt2b75_counter_config.period = timer_config->timer_period - 1;
    cyt2b75_counter_config.compareOrCapture = CY_TCPWM_COUNTER_MODE_COMPARE;
    cyt2b75_counter_config.runMode = (timer->type == ASDK_TIMER_TYPE_PERIODIC) ? CY_TCPWM_COUNTER_CONTINUOUS : CY_TCPWM_COUNTER_ONESHOT;
    cyt2b75_counter_config.countDirection = (timer->direction == ASDK_TIMER_COUNT_DIRECTION_UP) ? CY_TCPWM_COUNTER_COUNT_UP : CY_TCPWM_COUNTER_COUNT_DOWN;

    // timer mode settings
    cyt2b75_counter_config.compare0 = 0;
    cyt2b75_counter_config.capture0Input = 0;
    cyt2b75_counter_config.capture1Input = 0;
    cyt2b75_counter_config.capture0InputMode = CY_TCPWM_INPUT_LEVEL;
    cyt2b75_counter_config.capture1InputMode = CY_TCPWM_INPUT_LEVEL;

    cyt_status = Cy_Tcpwm_Counter_Init(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], &cyt2b75_counter_config);

    if (CY_RET_SUCCESS == cyt_status)
    {
        Cy_Tcpwm_Counter_Enable(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else
    {
        return ASDK_TIMER_INIT_FAILED;
    }

    return ASDK_TIMER_SUCCESS;
}

static asdk_errorcode_t _asdk_cyt2b75_init_timer_compare(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer)
{
    bool is_valid = false;
    asdk_timer_mode_compare_config_t *compare_config = &timer->mode.config.compare;
    uint32_t cyt_status = CY_RET_SUCCESS;

    /* validate period against counter resolution */

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, compare_config->timer_period);

    if (!is_valid)
    {
        return ASDK_TIMER_ERROR_INVALID_PERIOD;
    }

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, compare_config->compare_value);

    if (!is_valid)
    {
        return ASDK_TIMER_ERROR_INVALID_COMPARE_PERIOD;
    }

    /* enable interrupt */

    if (timer->interrupt.enable)
    {
        timer_isr_src = tcpwm_0_interrupts_0_IRQn + asdk_timer_channel;

        // enable both match and terminal-count interrupt
        cyt2b75_counter_config.interruptSources = CY_TCPWM_INT_ON_TC_OR_CC;

        // interrupt configuration
        timer_irq_cfg.sysIntSrc = timer_isr_src;
        timer_irq_cfg.intIdx = CPUIntIdx3_IRQn;
        timer_irq_cfg.isEnabled = true;

        // initialize timer interrupt
        Cy_SysInt_InitIRQ(&timer_irq_cfg);
        Cy_SysInt_SetSystemIrqVector(timer_isr_src, timer_isr[asdk_timer_channel]);
        NVIC_SetPriority(CPUIntIdx3_IRQn, timer->interrupt.priority);
        NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
        NVIC_EnableIRQ(CPUIntIdx3_IRQn);

        // assign callback handler
        if (NULL != compare_config->callback)
        {
            timer_callbacks[asdk_timer_channel] = compare_config->callback;
        }
    }
    else
    {
        cyt2b75_counter_config.interruptSources = CY_TCPWM_INT_NONE;
    }

    /* init timer in compare mode */

    // general settings
    cyt2b75_counter_config.clockPrescaler = timer->counter_clock.prescaler - 1;
    cyt2b75_counter_config.period = compare_config->timer_period - 1;
    cyt2b75_counter_config.compareOrCapture = CY_TCPWM_COUNTER_MODE_COMPARE;
    cyt2b75_counter_config.runMode = (timer->type == ASDK_TIMER_TYPE_PERIODIC) ? CY_TCPWM_COUNTER_CONTINUOUS : CY_TCPWM_COUNTER_ONESHOT;
    cyt2b75_counter_config.countDirection = (timer->direction == ASDK_TIMER_COUNT_DIRECTION_UP) ? CY_TCPWM_COUNTER_COUNT_UP : CY_TCPWM_COUNTER_COUNT_DOWN;

    // compare mode settings
    cyt2b75_counter_config.compare0 = compare_config->compare_value;
    cyt2b75_counter_config.capture0Input = 0;
    cyt2b75_counter_config.capture1Input = 0;
    cyt2b75_counter_config.capture0InputMode = CY_TCPWM_INPUT_LEVEL;
    cyt2b75_counter_config.capture1InputMode = CY_TCPWM_INPUT_LEVEL;

    cyt_status = Cy_Tcpwm_Counter_Init(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], &cyt2b75_counter_config);

    if (CY_RET_SUCCESS == cyt_status)
    {
        Cy_Tcpwm_Counter_Enable(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else
    {
        return ASDK_TIMER_INIT_FAILED;
    }

    return ASDK_TIMER_SUCCESS;
}

static asdk_errorcode_t _asdk_cyt2b75_init_capture(uint8_t asdk_timer_channel, uint8_t cyt2b75_timer_group, uint8_t cyt2b75_timer_channel, asdk_timer_t *timer)
{
    bool is_valid = false;
    asdk_timer_mode_capture_config_t *capture_config = &timer->mode.config.capture;
    uint32_t cyt_status = CY_RET_SUCCESS;
    asdk_errorcode_t pinmux_status = ASDK_PINMUX_SUCCESS;
    un_TCPWM_GRP_CNT_INTR_MASK_t *intr_mask = (un_TCPWM_GRP_CNT_INTR_MASK_t *)&cyt2b75_counter_config.interruptSources;

    /* validate period against counter resolution */

    is_valid = _asdk_cyt2b75_is_period_valid(cyt2b75_timer_group, capture_config->timer_period);

    if (!is_valid)
    {
        return ASDK_TIMER_ERROR_INVALID_PERIOD;
    }

    /* configure mcu pin for capture input */

    capture_pin_cfg[0].MCU_pin_num = capture_config->mcu_pin;

    pinmux_status = asdk_set_pinmux(capture_pin_cfg, 1);

    if (ASDK_PINMUX_SUCCESS != pinmux_status)
    {
        return ASDK_TIMER_ERROR_PINMUX;
    }

    /* enable interrupt */

    if (timer->interrupt.enable)
    {
        timer_isr_src = tcpwm_0_interrupts_0_IRQn + asdk_timer_channel;

        // enable terminal-count interrupt
        intr_mask->stcField.u1TC = 1;

        // interrupt configuration
        timer_irq_cfg.sysIntSrc = timer_isr_src;
        timer_irq_cfg.intIdx = CPUIntIdx3_IRQn;
        timer_irq_cfg.isEnabled = true;

        // initialize timer interrupt
        Cy_SysInt_InitIRQ(&timer_irq_cfg);
        Cy_SysInt_SetSystemIrqVector(timer_isr_src, timer_isr[asdk_timer_channel]);
        NVIC_SetPriority(CPUIntIdx3_IRQn, timer->interrupt.priority);
        NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
        NVIC_EnableIRQ(CPUIntIdx3_IRQn);

        // assign callback handler
        if (NULL != capture_config->callback)
        {
            capture_callbacks[asdk_timer_channel] = capture_config->callback;
        }
    }
    else
    {
        cyt2b75_counter_config.interruptSources = CY_TCPWM_INT_NONE;
    }

    /* init timer in capture mode */

    // general settings
    cyt2b75_counter_config.clockPrescaler = timer->counter_clock.prescaler - 1;
    cyt2b75_counter_config.period = capture_config->timer_period - 1;
    cyt2b75_counter_config.compareOrCapture = CY_TCPWM_COUNTER_MODE_CAPTURE;
    cyt2b75_counter_config.runMode = (timer->type == ASDK_TIMER_TYPE_PERIODIC) ? CY_TCPWM_COUNTER_CONTINUOUS : CY_TCPWM_COUNTER_ONESHOT;
    cyt2b75_counter_config.countDirection = (timer->direction == ASDK_TIMER_COUNT_DIRECTION_UP) ? CY_TCPWM_COUNTER_COUNT_UP : CY_TCPWM_COUNTER_COUNT_DOWN;

    // capture mode settings
    cyt2b75_counter_config.compare0 = 0;
    if (capture_config->edge == ASDK_TIMER_CAPTURE_ON_BOTH_EDGES)
    {
        cyt2b75_counter_config.capture0InputMode = CY_TCPWM_INPUT_RISING_EDGE;
        cyt2b75_counter_config.capture0Input = 2;

        cyt2b75_counter_config.capture1InputMode = CY_TCPWM_INPUT_FALLING_EDGE;
        cyt2b75_counter_config.capture1Input = 2;

        if (timer->interrupt.enable)
        {
            intr_mask->stcField.u1CC0_MATCH = 1; // rising-edge
            intr_mask->stcField.u1CC1_MATCH = 1; // falling-edge
        }
    }
    else if (capture_config->edge == ASDK_TIMER_CAPTURE_ON_RISING_EDGE)
    {
        cyt2b75_counter_config.capture0InputMode = CY_TCPWM_INPUT_RISING_EDGE;
        cyt2b75_counter_config.capture0Input = 2;

        cyt2b75_counter_config.capture1InputMode = CY_TCPWM_INPUT_LEVEL;
        cyt2b75_counter_config.capture1Input = 0;

        if (timer->interrupt.enable)
        {
            intr_mask->stcField.u1CC0_MATCH = 1; // rising-edge
            intr_mask->stcField.u1CC1_MATCH = 0; // falling-edge
        }
    }
    else // ASDK_TIMER_CAPTURE_ON_FALLING_EDGE
    {
        cyt2b75_counter_config.capture0InputMode = CY_TCPWM_INPUT_LEVEL;
        cyt2b75_counter_config.capture0Input = 0;

        cyt2b75_counter_config.capture1InputMode = CY_TCPWM_INPUT_FALLING_EDGE;
        cyt2b75_counter_config.capture1Input = 2;

        if (timer->interrupt.enable)
        {
            intr_mask->stcField.u1CC0_MATCH = 0; // rising-edge
            intr_mask->stcField.u1CC1_MATCH = 1; // falling-edge
        }
    }

    cyt_status = Cy_Tcpwm_Counter_Init(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel], &cyt2b75_counter_config);

    if (CY_RET_SUCCESS == cyt_status)
    {
        Cy_Tcpwm_Counter_Enable(&TCPWM0->GRP[cyt2b75_timer_group].CNT[cyt2b75_timer_channel]);
    }
    else
    {
        return ASDK_TIMER_INIT_FAILED;
    }

    return ASDK_TIMER_SUCCESS;
}

/* ISR handlers */

static inline void _asdk_timer_isr_handler(volatile stc_TCPWM_GRP_CNT_t *timer_base_reg, uint8_t asdk_timer_ch)
{
    // terminal count event
    if (timer_base_reg->unINTR_MASKED.stcField.u1TC)
    {
        Cy_Tcpwm_Counter_ClearTC_Intr(timer_base_reg);

        // timer and compare mode
        if (timer_base_reg->unCTRL.stcField.u3MODE == CY_TCPWM_COUNTER_MODE_COMPARE)
        {
            if (NULL != timer_callbacks[asdk_timer_ch])
            {
                timer_callbacks[asdk_timer_ch](ASDK_TIMER_TERMINAL_COUNT_EVENT);
            }
        }
        else // capture mode
        {
            if (NULL != capture_callbacks[asdk_timer_ch])
            {
                capture_callbacks[asdk_timer_ch](ASDK_TIMER_TERMINAL_COUNT_EVENT, ASDK_TIMER_CAPTURE_ON_NONE, 0);
            }
        }
    }

    // match or rising-edge capture event
    if (timer_base_reg->unINTR_MASKED.stcField.u1CC0_MATCH)
    {
        Cy_Tcpwm_Counter_ClearCC0_Intr(timer_base_reg);

        // compare mode
        if (timer_base_reg->unCTRL.stcField.u3MODE == CY_TCPWM_COUNTER_MODE_COMPARE)
        {
            if (NULL != timer_callbacks[asdk_timer_ch])
            {
                timer_callbacks[asdk_timer_ch](ASDK_TIMER_MATCH_EVENT);
            }
        }
        else // capture mode
        {
            if (NULL != capture_callbacks[asdk_timer_ch])
            {
                capture_callbacks[asdk_timer_ch](ASDK_TIMER_CAPTURE_EVENT, ASDK_TIMER_CAPTURE_ON_RISING_EDGE, Cy_Tcpwm_Counter_GetCompare0(timer_base_reg));
            }
        }
    }

    // falling-edge capture event
    if (timer_base_reg->unINTR_MASKED.stcField.u1CC1_MATCH)
    {
        Cy_Tcpwm_Counter_ClearCC1_Intr(timer_base_reg);

        // capture mode
        if (timer_base_reg->unCTRL.stcField.u3MODE == CY_TCPWM_COUNTER_MODE_CAPTURE)
        {
            if (NULL != capture_callbacks[asdk_timer_ch])
            {
                capture_callbacks[asdk_timer_ch](ASDK_TIMER_CAPTURE_EVENT, ASDK_TIMER_CAPTURE_ON_FALLING_EDGE, Cy_Tcpwm_Counter_GetCompare1(timer_base_reg));
            }
        }
    }
}

// group 0 ISR handlers
static void group0_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT0, ASDK_TIMER_MODULE_CH_0);
}

static void group0_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT1, ASDK_TIMER_MODULE_CH_1);
}

static void group0_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT2, ASDK_TIMER_MODULE_CH_2);
}

static void group0_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT3, ASDK_TIMER_MODULE_CH_3);
}

static void group0_ch_4_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT4, ASDK_TIMER_MODULE_CH_4);
}

static void group0_ch_5_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT5, ASDK_TIMER_MODULE_CH_5);
}

static void group0_ch_6_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT6, ASDK_TIMER_MODULE_CH_6);
}

static void group0_ch_7_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT7, ASDK_TIMER_MODULE_CH_7);
}

static void group0_ch_8_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT8, ASDK_TIMER_MODULE_CH_8);
}

static void group0_ch_9_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT9, ASDK_TIMER_MODULE_CH_9);
}

static void group0_ch_10_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT10, ASDK_TIMER_MODULE_CH_10);
}

static void group0_ch_11_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT11, ASDK_TIMER_MODULE_CH_11);
}

static void group0_ch_12_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT12, ASDK_TIMER_MODULE_CH_12);
}

static void group0_ch_13_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT13, ASDK_TIMER_MODULE_CH_13);
}

static void group0_ch_14_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT14, ASDK_TIMER_MODULE_CH_14);
}

static void group0_ch_15_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT15, ASDK_TIMER_MODULE_CH_15);
}

static void group0_ch_16_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT16, ASDK_TIMER_MODULE_CH_16);
}

static void group0_ch_17_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT17, ASDK_TIMER_MODULE_CH_17);
}

static void group0_ch_18_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT18, ASDK_TIMER_MODULE_CH_18);
}

static void group0_ch_19_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT19, ASDK_TIMER_MODULE_CH_19);
}

static void group0_ch_20_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT20, ASDK_TIMER_MODULE_CH_20);
}

static void group0_ch_21_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT21, ASDK_TIMER_MODULE_CH_21);
}

static void group0_ch_22_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT22, ASDK_TIMER_MODULE_CH_22);
}

static void group0_ch_23_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT23, ASDK_TIMER_MODULE_CH_23);
}

static void group0_ch_24_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT24, ASDK_TIMER_MODULE_CH_24);
}

static void group0_ch_25_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT25, ASDK_TIMER_MODULE_CH_25);
}

static void group0_ch_26_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT26, ASDK_TIMER_MODULE_CH_26);
}

static void group0_ch_27_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT27, ASDK_TIMER_MODULE_CH_27);
}

static void group0_ch_28_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT28, ASDK_TIMER_MODULE_CH_28);
}

static void group0_ch_29_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT29, ASDK_TIMER_MODULE_CH_29);
}

static void group0_ch_30_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT30, ASDK_TIMER_MODULE_CH_30);
}

static void group0_ch_31_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT31, ASDK_TIMER_MODULE_CH_31);
}

static void group0_ch_32_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT32, ASDK_TIMER_MODULE_CH_32);
}

static void group0_ch_33_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT33, ASDK_TIMER_MODULE_CH_33);
}

static void group0_ch_34_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT34, ASDK_TIMER_MODULE_CH_34);
}

static void group0_ch_35_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT35, ASDK_TIMER_MODULE_CH_35);
}

static void group0_ch_36_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT36, ASDK_TIMER_MODULE_CH_36);
}

static void group0_ch_37_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT37, ASDK_TIMER_MODULE_CH_37);
}

static void group0_ch_38_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT38, ASDK_TIMER_MODULE_CH_38);
}

static void group0_ch_39_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT39, ASDK_TIMER_MODULE_CH_39);
}

static void group0_ch_40_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT40, ASDK_TIMER_MODULE_CH_40);
}

static void group0_ch_41_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT41, ASDK_TIMER_MODULE_CH_41);
}

static void group0_ch_42_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT42, ASDK_TIMER_MODULE_CH_42);
}

static void group0_ch_43_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT43, ASDK_TIMER_MODULE_CH_43);
}

static void group0_ch_44_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT44, ASDK_TIMER_MODULE_CH_44);
}

static void group0_ch_45_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT45, ASDK_TIMER_MODULE_CH_45);
}

static void group0_ch_46_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT46, ASDK_TIMER_MODULE_CH_46);
}

static void group0_ch_47_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT47, ASDK_TIMER_MODULE_CH_47);
}

static void group0_ch_48_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT48, ASDK_TIMER_MODULE_CH_48);
}

static void group0_ch_49_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT49, ASDK_TIMER_MODULE_CH_49);
}

static void group0_ch_50_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT50, ASDK_TIMER_MODULE_CH_50);
}

static void group0_ch_51_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT51, ASDK_TIMER_MODULE_CH_51);
}

static void group0_ch_52_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT52, ASDK_TIMER_MODULE_CH_52);
}

static void group0_ch_53_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT53, ASDK_TIMER_MODULE_CH_53);
}

static void group0_ch_54_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT54, ASDK_TIMER_MODULE_CH_54);
}

static void group0_ch_55_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT55, ASDK_TIMER_MODULE_CH_55);
}

static void group0_ch_56_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT56, ASDK_TIMER_MODULE_CH_56);
}

static void group0_ch_57_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT57, ASDK_TIMER_MODULE_CH_57);
}

static void group0_ch_58_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT58, ASDK_TIMER_MODULE_CH_58);
}

static void group0_ch_59_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT59, ASDK_TIMER_MODULE_CH_59);
}

static void group0_ch_60_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT60, ASDK_TIMER_MODULE_CH_60);
}

static void group0_ch_61_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT61, ASDK_TIMER_MODULE_CH_61);
}

static void group0_ch_62_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP0_CNT62, ASDK_TIMER_MODULE_CH_62);
}

// group 1 ISR handlers
static void group1_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT0, ASDK_TIMER_MODULE_CH_63);
}

static void group1_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT1, ASDK_TIMER_MODULE_CH_64);
}

static void group1_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT2, ASDK_TIMER_MODULE_CH_65);
}

static void group1_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT3, ASDK_TIMER_MODULE_CH_66);
}

static void group1_ch_4_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT4, ASDK_TIMER_MODULE_CH_67);
}

static void group1_ch_5_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT5, ASDK_TIMER_MODULE_CH_68);
}

static void group1_ch_6_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT6, ASDK_TIMER_MODULE_CH_69);
}

static void group1_ch_7_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT7, ASDK_TIMER_MODULE_CH_70);
}

static void group1_ch_8_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT8, ASDK_TIMER_MODULE_CH_71);
}

static void group1_ch_9_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT9, ASDK_TIMER_MODULE_CH_72);
}

static void group1_ch_10_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT10, ASDK_TIMER_MODULE_CH_73);
}

static void group1_ch_11_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP1_CNT11, ASDK_TIMER_MODULE_CH_74);
}

// group 2 ISR handlers
static void group2_ch_0_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT0, ASDK_TIMER_MODULE_CH_75);
}

static void group2_ch_1_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT1, ASDK_TIMER_MODULE_CH_76);
}

static void group2_ch_2_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT2, ASDK_TIMER_MODULE_CH_77);
}

static void group2_ch_3_isr(void)
{
    _asdk_timer_isr_handler(TCPWM0_GRP2_CNT3, ASDK_TIMER_MODULE_CH_78);
}
