#include <stdlib.h>

/* asdk includes ***************************** */

#include "asdk_platform.h"

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_gpio.h"     // ASDK GPIO APIs

/* sdk includes ****************************** */

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
    STM32F4_GPIO_PORT_A = 0,
    STM32F4_GPIO_PORT_B,
    STM32F4_GPIO_PORT_C,
    STM32F4_GPIO_PORT_D,
    STM32F4_GPIO_PORT_E,
	STM32F4_GPIO_PORT_H,

	STM32F4_GPIO_PORT_MAX,
	STM32F4_GPIO_PORT_NOT_DEFINED = STM32F4_GPIO_PORT_MAX,
} stm32f4_port_t;

typedef enum
{
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

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

typedef struct
{
    asdk_mcu_pin_t mcu_pin; /* Actual pin number of the MCU */
    stm32f4_port_t port;    /* Actual port of the given GPIO */
    stm32f4_pin_t pin;      /* Actual pin of a the given GPIO Port */
} dal_pin_t;

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static asdk_errorcode_t get_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *state, bool read_input_state);
static asdk_errorcode_t set_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t state);
static inline asdk_errorcode_t set_mode_configuration(asdk_gpio_mode_t gpio_mode, GPIO_InitTypeDef *GPIO_Type);
static inline asdk_errorcode_t set_pull_configuration(asdk_gpio_pull_t gpio_pull, GPIO_InitTypeDef *GPIO_Type);
static inline asdk_errorcode_t set_speed_configuration(asdk_gpio_speed_t gpio_speed, GPIO_InitTypeDef *GPIO_Type);
static inline asdk_errorcode_t set_isr(asdk_gpio_config_t *gpio_cfg, stm32f4_port_t port, stm32f4_pin_t pin, GPIO_InitTypeDef *GPIO_Type);
static inline void __enable_gpio_clock(stm32f4_port_t port);

/* volatile global variables ***************** */

/* should use 'volatile' and not 'const' otherwise
 compiler throws warnings due to SDK implementation */
GPIO_TypeDef *const gpio_port[] = {
    GPIOA, GPIOB, GPIOC,
    GPIOD, GPIOE, GPIOH,
};

/* global variables ************************** */

/* static variables ************************** */

/* GPIO module callback declaration */
static asdk_gpio_input_callback_t user_gpio_callback_fun = NULL;

/**
 * @var dal_pin_t pin_map
    This table maps MCU pin number to actual GPIO port and pin number.
*/
/* Look-up table to map actual MCU pin with PORT.PIN type based on datasheet.
   Used for deriving PORT.PIN when MCU pin name is known and vice-versa. */
const dal_pin_t pin_map[] = {
    {MCU_PIN_0, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_2}, /* MCU_PIN_0, PE.2 */
    {MCU_PIN_1, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_3}, /* MCU_PIN_1, PE.3 */

    {MCU_PIN_2, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_4}, /* MCU_PIN_2, PE.4 */
    {MCU_PIN_3, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_5}, /* MCU_PIN_3, PE.5 */
    {MCU_PIN_4, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_6}, /* MCU_PIN_4, PE.6 */
    {MCU_PIN_5, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_5, VBAT */

    {MCU_PIN_6, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_13}, /* MCU_PIN_6, PC.13 */
    {MCU_PIN_7, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_14}, /* MCU_PIN_7, PC.14 */
    {MCU_PIN_8, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_15}, /* MCU_PIN_8, PC.15 */
    {MCU_PIN_9, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_9, VSS */

    {MCU_PIN_10, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_10, VDD */
    {MCU_PIN_11, STM32F4_GPIO_PORT_H, STM32F4_GPIO_PIN_0}, /* MCU_PIN_11, PH.0 */

    {MCU_PIN_12, STM32F4_GPIO_PORT_H, STM32F4_GPIO_PIN_1}, /* MCU_PIN_12, PH.1 */
    {MCU_PIN_13, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_13, NRST */

    {MCU_PIN_14, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_0}, /* MCU_PIN_14, PC.0 */
    {MCU_PIN_15, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_1}, /* MCU_PIN_15, PC.1 */
    {MCU_PIN_16, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_2}, /* MCU_PIN_16, PC.2 */
    {MCU_PIN_17, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_3}, /* MCU_PIN_17, PC.3 */

    {MCU_PIN_18, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_18, VDD */
    {MCU_PIN_19, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_19, VDDA */
    {MCU_PIN_20, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_20, VREF+ */
    {MCU_PIN_21, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_21, VSSA */
    {MCU_PIN_22, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_0}, /* MCU_PIN_22, PA.0 */
    {MCU_PIN_23, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_1}, /* MCU_PIN_23, PA.1 */

    {MCU_PIN_24, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_2}, /* MCU_PIN_24, PA.2 */
    {MCU_PIN_25, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_3}, /* MCU_PIN_25, PA.3 */
    {MCU_PIN_26, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_26, VSS */
    {MCU_PIN_27, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_27, VDD */
    {MCU_PIN_28, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_4}, /* MCU_PIN_28, PA.4 */

    {MCU_PIN_29, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_5}, /* MCU_PIN_29, PA.5 */
    {MCU_PIN_30, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_6}, /* MCU_PIN_30, PA.6 */
    {MCU_PIN_31, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_7}, /* MCU_PIN_31, PA.7 */
    {MCU_PIN_32, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_4}, /* MCU_PIN_32, PC.4 */
    {MCU_PIN_33, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_5}, /* MCU_PIN_33, PC.5 */
    {MCU_PIN_34, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_0}, /* MCU_PIN_34, PB.0 */

    {MCU_PIN_35, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_1}, /* MCU_PIN_35, PB.1 */
    {MCU_PIN_36, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_2}, /* MCU_PIN_36, PB.2 */
    {MCU_PIN_37, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_7}, /* MCU_PIN_37, PE.7 */

    {MCU_PIN_38, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_8}, /* MCU_PIN_38, PE.8 */
    {MCU_PIN_39, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_9}, /* MCU_PIN_39, PE.9 */
    {MCU_PIN_40, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_10}, /* MCU_PIN_40, PE.10 */

    {MCU_PIN_41, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_11}, /* MCU_PIN_41, PE.11 */
    {MCU_PIN_42, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_12}, /* MCU_PIN_42, PE.12 */
    {MCU_PIN_43, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_13}, /* MCU_PIN_43, PE.13 */
    {MCU_PIN_44, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_14}, /* MCU_PIN_44, PE.14 */

    {MCU_PIN_45, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_15}, /* MCU_PIN_45, PE.15 */
    {MCU_PIN_46, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_10}, /* MCU_PIN_46, PB.10 */
    {MCU_PIN_47, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_47, VCAP1 */
    {MCU_PIN_48, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_48, VSS */
    {MCU_PIN_49, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_49, VDD */

    {MCU_PIN_50, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_12}, /* MCU_PIN_50, PB.12 */
    {MCU_PIN_51, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_13}, /* MCU_PIN_51, PB.13 */

    {MCU_PIN_52, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_14}, /* MCU_PIN_52, PB.14 */
    {MCU_PIN_53, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_15}, /* MCU_PIN_53, PB.15 */
    {MCU_PIN_54, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_8}, /* MCU_PIN_54, PD.8 */
    {MCU_PIN_55, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_9}, /* MCU_PIN_55, PD.9 */
    {MCU_PIN_56, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_10}, /* MCU_PIN_56, PD.10 */
    {MCU_PIN_57, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_11}, /* MCU_PIN_57, PD.11 */
    {MCU_PIN_58, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_12}, /* MCU_PIN_58, PD.12 */
    {MCU_PIN_59, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_13}, /* MCU_PIN_59, PD.13 */

    {MCU_PIN_60, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_14}, /* MCU_PIN_60, PD.14 */
    {MCU_PIN_61, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_15}, /* MCU_PIN_61, PD.15 */
    {MCU_PIN_62, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_6}, /* MCU_PIN_62, PC.6 */
    {MCU_PIN_63, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_7}, /* MCU_PIN_63, PC.7 */

    {MCU_PIN_64, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_8}, /* MCU_PIN_64, PC.8 */
    {MCU_PIN_65, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_9}, /* MCU_PIN_65, PC.9 */
    {MCU_PIN_66, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_8}, /* MCU_PIN_66, PA.8 */

    {MCU_PIN_67, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_9}, /* MCU_PIN_67, PA.9 */
    {MCU_PIN_68, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_10}, /* MCU_PIN_68, PA.10 */
    {MCU_PIN_69, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_11}, /* MCU_PIN_69, PA.11 */
    {MCU_PIN_70, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_12}, /* MCU_PIN_70, PA.12 */
    {MCU_PIN_71, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_13}, /* MCU_PIN_71, PA.13 */
    {MCU_PIN_72, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_72, VCAP2 */
    {MCU_PIN_73, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_73, VSS */
    {MCU_PIN_74, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_74, VDD */

    {MCU_PIN_75, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_14}, /* MCU_PIN_75, PA.14 */
    {MCU_PIN_76, STM32F4_GPIO_PORT_A, STM32F4_GPIO_PIN_15}, /* MCU_PIN_76, PA.15 */

    {MCU_PIN_77, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_10}, /* MCU_PIN_77, PC.10 */
    {MCU_PIN_78, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_11}, /* MCU_PIN_78, PC.11 */
    {MCU_PIN_79, STM32F4_GPIO_PORT_C, STM32F4_GPIO_PIN_12}, /* MCU_PIN_79, PC.12 */
    {MCU_PIN_80, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_0}, /* MCU_PIN_80, PD.0 */

    {MCU_PIN_81, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_1}, /* MCU_PIN_81, PD.1 */
    {MCU_PIN_82, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_2}, /* MCU_PIN_82, PD.2 */
    {MCU_PIN_83, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_3}, /* MCU_PIN_83, PD.3 */
    {MCU_PIN_84, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_4}, /* MCU_PIN_84, PD.4 */

    {MCU_PIN_85, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_5}, /* MCU_PIN_85, PD.5 */
    {MCU_PIN_86, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_6}, /* MCU_PIN_86, PD.6 */
    {MCU_PIN_87, STM32F4_GPIO_PORT_D, STM32F4_GPIO_PIN_7}, /* MCU_PIN_87, PD.7 */
    {MCU_PIN_88, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_3}, /* MCU_PIN_88, PB.3 */
    {MCU_PIN_89, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_4}, /* MCU_PIN_89, PB.4 */

    {MCU_PIN_90, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_5}, /* MCU_PIN_90, PB.5 */

    {MCU_PIN_91, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_6}, /* MCU_PIN_91, PB.6 */
    {MCU_PIN_92, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_7}, /* MCU_PIN_92, PB.7 */
    {MCU_PIN_93, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_93, BOOT0 */
    {MCU_PIN_94, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_8}, /* MCU_PIN_94, PB.8 */

    {MCU_PIN_95, STM32F4_GPIO_PORT_B, STM32F4_GPIO_PIN_9}, /* MCU_PIN_95, PB.9 */
    {MCU_PIN_96, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_0}, /* MCU_PIN_96, PE.0 */
    {MCU_PIN_97, STM32F4_GPIO_PORT_E, STM32F4_GPIO_PIN_1}, /* MCU_PIN_97, PE.1 */
    {MCU_PIN_98, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_98, P23.6, VSS */
    {MCU_PIN_99, STM32F4_GPIO_PORT_NOT_DEFINED, STM32F4_GPIO_PIN_NOT_DEFINED}, /* MCU_PIN_99, P23.7, VDD */
};

const size_t array_size = sizeof(pin_map) / sizeof(pin_map[0]);


/*!  This function initializes GPIO pin based on the ECU input pin name. */
asdk_errorcode_t asdk_gpio_init(asdk_gpio_config_t *gpio_cfg)
{
    // todo: perform static analysis

    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;
    const dal_pin_t *stm32f4_gpio = NULL;

	GPIO_InitTypeDef GPIO_Type = {.Alternate = 0, .Mode = 0, .Pin = 0,
									  .Pull = 0, .Speed = 0};

    /* validate configuration parameters */

    if (NULL == gpio_cfg)
    {
        return ASDK_GPIO_ERROR_NULL_PTR;
    }

    if (MCU_PIN_NOT_DEFINED <= gpio_cfg->mcu_pin)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    stm32f4_gpio = &pin_map[gpio_cfg->mcu_pin];

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    /* initialize configuration parameters */

    GPIO_Type.Pin = (1U << stm32f4_gpio->pin);

    ret_value = set_mode_configuration(gpio_cfg->gpio_mode, &GPIO_Type);

    if (ASDK_GPIO_SUCCESS == ret_value)
    {
        ret_value = set_pull_configuration(gpio_cfg->gpio_pull, &GPIO_Type);
    }
    else
    {
        return ret_value;
    }

    /* init clock gating */
    __enable_gpio_clock(stm32f4_gpio->port);

    if (ASDK_GPIO_MODE_OUTPUT == gpio_cfg->gpio_mode)
    {
        /* assign initial state of GPIO output pin */
        if (ASDK_GPIO_STATE_INVALID <= gpio_cfg->gpio_init_state)
        {
            return ASDK_GPIO_ERROR_INVALID_STATE;
        }
        else
        {
        	HAL_GPIO_WritePin(gpio_port[stm32f4_gpio->port], (uint16_t)(1U << stm32f4_gpio->pin), (GPIO_PinState)gpio_cfg->gpio_init_state);
        }
    }

    if (ASDK_GPIO_SUCCESS == ret_value)
    {
        ret_value = set_speed_configuration(gpio_cfg->gpio_speed, &GPIO_Type);
    }
    else
    {
        return ret_value;
    }

    if (gpio_cfg->gpio_mode == ASDK_GPIO_MODE_INPUT)
    {
		if (ASDK_GPIO_SUCCESS == ret_value)
		{
			ret_value = set_isr(gpio_cfg, stm32f4_gpio->port, stm32f4_gpio->pin, &GPIO_Type);
		}
		else
		{
			return ret_value;
		}
    }

    if (ASDK_GPIO_SUCCESS == ret_value)
    {

        /* initialize pin as GPIO */
        HAL_GPIO_Init(gpio_port[stm32f4_gpio->port], &GPIO_Type);
    }

    return ret_value;
}

/*! This function de-initializes GPIO pin based on ECU input pin name.*/
asdk_errorcode_t asdk_gpio_deinit(asdk_mcu_pin_t pin_num)
{
    const dal_pin_t *stm32f4_gpio = NULL;

    if (pin_num >= ASDK_STM32F411_PIN_MAX)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    // derive actual PORT.PIN from MCU pin
    stm32f4_gpio = &pin_map[pin_num];

    // clear port for EXTI channel
    SYSCFG->EXTICR[pin_map[pin_num].pin/4] &= ~(0xFU << (pin_map[pin_num].pin%4));

    EXTI->IMR &= ~(1U << pin_map[pin_num].pin);

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    /* de-initialize pin */
    HAL_GPIO_DeInit(gpio_port[stm32f4_gpio->port], (1U << stm32f4_gpio->pin));


	EXTI->FTSR &= ~(1U << pin_map[pin_num].pin);
	EXTI->RTSR &= ~(1U << pin_map[pin_num].pin);
	EXTI->IMR &= ~(1U << pin_map[pin_num].pin);

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
    const dal_pin_t *stm32f4_gpio = NULL;

    if (ASDK_STM32F411_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    stm32f4_gpio = &pin_map[pin_num];

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    HAL_GPIO_TogglePin(gpio_port[stm32f4_gpio->port], (uint16_t)(1U << stm32f4_gpio->pin));

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

void EXTI0_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	if ((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0_Msk) == SYSCFG_EXTICR1_EXTI0_PH)
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0_Msk);

    if (user_gpio_callback_fun)
    {
        /* TODO : due to this loop latency is huge */
		for (asdk_mcu_pin_t pin = MCU_PIN_0; pin < MCU_PIN_MAX; pin++)
		{
			if ((pin_map[pin].pin == STM32F4_GPIO_PIN_0) &&
				(pin_map[pin].port == port))
			{
				input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << STM32F4_GPIO_PIN_0));
				user_gpio_callback_fun(pin, input_pin_state);
				break;
			}
		}
    }

    /* Clear pending interrupt for that line */
    EXTI->PR |= (1U << 0);
}

void EXTI1_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	if ((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI1_Msk) == SYSCFG_EXTICR1_EXTI1_PH)
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI1_Msk) >> SYSCFG_EXTICR1_EXTI1_Pos;

    if (user_gpio_callback_fun)
    {
        /* TODO : due to this loop latency is huge */
		for (asdk_mcu_pin_t pin = MCU_PIN_0; pin < MCU_PIN_MAX; pin++)
		{
			if ((pin_map[pin].pin == STM32F4_GPIO_PIN_1) &&
				(pin_map[pin].port == port))
			{
				input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << STM32F4_GPIO_PIN_1));
				user_gpio_callback_fun(pin, input_pin_state);
				break;
			}
		}
    }

    /* Clear pending interrupt for that line */
    EXTI->PR |= (1U << 1);
}

void EXTI2_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;


	stm32f4_port_t port;

	if ((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI2_Msk) == SYSCFG_EXTICR1_EXTI2_PH)
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI2_Msk) >> SYSCFG_EXTICR1_EXTI2_Pos;

    if (user_gpio_callback_fun)
    {
        /* TODO : due to this loop latency is huge */
		for (asdk_mcu_pin_t pin = MCU_PIN_0; pin < MCU_PIN_MAX; pin++)
		{
			if ((pin_map[pin].pin == STM32F4_GPIO_PIN_2) &&
				(pin_map[pin].port == port))
			{
				input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << STM32F4_GPIO_PIN_2));
				user_gpio_callback_fun(pin, input_pin_state);
				break;
			}
		}
    }

    /* Clear pending interrupt for that line */
    EXTI->PR |= (1U << 2);
}

void EXTI3_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	if ((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI3_Msk) == SYSCFG_EXTICR1_EXTI3_PH)
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI3_Msk) >> SYSCFG_EXTICR1_EXTI3_Pos;

    if (user_gpio_callback_fun)
    {
        /* TODO : due to this loop latency is huge */
		for (asdk_mcu_pin_t pin = MCU_PIN_0; pin < MCU_PIN_MAX; pin++)
		{
			if ((pin_map[pin].pin == STM32F4_GPIO_PIN_3) &&
				(pin_map[pin].port == port))
			{
				input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << STM32F4_GPIO_PIN_3));
				user_gpio_callback_fun(pin, input_pin_state);
				break;
			}
		}
    }
    /* Clear pending interrupt for that line */
    EXTI->PR |= (1U << 3);
}

void EXTI4_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	if ((SYSCFG->EXTICR[1] & SYSCFG_EXTICR2_EXTI4_Msk) == SYSCFG_EXTICR2_EXTI4_PH)
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[1] & SYSCFG_EXTICR2_EXTI4_Msk) >> SYSCFG_EXTICR2_EXTI4_Pos;

    if (user_gpio_callback_fun)
    {
        /* TODO : due to this loop latency is huge */
		for (asdk_mcu_pin_t pin = MCU_PIN_0; pin < MCU_PIN_MAX; pin++)
		{
			if ((pin_map[pin].pin == STM32F4_GPIO_PIN_4) &&
				(pin_map[pin].port == port))
			{
				input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << STM32F4_GPIO_PIN_4));
				user_gpio_callback_fun(pin, input_pin_state);
				break;
			}
		}
    }

    /* Clear pending interrupt for that line */
    EXTI->PR |= (1U << 4);
}

void EXTI9_5_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	uint32_t trigg_pins = (EXTI->PR & 0x000003E0);

	uint8_t pin = __builtin_ctz(trigg_pins);

	uint8_t pin_idx = pin/4;
	uint8_t pin_num = (pin % 4) * 4;

	if ((SYSCFG->EXTICR[pin_idx] & (SYSCFG_EXTICR2_EXTI4_Msk << pin_num)) == (0x7U << pin_num))
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[pin_idx] & (SYSCFG_EXTICR2_EXTI4_Msk << pin_num)) >> pin_num;

    /* TODO : due to this loop latency is huge */
	for (asdk_mcu_pin_t mcu_pin = MCU_PIN_0; mcu_pin < MCU_PIN_MAX; mcu_pin++)
	{
		if ((pin_map[mcu_pin].pin == pin) &&
			(pin_map[mcu_pin].port == port))
		{
			input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << pin));
			user_gpio_callback_fun(mcu_pin, input_pin_state);
			break;
		}
	}

    /* Clear pending interrupt of that line */
	EXTI->PR |= (1U << pin);
}

void EXTI15_10_IRQHandler(void)
{
	GPIO_PinState input_pin_state = GPIO_PIN_RESET;

	stm32f4_port_t port;

	uint32_t trigg_pins = (EXTI->PR & 0x00000FC00);

	uint8_t pin = __builtin_ctz(trigg_pins);

	uint8_t pin_idx = pin/4;
	uint8_t pin_num = (pin % 4) * 4;

	if ((SYSCFG->EXTICR[pin_idx] & (SYSCFG_EXTICR2_EXTI4_Msk << pin_num)) == (0x7U << pin_num))
		port = STM32F4_GPIO_PORT_H;
	else
		port = (SYSCFG->EXTICR[pin_idx] & (SYSCFG_EXTICR2_EXTI4_Msk << pin_num)) >> pin_num;

    /* TODO : due to this loop latency is huge */
	for (asdk_mcu_pin_t mcu_pin = MCU_PIN_0; mcu_pin < MCU_PIN_MAX; mcu_pin++)
	{
		if ((pin_map[mcu_pin].pin == pin) &&
			(pin_map[mcu_pin].port == port))
		{
			input_pin_state = HAL_GPIO_ReadPin(gpio_port[port], (1U << pin));
			user_gpio_callback_fun(mcu_pin, input_pin_state);
			break;
		}
	}

    /* Clear pending interrupt of that line */
	EXTI->PR |= (1U << pin);
}

/* static functions ************************** */

static asdk_errorcode_t set_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t state)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;
    const dal_pin_t *stm32f4_gpio = NULL;

    if (ASDK_STM32F411_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    stm32f4_gpio = &pin_map[pin_num];

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    HAL_GPIO_WritePin(gpio_port[stm32f4_gpio->port], (uint16_t)(1U << stm32f4_gpio->pin), (GPIO_PinState)state);

    return ret_value;
}

static asdk_errorcode_t get_gpio_state(asdk_mcu_pin_t pin_num, asdk_gpio_state_t *state, bool read_input_state)
{
    const dal_pin_t *stm32f4_gpio = NULL;

    if (ASDK_STM32F411_PIN_MAX <= pin_num)
    {
        return ASDK_GPIO_ERROR_INVALID_MCU_PIN;
    }

    stm32f4_gpio = &pin_map[pin_num];

    if (STM32F4_GPIO_PORT_MAX <= stm32f4_gpio->port)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PORT;
    }

    if (STM32F4_GPIO_PIN_MAX <= stm32f4_gpio->pin)
    {
        return ASDK_GPIO_ERROR_INVALID_GPIO_PIN;
    }

    *state = (asdk_gpio_state_t)HAL_GPIO_ReadPin(gpio_port[stm32f4_gpio->port], (uint16_t)(1U << stm32f4_gpio->pin));


    return ASDK_GPIO_SUCCESS;
}

static inline asdk_errorcode_t set_mode_configuration(asdk_gpio_mode_t gpio_mode, GPIO_InitTypeDef *GPIO_Type)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_mode)
    {
    case ASDK_GPIO_MODE_INPUT:
    	GPIO_Type->Mode = GPIO_MODE_INPUT;
        break;

    case ASDK_GPIO_MODE_OUTPUT:
    	GPIO_Type->Mode = GPIO_MODE_OUTPUT_PP;
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_GPIO_MODE;
        break;
    }

    return ret_value;
}

static inline asdk_errorcode_t set_pull_configuration(asdk_gpio_pull_t gpio_pull, GPIO_InitTypeDef *GPIO_Type)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_pull)
    {
    /* for GPIO as input */
    case ASDK_GPIO_HIGH_Z:
    	GPIO_Type->Pull = GPIO_NOPULL;
        break;

    /* for GPIO as output */
    case ASDK_GPIO_PUSH_PULL:
    	GPIO_Type->Pull = GPIO_NOPULL;
        break;

    /* for GPIO as input */
    case ASDK_GPIO_PULL_UP:
    	GPIO_Type->Pull = GPIO_PULLUP;
        break;

    case ASDK_GPIO_PULL_DOWN:
    	GPIO_Type->Pull = GPIO_PULLDOWN;
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_PULL_TYPE;
        break;
    }

    return ret_value;
}

static inline asdk_errorcode_t set_speed_configuration(asdk_gpio_speed_t gpio_speed, GPIO_InitTypeDef *GPIO_Type)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;

    switch (gpio_speed)
    {
    case ASDK_GPIO_SPEED_HIGH_FREQ:
    	GPIO_Type->Speed = GPIO_SPEED_FREQ_HIGH;
        break;

    case ASDK_GPIO_SPEED_LOW_FREQ:
    	GPIO_Type->Speed = GPIO_SPEED_FREQ_LOW;
        break;

    // TODO : THERE ARE MORE OPTIONS FROM ST

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_SPEED_TYPE;
    }

    return ret_value;
}

static inline asdk_errorcode_t set_isr(asdk_gpio_config_t *gpio_cfg, stm32f4_port_t port, stm32f4_pin_t pin, GPIO_InitTypeDef *GPIO_Type)
{
    asdk_errorcode_t ret_value = ASDK_GPIO_SUCCESS;
    uint8_t pin_idx = 0;
    uint8_t pin_num = 0;

    if (gpio_cfg->interrupt_config.priority > 15)
    {
    	return ASDK_GPIO_ERROR_INTERRUPT_PRIORITY_EXCEEDED;
    }

    pin_idx = pin/4;
    pin_num = pin%4;

    /* Configure Port of the source input */
	SYSCFG->EXTICR[pin_idx] &= ~(0xFU << pin_num);

	if (port == STM32F4_GPIO_PORT_H)
		SYSCFG->EXTICR[pin_idx] |= (0x7U << pin_num);
	else
		SYSCFG->EXTICR[pin_idx] |= (((uint16_t)port) << pin_num);

    switch (gpio_cfg->interrupt_config.type)
    {
    case ASDK_GPIO_INTERRUPT_DISABLED:
    	GPIO_Type->Mode = GPIO_MODE_INPUT;

    	EXTI->FTSR &= ~(1U << pin);
    	EXTI->RTSR &= ~(1U << pin);
    	EXTI->IMR &= ~(1U << pin);
        break;

    case ASDK_GPIO_INTERRUPT_RISING_EDGE:
        GPIO_Type->Mode = GPIO_MODE_IT_RISING;

    	EXTI->FTSR &= ~(1U << pin);
    	EXTI->RTSR |= (1U << pin);
    	EXTI->IMR |= (1U << pin);
        break;

    case ASDK_GPIO_INTERRUPT_FALLING_EDGE:
    	GPIO_Type->Mode = GPIO_MODE_IT_FALLING;

    	EXTI->FTSR |= (1U << pin);
    	EXTI->RTSR &= ~(1U << pin);
    	EXTI->IMR |= (1U << pin);
        break;

    case ASDK_GPIO_INTERRUPT_BOTH_EDGES:
    	GPIO_Type->Mode = GPIO_MODE_IT_RISING_FALLING;

    	EXTI->FTSR |= (1U << pin);
    	EXTI->RTSR |= (1U << pin);
    	EXTI->IMR |= (1U << pin);
        break;

    /* error */
    default:
        ret_value = ASDK_GPIO_ERROR_INVALID_INTERRUPT_TYPE;
        break;
    }

    if (pin <= STM32F4_GPIO_PIN_4)
    {
    	HAL_NVIC_SetPriority(EXTI0_IRQn + pin, gpio_cfg->interrupt_config.priority, 0);
    	HAL_NVIC_EnableIRQ(EXTI0_IRQn + pin);
    }
    else if (pin <= STM32F4_GPIO_PIN_9)
    {
    	HAL_NVIC_SetPriority(EXTI9_5_IRQn, gpio_cfg->interrupt_config.priority, 0);
    	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
    else
    {
    	HAL_NVIC_SetPriority(EXTI15_10_IRQn, gpio_cfg->interrupt_config.priority, 0);
    	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }

    return ret_value;
}

static inline void __enable_gpio_clock(stm32f4_port_t port)
{
    switch (port)
    {
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
