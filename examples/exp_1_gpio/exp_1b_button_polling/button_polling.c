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

static asdk_gpio_config_t user_button_0 =
    {
        .mcu_pin = MCU_PIN_29,
        .gpio_mode = ASDK_GPIO_MODE_INPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PULL_UP
    };

int main()
{
    uint8_t input_state = 0;    // variable to store the state of the mcu pin

    asdk_sys_enable_interrupts();
    
    asdk_clock_config_t clk_cfg = {
    .clk_source = ASDK_CLOCK_SRC_INT,
    .pll = {
        .input_frequency = 8000000,
        .output_frequency = 160000000,
    },
    .xtal_trim = {0}};

    asdk_clock_init(&clk_cfg);

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);

    // Initialising the gpio for input via button
    asdk_gpio_init(&user_button_0);

    for (;;)
    {
        // Get the state of the input mcu pin
        asdk_gpio_get_input_state(user_button_0.mcu_pin, &input_state);

        // The default state of button is logic 1 (HIGH), hence the gpio is cleared (turned off)
        if(input_state)
        {
            asdk_gpio_output_clear(user_led_1.mcu_pin);
            
        }
        else    // when led is pressed, input state is logic 0 (LOW), hence the gpio is set (turned on) 
        {
            asdk_gpio_output_set(user_led_1.mcu_pin);
        }
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
