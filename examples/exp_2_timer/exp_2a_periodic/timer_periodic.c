/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_timer.h"

#define DELAY_1_S 1000
#define DELAY_250_MS 250

static asdk_gpio_config_t user_led_1 =
    {
        .mcu_pin = MCU_PIN_77,
        .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PUSH_PULL
    };

void timer1_callback(asdk_timer_event_t timer_event)
{
    if(timer_event == ASDK_TIMER_TERMINAL_COUNT_EVENT)
    {
        asdk_gpio_output_toggle(user_led_1.mcu_pin);
    }
}

static asdk_timer_t user_timer_1 = 
    {
        .type = ASDK_TIMER_TYPE_PERIODIC,   // Observe the difference with ONESHOT and PERIODIC
        .direction = ASDK_TIMER_COUNT_DIRECTION_UP,
        .interrupt = {
                .enable = true,
                .priority = 1
            },
        .counter_clock = {
                .frequency = 200000,                     // choosing a clock of 200KHz
                .prescaler = ASDK_CLOCK_PRESCALER_4      // 200KHz / 4 = 50 KHz
            },
        .mode = {
            .type = ASDK_TIMER_MODE_TIMER,
            .config.timer = {
                .timer_period = 5000,                   // callback every 5000 ticks = 100ms
                .callback = &timer1_callback
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

    asdk_timer_init(1, &user_timer_1);

    asdk_gpio_output_clear(user_led_1.mcu_pin);

    asdk_timer_start(1);

    for (;;)
    {
        // Simulation of some process occurring, since we have nothing else to do
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
