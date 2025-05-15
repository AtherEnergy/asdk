/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_timer.h"

#define DELAY_1_S 1000
#define DELAY_250_MS 250

#define USER_TIMER_CH 1

uint32_t last_captured_value = 0;

static asdk_gpio_config_t user_led_1 =
    {
        .mcu_pin = MCU_PIN_77,
        .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PUSH_PULL
    };

static asdk_gpio_config_t user_led_4 =
    {
        .mcu_pin = MCU_PIN_47,
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

void timer1_capture_callback(asdk_timer_event_t timer_event, asdk_timer_capture_edge_t edge, uint32_t captured_value)
{
    if(timer_event == ASDK_TIMER_TERMINAL_COUNT_EVENT)
    {
        asdk_gpio_output_toggle(user_led_1.mcu_pin);    // led 1 should toggle every 1s
    }
    if(timer_event == ASDK_TIMER_CAPTURE_EVENT)
    {
        last_captured_value = captured_value;
        asdk_gpio_output_toggle(user_led_4.mcu_pin);    // led 4 should toggle everytime the button is pressed
    }
}

static asdk_timer_t user_timer_1 = 
    {
        .type = ASDK_TIMER_TYPE_PERIODIC,
        .direction = ASDK_TIMER_COUNT_DIRECTION_UP,
        .interrupt = {
                .enable = true,
                .priority = 1
            },
        .counter_clock = {
                .frequency = 1000,   // 1KHz
                .prescaler = ASDK_CLOCK_PRESCALER_2
            },
        .mode = {
            .type = ASDK_TIMER_MODE_CAPTURE,
            .config.capture = {
                .timer_period = 500,
                .mcu_pin = MCU_PIN_29,
                .edge = ASDK_TIMER_CAPTURE_ON_FALLING_EDGE,
                .callback = &timer1_capture_callback,
            },
        }, 
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

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);
    asdk_gpio_init(&user_led_4);
    asdk_gpio_init(&user_button_0);

    asdk_timer_init(USER_TIMER_CH, &user_timer_1);

    asdk_gpio_output_clear(user_led_1.mcu_pin);
    asdk_gpio_output_clear(user_led_4.mcu_pin);

    asdk_timer_start(USER_TIMER_CH);

    for (;;)
    {
        // Simulation of some process occurring, since we have nothing else to do
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
