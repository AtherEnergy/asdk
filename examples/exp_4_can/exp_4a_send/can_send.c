/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_can.h"

#define DELAY_250_MS 250

#define ACX_CAN_CH 1

uint8_t tx_data_buffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

asdk_can_message_t txd_message = {
    .can_id = 0x211,
    .dlc = 8,
    .message = tx_data_buffer,
};


static asdk_gpio_config_t user_led_1 =
    {
        .mcu_pin = MCU_PIN_77,
        .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
        .gpio_init_state = ASDK_GPIO_STATE_HIGH,
        .gpio_pull = ASDK_GPIO_PUSH_PULL
    };

static asdk_can_config_t user_can_0_cfg = 
    {
        .mcu_pins = 
        {
            .rx_pin = MCU_PIN_5,
            .tx_pin = MCU_PIN_4
        },

        .controller_settings =
        {
            .mode           = ASDK_CAN_MODE_STANDARD,
            .can_id_type    = ASDK_CAN_ID_STANDARD,
            .max_dlc        = ASDK_CAN_DLC_8,
            .bitrate_config.can =
            {
                .baudrate = ASDK_CAN_BAUDRATE_500K,
                .bit_time = {
                    .prop_segment       = 29,
                    .phase_segment1     = 5,
                    .phase_segment2     = 5,
                    .sync_jump_width    = 5,
                }
            },
            .interrupt_config =
            {
                .use_interrupt = true,
                .priority = 1
            } 
        },

        .hw_filter = {0}
    };

static void user_can_callback(uint8_t can_ch, asdk_can_event_t event, asdk_can_message_t *can_message)
{
    switch (event)
    {
    case ASDK_CAN_TX_COMPLETE_EVENT:
        break;
    
    case ASDK_CAN_RX_EVENT:
        break;

    default:
        break;
    }
}


int main()
{
    bool tx_status_busy = false;

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

    asdk_gpio_output_clear(user_led_1.mcu_pin);

    asdk_can_init(ACX_CAN_CH, &user_can_0_cfg);
    asdk_can_install_callback(user_can_callback);

    for (;;)
    {
        asdk_gpio_output_toggle(user_led_1.mcu_pin);
        
        asdk_can_is_tx_busy(ACX_CAN_CH, 0, &tx_status_busy);

        if(!tx_status_busy)
        {
            asdk_can_write(ACX_CAN_CH, 0, &txd_message);
        }
        
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
