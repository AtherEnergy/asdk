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
    asdk_spi.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_spi.c

    @Created on
    Aug 7, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the SPI module for CYT2B75 microcontroller.

    To do: Support for multiple slave selects in SPI
            Implement the blocking transfer API.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_scb.h"
#include "asdk_spi.h" // ASDK SPI APIs
#include "asdk_pinmux.h"
#include "asdk_clock.h" // CYT2B75 DAL clock APIs

// sdk includes
#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "scb/cy_scb_spi.h"    // CYT2B75 SPI driver APIs
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h" // CYT2B75 system Interrupt APIs

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/
static cy_stc_scb_spi_config_t SCB_SPI_cfg =
    {
        .spiMode = CY_SCB_SPI_MASTER,       /*** Specifies the mode of operation    ***/
        .subMode = CY_SCB_SPI_MOTOROLA,     /*** Specifies the submode of SPI operation    ***/
        .sclkMode = CY_SCB_SPI_CPHA0_CPOL0, /*** Clock is active low, data is changed on first edge ***/
        .oversample = SCB_SPI_OVERSAMPLING, /*** SPI_CLOCK divided by SCB_SPI_OVERSAMPLING shoud be baudrate  ***/
        .rxDataWidth = 16,                  /*** The width of RX data (valid range 4-16). It must be the same as \ref txDataWidth except in National sub-mode. ***/
        .txDataWidth = 16,                  /*** The width of TX data (valid range 4-16). It must be the same as \ref rxDataWidth except in National sub-mode. ***/
        .enableMsbFirst = true,             /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
        .enableFreeRunSclk = false,         /*** Enables the master to generate a continuous SCLK regardless of whether there is data to send  ***/
        .enableInputFilter = false,         /*** Enables a digital 3-tap median filter to be applied to the input of the RX FIFO to filter glitches on the line. ***/
        .enableMisoLateSample = true,       /*** Enables the master to sample MISO line one half clock later to allow better timings. ***/
        .enableTransferSeperation = false,  /*** Enables the master to transmit each data element separated by a de-assertion of the slave select line (only applicable for the master mode) ***/
        .ssPolarity0 = 0,                   /*** SS0: active low ***/
        .ssPolarity1 = 0,                   /*** SS1: active low ***/
        .ssPolarity2 = 0,                   /*** SS2: active low ***/
        .ssPolarity3 = 0,                   /*** SS3: active low ***/
        .enableWakeFromSleep = 0,           /*** When set, the slave will wake the device when the slave select line becomes active. Note that not all SCBs support this mode. Consult the device datasheet to determine which SCBs support wake from deep sleep. ***/
        // In case of using high level APIs, an user does not need to set these parameter. These parameter will be set in high level APIs such as "Cy_SCB_SPI_Transfer".
        .txFifoTriggerLevel = 0,       /*** When there are fewer entries in the TX FIFO, then at this level the TX trigger output goes high. This output can be connected to a DMA channel through a trigger mux. Also, it controls the \ref CY_SCB_SPI_TX_TRIGGER interrupt source. ***/
        .rxFifoTriggerLevel = 255,     /*** When there are more entries in the RX FIFO, then at this level the RX trigger output goes high. This output can be connected to a DMA channel through a trigger mux. Also, it controls the \ref CY_SCB_SPI_RX_TRIGGER interrupt source.  ***/
        .rxFifoIntEnableMask = 0,      /*** Bits set in this mask will allow events to cause an interrupt  ***/
        .txFifoIntEnableMask = 0,      /*** Bits set in this mask allow events to cause an interrupt  ***/
        .masterSlaveIntEnableMask = 0, /*** Bits set in this mask allow events to cause an interrupt  ***/
        .enableSpiDoneInterrupt = 0,
        .enableSpiBusErrorInterrupt = 0,
};

static const cy_stc_scb_spi_config_t SCB_SPI_Slave_cfg =
    {
        .spiMode = CY_SCB_SPI_SLAVE,        /*** Specifies the mode of operation    ***/
        .subMode = CY_SCB_SPI_MOTOROLA,     /*** Specifies the submode of SPI operation    ***/
        .sclkMode = CY_SCB_SPI_CPHA1_CPOL0, /*** Clock is active low, data is changed on first edge ***/
        .oversample = SCB_SPI_OVERSAMPLING, /*** SPI_CLOCK divided by SCB_SPI_OVERSAMPLING shoud be baudrate  ***/
        .rxDataWidth = 8,                   /*** The width of RX data (valid range 4-16). It must be the same as \ref txDataWidth except in National sub-mode. ***/
        .txDataWidth = 8,                   /*** The width of TX data (valid range 4-16). It must be the same as \ref rxDataWidth except in National sub-mode. ***/
        .enableMsbFirst = true,             /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
        .enableFreeRunSclk = false,         /*** Enables the master to generate a continuous SCLK regardless of whether there is data to send  ***/
        .enableInputFilter = false,         /*** Enables a digital 3-tap median filter to be applied to the input of the RX FIFO to filter glitches on the line. ***/
        .enableMisoLateSample = true,       /*** Enables the master to sample MISO line one half clock later to allow better timings. ***/
        .enableTransferSeperation = false,  /*** Enables the master to transmit each data element separated by a de-assertion of the slave select line (only applicable for the master mode) ***/
        .ssPolarity0 = 0,                   /*** SS0: active low ***/
        .ssPolarity1 = 0,                   /*** SS1: active low ***/
        .ssPolarity2 = 0,                   /*** SS2: active low ***/
        .ssPolarity3 = 0,                   /*** SS3: active low ***/
        .enableWakeFromSleep = 0,           /*** When set, the slave will wake the device when the slave select line becomes active. Note that not all SCBs support this mode. Consult the device datasheet to determine which SCBs support wake from deep sleep. ***/
        // In case of using high level APIs, an user does not need to set these parameter. These parameter will be set in high level APIs such as "Cy_SCB_SPI_Transfer".
        .txFifoTriggerLevel = 0,       /*** When there are fewer entries in the TX FIFO, then at this level the TX trigger output goes high. This output can be connected to a DMA channel through a trigger mux. Also, it controls the \ref CY_SCB_SPI_TX_TRIGGER interrupt source. ***/
        .rxFifoTriggerLevel = 255,     /*** When there are more entries in the RX FIFO, then at this level the RX trigger output goes high. This output can be connected to a DMA channel through a trigger mux. Also, it controls the \ref CY_SCB_SPI_RX_TRIGGER interrupt source.  ***/
        .rxFifoIntEnableMask = 0,      /*** Bits set in this mask will allow events to cause an interrupt  ***/
        .txFifoIntEnableMask = 0,      /*** Bits set in this mask allow events to cause an interrupt  ***/
        .masterSlaveIntEnableMask = 0, /*** Bits set in this mask allow events to cause an interrupt  ***/
        .enableSpiDoneInterrupt = 0,
        .enableSpiBusErrorInterrupt = 0,
};

extern volatile stc_SCB_t *scb_base_ptrs[];
extern uint8_t scb_spi[];
/*Driver state structure*/
static cy_stc_scb_spi_context_t g_stc_spi_context[ASDK_SPI_MAX];

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/
/*Considering OVS factor of 4*/
#define SPI_PERIPHERAL_CLOCK_IN_Hz 500000

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
/* static array of function pointer to hold user callback function */
static asdk_spi_callback_fun_t user_spi_callback_fun_list[ASDK_SPI_MAX];
static asdk_clock_peripheral_t spi_clock = {0};

/* Callback for all peripherals which supports SPI features */
void SPI0_ISR_UserCallback(void);
void SPI1_ISR_UserCallback(void);
void SPI2_ISR_UserCallback(void);
void SPI3_ISR_UserCallback(void);
void SPI4_ISR_UserCallback(void);
void SPI5_ISR_UserCallback(void);
void SPI6_ISR_UserCallback(void);
void SPI7_ISR_UserCallback(void);

scb_user_cb_type SPI_User_callbacks[ASDK_SPI_MAX] = {
    SPI0_ISR_UserCallback,
    SPI1_ISR_UserCallback,
    SPI2_ISR_UserCallback,
    SPI3_ISR_UserCallback,
    SPI4_ISR_UserCallback,
    SPI5_ISR_UserCallback,
    SPI6_ISR_UserCallback,
    SPI7_ISR_UserCallback,
};

/*SCB SPI callback event handler is called when the specific event occurs*/
void scb_spi0_callback_handler(uint32_t event_t);
void scb_spi1_callback_handler(uint32_t event_t);
void scb_spi2_callback_handler(uint32_t event_t);
void scb_spi3_callback_handler(uint32_t event_t);
void scb_spi4_callback_handler(uint32_t event_t);
void scb_spi5_callback_handler(uint32_t event_t);
void scb_spi6_callback_handler(uint32_t event_t);
void scb_spi7_callback_handler(uint32_t event_t);

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/*Initialization function for the SPI in Master / Slave type*/
asdk_errorcode_t asdk_spi_init(asdk_spi_config_t *spi_config_data)
{

    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;
    cy_en_scb_spi_status_t cy_spi_init_status;

    asdk_pinmux_config_t SPI_pinmux_array[3] = {
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MOSI, spi_config_data->SPI_no), .MCU_pin_num = spi_config_data->SPI_MOSI_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF},
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MISO, spi_config_data->SPI_no), .MCU_pin_num = spi_config_data->SPI_MISO_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_HIGHZ},
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CLK, spi_config_data->SPI_no), .MCU_pin_num = spi_config_data->SPI_CLK_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF},

    };

    /*****************************************/

    /* validate configuration parameters */

    if (NULL == spi_config_data)
    {
        return ASDK_SPI_ERROR_NULL_PTR;
    }

    if (ASDK_SPI_MAX <= spi_config_data->SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    if (ASDK_SPI_SLAVE_MAX <= spi_config_data->no_of_slaves)
    {
        return ASDK_SPI_ERROR_SLAVE_RANGE_EXCEEDED;
    }

    if ((ASDK_SPI_DATAWIDTH_8_BITS != spi_config_data->SPI_datawidth) && (ASDK_SPI_DATAWIDTH_16_BITS != spi_config_data->SPI_datawidth))
    {
        return ASDK_SPI_ERROR_INVALID_DATA_SIZE;
    }

    if (ASDK_SPI_TRANSFER_RATE_MAX <= spi_config_data->SPI_transfer_rate)
    {
        return ASDK_SPI_ERROR_INVALID_BAUD_RATE;
    }

    if ((ASDK_SPI_BIT_ORDER_MSB_FIRST != spi_config_data->SPI_bit_order) && (ASDK_SPI_BIT_ORDER_LSB_FIRST != spi_config_data->SPI_bit_order))
    {
        return ASDK_SPI_ERROR_INVALID_BIT_ORDER;
    }

    if (ASDK_SPI_DEVICE_MASTER == spi_config_data->SPI_type)
    {
        SPI_pinmux_array[0] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MOSI, spi_config_data->SPI_no), spi_config_data->SPI_MOSI_mcu_pin_no, ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF};

        SPI_pinmux_array[1] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MISO, spi_config_data->SPI_no), spi_config_data->SPI_MISO_mcu_pin_no, ASDK_GPIO_PULL_TYPE_HIGHZ};

        SPI_pinmux_array[2] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CLK, spi_config_data->SPI_no), spi_config_data->SPI_CLK_mcu_pin_no, ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF};
    }
    else if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
    {
        SPI_pinmux_array[0] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MOSI, spi_config_data->SPI_no), spi_config_data->SPI_MOSI_mcu_pin_no, ASDK_GPIO_PULL_TYPE_HIGHZ};

        SPI_pinmux_array[1] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_MISO, spi_config_data->SPI_no), spi_config_data->SPI_MISO_mcu_pin_no, ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF};

        SPI_pinmux_array[2] = (asdk_pinmux_config_t){ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CLK, spi_config_data->SPI_no), spi_config_data->SPI_CLK_mcu_pin_no, ASDK_GPIO_PULL_TYPE_HIGHZ};
    }
    else
    {
        return ASDK_SPI_ERROR_INVALID_TYPE;
    }

    /*Setting the PinMux for the SPI MOSI, SPI MISO and SPI CLK pins*/
    ret_value = asdk_set_pinmux(SPI_pinmux_array, 3);

    /*Cyt specific :Mapping SPI no to the associated SCB Module*/
    scb_index = scb_spi[spi_config_data->SPI_no];
    scb = scb_base_ptrs[scb_index];

    if (false == (SCB_Availability_Status(scb)))
    {
        return ASDK_SPI_ERROR_MODULE_UNAVAILABLE;
    }

    if ((true == (spi_config_data->SPI_Interrupt_config.use_interrupt)))
    {
        /* Configure the user interrupt handler to make the high-level API work*/
        SCB_Set_ISR(scb_index, SPI_User_callbacks[spi_config_data->SPI_no], spi_config_data->SPI_Interrupt_config.priority);
    }
    else
    {
        return ASDK_SPI_ERROR_ISR_REQUIRED;
    }

    /*For teting to be moved to clock config*/

    /*Source clock = 80000000
      Target freq = OVS*BaudRate = 4 * 125000kbps = 500000
    */

    spi_clock.ch_no = 0;
    spi_clock.module_no = scb_index;
    spi_clock.peripheral_type = ASDK_SPI;

    if (ASDK_SPI_TRANSFER_RATE_125KHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 500000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_250KHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 1000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_500KHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 2000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_1MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 4000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_2MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 8000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_3MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 12000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_4MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 16000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_5MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 20000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_6MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 24000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_7MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 28000000;
    }
    else if (ASDK_SPI_TRANSFER_RATE_8MHz == spi_config_data->SPI_transfer_rate)
    {
        spi_clock.target_frequency = 32000000;
    }
    else
    {
        return ASDK_SPI_ERROR_INVALID_BAUD_RATE;
    }

    ret_value = asdk_clock_enable(&spi_clock, NULL);
    if (ASDK_CLOCK_SUCCESS != ret_value)
    {
        // clock error
        return ret_value;
    }

    /*Updating the necessary configuration as per the Master/Slave type*/
    SCB_SPI_cfg.spiMode = spi_config_data->SPI_type;
    if (ASDK_SPI_DATAWIDTH_8_BITS == spi_config_data->SPI_datawidth)
    {
        SCB_SPI_cfg.rxDataWidth = 8;
        SCB_SPI_cfg.txDataWidth = 8;
    }
    else if (ASDK_SPI_DATAWIDTH_16_BITS == spi_config_data->SPI_datawidth)
    {
        SCB_SPI_cfg.rxDataWidth = 16;
        SCB_SPI_cfg.txDataWidth = 16;
    }
    else
    {
        return ASDK_SPI_ERROR_INVALID_DATA_SIZE;
    }

    if (ASDK_SPI_MODE_0 == spi_config_data->SPI_mode)
    {
        SCB_SPI_cfg.sclkMode = CY_SCB_SPI_CPHA0_CPOL0;
    }
    else if (ASDK_SPI_MODE_1 == spi_config_data->SPI_mode)
    {
        SCB_SPI_cfg.sclkMode = CY_SCB_SPI_CPHA1_CPOL0;
    }
    else if (ASDK_SPI_MODE_2 == spi_config_data->SPI_mode)
    {
        SCB_SPI_cfg.sclkMode = CY_SCB_SPI_CPHA0_CPOL1;
    }
    else if (ASDK_SPI_MODE_3 == spi_config_data->SPI_mode)
    {
        SCB_SPI_cfg.sclkMode = CY_SCB_SPI_CPHA1_CPOL1;
    }
    else
    {
        return ASDK_SPI_ERROR_INVALID_SPI_MODE;
    }

    SCB_SPI_cfg.enableMsbFirst = spi_config_data->SPI_bit_order;
    SCB_SPI_cfg.oversample = 4;

    for (uint8_t Slave_no = 0; Slave_no < spi_config_data->no_of_slaves; Slave_no++)
    {

        asdk_pinmux_config_t SPI_pinmux_array_CS[1] = {{.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS1, 5), .MCU_pin_num = spi_config_data->SPI_CS_mcu_pin_no[Slave_no], .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF}};
        if ((0xFF != spi_config_data->SPI_CS_mcu_pin_no[0]) && (0 == Slave_no))
        {
            SCB_SPI_cfg.ssPolarity0 = 0;
            if (ASDK_SPI_DEVICE_MASTER == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS0, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF

                };
            }
            else if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS0, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_HIGHZ

                };
            }
        }
        else if ((0xFF != spi_config_data->SPI_CS_mcu_pin_no[1]) && (1 == Slave_no))
        {
            SCB_SPI_cfg.ssPolarity1 = 0;
            if (ASDK_SPI_DEVICE_MASTER == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS1, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF

                };
            }
            else if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS1, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_HIGHZ

                };
            }
        }
        else if ((0xFF != spi_config_data->SPI_CS_mcu_pin_no[2]) && (2 == Slave_no))
        {
            SCB_SPI_cfg.ssPolarity2 = 0;
            if (ASDK_SPI_DEVICE_MASTER == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS2, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF

                };
            }
            else if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS2, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_HIGHZ

                };
            }
        }
        else if ((0xFF != spi_config_data->SPI_CS_mcu_pin_no[3]) && (3 == Slave_no))
        {
            SCB_SPI_cfg.ssPolarity3 = 0;
            if (ASDK_SPI_DEVICE_MASTER == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS3, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF

                };
            }
            else if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
            {
                SPI_pinmux_array_CS[0] = (asdk_pinmux_config_t){
                    ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_SPI, ASDK_PINMUX_SPI_SUBFUN_CS3, spi_config_data->SPI_no), spi_config_data->SPI_CS_mcu_pin_no[Slave_no], ASDK_GPIO_PULL_TYPE_HIGHZ

                };
            }
        }
        else
        {
            return ASDK_SPI_ERROR_INVALID_SLAVE_SELECT;
        }
        ret_value = asdk_set_pinmux(SPI_pinmux_array_CS, 1);
    }

    /*De-Initialize the SPI Instance*/
    Cy_SCB_SPI_DeInit(scb);

    if (ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type)
    {

        cy_spi_init_status = Cy_SCB_SPI_Init(scb, &SCB_SPI_Slave_cfg, &g_stc_spi_context[spi_config_data->SPI_no]);
    }
    else
    {
        /*SCB Initilization for SPI communication*/
        cy_spi_init_status = Cy_SCB_SPI_Init(scb, &SCB_SPI_cfg, &g_stc_spi_context[spi_config_data->SPI_no]);
    }

    if ((ASDK_SPI_DEVICE_SLAVE == spi_config_data->SPI_type))
    {
        Cy_SCB_SPI_SetActiveSlaveSelect(scb, 0);
    }

    if (CY_SCB_SPI_SUCCESS == cy_spi_init_status)
    {
        ret_value = ASDK_SPI_STATUS_SUCCESS;
    }
    else 
    {
        ret_value = ASDK_SPI_ERROR_INIT_FAIL;

    }
    /*Enable the SPI module*/
    Cy_SCB_SPI_Enable(scb);

    return ret_value;
}

/*De-initialization function of SPI Module */
asdk_errorcode_t asdk_spi_deinit(asdk_spi_num_t SPI_no)
{

    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;

    if (ASDK_SPI_MAX <= SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    /*Cyt specific :Mapping SPI no to the associated SCB Module*/
    scb_index = scb_spi[SPI_no];
    scb = scb_base_ptrs[scb_index];
    /*De-Initialize the SPI Instance*/
    Cy_SCB_SPI_DeInit(scb);

    return ret_value;
}

/*Get the peripheral transfer status*/
asdk_errorcode_t asdk_spi_transfer_status(asdk_spi_num_t SPI_no, asdk_spi_transfer_status_t *SPI_transfer_status)
{

    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;
    uint32_t transfer_status;

    if (ASDK_SPI_MAX <= SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    /*Cyt specific :Mapping SPI no to the associated SCB Module*/
    scb_index = scb_spi[SPI_no];
    scb = scb_base_ptrs[scb_index];
    /*Get the transfer status of SPI*/
    transfer_status = Cy_SCB_SPI_GetTransferStatus(scb, &g_stc_spi_context[SPI_no]);

    /*Find the transfer status*/
    if (transfer_status)
    {
        if (transfer_status & CY_SCB_SPI_TRANSFER_ACTIVE)
        {
            *SPI_transfer_status = ASDK_SPI_TRANSFER_STATUS_ACTIVE;
        }

        if (transfer_status & CY_SCB_SPI_TRANSFER_IN_FIFO)
        {
            *SPI_transfer_status = ASDK_SPI_TRANSFER_STATUS_INFIFO;
        }

        if (transfer_status & CY_SCB_SPI_SLAVE_TRANSFER_ERR)
        {
            *SPI_transfer_status = ASDK_SPI_TRANSFER_STATUS_SLAVE_ERROR;
        }

        if (transfer_status & CY_SCB_SPI_TRANSFER_OVERFLOW)
        {
            *SPI_transfer_status = ASDK_SPI_TRANSFER_STATUS_OVERFLOW;
        }

        if (transfer_status & CY_SCB_SPI_TRANSFER_UNDERFLOW)
        {
            *SPI_transfer_status = ASDK_SPI_TRANSFER_STATUS_UNDERFLOW;
        }

        ret_value = ASDK_SPI_STATUS_SUCCESS;
    }
    else
    {
        ret_value = ASDK_SPI_STATUS_ERROR;
    }

    return ret_value;
}

asdk_errorcode_t asdk_spi_install_callback(asdk_spi_num_t SPI_no, asdk_spi_callback_fun_t callback_fun)
{
    /* Local Variables */
    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;

    /* check for max spi module */
    if (ASDK_SPI_MAX <= SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_spi[SPI_no];
    scb = scb_base_ptrs[scb_index];

    /* assign hw callback function name */
    switch (SPI_no)
    {
    case 0:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi0_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 1:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi1_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 2:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi2_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 3:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi3_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 4:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi4_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 5:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi5_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 6:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi6_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    case 7:
        Cy_SCB_SPI_RegisterCallback(scb, (scb_spi_handle_events_t)scb_spi7_callback_handler, &g_stc_spi_context[SPI_no]);
        break;
    default:
        break;
    }

    /* store callback function */
    user_spi_callback_fun_list[SPI_no] = callback_fun;

    return ret_value;

} /* spi_install_callback */

/*Transfer the data in the non blocking mode*/
asdk_errorcode_t asdk_spi_transfer_non_blocking(asdk_spi_num_t SPI_no, uint8_t *send_buf, uint8_t *recv_buf, uint16_t length, uint8_t Slave_Select_no)

{

    /* Local Variables */
    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    cy_en_scb_spi_status_t cy_spi_transfer_status;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;

    /* check for max spi module */
    if (ASDK_SPI_MAX <= SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_spi[SPI_no];
    scb = scb_base_ptrs[scb_index];

    Cy_SCB_SPI_Disable(scb, &g_stc_spi_context[SPI_no]);
    /*Select the Slave with which the communication needs to be established*/
    Cy_SCB_SPI_SetActiveSlaveSelect(scb, Slave_Select_no);

    Cy_SCB_SPI_Enable(scb);

    /*Call the driver transfer function*/
    cy_spi_transfer_status = Cy_SCB_SPI_Transfer(scb, send_buf, recv_buf, length, &g_stc_spi_context[SPI_no]);
    if (CY_SCB_SPI_SUCCESS != cy_spi_transfer_status)
    {
        ret_value = ASDK_SPI_ERROR_TRANSFER_FAIL;
    }

    return ret_value;
}

/*Transfer the data in the blocking mode*/
asdk_errorcode_t asdk_spi_transfer_blocking(uint8_t SPI_no, uint8_t *send_buf, uint8_t *recv_buf, uint16_t length, uint8_t Slave_Select_no)
{

    /* Local Variables */
    asdk_errorcode_t ret_value = ASDK_SPI_STATUS_SUCCESS;
    // cy_en_scb_spi_status_t cy_spi_transfer_status;
    // uint8_t scb_index;
    // volatile stc_SCB_t *scb = NULL;

    /* check for max spi module */
    if (ASDK_SPI_MAX <= SPI_no)
    {
        return ASDK_SPI_ERROR_RANGE_EXCEEDED;
    }

    // scb_index = scb_spi[SPI_no];
    // scb = scb_base_ptrs[scb_index];
    ret_value = ASDK_SPI_ERROR_FEATURE_NOT_IMPLEMENTED;

    return ret_value;
}

/*----------------------------------------------------------------------------*/
/* Function : spi_master_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi master module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi0_callback_handler(uint32_t event)
{

    uint8_t spi_no = 0;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:
        buf = (uint8_t *)g_stc_spi_context[0].rxBuf;
        numToCopy = g_stc_spi_context[0].rxBufIdx;
        if (16 == SCB_SPI_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }
        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[0].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : spi_slave_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi slave module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi1_callback_handler(uint32_t event)
{
    uint8_t spi_no = 1;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:

        buf = (uint8_t *)g_stc_spi_context[1].rxBuf;
        numToCopy = g_stc_spi_context[1].rxBufIdx;
        if (16 == SCB_SPI_Slave_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }

        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[1].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_spi2_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi master module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi2_callback_handler(uint32_t event)
{

    uint8_t spi_no = 2;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:
        buf = (uint8_t *)g_stc_spi_context[2].rxBuf;
        numToCopy = g_stc_spi_context[2].rxBufIdx;
        if (16 == SCB_SPI_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }
        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[2].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_spi3_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi slave module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi3_callback_handler(uint32_t event)
{
    uint8_t spi_no = 3;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:

        buf = (uint8_t *)g_stc_spi_context[1].rxBuf;
        numToCopy = g_stc_spi_context[1].rxBufIdx;
        if (16 == SCB_SPI_Slave_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }

        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[3].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}
/*----------------------------------------------------------------------------*/
/* Function : scb_spi4_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi master module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi4_callback_handler(uint32_t event)
{

    uint8_t spi_no = 4;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:
        buf = (uint8_t *)g_stc_spi_context[4].rxBuf;
        numToCopy = g_stc_spi_context[4].rxBufIdx;
        if (16 == SCB_SPI_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }
        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[4].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_spi5_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi slave module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi5_callback_handler(uint32_t event)
{
    uint8_t spi_no = 5;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:

        buf = (uint8_t *)g_stc_spi_context[5].rxBuf;
        numToCopy = g_stc_spi_context[5].rxBufIdx;
        if (16 == SCB_SPI_Slave_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }

        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[5].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_spi6_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi master module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
void scb_spi6_callback_handler(uint32_t event)
{

    uint8_t spi_no = 6;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:
        buf = (uint8_t *)g_stc_spi_context[6].rxBuf;
        numToCopy = g_stc_spi_context[6].rxBufIdx;
        if (16 == SCB_SPI_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }
        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[6].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_spi7_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the spi slave module using non blocking method

  @param void *driverState

  @param spi_event_t event

  @param void *userData

  @return void

*/
#if 1
void scb_spi7_callback_handler(uint32_t event)
{
    uint8_t spi_no = 7;
    uint32_t numToCopy = 0;
    uint8_t *buf;
    volatile stc_SCB_t *scb = NULL;
    scb = scb_base_ptrs[spi_no];

    switch (event)
    {
    case CY_SCB_SPI_TRANSFER_CMPLT_EVENT:

        buf = (uint8_t *)g_stc_spi_context[7].rxBuf;
        numToCopy = g_stc_spi_context[7].rxBufIdx;
        if (16 == SCB_SPI_Slave_cfg.rxDataWidth)
        {
            buf = buf - (2 * numToCopy);
        }
        else
        {
            buf = buf - (numToCopy);
        }

        Cy_SCB_SPI_ClearRxFifo(scb);
        Cy_SCB_SPI_ClearTxFifo(scb);

        /* process user function after end of transfer */
        user_spi_callback_fun_list[spi_no](spi_no, buf, g_stc_spi_context[7].rxBufIdx, ASDK_SPI_DEVICE_MASTER);
        break;

    default:
        break;
    }
}
#endif

/*SPI User Callback for Starting the transfer*/
void SPI0_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[0]], &g_stc_spi_context[0]);
}
void SPI1_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[1]], &g_stc_spi_context[1]);
}
void SPI2_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[2]], &g_stc_spi_context[2]);
}
void SPI3_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[3]], &g_stc_spi_context[3]);
}
void SPI4_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[4]], &g_stc_spi_context[4]);
}
void SPI5_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[5]], &g_stc_spi_context[5]);
}
void SPI6_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[6]], &g_stc_spi_context[6]);
}
void SPI7_ISR_UserCallback(void)
{
    /* SPI interrupt handler for High-Level APIs */
    Cy_SCB_SPI_Interrupt(scb_base_ptrs[scb_spi[7]], &g_stc_spi_context[7]);
}
