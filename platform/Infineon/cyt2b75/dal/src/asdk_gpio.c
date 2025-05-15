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
    asdk_gpio.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_gpio.c

    @Created on
    Jun 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the GPIO module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

#include "asdk_platform.h"

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_gpio.h"     // ASDK GPIO APIs

/* sdk includes ****************************** */

#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "gpio/cy_gpio.h"      // CYT2B75 GPIO driver APIs
#include "sysint/cy_sysint.h"  // CYT2B75 system Interrupt APIs

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

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

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static asdk_errorcode_t get_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *state, bool read_input_state);
static asdk_errorcode_t set_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t state);
static inline asdk_errorcode_t set_mode_configuration(asdk_gpio_mode_t gpio_mode, cy_stc_gpio_pin_config_t *cy_config_out);
static inline asdk_errorcode_t set_pull_configuration(asdk_gpio_pull_t gpio_pull, cy_stc_gpio_pin_config_t *cy_config_out);
static inline asdk_errorcode_t set_speed_configuration(asdk_gpio_speed_t gpio_speed, cy_stc_gpio_pin_config_t *cy_config_out);
static inline asdk_errorcode_t set_isr(asdk_gpio_config_t *gpio_cfg, cy_stc_gpio_pin_config_t *cy_config_out);
static inline void gpio_isr(const dal_pin_t *port, cyt2b75_port_t port_no);
static void port0_isr(void);
static void port2_isr(void);
static void port3_isr(void);
static void port5_isr(void);
static void port6_isr(void);
static void port7_isr(void);
static void port8_isr(void);
static void port11_isr(void);
static void port12_isr(void);
static void port13_isr(void);
static void port14_isr(void);
static void port17_isr(void);
static void port18_isr(void);
static void port19_isr(void);
static void port21_isr(void);
static void port22_isr(void);
static void port23_isr(void);

/* volatile global variables ***************** */

/* should use 'volatile' and not 'const' otherwise
 compiler throws warnings due to SDK implementation */
volatile stc_GPIO_PRT_t *const gpio_port[] = {
    GPIO_PRT0, GPIO_PRT1, GPIO_PRT2, GPIO_PRT3,
    GPIO_PRT4, GPIO_PRT5, GPIO_PRT6, GPIO_PRT7,
    GPIO_PRT8, GPIO_PRT9, GPIO_PRT10, GPIO_PRT11,
    GPIO_PRT12, GPIO_PRT13, GPIO_PRT14, GPIO_PRT15,
    GPIO_PRT16, GPIO_PRT17, GPIO_PRT18, GPIO_PRT19,
    GPIO_PRT20, GPIO_PRT21, GPIO_PRT22, GPIO_PRT23};

/* global variables ************************** */

/* static variables ************************** */

static cy_stc_sysint_irq_t irq_cfg = {
    .intIdx = CPUIntIdx3_IRQn,
    .isEnabled = true,
};

/* GPIO module callback declaration */
static asdk_gpio_input_callback_t user_gpio_callback_fun = NULL;

/* Port mask value as required for each port for processing interrupts. */
static uint8_t port_mask[] = {
    0x0F, /* PORT_0, 4-pins */
    0x00, /* PORT_1, pins not defined */
    0x0F, /* PORT_2, 4-pins */
    0x03, /* PORT_3, 2-pins */
    0x00, /* PORT_4, pins not defined */
    0x0F, /* PORT_5, 4-pins */
    0x3F, /* PORT_6, 6-pins */
    0x3F, /* PORT_7, 6-pins */
    0x07, /* PORT_8, 3-pins */
    0x00, /* PORT_9, pins not defined */
    0x00, /* PORT_10, pins not defined */
    0x07, /* PORT_11, 3-pins */
    0x1F, /* PORT_12, 5-pins */
    0xFF, /* PORT_13, 8-pins */
    0x0F, /* PORT_14, 4-pins */
    0x00, /* PORT_15, pins not defined */
    0x00, /* PORT_16, pins not defined */
    0x07, /* PORT_17, 3-pins */
    0xFF, /* PORT_18, 8-pins */
    0x0F, /* PORT_19, 4-pins */
    0x00, /* PORT_20, pins not defined */
    0x2F, /* PORT_21, 5-pins, P21.4 not defined */
    0x0F, /* PORT_22, 4-pins */
    0x1F, /* PORT_23, 5-pins */
};

/** 
 * @var dal_pin_t pin_map
    This table maps MCU pin number to actual GPIO port and pin number.
*/
/* Look-up table to map actual MCU pin with PORT.PIN type based on datasheet.
   Used for deriving PORT.PIN when MCU pin name is known and vice-versa. */
const dal_pin_t pin_map[] = {
    {MCU_PIN_0, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_0, not defined in 100-LQFP */
    {MCU_PIN_1, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_1, not defined in 100-LQFP */

    {MCU_PIN_2, CYT2B75_GPIO_PORT_0, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_2, P0.0 */
    {MCU_PIN_3, CYT2B75_GPIO_PORT_0, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_3, P0.1 */
    {MCU_PIN_4, CYT2B75_GPIO_PORT_0, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_4, P0.2 */
    {MCU_PIN_5, CYT2B75_GPIO_PORT_0, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_5, P0.3 */

    {MCU_PIN_6, CYT2B75_GPIO_PORT_2, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_6, P2.0 */
    {MCU_PIN_7, CYT2B75_GPIO_PORT_2, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_7, P2.1 */
    {MCU_PIN_8, CYT2B75_GPIO_PORT_2, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_8, P2.2 */
    {MCU_PIN_9, CYT2B75_GPIO_PORT_2, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_9, P2.3 */

    {MCU_PIN_10, CYT2B75_GPIO_PORT_3, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_10, P3.0 */
    {MCU_PIN_11, CYT2B75_GPIO_PORT_3, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_11, P3.1 */

    {MCU_PIN_12, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_12, not defined in 100-LQFP */
    {MCU_PIN_13, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_13, not defined in 100-LQFP */

    {MCU_PIN_14, CYT2B75_GPIO_PORT_5, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_14, P5.0 */
    {MCU_PIN_15, CYT2B75_GPIO_PORT_5, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_15, P5.1 */
    {MCU_PIN_16, CYT2B75_GPIO_PORT_5, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_16, P5.2 */
    {MCU_PIN_17, CYT2B75_GPIO_PORT_5, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_17, P5.3 */

    {MCU_PIN_18, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_18, P6.0 */
    {MCU_PIN_19, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_19, P6.1 */
    {MCU_PIN_20, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_20, P6.2 */
    {MCU_PIN_21, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_21, P6.3 */
    {MCU_PIN_22, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_22, P6.4 */
    {MCU_PIN_23, CYT2B75_GPIO_PORT_6, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_23, P6.5 */

    {MCU_PIN_24, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_24, not defined in 100-LQFP */
    {MCU_PIN_25, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_25, not defined in 100-LQFP */
    {MCU_PIN_26, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_26, not defined in 100-LQFP */
    {MCU_PIN_27, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_27, not defined in 100-LQFP */
    {MCU_PIN_28, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_28, not defined in 100-LQFP */

    {MCU_PIN_29, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_29, P7.0 */
    {MCU_PIN_30, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_30, P7.1 */
    {MCU_PIN_31, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_31, P7.2 */
    {MCU_PIN_32, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_32, P7.3 */
    {MCU_PIN_33, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_33, P7.4 */
    {MCU_PIN_34, CYT2B75_GPIO_PORT_7, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_34, P7.5 */

    {MCU_PIN_35, CYT2B75_GPIO_PORT_8, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_35, P8.0 */
    {MCU_PIN_36, CYT2B75_GPIO_PORT_8, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_36, P8.1 */
    {MCU_PIN_37, CYT2B75_GPIO_PORT_8, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_37, P8.2 */

    {MCU_PIN_38, CYT2B75_GPIO_PORT_11, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_38, P11.0 */
    {MCU_PIN_39, CYT2B75_GPIO_PORT_11, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_39, P11.1 */
    {MCU_PIN_40, CYT2B75_GPIO_PORT_11, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_40, P11.2 */

    {MCU_PIN_41, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_41, not defined in 100-LQFP */
    {MCU_PIN_42, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_42, not defined in 100-LQFP */
    {MCU_PIN_43, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_43, not defined in 100-LQFP */
    {MCU_PIN_44, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_44, not defined in 100-LQFP */

    {MCU_PIN_45, CYT2B75_GPIO_PORT_12, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_45, P12.0 */
    {MCU_PIN_46, CYT2B75_GPIO_PORT_12, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_46, P12.1 */
    {MCU_PIN_47, CYT2B75_GPIO_PORT_12, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_47, P12.2 */
    {MCU_PIN_48, CYT2B75_GPIO_PORT_12, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_48, P12.3 */
    {MCU_PIN_49, CYT2B75_GPIO_PORT_12, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_49, P12.4 */

    {MCU_PIN_50, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_50, not defined in 100-LQFP */
    {MCU_PIN_51, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_51, not defined in 100-LQFP */

    {MCU_PIN_52, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_52, P13.0 */
    {MCU_PIN_53, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_53, P13.1 */
    {MCU_PIN_54, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_54, P13.2 */
    {MCU_PIN_55, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_55, P13.3 */
    {MCU_PIN_56, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_56, P13.4 */
    {MCU_PIN_57, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_57, P13.5 */
    {MCU_PIN_58, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_6}, /* MCU_PIN_58, P13.6 */
    {MCU_PIN_59, CYT2B75_GPIO_PORT_13, CYT2B75_GPIO_PIN_7}, /* MCU_PIN_59, P13.7 */

    {MCU_PIN_60, CYT2B75_GPIO_PORT_14, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_60, P14.0 */
    {MCU_PIN_61, CYT2B75_GPIO_PORT_14, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_61, P14.1 */
    {MCU_PIN_62, CYT2B75_GPIO_PORT_14, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_62, P14.2 */
    {MCU_PIN_63, CYT2B75_GPIO_PORT_14, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_63, P14.3 */

    {MCU_PIN_64, CYT2B75_GPIO_PORT_17, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_64, P17.0 */
    {MCU_PIN_65, CYT2B75_GPIO_PORT_17, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_65, P17.1 */
    {MCU_PIN_66, CYT2B75_GPIO_PORT_17, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_66, P17.2 */

    {MCU_PIN_67, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_67, P18.0 */
    {MCU_PIN_68, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_68, P18.1 */
    {MCU_PIN_69, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_69, P18.2 */
    {MCU_PIN_70, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_70, P18.3 */
    {MCU_PIN_71, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_71, P18.4 */
    {MCU_PIN_72, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_72, P18.5 */
    {MCU_PIN_73, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_6}, /* MCU_PIN_73, P18.6 */
    {MCU_PIN_74, CYT2B75_GPIO_PORT_18, CYT2B75_GPIO_PIN_7}, /* MCU_PIN_74, P18.7 */

    {MCU_PIN_75, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_75, not defined in 100-LQFP */
    {MCU_PIN_76, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_76, not defined in 100-LQFP */

    {MCU_PIN_77, CYT2B75_GPIO_PORT_19, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_77, P19.0 */
    {MCU_PIN_78, CYT2B75_GPIO_PORT_19, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_78, P19.1 */
    {MCU_PIN_79, CYT2B75_GPIO_PORT_19, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_79, P19.2 */
    {MCU_PIN_80, CYT2B75_GPIO_PORT_19, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_80, P19.3 */

    {MCU_PIN_81, CYT2B75_GPIO_PORT_21, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_81, P21.0 */
    {MCU_PIN_82, CYT2B75_GPIO_PORT_21, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_82, P21.1 */
    {MCU_PIN_83, CYT2B75_GPIO_PORT_21, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_83, P21.2 */
    {MCU_PIN_84, CYT2B75_GPIO_PORT_21, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_84, P21.3 */

    {MCU_PIN_85, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_85, not defined in 100-LQFP */
    {MCU_PIN_86, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_86, not defined in 100-LQFP */
    {MCU_PIN_87, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_87, not defined in 100-LQFP */
    {MCU_PIN_88, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_88, not defined in 100-LQFP */
    {MCU_PIN_89, CYT2B75_GPIO_PORT_NOT_DEFINED, CYT2B75_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_89, not defined in 100-LQFP */

    {MCU_PIN_90, CYT2B75_GPIO_PORT_21, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_90, P21.5 */

    {MCU_PIN_91, CYT2B75_GPIO_PORT_22, CYT2B75_GPIO_PIN_0}, /* MCU_PIN_91, P22.0 */
    {MCU_PIN_92, CYT2B75_GPIO_PORT_22, CYT2B75_GPIO_PIN_1}, /* MCU_PIN_92, P22.1 */
    {MCU_PIN_93, CYT2B75_GPIO_PORT_22, CYT2B75_GPIO_PIN_2}, /* MCU_PIN_93, P22.2 */
    {MCU_PIN_94, CYT2B75_GPIO_PORT_22, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_94, P22.3 */

    {MCU_PIN_95, CYT2B75_GPIO_PORT_23, CYT2B75_GPIO_PIN_3}, /* MCU_PIN_95, P23.3 */
    {MCU_PIN_96, CYT2B75_GPIO_PORT_23, CYT2B75_GPIO_PIN_4}, /* MCU_PIN_96, P23.4, SWO_TDO */
    {MCU_PIN_97, CYT2B75_GPIO_PORT_23, CYT2B75_GPIO_PIN_5}, /* MCU_PIN_97, P23.5, SWCLK_TCLK */
    {MCU_PIN_98, CYT2B75_GPIO_PORT_23, CYT2B75_GPIO_PIN_6}, /* MCU_PIN_98, P23.6, SWDIO_TMS */
    {MCU_PIN_99, CYT2B75_GPIO_PORT_23, CYT2B75_GPIO_PIN_7}, /* MCU_PIN_99, P23.7, SWDOE_TDI */
};

const size_t array_size = sizeof(pin_map) / sizeof(pin_map[0]);
/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/*!  This function initializes GPIO pin based on the ECU input pin name. */
asdk_errorcode_t asdk_gpio_init(asdk_gpio_config_t *gpio_cfg)
{
    // todo: check for GPIO module enabled (clock gating)
    // todo: perform static analysis

    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;
    const dal_pin_t *cyt2b75_gpio = NULL;
    cy_stc_gpio_pin_config_t cy_pin_cfg = {0};
    cy_en_gpio_status_t cy_gpio_init_status = CY_GPIO_SUCCESS;

    /* validate configuration parameters */

    if (NULL == gpio_cfg)
    {
        return ASDK_GPIO_ERROR_NULL_PTR;
    }

    if (MCU_PIN_NOT_DEFINED <= gpio_cfg->mcu_pin)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    cyt2b75_gpio = &pin_map[gpio_cfg->mcu_pin];

    if (CYT2B75_GPIO_PORT_MAX <= cyt2b75_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (CYT2B75_GPIO_PIN_MAX <= cyt2b75_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    /* initialize configuration parameters */

    ret_value = set_mode_configuration(gpio_cfg->gpio_mode, &cy_pin_cfg);

    if (ASDK_GPIO_SUCCESS == ret_value)
    {
        ret_value = set_pull_configuration(gpio_cfg->gpio_pull, &cy_pin_cfg);
    }
    else
    {
        return ret_value;
    }

    if (ASDK_GPIO_MODE_OUTPUT == gpio_cfg->gpio_mode)
    {
        /* assign initial state of GPIO output pin */
        if (ASDK_GPIO_STATE_INVALID <= gpio_cfg->gpio_init_state)
        {
            return ASDK_GPIO_ERROR_INVALID_STATE;
        }
        else
        {
            cy_pin_cfg.outVal = gpio_cfg->gpio_init_state;
        }
    }

    if (ASDK_GPIO_SUCCESS == ret_value)
    {
        ret_value = set_speed_configuration(gpio_cfg->gpio_speed, &cy_pin_cfg);
    }
    else
    {
        return ret_value;
    }

    if (ASDK_GPIO_SUCCESS == ret_value)
    {
        ret_value = set_isr(gpio_cfg, &cy_pin_cfg);
    }
    else
    {
        return ret_value;
    }

    if (ASDK_GPIO_SUCCESS == ret_value)
    {

        /* initialize pin as GPIO */
        cy_gpio_init_status = Cy_GPIO_Pin_Init(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, &cy_pin_cfg);

        if (CY_GPIO_SUCCESS != cy_gpio_init_status)
        {
            ret_value = ASDK_GPIO_ERROR_INIT;
        }

        /* enable interrupt */
        if (gpio_cfg->interrupt_config.type != ASDK_GPIO_INTERRUPT_DISABLED)
        {
            NVIC_SetPriority(irq_cfg.intIdx, gpio_cfg->interrupt_config.priority);
            NVIC_EnableIRQ(irq_cfg.intIdx);
        }
    }

    return ret_value;
}

/*! This function de-initializes GPIO pin based on ECU input pin name.*/
asdk_errorcode_t asdk_gpio_deinit(asdk_mcu_pin_t pin_num)
{
    uint32_t reg_value = 0;
    uint32_t pin_mask = 0;
    const dal_pin_t *cyt2b75_gpio = NULL;
    volatile stc_GPIO_PRT_t *cyt2b75_port_reg = NULL;

    if (pin_num >= ASDK_CYT2B75_PIN_MAX)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    // derive actual PORT.PIN from MCU pin
    cyt2b75_gpio = &pin_map[pin_num];

    if (CYT2B75_GPIO_PORT_MAX <= cyt2b75_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (CYT2B75_GPIO_PIN_MAX <= cyt2b75_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    /* clear the expected bit alone for a given pin number
       and update the GPIO Port registers */

    // clear interrupt configuration reg.
    Cy_GPIO_SetInterruptEdge(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear interrupt mask reg.
    Cy_GPIO_SetInterruptMask(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear out buffer reg.
    pin_mask = ~(1 << cyt2b75_gpio->pin); // clear the bit
    cyt2b75_port_reg = gpio_port[cyt2b75_gpio->port];
    reg_value = cyt2b75_port_reg->unOUT.u32Register & pin_mask;
    cyt2b75_port_reg->unOUT.u32Register = reg_value;

    // clear drive mode
    Cy_GPIO_SetDrivemode(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear input buffer configuration register
    Cy_GPIO_SetVtrip(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear slew rate configuration
    Cy_GPIO_SetSlewRate(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear automotive configuration
    GPIO_SetVtripAuto(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    // clear alternate function selection regs.
    Cy_GPIO_SetHSIOM(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, 0);

    return ASDK_GPIO_SUCCESS;
}

/*!  This function assigns user callback function for GPIO module. It sets up
  interrupt for all available GPIO Port interrupts and then assigns the
  user-callback which is called whenever an GPIO interrupt occurs.*/
asdk_errorcode_t asdk_gpio_install_callback(asdk_gpio_input_callback_t callback)
{
    if (NULL == callback)
    {
        return ASDK_GPIO_ERROR_NULL_PTR;
    }

    user_gpio_callback_fun = callback;

    return ASDK_GPIO_SUCCESS;
}

/*!This function writes ASDK_GPIO_STATE_HIGH (1) to the given ECU input pin name.*/
asdk_errorcode_t asdk_gpio_output_set(asdk_mcu_pin_t pin_num)
{
    return set_gpio_state(pin_num, ASDK_GPIO_STATE_HIGH);
}

asdk_errorcode_t asdk_gpio_output_clear(asdk_mcu_pin_t pin_num)
{
    return set_gpio_state(pin_num, ASDK_GPIO_STATE_LOW);
}

/*!This function toggles the current state of the given ECU input pin name.*/

asdk_errorcode_t asdk_gpio_output_toggle(asdk_mcu_pin_t pin_num)
{
    const dal_pin_t *cyt2b75_gpio = NULL;

    if (ASDK_CYT2B75_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    cyt2b75_gpio = &pin_map[pin_num];

    if (CYT2B75_GPIO_PORT_MAX <= cyt2b75_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (CYT2B75_GPIO_PIN_MAX <= cyt2b75_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    Cy_GPIO_Inv(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin);

    return ASDK_GPIO_SUCCESS;
}


asdk_errorcode_t asdk_gpio_get_input_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *gpio_state)
{
    *gpio_state = ASDK_GPIO_STATE_INVALID;

    return get_gpio_state(pin_num, gpio_state, true);
}

/*!This function returns the current state of the given ECU output pin name.*/
asdk_errorcode_t asdk_gpio_get_output_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *gpio_state)
{
    *gpio_state = ASDK_GPIO_STATE_INVALID;

    return get_gpio_state(pin_num, gpio_state, false);
}

/* static functions ************************** */

static asdk_errorcode_t set_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t state)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;
    const dal_pin_t *cyt2b75_gpio = NULL;

    if (ASDK_CYT2B75_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    cyt2b75_gpio = &pin_map[pin_num];

    if (CYT2B75_GPIO_PORT_MAX <= cyt2b75_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (CYT2B75_GPIO_PIN_MAX <= cyt2b75_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    Cy_GPIO_Write(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin, state);

    return ret_value;
}

static asdk_errorcode_t get_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *state, bool read_input_state)
{
    const dal_pin_t *cyt2b75_gpio = NULL;

    if (ASDK_CYT2B75_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    cyt2b75_gpio = &pin_map[pin_num];

    if (CYT2B75_GPIO_PORT_MAX <= cyt2b75_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (CYT2B75_GPIO_PIN_MAX <= cyt2b75_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    if (read_input_state)
    {
        *state = (asdk_gpio_state_t)Cy_GPIO_Read(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin);
    }
    else
    {
        *state = (asdk_gpio_state_t)Cy_GPIO_ReadOut(gpio_port[cyt2b75_gpio->port], cyt2b75_gpio->pin);
    }

    return ASDK_GPIO_SUCCESS;
}

static inline asdk_errorcode_t set_mode_configuration(asdk_gpio_mode_t gpio_mode, cy_stc_gpio_pin_config_t *cy_config_out)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_mode)
    {
    case ASDK_GPIO_MODE_INPUT:
    case ASDK_GPIO_MODE_OUTPUT:
        cy_config_out->hsiom = 0; // use as GPIO
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_GPIO_MODE;
        break;
    }

    return ret_value;
}

static inline asdk_errorcode_t set_pull_configuration(asdk_gpio_pull_t gpio_pull, cy_stc_gpio_pin_config_t *cy_config_out)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_pull)
    {
    case ASDK_GPIO_HIGH_Z:
        cy_config_out->driveMode = CY_GPIO_DM_HIGHZ;
        break;

    /* for GPIO as output */
    case ASDK_GPIO_PUSH_PULL:
        cy_config_out->driveMode = CY_GPIO_DM_STRONG_IN_OFF;
        break;

    /* for GPIO as input */
    case ASDK_GPIO_PULL_UP:
        cy_config_out->driveMode = CY_GPIO_DM_PULLUP;
        cy_config_out->outVal = 1;
        break;

    case ASDK_GPIO_PULL_DOWN:
        cy_config_out->driveMode = CY_GPIO_DM_PULLDOWN;
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_PULL_TYPE;
        break;
    }

    // if (ASDK_GPIO_SUCCESS == ret_value)
    // {
    //     cy_config_out->outVal = 1;
    // }

    return ret_value;
}

static inline asdk_errorcode_t set_speed_configuration(asdk_gpio_speed_t gpio_speed, cy_stc_gpio_pin_config_t *cy_config_out)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_speed)
    {
    case ASDK_GPIO_SPEED_HIGH_FREQ:
        cy_config_out->slewRate = CY_GPIO_SLEW_FAST;
        break;

    case ASDK_GPIO_SPEED_LOW_FREQ:
        cy_config_out->slewRate = CY_GPIO_SLEW_SLOW;
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_SPEED_TYPE;
    }

    return ret_value;
}

static inline asdk_errorcode_t set_isr(asdk_gpio_config_t *gpio_cfg, cy_stc_gpio_pin_config_t *cy_config_out)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_cfg->interrupt_config.type)
    {
    case ASDK_GPIO_INTERRUPT_DISABLED:
        cy_config_out->intEdge = CY_GPIO_INTR_DISABLE;
        break;

    case ASDK_GPIO_INTERRUPT_RISING_EDGE:
        cy_config_out->intEdge = CY_GPIO_INTR_RISING;
        break;

    case ASDK_GPIO_INTERRUPT_FALLING_EDGE:
        cy_config_out->intEdge = CY_GPIO_INTR_FALLING;
        break;

    case ASDK_GPIO_INTERRUPT_BOTH_EDGES:
        cy_config_out->intEdge = CY_GPIO_INTR_BOTH;
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_INTERRUPT_TYPE;
        break;
    }

    /* interrupt forwarding enabled */
    if ((ASDK_GPIO_SUCCESS == ret_value) &&
        (ASDK_GPIO_INTERRUPT_DISABLED != gpio_cfg->interrupt_config.type))
    {
        cy_config_out->intMask = 1;

        switch (pin_map[gpio_cfg->mcu_pin].port)
        {
        case CYT2B75_GPIO_PORT_0:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_0_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port0_isr);
            break;

        case CYT2B75_GPIO_PORT_2:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_2_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port2_isr);
            break;

        case CYT2B75_GPIO_PORT_3:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_3_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port3_isr);
            break;

        case CYT2B75_GPIO_PORT_5:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_5_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port5_isr);
            break;

        case CYT2B75_GPIO_PORT_6:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_6_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port6_isr);
            break;

        case CYT2B75_GPIO_PORT_7:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_7_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port7_isr);
            break;

        case CYT2B75_GPIO_PORT_8:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_8_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port8_isr);
            break;

        case CYT2B75_GPIO_PORT_11:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_11_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port11_isr);
            break;

        case CYT2B75_GPIO_PORT_12:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_12_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port12_isr);
            break;

        case CYT2B75_GPIO_PORT_13:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_13_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port13_isr);
            break;

        case CYT2B75_GPIO_PORT_14:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_14_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port14_isr);
            break;

        case CYT2B75_GPIO_PORT_17:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_17_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port17_isr);
            break;

        case CYT2B75_GPIO_PORT_18:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_18_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port18_isr);
            break;

        case CYT2B75_GPIO_PORT_19:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_19_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port19_isr);
            break;

        case CYT2B75_GPIO_PORT_21:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_21_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port21_isr);
            break;

        case CYT2B75_GPIO_PORT_22:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_22_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port22_isr);
            break;

        case CYT2B75_GPIO_PORT_23:
            irq_cfg.sysIntSrc = ioss_interrupts_gpio_23_IRQn;
            Cy_SysInt_InitIRQ(&irq_cfg);
            Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, port23_isr);
            break;

        default:
            ret_value = ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
            break;
        }
    }

    return ret_value;
}

/* ISR handlers */

static inline void gpio_isr(const dal_pin_t *port, cyt2b75_port_t port_no)
{
    /* Get pin interrupt flag */
    uint32_t interrupt_status = gpio_port[port_no]->unINTR_MASKED.u32Register;
    uint8_t port_pin_num = 0;
    uint32_t input_pin_state = 0;

    interrupt_status = interrupt_status & port_mask[port_no];

    if (interrupt_status)
    {
        for (port_pin_num = 0; port_pin_num < CYT2B75_GPIO_PIN_MAX; port_pin_num++)
        {
            if (interrupt_status & (1 << port_pin_num))
            {
                /* Clear external interrupt flag. */
                Cy_GPIO_ClearInterrupt(gpio_port[port_no], port_pin_num);

                /* call user callback function */
                if (user_gpio_callback_fun)
                {
                    input_pin_state = Cy_GPIO_Read(gpio_port[port_no], port_pin_num);
                    user_gpio_callback_fun(port[port_pin_num].mcu_pin, input_pin_state);
                }

                break;
            }
        }
    }
}

static void port0_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_2], CYT2B75_GPIO_PORT_0);
}

static void port2_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_6], CYT2B75_GPIO_PORT_2);
}

static void port3_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_10], CYT2B75_GPIO_PORT_3);
}

static void port5_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_14], CYT2B75_GPIO_PORT_5);
}

static void port6_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_18], CYT2B75_GPIO_PORT_6);
}

static void port7_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_29], CYT2B75_GPIO_PORT_7);
}

static void port8_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_35], CYT2B75_GPIO_PORT_8);
}

static void port11_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_38], CYT2B75_GPIO_PORT_11);
}

static void port12_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_45], CYT2B75_GPIO_PORT_12);
}

static void port13_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_52], CYT2B75_GPIO_PORT_13);
}

static void port14_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_60], CYT2B75_GPIO_PORT_14);
}

static void port17_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_64], CYT2B75_GPIO_PORT_17);
}

static void port18_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_67], CYT2B75_GPIO_PORT_18);
}

static void port19_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_77], CYT2B75_GPIO_PORT_19);
}

static void port21_isr(void)
{
    const dal_pin_t *port = &pin_map[MCU_PIN_81];

    /* Get pin interrupt flag */
    uint32_t interrupt_status = gpio_port[CYT2B75_GPIO_PORT_21]->unINTR_MASKED.u32Register;
    uint8_t port_pin_num = 0;
    uint32_t input_pin_state = 0;

    interrupt_status = interrupt_status & port_mask[CYT2B75_GPIO_PORT_21];

    if (interrupt_status)
    {
        for (port_pin_num = 0; port_pin_num < CYT2B75_GPIO_PIN_MAX; port_pin_num++)
        {
            if (interrupt_status & (1 << port_pin_num))
            {
                // between P21.3 & P21.5, there exists 5 undefined pins in pin_map
                // so moving *port to MCU_PIN_85 for proper indexing of P21.5
                if (port_pin_num == CYT2B75_GPIO_PIN_5)
                {
                    port = &pin_map[MCU_PIN_85];
                }

                /* Clear external interrupt flag. */
                Cy_GPIO_ClearInterrupt(gpio_port[CYT2B75_GPIO_PORT_21], port_pin_num);

                /* call user callback function */
                if (user_gpio_callback_fun)
                {
                    input_pin_state = Cy_GPIO_Read(gpio_port[CYT2B75_GPIO_PORT_21], port_pin_num);
                    user_gpio_callback_fun(port[port_pin_num].mcu_pin, input_pin_state);
                }

                break;
            }
        }
    }
}

static void port22_isr(void)
{
    gpio_isr(&pin_map[MCU_PIN_91], CYT2B75_GPIO_PORT_22);
}

static void port23_isr(void)
{
    // P23.1...P23.2 are undefined
    // so pin_map[MCU_PIN_92] properly indexes for P23.3

    gpio_isr(&pin_map[MCU_PIN_92], CYT2B75_GPIO_PORT_23);
}
