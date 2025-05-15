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
        .gpio_pull = ASDK_GPIO_PULL_UP,
        .interrupt_config = {
            .type = ASDK_GPIO_INTERRUPT_FALLING_EDGE,
            .priority = 1
        }
    };

void user_button_interrupt_callback(asdk_mcu_pin_t mcu_pin, uint32_t param)
{
    // 
    // All GPIO interrupts have a single callback.
    // Hence it is essential to check the gpio which caused this callback and
    // take necessary action for that interrupt.
    // 
    if(mcu_pin == user_button_0.mcu_pin)
    {
        // every time a falling edge is detected on user_button_0, the led pin is toggled
        asdk_gpio_output_toggle(user_led_1.mcu_pin);    
    }
}

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

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);

    // Initialising the gpio for input via button
    asdk_gpio_init(&user_button_0);

    // Assigning callback function for all gpio interrupts
    asdk_gpio_install_callback(&user_button_interrupt_callback);

    for (;;)
    {
        // Simulation of some process occurring, since we have nothing else to do
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
