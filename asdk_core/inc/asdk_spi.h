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
    asdk_spi.h

    @path
    inc/asdk_spi.h

    @Created on
    Aug 3, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the SPI Module for ASDK.
*/

#ifndef ASDK_SPI_H
#define ASDK_SPI_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_mcu_pins.h"
#include "asdk_error.h"
#include "asdk_platform.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define MAX_SLAVE_SELECTS 4
#define SPI_TRANSFER_PIN 2
#define SCB_SPI_OVERSAMPLING 4u
#define ASDK_SPI_SLAVE_MAX 8

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_spi_enum_group Enumerations
 *  Lists all the enumerations used by the SPI module.
 *  @{
 */

/*!
 * @brief SPI SubMode of Operation
 */

typedef enum
{
    ASDK_SPI_MODE_0 = 0, /*!<CPOL = 0, CPHA = 0 : Data is driven on a falling edge of SCLK. Data is captured on a rising edge of SCLK. SCLK idle state is '0'.*/
    ASDK_SPI_MODE_1,     /*!<CPOL = 0, CPHA = 1 : Data is driven on a rising edge of SCLK. Data is captured on a falling edge of SCLK. SCLK idle state is '0'.*/
    ASDK_SPI_MODE_2,     /*!<CPOL = 1, CPHA = 0 : Data is driven on a rising edge of SCLK. Data is captured on a falling edge of SCLK. SCLK idle state is '1'.*/
    ASDK_SPI_MODE_3,     /*!<CPOL = 1, CPHA = 1 : Data is driven on a falling edge of SCLK. Data is captured on a rising edge of SCLK. SCLK idle state is '1'.*/
    ASDK_SPI_MODE_MAX,
    ASDK_SPI_MODE_UNDEFINED = ASDK_SPI_MODE_MAX,
} asdk_spi_mode_t;


/*!
 * @brief SPI Mode of Operation
 */

typedef enum
{
    ASDK_SPI_DEVICE_SLAVE = 0,  /*!< Use as SPI Slave. */
    ASDK_SPI_DEVICE_MASTER,     /*!< Use as SPI Master. */
    ASDK_SPI_DEVICE_MAX,
    ASDK_SPI_DEVICE_UNDEFINED = ASDK_SPI_DEVICE_MAX,
} asdk_spi_device_type_t;

/*!
 * @brief SPI Tranfer Rate
 */

typedef enum
{
    ASDK_SPI_TRANSFER_RATE_125KHz = 0, /*!< Transfer rate 125KHz */
    ASDK_SPI_TRANSFER_RATE_250KHz,  /*!< Transfer rate 250KHz */
    ASDK_SPI_TRANSFER_RATE_500KHz,  /*!< Transfer rate 500KHz */
    ASDK_SPI_TRANSFER_RATE_1MHz,    /*!< Transfer rate 1MHz */
    ASDK_SPI_TRANSFER_RATE_2MHz,    /*!< Transfer rate 2MHz */
    ASDK_SPI_TRANSFER_RATE_3MHz,    /*!< Transfer rate 3MHz */
    ASDK_SPI_TRANSFER_RATE_4MHz,    /*!< Transfer rate 4MHz */
    ASDK_SPI_TRANSFER_RATE_5MHz,    /*!< Transfer rate 5MHz */
    ASDK_SPI_TRANSFER_RATE_6MHz,    /*!< Transfer rate 6MHz */
    ASDK_SPI_TRANSFER_RATE_7MHz,    /*!< Transfer rate 7MHz */
    ASDK_SPI_TRANSFER_RATE_8MHz,    /*!< Transfer rate 8MHz */
    ASDK_SPI_TRANSFER_RATE_10MHz,   /*!< Transfer rate 10MHz */
    ASDK_SPI_TRANSFER_RATE_16MHz,   /*!< Transfer rate 16MHz */
    ASDK_SPI_TRANSFER_RATE_20MHz,   /*!< Transfer rate 20MHz */
    ASDK_SPI_TRANSFER_RATE_MAX,
    ASDK_SPI_TRANSFER_RATE_UNDEFINED = ASDK_SPI_TRANSFER_RATE_MAX,
} asdk_spi_transfer_rate_t;

/*!
 * @brief SPI Bit Order
 */

typedef enum
{
    ASDK_SPI_BIT_ORDER_LSB_FIRST = 0,
    ASDK_SPI_BIT_ORDER_MSB_FIRST,
    ASDK_SPI_BIT_ORDER_MAX,
    ASDK_SPI_BIT_ORDER_UNDEFINED = ASDK_SPI_BIT_ORDER_MAX,
} asdk_spi_bit_order_t;

/*!
 * @brief SPI DataWidth
 *
 * @note:Please use the recommended adatwidth values
 */

typedef enum
{
    ASDK_SPI_DATAWIDTH_8_BITS = 0,
    ASDK_SPI_DATAWIDTH_16_BITS,
    ASDK_SPI_DATAWIDTH_MAX,
    ASDK_SPI_DATAWIDTH_UNDEFINED = ASDK_SPI_DATAWIDTH_MAX,
} asdk_spi_datawidth_t;

/*!
 * @brief SPI Transfer Mode
 */

typedef enum
{
    ASDK_SPI_TRANSFER_MODE_POLLING = 0,
    ASDK_SPI_TRANSFER_MODE_INTERRUPT,
    ASDK_SPI_TRANSFER_MODE_DMA,
    ASDK_SPI_TRANSFER_MODE_MAX,
    ASDK_SPI_TRANSFER_MODE_UNDEFINED = ASDK_SPI_TRANSFER_MODE_MAX,
} asdk_spi_transfer_mode_t;

/*!
 * @brief SPI Transfer Status
 */

typedef enum
{
    ASDK_SPI_TRANSFER_STATUS_ACTIVE = 0,
    ASDK_SPI_TRANSFER_STATUS_INFIFO,
    ASDK_SPI_TRANSFER_STATUS_OVERFLOW,
    ASDK_SPI_TRANSFER_STATUS_UNDERFLOW,
    ASDK_SPI_TRANSFER_STATUS_SLAVE_ERROR,
    ASDK_SPI_TRANSFER_STATUS_MAX,
    ASDK_SPI_TRANSFER_STATUS_UNDEFINED = ASDK_SPI_TRANSFER_STATUS_MAX,
} asdk_spi_transfer_status_t;

/*!
 * @brief SPI hw trigger event.
 *
 * @note:This will be active only when hw_trigger is selected
 */

typedef enum
{
    ASDK_SPI_HW_TRG_SRC_PWM_INIT_EVENT,  /*!< Trigger Source set to PWM INIT Event (MOD == CNT). */
    ASDK_SPI_HW_TRG_SRC_PWM_RESET_EVENT, /*!< Tgger Source set to PWM RESET Event */
    ASDK_SPI_HW_TRG_SRC_PWM_HALF_EVENT,  /*!< Trigger Source set to PWM Half Event (MOD/2 == CNT). */

    ASDK_SPI_HW_TRG_SRC_GPIO_SET_EVENT,    /*!< Trigger Source set to GPIO Set Event. */
    ASDK_SPI_HW_TRG_SRC_GPIO_CLEAR_EVENT,  /*!< Trigger Source set to GPIO Clear Event. */
    ASDK_SPI_HW_TRG_SRC_GPIO_TOGGLE_EVENT, /*!< Trigger Source set to GPIO Toggle Event. */

    ASDK_SPI_HW_TRG_SRC_ADC_CONVERSION_COMPLETED, /*!< Trigger Source set to ADC Conversion done event. */

    ASDK_SPI_HW_TRG_SRC_MAX,
    ASDK_SPI_HW_TRG_SRC_INVALID = ASDK_SPI_HW_TRG_SRC_MAX

} asdk_spi_hw_trigger_src_t;

/** @} */ // end of asdk_spi_enum_group

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_spi_ds_group Data structures
 *  Lists all the data structures used by the SPI module.
 *  @{
 */

/*!
 * @brief SPI transfer via DMA related config
 *
 * @note:The DMA should be configured for Byte transfer
 */

typedef struct
{
    uint8_t SPI_dma_rx_channel_no;
    uint8_t SPI_dma_tx_channel_no;

    uint8_t *SPI_tx_data_buffer;
    uint32_t SPI_tx_data_len_bytes;

    uint8_t *SPI_rx_data_buffer;
    uint32_t SPI_rx_data_len_bytes;
} asdk_spi_dma_config_t;


/*!
 * @brief An data structure to represent SPI interrupt
 * settings.
 */
typedef struct
{
    bool use_interrupt; /*!< Enable or disable interrupts. */
    uint8_t priority;   /*!< Priority of the interrupt. */
} asdk_spi_interrupt_cfg_t;


/*!
 * @brief SPI Configuration structure
 *
 * @note: Slave select num should be >=1 when configured for Slave operation
 *        For tranfer via DMA ensure dataframe size to be Byte
 *        When configured for slave mode the Slave Select will be ignored
 */

typedef struct
{
    asdk_spi_num_t SPI_no;        /*!< SPI no indicates the SPI module no of the ECU */
    uint8_t SPI_CLK_mcu_pin_no;   /*!< SPI CLK mcu pin no*/
    uint8_t SPI_MOSI_mcu_pin_no;  /*!< SPI MOSI mcu pin no*/
    uint8_t SPI_MISO_mcu_pin_no;  /*!< SPI MISO mcu pin no*/
    uint8_t SPI_CS_mcu_pin_no[8]; /*!< SPI CS mcu pin no*/

    asdk_spi_mode_t SPI_mode;        /*!< SPI Mode indicates the clock phase and clock polarity*/
    asdk_spi_device_type_t SPI_type; /*!< SPI device type to be configured Master/Slave*/

    uint8_t no_of_slaves;                     /*!< SPI MOSI mcu pin no*/

    // TBD:Transfer type as value or enum type
    asdk_spi_transfer_rate_t SPI_transfer_rate; /*!< SPI transfer rate*/
    uint32_t *SPI_actual_frequency_configured;  /*!< SPI actual frequency if incase user's input is not supported */

    asdk_spi_bit_order_t SPI_bit_order;         /*!< SPI bit order MSB first or LSB first*/
    asdk_spi_transfer_mode_t SPI_transfer_mode; /*!< SPI transfer mode interrupt/dma/polling */
    asdk_spi_interrupt_cfg_t SPI_Interrupt_config; /*!SPI Interrupt configuration for the user*/

    asdk_spi_datawidth_t SPI_datawidth; /*!< Number of rx bits/frame, minimum is 8 bits varies with MCU*/
    

    /*DMA related configs*/
    asdk_spi_dma_config_t SPI_dma_config;

    /*Hw trigger related configs*/
    bool enable_hw_trigger;
    asdk_spi_hw_trigger_src_t SPI_hw_trigger_event;

} asdk_spi_config_t;

/** @} */ // end of asdk_spi_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_spi_cb_group Callback function type
 *  Lists the callback functions from the SPI module.
 *  @{
 */

/*!
 * @brief The SPI module's callback function type.

   @param  spi_no The SPI channel on which the event occurred
   @param  spi_data Pointer to the data buffer
   @param  data_size Length of the data
   @param  asdk_spi_mode SPI mode of operation
   
 */
typedef void (*asdk_spi_callback_fun_t)(uint8_t spi_no, uint8_t *spi_data, uint8_t data_size, asdk_spi_mode_t asdk_spi_mode);

/** @} */ // end of asdk_spi_cb_group

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_spi_fun_group Functions
 *  Lists the functions/APIs from the SPI module.
 *  @{
 */

/*SPI Master/Slave APIs*/
/*----------------------------------------------------------------------------*/
/* Function : asdk_spi_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the SPI Module based on the config structure passed as parameter.

  @param [in] spi_config_data ASDK SPI configuration parameter.

  @return
    - @ref ASDK_SPI_STATUS_SUCCESS
    - @ref ASDK_SPI_ERROR_NULL_PTR
    - @ref ASDK_SPI_STATUS_ERROR
    - @ref ASDK_SPI_ERROR_INVALID_SPI_NO
    - @ref ASDK_SPI_ERROR_INVALID_SLAVE_SELECT
    - @ref ASDK_SPI_ERROR_NOT_INITIALIZED
    - @ref ASDK_SPI_ERROR_INITIALIZED
    - @ref ASDK_SPI_ERROR_INVALID_DATA_SIZE
    - @ref ASDK_SPI_ERROR_INVALID_BAUD_RATE
    - @ref ASDK_SPI_ERROR_RANGE_EXCEEDED
    - @ref ASDK_SPI_ERROR_SLAVE_RANGE_EXCEEDED
    - @ref ASDK_SPI_ERROR_INIT_FAIL
    - @ref ASDK_SPI_ERROR_DEINIT_FAIL
    - @ref ASDK_SPI_ERROR_TRANSFER_FAIL
    - @ref ASDK_SPI_ERROR_INVALID_BIT_ORDER
    - @ref ASDK_SPI_ERROR_INVALID_SPI_MODE
    - @ref ASDK_SPI_ERROR_INVALID_CALLBACK_FUNCTION
    - @ref ASDK_SPI_ERROR_INVALID_DMA_CONFIG
    - @ref ASDK_SPI_ERROR_INVALID_DMA_DESTINATION
    - @ref ASDK_SPI_ERROR_INVALID_DMA_SOURCE
    - @ref ASDK_SPI_ERROR_INVALID_DMA_RX_CHANNEL_NUM
    - @ref ASDK_SPI_ERROR_INVLAID_DMA_TX_CHANNEL_NUM
    - @ref ASDK_SPI_ERROR_INVALID_HW_TRIGGER_SOURCE
    - @ref ASDK_SPI_ERROR_INVALID_HW_TRIGGER_NUM
    - @ref ASDK_SPI_ERROR_FEATURE_NOT_IMPLEMENTED
*/
asdk_errorcode_t asdk_spi_init(asdk_spi_config_t *spi_config_data);

/*----------------------------------------------------------------------------*/
/* Function : asdk_spi_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the SPI Module based on the config structure passed as parameter.

  @param [in] SPI_no ASDK SPI no to be initialized.

  @return
    - @ref ASDK_SPI_STATUS_SUCCESS
    - @ref ASDK_SPI_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_spi_deinit(asdk_spi_num_t SPI_no);

/*----------------------------------------------------------------------------*/
/* Function : spi_transfer_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function transfers data out through the SPI slave module using blocking method

  @param [in] SPI_no
  @param [in] send_buf
  @param [out] recv_buf
  @param [in] length
  @param [in] Slave_Select_no

  @return 
    - @ref ASDK_SPI_STATUS_SUCCESS
    - @ref ASDK_SPI_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_spi_transfer_blocking(asdk_spi_num_t SPI_No, uint8_t *send_buf, uint8_t *recv_buf, uint16_t length, uint8_t Slave_Select_no);

/*----------------------------------------------------------------------------*/
/* Function : spi_transfer_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function transfers data out through the SPI slave module using non-blocking method

  @param [in] SPI_no
  @param [in] send_buf
  @param [out] recv_buf
  @param [in] length
  @param [in] Slave_Select_no

  @return
    - @ref ASDK_SPI_STATUS_SUCCESS
    - @ref ASDK_SPI_ERROR_RANGE_EXCEEDED
    - @ref ASDK_SPI_ERROR_TRANSFER_FAIL
*/
asdk_errorcode_t asdk_spi_transfer_non_blocking(asdk_spi_num_t SPI_No, uint8_t *send_buf, uint8_t *recv_buf, uint16_t length, uint8_t Slave_Select_no);

/*----------------------------------------------------------------------------*/
/* Function : asdk_spi_transfer_status */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the Transfer Status of the SPI Module.

  @param SPI_no ASDK SPI no for which transfer status is required.
  @param SPI_transfer_status Variable in which transfer status of SPI will be stored.

  @return
    - @ref ASDK_SPI_STATUS_SUCCESS
    - @ref ASDK_SPI_ERROR_RANGE_EXCEEDED
    - @ref ASDK_SPI_ERROR_STATUS_ERROR
*/
asdk_errorcode_t asdk_spi_transfer_status(asdk_spi_num_t SPI_no, asdk_spi_transfer_status_t *master_transfer_status);

/*Callback Register function*/
/*----------------------------------------------------------------------------*/
/* Function : asdk_spi_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function install callback for spi module.

  @param SPI_no SPI no
  @param callback_fun

  @param callback_data

  @return
    - @ref ASDK_SPI_RANGE_EXCEEDED 
    - @ref ASDK_SPI_STATUS_SUCCESS 

*/
asdk_errorcode_t asdk_spi_install_callback(asdk_spi_num_t SPI_No, asdk_spi_callback_fun_t callback_fun);

 /** @} */ // end of asdk_spi_fun_group

#endif /* ASDK_PINMUX_H */
