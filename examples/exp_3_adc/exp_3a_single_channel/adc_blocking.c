/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_adc.h"

#define DELAY_1_S 1000
#define DELAY_250_MS 250

static asdk_gpio_config_t user_led_1 =
    {
        .mcu_pin = MCU_PIN_77,
        .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PUSH_PULL
    };

asdk_mcu_pin_t adc_pin[] = {MCU_PIN_45};

uint32_t adc_data;
float adc_pin_voltage;

asdk_adc_config_t adc_conf = {
    .pin_count = 1,
    .pin_nums = adc_pin,

    .adc_resolution = ASDK_ADC_RESOLUTION_12_BITS,
    .reference_voltage_src = ASDK_ADC_REF_VOLTAGE_SRC_INTERNAL,
    .adc_sampling_freq = ASDK_ADC_SAMPLING_FREQ_50KHz,
    .enable_adc_calibration = false,
    
    // .enable_interrupt = true,
    // .interrupt_priority = 1,
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

    asdk_clock_init(clk_cfg);

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);

    asdk_adc_init(&adc_conf);

    for (;;)
    {
        asdk_gpio_output_toggle(user_led_1.mcu_pin);
        asdk_adc_start_conversion_blocking(MCU_PIN_45, &adc_data);
        adc_pin_voltage = ((float)(adc_data/4095) * 3.3);
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
