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
    asdk_i2c.h

    @path
    inc/asdk_i2c.h

    @Created on
    Sept 9, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the I2C Module for ASDK.
*/

#ifndef ASDK_I2C_H
#define ASDK_I2C_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_mcu_pins.h"
#include "asdk_clock.h"
#include "asdk_error.h"
#include "asdk_platform.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_i2c_enum_group Enumerations
 *  Lists all the enumerations used by the I2C module.
 *  @{
 */

/*!
 * @brief I2C Mode of operation
 */
typedef enum
{
    ASDK_I2C_MODE_MASTER = 0,   /*!< Use I2C as Master. */
    ASDK_I2C_MODE_SLAVE,        /*!< Use I2C as Slave. */
    ASDK_I2C_MODE_MASTER_SLAVE, /*!< Use I2C as Master-Slave. */
    ASDK_I2C_MODE_MAX,
    ASDK_I2C_MODE_UNDEFINED = ASDK_I2C_MODE_MAX,
} asdk_i2c_mode_t;

/*!
 * @brief I2C Transfer Rate
 */
typedef enum
{
    ASDK_I2C_TRANSFER_RATE_100kbps = 100000U, /*!< I2C-bus Standard mode, 100 kbit/s. */
    ASDK_I2C_TRANSFER_RATE_400kbps = 400000U,     /*!< I2C-bus Fast mode, 400 kbit/s. */
    ASDK_I2C_TRANSFER_RATE_1mbps = 1000000U,    /*!< I2C-bus Flas mode Plus, 1 Mbit/s. */
    ASDK_I2C_TRANSFER_RATE_3_4mbps = 3400000U,    /*!< I2C-bus High-Speed mode, 3.4 Mbit/s. */
    ASDK_I2C_TRANSFER_RATE_MAX,
    ASDK_I2C_TRANSFER_RATE_UNDEFINED = ASDK_I2C_TRANSFER_RATE_MAX,
} asdk_i2c_transfer_rate_t;

/*!
 * @brief I2C Transfer Mode
 */
typedef enum
{
    ASDK_I2C_TRANSFER_MODE_INTERRUPT = 0, /*!< Transfer data using interrupt mode. */
    ASDK_I2C_TRANSFER_MODE_POLLING, /*!< Transfer data using polling mode. */
    ASDK_I2C_TRANSFER_MODE_MAX,
    ASDK_I2C_TRANSFER_MODE_UNDEFINED = ASDK_I2C_TRANSFER_MODE_MAX,
} asdk_i2c_transfer_mode_t;

/*!
 * @brief I2C Master/Slave status
 */
typedef enum
{
    ASDK_I2C_STATUS_WR_COMPLETE = 0, /*!< Write operation complete. */
    ASDK_I2C_STATUS_WR_IN_FIFO,      /*!< Data has been loaded in TxFIFO. */
    ASDK_I2C_STATUS_RD_COMPLETE,     /*!< Read operation complete. */
    ASDK_I2C_STATUS_BUSY,            /*!< I2C is busy. */
    ASDK_I2C_STATUS_ARB_LOST,        /*!< Master lost arbitration. */
    ASDK_I2C_STATUS_BUS_ERROR,       /*!< Master detected error with start or stop condition. */
    ASDK_I2C_STATUS_MASTER_ADDR_NACK, /*!< Slave has NACKed the address. */
    ASDK_I2C_STATUS_MASTER_DATA_NACK, /*!< Master write stopped before transmitting all data due to NACK. */
    ASDK_I2C_STATUS_MASTER_ABORT_START, /*!< Master initiated transaction has been aborted. */
    ASDK_I2C_STATUS_ERROR,            /*!< Failed determine I2C error. */
    ASDK_I2C_STATUS_MAX,
    ASDK_I2C_STATUS_UNDEFINED = ASDK_I2C_STATUS_MAX,
}asdk_i2c_status_t;

/*!
 * @brief I2C transfer via DMA related config
 *
 * @note:The DMA should be configured for Byte transfer
 */
typedef struct
{
    uint8_t i2c_dma_rx_channel_no;
    uint8_t i2c_dma_tx_channel_no;

    uint8_t *i2c_tx_data_buffer;
    uint32_t i2c_tx_data_len_bytes;

    uint8_t *i2c_rx_data_buffer;
    uint32_t i2c_rx_data_len_bytes;
} asdk_i2c_dma_config_t;

/** @} */ // end of asdk_i2c_enum_group

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_i2c_ds_group Data structures
 *  Lists all the data structures used by the I2C module.
 *  @{
 */

/*!
 * @brief An data structure to represent I2C interrupt
 * settings.
 */
typedef struct
{
    bool use_interrupt; /*!< Enable or disable interrupts. */
    uint8_t priority;   /*!< Priority of the interrupt. */
} asdk_i2c_interrupt_cfg_t;


/*!
 * @brief I2C Configuration structure
 *
 * @note: Slave address and address-mask to be configured when for Slave operation
 *        For tranfer via DMA ensure dataframe size to be Byte
 *
 */
typedef struct
{
    asdk_i2c_num_t i2c_no;      /* I2C no. indicatres the I2C module no. of the ECU */
    uint8_t i2c_scl_mcu_pin_no; /* I2C SCL mcu pin no */
    uint8_t i2c_sda_mcu_pin_no; /* I2C SDA mcu pin no */

    asdk_i2c_mode_t i2c_mode;               /* I2C mode Master/Slave */
    asdk_i2c_transfer_rate_t transfer_rate; /* I2C Bus speed, required for Slave as well */
    double i2c_actual_frequency_configured; /* I2C actual frequency if incase user's input is not supported */

    asdk_i2c_transfer_mode_t transfer_mode; /* I2C transfer mode Interrupt/Polling */
    asdk_i2c_interrupt_cfg_t interrupt_config; /*!SPI Interrupt configuration for the user*/

    asdk_i2c_dma_config_t i2c_dma_config;   /* I2C DMA config */

    uint8_t slave_Address;      /* I2C Slave address, for slave mode only */
    uint8_t slave_Address_Mask; /* I2C Slave address mask, for slave mode only */
    
    bool accept_Addr_In_Fifo;    // TODO: I2C Slave address to be accepted in Rx FIFO or not
    bool ack_General_Addr;       // TODO: I2C accept general call address or not
    bool enable_Wake_From_Sleep; // TODO: I2C Slave to wake the device from deep sleep
} asdk_i2c_config_t;

/** @} */ // end of asdk_i2c_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_i2c_cb_group Callback function type
 *  Lists the callback functions from the I2C module.
 *  @{
 */

/*!
 * @brief The I2C module's callback function type. For the supported I2C status
   refer @ref asdk_i2c_install_callback function.

   @param  i2c_no The I2C channel on which the event occurred
   @param  i2c_data Pointer to the data buffer
   @param  data_size Length of the data
   @param  status The status of the I2C transfer
 */
typedef void (*asdk_i2c_callback_fun_t)(uint8_t i2c_no, uint8_t *i2c_data, uint8_t data_size, asdk_i2c_status_t status);//todo: read reg to get i2c-mode

/** @} */ // end of asdk_i2c_cb_group

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_i2c_fun_group Functions
 *  Lists the functions/APIs from the I2C module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the I2C Module based on the config structure passed
  as parameter.

  @param [in] i2c_config_data I2C configuration.

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_INITIALIZED
    - @ref ASDK_I2C_ERROR_NULL_PTR
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED
    - @ref ASDK_I2C_ERROR_INVALID_TRANSFER_RATE
    - @ref ASDK_I2C_ERROR_MODULE_UNAVAILABLE
    - @ref ASDK_I2C_ERROR_INIT_FAIL

*/
asdk_errorcode_t asdk_i2c_init(asdk_i2c_config_t *i2c_config_data);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the I2C Module based on the config structure passed as parameter.

  @param [in] i2c_no I2C channel number

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_i2c_deinit(asdk_i2c_num_t i2c_no);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function install callback for either i2c master or slave data transfer.
  Refer @ref asdk_i2c_callback_fun_t to define the callback function.
  The Following events are supported.
  - @ref ASDK_I2C_STATUS_WR_COMPLETE
  - @ref ASDK_I2C_STATUS_WR_IN_FIFO
  - @ref ASDK_I2C_STATUS_RD_COMPLETE
  - @ref ASDK_I2C_STATUS_BUSY
  - @ref ASDK_I2C_STATUS_ARB_LOST
  - @ref ASDK_I2C_STATUS_BUS_ERROR
  - @ref ASDK_I2C_STATUS_ERROR

  @param [in] i2c_no I2C channel number
  @param [in] callback_fun Callback function for handling status events.

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_i2c_install_callback(asdk_i2c_num_t i2c_no, asdk_i2c_callback_fun_t callback_fun);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_slave_config_write_buf */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures the write buffer for I2C Slave mode, to which other I2C Master device writes data.
  This has to be called after every I2C Master write operation.

  @param [in] i2c_no I2C channel number
  @param [in] wr_buff Pointer to write buffer
  @param [in] wr_buff_size Size of write buffer

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_i2c_slave_config_write_buf(asdk_i2c_num_t i2c_no, uint8_t *wr_buff, uint8_t wr_buff_size);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_slave_config_read_buf */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures the read buffer for I2C Slave mode, from which other I2C Master device reads data.
  This has to be called before every I2C Master read operation.

  @param [in] i2c_no I2C channel number
  @param [out] rd_buff Pointer to read buffer
  @param [in] rd_buff_size Size of read buffer

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED

*/
asdk_errorcode_t asdk_i2c_slave_config_read_buf(asdk_i2c_num_t i2c_no, uint8_t *rd_buff, uint8_t rd_buff_size);

asdk_errorcode_t asdk_i2c_master_write_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *send_buf, uint16_t length, uint16_t time_out);
asdk_errorcode_t asdk_i2c_master_read_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *recv_buf, uint16_t length, uint16_t time_out);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_master_write_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function Sends data out through the I2C master or slave module using non-blocking method

  @param [in] i2c_no I2C channel number
  @param [in] slave_addr Slave address
  @param [in] send_buf Pointer to buffer containing data to be sent
  @param [in] length Length of data to be sent

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED
    - @ref ASDK_I2C_ERROR_WRITE_FAIL
*/
asdk_errorcode_t asdk_i2c_master_write_non_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *send_buf, uint16_t length);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_master_read_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads data through the I2C master or slave module using non-blocking method

  @param [in] i2c_no I2C channel number
  @param [in] slave_addr Slave address
  @param [out] recv_buf Pointer to the buffer to receive read data
  @param [in] length Length of the data to be read

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED
    - @ref ASDK_I2C_ERROR_READ_FAIL
*/
asdk_errorcode_t asdk_i2c_master_read_non_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *recv_buf, uint16_t length);

/*----------------------------------------------------------------------------*/
/* Function : asdk_i2c_transfer_status */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the transfer status of the I2C for Master, Slave and Master-Slave mode.

  @param [in] i2c_no I2C no.

  @return
    - @ref ASDK_I2C_STATUS_SUCCESS
    - @ref ASDK_I2C_STATUS_BUSY
    - @ref ASDK_I2C_ERROR_RANGE_EXCEEDED
*/
 asdk_errorcode_t asdk_i2c_get_transfer_status(asdk_i2c_num_t i2c_no, asdk_i2c_status_t *i2c_status);

 /** @} */ // end of asdk_can_fun_group

#endif /* ASDK_I2C_H */
