/* ASDK: do not edit main file ********************************************* */

/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"

#define DELAY_1_S 1000
#define DELAY_250_MS 250

static asdk_gpio_config_t user_led_1 =
    {
        .mcu_pin = MCU_PIN_77,
        .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PUSH_PULL
    };

int main()
{
    asdk_sys_enable_interrupts();
    
    asdk_clock_config_t clk_cfg = {
    .clk_source = ASDK_CLOCK_SRC_INT,
    .pll = {
        .input_frequency = 8000000,
        .output_frequency = 160000000,
    },
    .xtal_trim = {0}};

    asdk_clock_init(&clk_cfg);

    asdk_gpio_init(&user_led_1);

    for (;;)
    {
        asdk_gpio_output_set(user_led_1.mcu_pin);
        ASDK_DELAY_MS(DELAY_1_S);

        asdk_gpio_output_clear(user_led_1.mcu_pin);
        ASDK_DELAY_MS(DELAY_1_S);
    }

    return 0;
}

/* ASDK: do not edit main file ********************************************* */
