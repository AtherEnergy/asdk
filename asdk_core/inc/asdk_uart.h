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
    asdk_uart.h

    @path
    inc/asdk_uart.h

    @Created on
    Oct 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the UART Module for ASDK.
*/

#ifndef ASDK_UART_H
#define ASDK_UART_H

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

/** @defgroup asdk_uart_enum_group Enumerations
 *  Lists all the enumerations used by the UART module.
 *  @{
 */

/*!
 * @brief UART status events.
 */
typedef enum
{
  ASDK_UART_STATUS_TRANSMIT_COMPLETE = 0,   /*!< Transmit complete. */
  ASDK_UART_STATUS_TRANSMIT_ERROR,          /*!< Error occurred in transmission. */
  ASDK_UART_STATUS_RECEIVE_COMPLETE,        /*!< Receive complete */
  ASDK_UART_STATUS_RECEIVE_OVERFLOW,        /*!< UART buffer overflow. */
  ASDK_UART_STATUS_RECEIVE_ERR_FRAME,       /*!< Frame error, either a start or stop bit error on receive line */
  ASDK_UART_STATUS_RECEIVE_ERR_PARITY,      /*!< Parity error on receive line */
  ASDK_UART_STATUS_RECEIVE_BREAK_DETECT,    /*!< Break detect on receive line */
  ASDK_UART_STATUS_MAX,
  ASDK_UART_STATUS_UNDEFINED = ASDK_UART_STATUS_MAX,
}asdk_uart_status_t;

/*!
 * @brief UART Baud Rate
 */
typedef enum
{
    ASDK_UART_BAUD_RATE_300 = 300,          /*!< Use 300 bits/second baud rate */
    ASDK_UART_BAUD_RATE_1200 = 1200,        /*!< Use 1200 bits/second baud rate */
    ASDK_UART_BAUD_RATE_2400 = 2400,        /*!< Use 2400 bits/second baud rate */
    ASDK_UART_BAUD_RATE_4800 = 4800,        /*!< Use 4800 bits/second baud rate */
    ASDK_UART_BAUD_RATE_9600 = 9600,        /*!< Use 9600 bits/second baud rate */
    ASDK_UART_BAUD_RATE_19200 = 19200,      /*!< Use 19200 bits/second baud rate */
    ASDK_UART_BAUD_RATE_38400 = 38400,      /*!< Use 38400 bits/second baud rate */
    ASDK_UART_BAUD_RATE_57600 = 57600,      /*!< Use 57600 bits/second baud rate */
    ASDK_UART_BAUD_RATE_115200 = 115200,    /*!< Use 115200 bits/second baud rate */
    ASDK_UART_BAUD_RATE_230400 = 230400,    /*!< Use 230400 bits/second baud rate */
    ASDK_UART_BAUD_RATE_460800 = 460800,    /*!< Use 460800 bits/second baud rate */
    ASDK_UART_BAUD_RATE_500000 = 500000,    /*!< Use 500000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_576000 = 576000,    /*!< Use 576000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_921600 = 921600,    /*!< Use 921600 bits/second baud rate */
    ASDK_UART_BAUD_RATE_1000000 = 1000000,  /*!< Use 1000000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_1152000 = 1152000,  /*!< Use 1152000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_1500000 = 1500000,  /*!< Use 1500000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_2000000 = 2000000,  /*!< Use 2000000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_2500000 = 2500000,  /*!< Use 2500000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_3000000 = 3000000,  /*!< Use 3000000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_3500000 = 3500000,  /*!< Use 3500000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_4000000 = 4000000,  /*!< Use 4000000 bits/second baud rate */
    ASDK_UART_BAUD_RATE_MAX,
    ASDK_UART_BAUD_RATE_UNDEFINED = ASDK_UART_BAUD_RATE_MAX,
} asdk_uart_baud_rate_t;

/*!
 * @brief UART Mode of operation
 */

typedef enum
{
    ASDK_UART_MODE_STANDARD = 0,    /*!< Standard UART mode */
    ASDK_UART_MODE_SMARTCARD,
    ASDK_UART_MODE_IRDA,
    ASDK_UART_MODE_MAX,
    ASDK_UART_MODE_UNDEFINED = ASDK_UART_MODE_MAX,
}asdk_uart_op_mode_t;

/*!
 * @brief UART Parity Mode
 *
 * Implements : asdk_uart_parity_mode_t
 */
typedef enum
{
    ASDK_UART_PARITY_NONE = 0,  /*!< No parity */
    ASDK_UART_PARITY_EVEN = 2,  /*!< Use even parity */
    ASDK_UART_PARITY_ODD = 3,   /*!< Use odd parity */
    ASDK_UART_PARITY_MAX,
    ASDK_UART_PARITY_UNDEFINED = ASDK_UART_PARITY_MAX,
} asdk_uart_parity_mode_t;

/*!
 * @brief UART Stop bits count
 *
 * Implements : asdk_uart_stop_bits_t
 */
typedef enum
{
    ASDK_UART_STOP_BITS_1 = 0,  /*!< Use 1 stop bit */
    ASDK_UART_STOP_BITS_2,      /*!< Use 2 stop bits */
    ASDK_UART_STOP_BITS_MAX,
} asdk_uart_stop_bits_t;

/*!
 * @brief UART Data bits count
 */
typedef enum
{
    ASDK_UART_DATA_BITS_5 = 5,  /*!< Use 5 data bits per frame */
    ASDK_UART_DATA_BITS_6,      /*!< Use 6 data bits per frame */
    ASDK_UART_DATA_BITS_7,      /*!< Use 7 data bits per frame */
    ASDK_UART_DATA_BITS_8,      /*!< Use 8 data bits per frame */
    ASDK_UART_DATA_BITS_9,      /*!< Use 9 data bits per frame */
    ASDK_UART_DATA_BITS_MAX,
    ASDK_UART_DATA_BITS_UNDEFINED = ASDK_UART_DATA_BITS_MAX,
} asdk_uart_data_bits_t;

/*!
 * @brief An data structure to represent UART interrupt
 * settings.
 */
typedef struct
{
    bool use_interrupt; /*!< Enable or disable interrupts. */
    uint8_t priority;   /*!< Priority of the interrupt. */
} asdk_uart_interrupt_cfg_t;

/** @} */ // end of asdk_uart_enum_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_uart_ds_group Data structures
 *  Lists all the data structures used by the UART module.
 *  @{
 */

/*!
 * @brief UART transfer via DMA related config
 *
 * @note:The DMA should be configured for Byte transfer
 */
typedef struct
{
    uint8_t uart_dma_rx_channel_no;
    uint8_t uart_dma_tx_channel_no;

    uint8_t *uart_tx_data_buffer;
    uint32_t uart_tx_data_len_bytes;

    uint8_t *uart_rx_data_buffer;
    uint32_t uart_rx_data_len_bytes;
} asdk_uart_dma_config_t;

/*!
 * @brief UART Configuration structure
 *
 * @note: For tranfer via DMA ensure dataframe size to be Byte
 */
typedef struct
{
    asdk_uart_num_t uart_no; /*!< UART no. indicates the UART module no. of the ECU */
    uint8_t uart_tx_mcu_pin_no; /*!< UART TX mcu pin no */
    uint8_t uart_rx_mcu_pin_no; /*!< UART RX mcu pin no */
    uint8_t uart_cts_mcu_pin_no; /*!< UART CTS mcu pin no, currently not supported */
    uint8_t uart_rts_mcu_pin_no; /*!< UART RTS mcu pin no, currently not supported */

    asdk_uart_op_mode_t op_mode; /*!< UART Operation Mode */
    asdk_uart_baud_rate_t baud_rate; /*!< UART Baud Rate */
    asdk_uart_data_bits_t data_bits; /*!< UART Data bits count */
    asdk_uart_parity_mode_t parity_mode; /*!< UART Parity Mode */
    asdk_uart_stop_bits_t stop_bits; /*!< UART Stop bits count, currently 1 or 2 stop bits supported */

    asdk_uart_interrupt_cfg_t interrupt_config; /*!< UART interrupt config */
    asdk_uart_dma_config_t uart_dma_config; /*!< UART DMA config */

    bool enable_msb_first;  /*!< Enables to shift MSB first, otherwise, LSB first */
    bool enable_cts;    /*!< Enalbes the usage of CTS input signal */
    bool enable_rts;    /*!< Enables the usage of RTS output signal */
} asdk_uart_config_t;

/** @} */ // end of asdk_uart_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_uart_cb_group Callback function type
 *  Lists the callback functions from the UART module.
 *  @{
 */

/**
 * @brief The UART module's callback function type. Refer
   @ref asdk_uart_status_t for the supported status events.
 
   @param uart_no The UART channel on which the event occurred.
   @param data Pointer to data.
   @param data_len Length of data.
   @param event UART status event.

   @todo Use uint8_t instead of asdk_uart_num_t for UART number.
*/
typedef void (*asdk_uart_callback_fun_t)(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len, asdk_uart_status_t event);

/** @} */ // end of asdk_uart_cb_group

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_uart_fun_group Functions
 *  Lists the functions/APIs from the UART module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_uart_init                                                 */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the UART Module based on the config structure passed as parameter.

  @param [in] uart_config_data ASDK UART configuration parameter.

  @return
    - @ref ASDK_UART_STATUS_SUCCESS
    - @ref ASDK_UART_ERROR_INITIALIZED
    - @ref ASDK_UART_ERROR_NULL_PTR
    - @ref ASDK_UART_ERROR_RANGE_EXCEEDED
    - @ref ASDK_UART_ERROR_INVALID_BAUD_RATE
    - @ref ASDK_UART_ERROR_MODULE_UNAVAILABLE
    - @ref ASDK_UART_ERROR_INIT_FAIL
*/
asdk_errorcode_t asdk_uart_init(asdk_uart_config_t *uart_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_uart_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the UART Module based on the UART number passed as parameter.

  @param [in] uart_no ASDK UART no. to be de-initialized.

  @return
    - @ref ASDK_UART_STATUS_SUCCESS
    - @ref ASDK_UART_ERROR_RANGE_EXCEEDED
*/
asdk_errorcode_t asdk_uart_deinit(asdk_uart_num_t uart_no);

/*----------------------------------------------------------------------------*/
/* Function : asdk_uart_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function install callback for UART. Refer @ref asdk_uart_callback_fun_t
  to define the callback function. Refer @ref asdk_uart_status_t for the
  supported status events.

  @param [in] uart_no ASDK UART number
  @param [in] callback_fun Callback function pointer

  @return
    - @ref ASDK_UART_STATUS_SUCCESS
    - @ref ASDK_UART_ERROR_RANGE_EXCEEDED
*/
asdk_errorcode_t asdk_uart_install_callback(asdk_uart_num_t uart_no, asdk_uart_callback_fun_t callback_fun);

asdk_errorcode_t asdk_uart_write_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len);
asdk_errorcode_t asdk_uart_read_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len);

/*----------------------------------------------------------------------------*/
/* Function : asdk_uart_write_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sends data out through UART module using non-blocking method.

  @param [in] uart_no ASDK UART number
  @param [in] data Pointer to buffer containing data to be sent.
  @param [in] data_len Length of data to be sent.

  @return
    - @ref ASDK_UART_STATUS_SUCCESS
    - @ref ASDK_UART_ERROR_RANGE_EXCEEDED
    - @ref ASDK_UART_ERROR_WRITE_FAIL
*/
asdk_errorcode_t asdk_uart_write_non_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len);

/*----------------------------------------------------------------------------*/
/* Function : asdk_uart_read_non_blocking */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data from UART module using non-blocking method.

  @param [in] uart_no ASDK UART number
  @param [out] data Pointer to buffer containing data received.
  @param [in] data_len Length of data to be received.

  @return
    - @ref ASDK_UART_STATUS_SUCCESS
    - @ref ASDK_UART_ERROR_RANGE_EXCEEDED
    - @ref ASDK_UART_ERROR_READ_FAIL
*/
asdk_errorcode_t asdk_uart_read_non_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len);

/** @} */ // end of asdk_uart_fun_group

#endif /* ASDK_UART_H */
