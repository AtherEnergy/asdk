/* ASDK User actions starts ************************************************ */

/* ASDK User Action: add application include files */
#include "asdk_platform.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"

#define DELAY_250_MS 250

typedef enum
{
    INVALID_LED_EVT = 0,
    DISABLE_LED_EVT,
    ENABLE_LED_EVT,

    MAX_EVTS
} gpio_events_t;

static gpio_events_t gpio_evt = INVALID_LED_EVT;

static asdk_gpio_config_t user_led_1 = {
    .mcu_pin = MCU_PIN_77,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .gpio_init_state = ASDK_GPIO_STATE_HIGH,
    .gpio_pull = ASDK_GPIO_PUSH_PULL,
};

static asdk_gpio_config_t user_button = {
    .mcu_pin = MCU_PIN_29,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_pull = ASDK_GPIO_HIGH_Z,
    .interrupt_config = {
        .type = ASDK_GPIO_INTERRUPT_FALLING_EDGE,
        .priority = 3,
    },
};

static asdk_gpio_config_t other_button = {
    .mcu_pin = MCU_PIN_34,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_pull = ASDK_GPIO_PULL_UP,
    .interrupt_config = {
        .type = ASDK_GPIO_INTERRUPT_BOTH_EDGES,
        .priority = 3,
    },
};

static void gpio_callback(asdk_mcu_pin_t mcu_pin, uint32_t param)
{
    switch (mcu_pin)
    {
    case MCU_PIN_29:
        gpio_evt = DISABLE_LED_EVT;
        break;

    case MCU_PIN_34:
        gpio_evt = ENABLE_LED_EVT;
        break;

    default:
        gpio_evt = INVALID_LED_EVT;
        break;
    }
}

/* below functions are called by main.c */

void asdk_app_init()
{
    asdk_errorcode_t status = ASDK_GPIO_SUCCESS;

    asdk_clock_config_t clk_cfg = {
        .clk_source = ASDK_CLOCK_SRC_INT,
        .pll = {
            .input_frequency = 8000000,
            .output_frequency = 160000000,
        },
        .xtal_trim = {0},
    };

    asdk_clock_init(&clk_cfg);

    /* ASDK User Action: ASDK module init calls here */

    status = asdk_gpio_init(&user_led_1);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);

    status = asdk_gpio_init(&user_button); // J2-A0
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);

    status = asdk_gpio_init(&other_button); // J7-A8
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);

    status = asdk_gpio_install_callback(gpio_callback);
    ASDK_DEV_ERROR_ASSERT(status, ASDK_GPIO_SUCCESS);
}

void asdk_app_loop()
{
    for (;;)
    {
        /* ASDK User Action: add application iteration calls */

        switch (gpio_evt)
        {
        case ENABLE_LED_EVT:
            asdk_gpio_init(&user_led_1);
            gpio_evt = INVALID_LED_EVT;
            break;

        case DISABLE_LED_EVT:
            asdk_gpio_deinit(user_led_1.mcu_pin);
            gpio_evt = INVALID_LED_EVT;
            break;

        default:
            asdk_gpio_output_toggle(user_led_1.mcu_pin);
            break;
        }

        ASDK_DELAY_MS(DELAY_250_MS);
    }
}

/* ASDK User actions ends ************************************************** */
