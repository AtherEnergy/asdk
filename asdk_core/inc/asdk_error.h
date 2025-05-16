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
    asdk_error.h

    @path
    inc/asdk_error.h

    @Created on
    Aug 07, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file represents the error codes for the ASDK.
*/

#ifndef ASDK_ERROR_H
#define ASDK_ERROR_H

/*!
 * @brief An enumerator to represent all error codes within ASDK.
 */
typedef enum
{
    ASDK_SUCCESS = 0,
    ASDK_ERROR,

    ASDK_GPIO_SUCCESS = 101,
    ASDK_GPIO_ERROR_NULL_PTR,
    ASDK_GPIO_ERROR_INVALID_MCU_PIN,
    ASDK_GPIO_ERROR_INVALID_GPIO_PORT,
    ASDK_GPIO_ERROR_INVALID_GPIO_PIN,
    ASDK_GPIO_ERROR_INVALID_PULL_TYPE,
    ASDK_GPIO_ERROR_INVALID_SPEED_TYPE,
    ASDK_GPIO_ERROR_INVALID_INTERRUPT_TYPE,
    ASDK_GPIO_ERROR_INVALID_STATE,
    ASDK_GPIO_ERROR_INVALID_GPIO_MODE,
    ASDK_GPIO_ERROR_INIT,
    ASDK_GPIO_ERROR_DEINIT,
    ASDK_GPIO_ERROR_INTERRUPT_PRIORITY_EXCEEDED,
    ASDK_GPIO_ERROR_MAX,

    ASDK_CLOCK_SUCCESS = 201,
    ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED,
    ASDK_CLOCK_ERROR_DIV_NOT_ASSIGNED,
    ASDK_CLOCK_ERROR_DIV_NOT_ENABLED,
    ASDK_CLOCK_ERROR_MAX,

    ASDK_PINMUX_SUCCESS = 301,                    /*!< PINMUX assignment is success*/
    ASDK_PINMUX_ERROR_NULL_PTR,                   /*!< The pointer passed as parameter is NULL*/
    ASDK_PINMUX_ERROR_INVALID_MCU_PIN,            /*!< The pin number passed is invalid. Refer @ref asdk_mcu_pin_t for valid pin input */
    ASDK_PINMUX_ERROR_INVALID_GPIO_PORT,          /*!< The GPIO port is invalid. Refer @ref cyt2b75_port_t for valid GPIO port */
    ASDK_PINMUX_ERROR_INVALID_GPIO_PIN,           /*!< The GPIO pin is invalid. Refer @ref cyt2b75_pin_t for valid GPIO pin */
    ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION, /*!< The Alternate function assigned to the MCU pin is invalid. Refer @ref asdk_pinmux_modules_t for valid Alternate Function */
    ASDK_PINMUX_ERROR_INIT_CONFIG_ERROR,          /*!< The configuration passed for Pinmux init is invalid*/
    ASDK_PINMUX_ERROR_DEINIT_CONFIG_ERROR,        /*!< The configuration passed for Pinmux de-init is invalid*/
    ASDK_PINMUX_ERROR_INVALID_GPIO_SLEW_RATE,     /*!< The configuration passed for GPIO slew rate is invalid */
    ASDK_PINMUX_ERROR_ERROR_MAX,

    ASDK_SPI_STATUS_SUCCESS = 401,             /*!< The SPI status is Success*/
    ASDK_SPI_ERROR_NULL_PTR,                   /*!< The pointer passed as parameter is NULL*/
    ASDK_SPI_STATUS_ERROR,                     /*!< The SPI status is ERROR*/
    ASDK_SPI_ERROR_INVALID_SPI_NO,             /*!<The SPI number passed is invalid.Refer @ref asdk_spi_num_t for valid SPI number*/
    ASDK_SPI_ERROR_INVALID_SLAVE_SELECT,       /*!<The SPI Slave Select is invalid*/
    ASDK_SPI_ERROR_NOT_INITIALIZED,            /*!<The SPI module is not initialized*/
    ASDK_SPI_ERROR_INITIALIZED,                /*!<The SPI initialization is not Success*/
    ASDK_SPI_ERROR_INVALID_DATA_SIZE,          /*!<The SPI data size is invalid.Refer @ref asdk_spi_datawidth_t for valid data size*/
    ASDK_SPI_ERROR_INVALID_BAUD_RATE,          /*!<The SPI baud rate is invalid.Refer @ref asdk_spi_transfer_rate_t for valid baud size*/
    ASDK_SPI_ERROR_RANGE_EXCEEDED,             /*!<The SPI number passed is more than total SPI numbers supported.Refer @ref asdk_spi_num_t for valid SPI number*/
    ASDK_SPI_ERROR_SLAVE_RANGE_EXCEEDED,       /*!<The SPI slave number passed is more than the supported Slaves*/
    ASDK_SPI_ERROR_INIT_FAIL,                  /*!<The SPI is failed to initialize*/
    ASDK_SPI_ERROR_INVALID_TYPE,               /*!<The SPI device type selected is invalid.Refer @ref asdk_spi_device_type_t for valid device type*/
    ASDK_SPI_ERROR_DEINIT_FAIL,                /*!<The SPI de-initialization is not success*/
    ASDK_SPI_ERROR_TRANSFER_FAIL,              /*!<The SPI data transfer operation is not success*/
    ASDK_SPI_ERROR_INVALID_BIT_ORDER,          /*!<The SPI bit order for data transfer is invalid.Refer @ref asdk_spi_bit_order_t for valid bit order*/
    ASDK_SPI_ERROR_INVALID_SPI_MODE,           /*!<The SPI mode of operation is invalid.Refer @ref asdk_spi_mode_t for valid SPI operation mode*/
    ASDK_SPI_ERROR_INVALID_CALLBACK_FUNCTION,  /*!<The SPI callback function is invalid*/
    ASDK_SPI_ERROR_INVALID_DMA_CONFIG,         /*!<The SPI DMA confuiguration is invalid.Refer @ref asdk_spi_dma_config_t for valid DMA configuration*/
    ASDK_SPI_ERROR_INVALID_DMA_DESTINATION,    /*!<The SPI DMA destination is invalid*/
    ASDK_SPI_ERROR_INVALID_DMA_SOURCE,         /*!<The SPI DMA source is invalid*/
    ASDK_SPI_ERROR_INVALID_DMA_RX_CHANNEL_NUM, /*!<The SPI DMA_Rx channel is invalid*/
    ASDK_SPI_ERROR_INVLAID_DMA_TX_CHANNEL_NUM, /*!<The SPI DMA_Tx channel is invalid*/
    ASDK_SPI_ERROR_INVALID_HW_TRIGGER_SOURCE,  /*!<The SPI hardware trigger source is invalid*/
    ASDK_SPI_ERROR_INVALID_HW_TRIGGER_NUM,     /*!<The SPI hardware trigger number is invalid*/
    ASDK_SPI_ERROR_MODULE_UNAVAILABLE,         /*!<The SPI module is unavailable to be used as SPI.This module is being used by other peripheral module like UART,I2C*/
    ASDK_SPI_ERROR_ISR_REQUIRED,               /*!<The SPI ISR assignment is required for the SPI operation*/
    ASDK_SPI_ERROR_FEATURE_NOT_IMPLEMENTED,    /*!<The SPI feature is not implemented*/
    ASDK_SPI_ERROR_MAX,

    ASDK_WDT_STATUS_SUCCESS = 501,      /*!< The WDT status is Success*/
    ASDK_WDT_STATUS_ERROR,              /*!< The WDT status is not Success*/
    ASDK_WDT_ERROR_NULL_PTR,            /*!< The pointer passed as configuration paramater is NULL*/
    ASDK_WDT_ERROR_NOT_IMPLEMENTED,     /*!< The WDT feature is not implemented*/
    ASDK_WDT_ERROR_NOT_INITIALIZED,     /*!< The WDT is not initialized*/
    ASDK_WDT_ERROR_INVALID_HANDLER,     /*!< The WDT handler for interrupt is invalid*/
    ASDK_WDT_ERROR_INVALID_WDT_NO,      /*!< The WDT number passed is invalid*/
    ASDK_WDT_ERROR_MAX_WDT_NO_EXCEEDED, /*!< The WDT number passed is more than the supported watchdogs*/
    ASDK_WDT_ERROR_MAX,

    ASDK_CAN_SUCCESS = 601, /*!< CAN module status success. */
    ASDK_CAN_ERROR_INVALID_CHANNEL, /*!< CAN channel is invalid. */
    ASDK_CAN_ERROR_INVALID_PIN, /*!< The configured CAN pin is invalid. */
    ASDK_CAN_ERROR_CLOCK_NOT_ENABLED, /*!< Error occured while initializing CAN peripheral clock. */
    ASDK_CAN_ERROR_UNSUPPORTED_BAUDRATE, /*!< The configured baudrate is invalid. Refer @ref asdk_can_baudrate_t for supported baudrates. */
    ASDK_CAN_ERROR_INVALID_DLC, /*!< The configured DLC is invalid. Refer @ref asdk_can_dlc_t for supported DLC values. */
    ASDK_CAN_ERROR_INIT_FAILED,
    ASDK_CAN_ERROR_INVALID_BAUDRATE,
    ASDK_CAN_ERROR_INVALID_DATA_BAUDRATE,
    ASDK_CAN_ERROR_NULL_PTR,
    ASDK_CAN_ERROR_WRITE_FAILED,
    ASDK_CAN_ERROR_READ_FAILED,
    ASDK_CAN_ERROR_DEINIT_FAILED,
    ASDK_CAN_ERROR_HW_FEATURE_NOT_SUPPORTED,
    ASDK_CAN_ERROR_MAX,

    ASDK_FLASH_STATUS_SUCCESS = 701,               /*!< The FLASH status is Success*/
    ASDK_FLASH_STATUS_ERROR,                       /*!< The FLASH status is not Success*/
    ASDK_FLASH_STATUS_BUSY,                        /*!< The FLASH is BUSY doing some other operation*/
    ASDK_FLASH_STATUS_TIMEOUT,                     /*!< The FLASH operation is timedout*/
    ASDK_FLASH_ERROR_NULL_PTR,                     /*!< The pointer passed as configuration parameter is NULL*/
    ASDK_FLASH_ERROR_INIT_FAIL,                    /*!< The FLASH initialization is not success*/
    ASDK_FLASH_ERROR_DEINIT_FAIL,                  /*!< The FLASH de-initialization is not success*/
    ASDK_FLASH_ERROR_NOT_IMPLEMENTED,              /*!< The FLASH feature is not implemented*/
    ASDK_FLASH_ERROR_NOT_INITIALIZED,              /*!< The FLASH to be used is not initialized*/
    ASDK_FLASH_ERROR_INVALID_HANDLER,              /*!< The handler assigned for the flash operation is invalid*/
    ASDK_FLASH_ERROR_INVALID_FLASH_TYPE,           /*!< The FLASH type selected for flash operation is invalid.Refer @ref asdk_flash_type_t for valid flash type*/
    ASDK_FLASH_ERROR_INVALID_DATA_SIZE,            /*!< The FLASH data type selected for flash operation is invalid.*/
    ASDK_FLASH_ERROR_INVALID_SECTOR_SIZE,          /*!<The FLASH sector size is invalid*/
    ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS,        /*!<The FLASH address for the Flash operation is invalid*/
    ASDK_FLASH_ERROR_INVALID_DATA_ALIGNMENT,       /*!<The FLASH data alignment for the Flash operation is invalid*/
    ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE, /*!<The FLASH operation mode is invalid.Refer @ref asdk_flash_operation_mode_t for valid flash operation mode*/
    ASDK_FLASH_ERROR_MAX,

    ASDK_EMULATED_EEPROM_STATUS_SUCCESS = 801,               /*!< The Emulated EEPROM status is Success*/
    ASDK_EMULATED_EEPROM_STATUS_ERROR,                       /*!< The Emulated EEPROM status is not Success*/
    ASDK_EMULATED_EEPROM_STATUS_BUSY,                        /*!< The Emulated EEPROM is BUSY doing some other operation*/
    ASDK_EMULATED_EEPROM_STATUS_TIMEOUT,                     /*!< The Emulated EEPROM operation is timedout*/
    ASDK_EMULATED_EEPROM_ERROR_NULL_PTR,                     /*!< The pointer passed as configuration parameter is NULL*/
    ASDK_EMULATED_EEPROM_ERROR_INIT_FAIL,                    /*!< The Emualted EEPROM initialization is not success*/
    ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL,                  /*!< The Emulated EEPROM de-initialization is not success*/
    ASDK_EMULATED_EEPROM_ERROR_NOT_IMPLEMENTED,              /*!< The Emualted EEPROM featureis not implemented*/
    ASDK_EMULATED_EEPROM_ERROR_NOT_INITIALIZED,              /*!< The Emulated EEPROM to be used is not initialized*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_HANDLER,              /*!< The handler assigned for the Emulated EEPROM operation is invalid*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_FLASH_TYPE,           /*!< The FLASH type selected for flash operation is invalid.Refer @ref asdk_flash_type_t for valid flash type*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_DATA_SIZE,            /*!< The FLASH data type selected for flash operation is invalid.*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_SECTOR_SIZE,          /*!<The FLASH sector size is invalid*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_FLASH_ADDRESS,        /*!<The FLASH address for the Emulated EEPROM operation is invalid*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_DATA_ALIGNMENT,       /*!<The  data alignment for the Emulated EEPROM operation is invalid*/
    ASDK_EMULATED_EEPROM_ERROR_INVALID_FLASH_OPERATION_MODE, /*!<The FLASH operation mode is invalid.Refer @ref asdk_flash_operation_mode_t for valid flash operation mode*/
    ASDK_EMULATED_EEPROM_ERROR_MAX,

    ASDK_SYS_SUCCESS = 901,
    ASDK_SYS_ERROR_TIMER_INTERRUPT_INIT_FAILED,
    ASDK_SYS_ERROR_TIMER_INIT_FAILED,
    ASDK_SYS_ERROR_MAX,

    ASDK_TIMER_SUCCESS = 1001,
    ASDK_TIMER_ERROR_INVALID_CHANNEL,
    ASDK_TIMER_ERROR_INVALID_TYPE,
    ASDK_TIMER_ERROR_INVALID_COUNT_DIRECTION,
    ASDK_TIMER_ERROR_INVALID_MODE,
    ASDK_TIMER_ERROR_INVALID_PERIOD,
    ASDK_TIMER_ERROR_INVALID_PRESCALER,
    ASDK_TIMER_ERROR_INVALID_COMPARE_PERIOD,
    ASDK_TIMER_ERROR_PINMUX,
    ASDK_TIMER_INIT_FAILED,

    ASDK_MW_CAN_SERVICE_SUCCESS = 1101,
    ASDK_MW_CAN_SERVICE_TX_BUSY,
    ASDK_MW_CAN_SERVICE_ERROR_NULL_PTR,
    ASDK_MW_CAN_SERVICE_INVALID_CAN_DATA,
    ASDK_MW_ERROR_MAX,

    ASDK_I2C_STATUS_SUCCESS = 1201,
    ASDK_I2C_ERROR_NULL_PTR,
    ASDK_I2C_ERROR_NOT_INITIALIZED,
    ASDK_I2C_ERROR_INITIALIZED,
    ASDK_I2C_ERROR_ISR_REQUIRED,
    ASDK_I2C_ERROR_INVALID_TRANSFER_RATE,
    ASDK_I2C_ERROR_INIT_FAIL,
    ASDK_I2C_ERROR_WRITE_FAIL,
    ASDK_I2C_ERROR_READ_FAIL,
    ASDK_I2C_ERROR_RANGE_EXCEEDED,
    ASDK_I2C_ERROR_FEATURE_NOT_IMPLEMENTED,
    ASDK_I2C_ERROR_MODULE_UNAVAILABLE,
    ASDK_I2C_ERROR_INVALID_SIZE,
    ASDK_I2C_ERROR_MAX,

    ASDK_ADC_SUCCESS = 1301,                      /*!< The ADC status is Success*/
    ASDK_ADC_ERROR_INIT_FAIL,                     /*!< The ADC Module initialization status is not Success*/
    ASDK_ADC_ERROR_CHANNEL_INIT_FAIL,             /*!< The ADC Channel initialization status is not Success*/
    ASDK_ADC_ERROR_NULL_PTR,                      /*!< The pointer passed as configuration parameter is NULL*/
    ASDK_ADC_ERROR_INVALID_PIN,                   /*!< Pin for the ADC operation is invalid.Refer @ref asdk_mcu_pin_t for valid MCU pin */
    ASDK_ADC_ERROR_INVALID_PIN_COUNT,             /*!< Pin Count for the ADC operation is invalid.It should not be zero*/
    ASDK_ADC_ERROR_INVALID_RESOLUTION,            /*!< The ADC converted result resoltuion is invalid.Refer @ref asdk_adc_resolution_bits_t for valid resolution */
    ASDK_ADC_ERROR_INVALID_REFERENCE_VOLTAGE_SRC, /*!< The ADC refrence voltage chosen for conversion is invalid.Refer @ref asdk_adc_ref_voltage_src_t for valid refrence voltage */
    ASDK_ADC_ERROR_INVALID_SAMPLE_FREQ,           /*!< The ADC sampling frequency is invalid.Refer @ref asdk_adc_sampling_freq_t for valid sampling frequency selection*/
    ASDK_ADC_ERROR_INVALID_CALIBRATION_INPUT,     /*!< The Calibration input to the ADC channel is invalid*/
    ASDK_ADC_ERROR_INVALID_HW_TRIGGER_INPUT,      /*!< The ADC channel conversion trigger input is invalid.*/
    ASDK_ADC_ERROR_INVALID_TRIGGER_SRC,           /*!< The ADC channel conversion trigger source is invalid.Refer @ref asdk_adc_hw_trig_src_t for valid trigger source*/
    ASDK_ADC_ERROR_INVALID_TRIGGER_NUM,           /*!< The ADC channel conversion trigger number is invalid.*/
    ASDK_ADC_ERROR_INVALID_GROUP_INPUT,           /*!< The ADC group conversion input is invalid.*/
    ASDK_ADC_ERROR_INVALID_GROUP_INSTANCE,        /*!< The ADC group instance is invalid.*/
    ASDK_ADC_ERROR_INVALID_GROUP_CALLBACK,        /*!< The ADC group callback is invalid.*/
    ASDK_ADC_ERROR_INVALID_CALLBACK,              /*!< The ADC callback is invalid.*/
    ASDK_ADC_ERROR_INVALID_DMA_CONFIG,            /*!< The ADC DMA configuration is invalid*/
    ASDK_ADC_ERROR_INVALID_DMA_DESTINATION,       /*!< The ADC DMA destination buffer address is invalid*/
    ASDK_ADC_ERROR_INVALID_DMA_DESTINATION_LEN,   /*!< The ADC DMA destination buffer length is invalid*/
    ASDK_ADC_ERROR_FUNCTION_NOT_IMPLEMENTED,      /*!< The ADC feature is not implemented*/
    ASDK_ADC_ERROR_GROUP_HANDLE_NOT_AVAILABLE,    /*!< The ADC group handle is not available*/
    ASDK_ADC_ERROR_PIN_BUFFER_NOT_AVAILABLE,      /*!< The ADC pin buffer is not available*/
    ASDK_ADC_ERROR_HW_TRIG_NOT_SUPPORTED,         /*!< In the given ADC channel/module hardware trigger is not supported*/
    ASDK_ADC_ERROR_DMA_TRIG_NOT_SUPPORTED,        /*!< In the given ADC channel/module DMA trigger is not supported*/
    ASDK_ADC_ERROR_DAL_INVALID_PIN_MAP,           /*!< Pin for the ADC operation is invalid.Refer @ref asdk_mcu_pin_t for valid MCU pin */
    ASDK_ADC_ERROR_PIN_ALREADY_INITIALIZED,       /*!< The ADC channel is already initialized */
    ASDK_ADC_ERROR_CLK_INIT_FAILED,               /*!< The ADC clock initialization is not success */
    ASDK_ADC_ERROR_ISR_REQUIRED,                  /*!<The ADC ISR assignment is required for the ADC operation*/
    ASDK_ADC_ERROR_CONVERSION_FAIL,               /*!< The ADC conversion is not success */
    ASDK_ADC_ERROR_MAX,

    ASDK_UART_STATUS_SUCCESS = 1401,
    ASDK_UART_ERROR_NULL_PTR,
    ASDK_UART_ERROR_NOT_INITIALIZED,
    ASDK_UART_ERROR_INITIALIZED,
    ASDK_UART_ERROR_ISR_REQUIRED,
    ASDK_UART_ERROR_INVALID_BAUDRATE,
    ASDK_UART_ERROR_INIT_FAIL,
    ASDK_UART_ERROR_WRITE_FAIL,
    ASDK_UART_ERROR_READ_FAIL,
    ASDK_UART_ERROR_BITS_PER_CHAR_RANGE_EXCEEDED,
    ASDK_UART_ERROR_RANGE_EXCEEDED,
    ASDK_UART_ERROR_INVALID_DATABITS,
    ASDK_UART_ERROR_INVALID_PARITY,
    ASDK_UART_ERROR_INVALID_STOPBITS,
    ASDK_UART_ERROR_MODULE_UNAVAILABLE,
    ASDK_UART_ERROR_FEATURE_NOT_IMPLEMENTED,
    ASDK_UART_ERROR_MAX,

    ASDK_PWM_SUCCESS = 1501,                        /*!< PWM status is Success*/
    ASDK_PWM_ERROR_INVALID_CHANNEL,                 /*!< PWM channel is invalid.Refer @ref asdk_pwm_channel_t for valid PWM channel */
    ASDK_PWM_ERROR_NULL_PTR,                        /*!< The pointer passed as PWM configuration structure is NULL*/
    ASDK_PWM_ERROR_INVALID_DEADTIME,                /*!< PWM deadtime passed is invalid or out of range*/
    ASDK_PWM_ERROR_INVALID_MODE,                    /*!< PWM mode selected is invalid.Refer @ref asdk_pwm_modes_t for valid PWM Mode*/
    ASDK_PWM_ERROR_INVALID_COUNT_DIRECTION,         /*!< PWM counter count direction selected is invalid.Refer @ref asdk_pwm_count_direction_t for valid PWM count direction*/
    ASDK_PWM_ERROR_INVALID_RUN_MODE,                /*!< PWM run mode selected is invalid.Refer @ref asdk_pwm_run_mode_t for valid PWM rum mode*/
    ASDK_PWM_ERROR_INVALID_PRESCALER,               /*!< Prescaler selected for PWM Module is invalid.Refer @ref asdk_prescaler_config_t for valid PWM prescaler*/
    ASDK_PWM_ERROR_INVALID_FREQUENCY,               /*!< PWM frequency is invalid or out of range*/
    ASDK_PWM_ERROR_INVALID_DUTY_CYCLE,              /*!< PWM duty cycle is invalid*/
    ASDK_PWM_ERROR_INVALID_OUTPUT_ALIGN,            /*!< PWM output align is invalid.Refer @ref asdk_pwm_output_align_t for valid PWM output align*/
    ASDK_PWM_ERROR_INVALID_FEATURE_NOT_IMPLEMENTED, /*!< PWM feature not implemented*/
    ASDK_PWM_ERROR_PINMUX,                          /*!< PWM invalid pinmux for the given MCU pin*/
    ASDK_PWM_ERROR_INIT_FAILED,                     /*!< PWM initialization not success*/
    ASDK_PWM_ERROR_MAX,

    ASDK_ERROR_MAX,
} asdk_errorcode_t;

#define ASDK_DEV_ERROR_ASSERT(ret, expected_ret) ( \
    { if (ret != expected_ret) {while(1) __ASM("NOP");} })

// overflow check
#if ASDK_GPIO_ERROR_MAX > 200
#error "ASDK GPIO error code overflowing."
#endif

#if ASDK_CLOCK_ERROR_MAX > 300
#error "ASDK CLOCK error code overflowing."
#endif

#if ASDK_CAN_ERROR_MAX > 400
#error "ASDK CAN error code overflowing."
#endif

#endif /* ASDK_ERROR_H */
