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
    asdk_can.h

    @path
    inc/asdk_can.h

    @Created on
    Aug 07, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the CAN DAL module of ASDK
*/

#ifndef ASDK_CAN_H
#define ASDK_CAN_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>
#include <stdbool.h>

/* asdk includes ***************************** */

#include "asdk_error.h" // Defines ASDK CAN module error codes

/* dal includes ****************************** */

#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_platform.h" // Defines instances of timers

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/** @defgroup asdk_can_enum_group Enumerations
 *  Lists all the enumerations used by the CAN module.
 *  @{
 */

/*!
 * @brief An enumerator to represent CAN controller mode.
 */
typedef enum
{
    ASDK_CAN_MODE_STANDARD = 0, /*!< Use as Classic CAN */
    ASDK_CAN_MODE_FD,           /*!< Use as CAN with Flexible Datarate (CAN-FD) */
} asdk_can_controller_mode_t;

/*!
 * @brief An enumerator to represent CAN identifier type.
 */
typedef enum
{
    ASDK_CAN_ID_STANDARD = 0, /*!< Standard CAN identifier, 11-bit. */
    ASDK_CAN_ID_EXTENDED,     /*!< Extended CAN identifier, 29-bit. */
} asdk_can_id_t;

/*!
 * @brief An enumerator to represent CAN data length code.
 *
 * @note
 * The enum values are coded to actual lengths and does
 * not use a sequence numbers.
 */
typedef enum
{
    ASDK_CAN_DLC_8 = 8,   /*!< Data size is 8 bytes */
    ASDK_CAN_DLC_12 = 12, /*!< Data size is 12 bytes */
    ASDK_CAN_DLC_16 = 16, /*!< Data size is 16 bytes */
    ASDK_CAN_DLC_20 = 20, /*!< Data size is 20 bytes */
    ASDK_CAN_DLC_24 = 24, /*!< Data size is 24 bytes */
    ASDK_CAN_DLC_32 = 32, /*!< Data size is 32 bytes */
    ASDK_CAN_DLC_48 = 48, /*!< Data size is 48 bytes */
    ASDK_CAN_DLC_64 = 64, /*!< Data size is 64 bytes */
} asdk_can_dlc_t;

/*!
 * @brief An enumerator to represent the CAN baudrate (nominal).
 */
typedef enum
{
    ASDK_CAN_BAUDRATE_125K = 0, /*!< Baudrate is 125,000 bps. */
    ASDK_CAN_BAUDRATE_250K,     /*!< Baudrate is 250,000 bps. */
    ASDK_CAN_BAUDRATE_500K,     /*!< Baudrate is 500,000 bps. */
    ASDK_CAN_BAUDRATE_1M,       /*!< Baudrate is 1,000,000 bps. */
} asdk_can_baudrate_t;

/*!
 * @brief An enumerator to represent the data baudrate.
 */
typedef enum
{
    ASDK_CAN_DATA_BAUDRATE_500K = 0, /*!< Baudrate is 500,000 bps. */
    ASDK_CAN_DATA_BAUDRATE_1M,       /*!< Baudrate is 1,000,000 bps. */
    ASDK_CAN_DATA_BAUDRATE_2M,       /*!< Baudrate is 2,000,000 bps. */
    ASDK_CAN_DATA_BAUDRATE_4M,       /*!< Baudrate is 4,000,000 bps. */
    ASDK_CAN_DATA_BAUDRATE_5M,       /*!< Baudrate is 5,000,000 bps. */
    ASDK_CAN_DATA_BAUDRATE_8M,       /*!< Baudrate is 8,000,000 bps. */
} asdk_can_data_baudrate_t;

/*!
 * @brief An enumerator to represent hardware based events.
 */
typedef enum
{
    ASDK_CAN_TX_COMPLETE_EVENT = 0, /*!< Transmit complete event. */
    ASDK_CAN_RX_EVENT,              /*!< Receive event, new message arrived. */
    ASDK_CAN_TX_ERROR_EVENT,        /*!< Transmit error event. */
    ASDK_CAN_RX_ERROR_EVENT,        /*!< Recieve error event. */
    ASDK_CAN_ERROR_EVENT,           /*!< Unknown hardware error event. */
    ASDK_CAN_BUS_OFF_EVENT,         /*!< Bus-off event. */
    ASDK_CAN_SLEEP_EVENT,           /*!< Sleep event. */
    ASDK_CAN_WAKE_UP_EVENT,         /*!< Wake up event. */

    ASDK_CAN_MAX_EVENT, /*!< Total number of hardware events */
} asdk_can_event_t;

/** @} */ // end of asdk_can_enum_group

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/** @defgroup asdk_can_ds_group Data structures
 *  Lists all the data structures used by the CAN module.
 *  @{
 */

/*!
 * @brief An data structure to represent CAN bit-time settings which is
 * used for setting the sampling-point.
 *
 * The bit-time value is represented in time-quantum (tq) which is split
 * into 4 segments. It is derived based on the CAN peripheral clock and
 * the desired baudrate.
 *
 * Example: 
 * * CAN clock = 20MHz, Baudrate = 500kbps, Clock prescaler = 1
 * * time quanta = (clock / prescaler) / baudarate
 * * time quanta = (20MHz / 1) / 500kbps = 40tq per bit-time
 *
 * 1 bit-time is split into 4 segments:
 * 1. Sync-Segment - Synchornizes to the receiving frame & is fixed to 1tq.
 * 2. Propogation-Segment - Compensates for physical delay times.
 * 3. Phase-Segment-1 - Compensates for edge-phase error before sampling point.
 * 4. Phase-Segment-2 - Compensates for edge-phase error after sampling point.
 *
 * To know the actual tq for a given baudrate please refer to platform
 * documentation or source file (asdk_can.c) of the corresponding MCU.
 * It presumes the clock pre-scaler for a given baudrate because some
 * micrcontrollers based on the implementation of CAN peripheral might
 * allow for values larger than the suggested values in the below table.
 *
 * The table below shows the range for each segment.
 * |segment         |min |max |
 * |----------------|----|----|
 * |sync segment    |  1 |  1 |
 * |prop segment    |  1 |  8 |
 * |phase segment-1 |  1 |  8 |
 * |phase segment-2 |  2 |  8 |
 * |================|====|====|
 * |Total tq range: |  5 | 25 |
 *
 * Reference:
 * - http://esd.cs.ucr.edu/webres/can20.pdf
 * - https://www.can-cia.org/fileadmin/resources/documents/proceedings/2012_taralkar.pdf
 * - https://www.can-cia.org/fileadmin/resources/documents/proceedings/2013_hartwich_v2.pdf
 */
typedef struct
{
    uint8_t prop_segment;    /*!< Compensate for physical delay times. */
    uint8_t phase_segment1;  /*!< Compensate for edge-phase error before sampling point. */
    uint8_t phase_segment2;  /*!< Compensate for edge-phase error after sampling point. */
    uint8_t sync_jump_width; /*!< Resynchronization jump width for sampling-point tolerance.
                                  Programmable between 1 and min(4, phase-segment-1). */
} asdk_can_bit_time_settings_t;

/*!
 * @brief An data structure to represent CAN bitrate settings.
 */
typedef struct
{
    asdk_can_baudrate_t baudrate;          /*!< CAN baudrate. */
    asdk_can_bit_time_settings_t bit_time; /*!< Bit time settings. */
} asdk_can_bitrate_t;

/*!
 * @brief An data structure to represent CAN-FD bitrate settings.
 */
typedef struct
{
    asdk_can_baudrate_t nominal_baudrate;          /*!< Nominal baudrate. */
    asdk_can_bit_time_settings_t nominal_bit_time; /*!< Nominal bit time settings. */
    asdk_can_data_baudrate_t data_baudrate;        /*!< Data baudrate. */
    asdk_can_bit_time_settings_t data_bit_time;    /*!< Data bit time settings. */
} asdk_canfd_bitrate_t;

/*!
 * @brief An union data structure to represent CAN peripheral
 * bitrate settings. Either CAN or CAN-FD bitrate settings
 * apply.
 */
typedef union
{
    asdk_can_bitrate_t can;     /*!< Bitrate settings for CAN. */
    asdk_canfd_bitrate_t canfd; /*!< Bitrate settings for CAN-FD. */
} asdk_can_bitrate_config_t;

/*!
 * @brief An data structure to represent CAN interrupt
 * settings.
 */
typedef struct
{
    bool use_interrupt; /*!< Enable or disable interrupts. */
    uint8_t priority;   /*!< Priority of the interrupt. */
} asdk_can_interrupt_cfg_t;

/*!
 * @brief An data structure to represent CAN controller
 * settings.
 */
typedef struct
{
    asdk_can_controller_mode_t mode;           /*!< Use as classic CAN or CAN-FD. */
    asdk_can_id_t can_id_type;                 /*!< CAN identifier type. */
    asdk_can_dlc_t max_dlc;                    /*!< Maximum DLC required. Setups the mailbox size. */
    asdk_can_bitrate_config_t bitrate_config;  /*!< Bitrate configuration settings. */
    asdk_can_interrupt_cfg_t interrupt_config; /*!< Interrupt configuration settings. */
} asdk_can_controller_t;

/*!
 * @brief An data structure to represent CAN mailbox
 * settings.
 */
typedef struct
{
    uint8_t virtual_mailbox_no; /*!< Virtual mailbox number. */
    uint32_t can_id_mask;       /*!< Mask for accepting CAN message into mailbox Tx or Rx. */
} asdk_can_mailbox_t;

/*!
 * @brief An data structure to represent acceptance filtering
 * for Rx FIFO.
 */
typedef struct
{
    uint32_t *can_ids; /*!< An array of CAN IDs to be accpeted for reception in FIFO. */
    uint8_t length;    /*!< Size of the array (can_ids). */
} asdk_can_rx_fifo_acceptance_filter_t;

/*!
 * @brief An data structure to represent hardware filtering
 * settings.
 */
typedef struct
{
    asdk_can_mailbox_t *tx_mailboxes; /*!< An array of mailboxes to be configured for transmission. */
    uint8_t no_of_tx_mailbox;         /*!< Number elements in tx_mailboxes. */

    asdk_can_mailbox_t *rx_mailboxes; /*!< An array of mailboxes to be configured for reception. */
    uint8_t no_of_rx_mailbox;         /*!< Number elements in rx_mailboxes. */

    asdk_can_rx_fifo_acceptance_filter_t rx_fifo_acceptance_filter; /*!< Rx FIFO acceptance filter settings. */
} asdk_can_hw_filter_t;

/*!
 * @brief An data structure to represent the pins required by
 * the CAN peripheral.
 */
typedef struct
{
    asdk_mcu_pin_t tx_pin; /*!< Transmit pin of the MCU */
    asdk_mcu_pin_t rx_pin; /*!< Receive pin of the MCU */
} asdk_can_pin_t;

/*!
 * @brief An data structure to represent the CAN peripheral
 * configuration.
 */
typedef struct
{
    asdk_can_pin_t mcu_pins; /*!< CAN peripheral Tx and Rx pins. */

    asdk_can_controller_t controller_settings; /*!< CAN peripheral settings. */

    /* hw filter settings */
    asdk_can_hw_filter_t hw_filter; /*!< CAN hardware filter settings. */
} asdk_can_config_t;

/*!
 * @brief An data structure to represent the CAN message.
 */
typedef struct
{
    uint32_t can_id;  /*!< The CAN message identifier (CAN ID). */
    uint8_t dlc;      /*!< Length of the message. */
    uint8_t *message; /*!< A pointer to a buffer for holding data. */
} asdk_can_message_t;

/** @} */ // end of asdk_can_ds_group

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           CALLBACK FUNCTION TYPES

==============================================================================*/

/** @defgroup asdk_can_cb_group Callback function type
 *  Lists the callback functions from the CAN module.
 *  @{
 */

/*!
 * @brief The CAN module's callback function type. For the supported CAN events
   refer @ref asdk_can_install_callback function.

   @param can_ch The CAN channel on which the event occurred.
   @param event The CAN hardware event that caused the callback.
   @param can_message CAN message is expected only during the @ref ASDK_CAN_TX_COMPLETE_EVENT and @ref ASDK_CAN_RX_EVENT, otherwise this parameter is NULL.
 */
typedef void (*asdk_can_callback_t)(uint8_t can_ch, asdk_can_event_t event, asdk_can_message_t *can_message);

/** @} */ // end of asdk_can_cb_group

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

/** @defgroup asdk_can_fun_group Functions
 *  Lists the functions/APIs from the CAN module.
 *  @{
 */

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function initializes the CAN peripheral.

  The CAN peripheral mode should be configured either as Classic CAN or CAN with
  Flexible Datarate (FD). Based on the configured mode and baudrate the
  corresponding sampling-point settings must be set by the user. Refer the
  Platforms section of ASDK documentation and look for the target platform
  document. It contains information on baudarates and its supporting
  time-quanta values.

  To enable interrupts refer @ref asdk_can_interrupt_cfg_t and configure 
  "interrupt_config" within the configuration parameter before calling this API.
  Then define the callback function by referring @ref asdk_can_callback_t and
  install it with @ref asdk_can_install_callback API.

  @param [in] can_ch CAN channel number.
  @param [in] can_config CAN configuration.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_PINMUX_ERROR_NULL_PTR
    - @ref ASDK_PINMUX_ERROR_INVALID_MCU_PIN
    - @ref ASDK_PINMUX_ERROR_INVALID_ALTERNATE_FUNCTION
    - @ref ASDK_PINMUX_ERROR_INIT_CONFIG_ERROR
    - @ref ASDK_PINMUX_ERROR_DEINIT_CONFIG_ERROR
    - @ref ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ENABLED
    - @ref ASDK_CLOCK_ERROR_DIV_NOT_ASSIGNED
    - @ref ASDK_CAN_ERROR_INVALID_DLC
    - @ref ASDK_CAN_ERROR_INIT_FAILED
*/
asdk_errorcode_t asdk_can_init(asdk_can_channel_t can_ch, asdk_can_config_t *can_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function deinitializes CAN peripheral.

  @param [in] can_ch CAN channel number.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_INVALID_CHANNEL
    - @ref ASDK_CAN_ERROR_DEINIT_FAILED
*/
asdk_errorcode_t asdk_can_deinit(asdk_can_channel_t can_ch);

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function registers the user defined CAN callback. Refer
  @ref asdk_can_install_callback to define the callback function.
  The following events are supported.
  - @ref ASDK_CAN_TX_COMPLETE_EVENT
  - @ref ASDK_CAN_RX_EVENT
  - @ref ASDK_CAN_RX_ERROR_EVENT
  - @ref ASDK_CAN_ERROR_EVENT
  - @ref ASDK_CAN_BUS_OFF_EVENT

  @note
  This a module level callback.

  @param [in] callback CAN callback function for handling
  Tx, Rx events and Error events.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_NULL_PTR
*/
asdk_errorcode_t asdk_can_install_callback(asdk_can_callback_t callback);

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_sleep */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function puts the CAN peripheral to sleep mode.

  @param [in] can_ch CAN channel number.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_HW_FEATURE_NOT_SUPPORTED
*/
asdk_errorcode_t asdk_can_sleep(asdk_can_channel_t can_ch);


/*----------------------------------------------------------------------------*/
/* Function : asdk_can_is_tx_busy */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function checks whether the given mailbox is busy or not. The
  given mailbox should have been configured for transimission.

  @param [in] can_ch CAN channel number.
  @param [in] mailbox_no Tx mailbox number.
  @param [out] status true if busy, false otherwise.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_INVALID_CHANNEL
*/
asdk_errorcode_t asdk_can_is_tx_busy(asdk_can_channel_t can_ch, uint8_t virtual_mailbox_no, bool *status);

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function fills the given message in transmit mailbox. The given
  mailbox should have been configured for transmission.

  @param [in] can_ch CAN channel number.
  @param [in] mailbox_no Tx mailbox number.
  @param [in] can_message The CAN message to be transmitted.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_INVALID_CHANNEL
    - @ref ASDK_CAN_ERROR_WRITE_FAILED
*/
asdk_errorcode_t asdk_can_write(asdk_can_channel_t can_ch, uint8_t virtual_mailbox_no, asdk_can_message_t *can_message);

/*----------------------------------------------------------------------------*/
/* Function : asdk_can_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function reads the message from the receive mailbox. The given
  mailbox should have been configured for reception.

  @param [in] can_ch CAN channel number.
  @param [in] mailbox_no Rx mailbox number.
  @param [out] can_message The CAN message buffer.

  @return
    - @ref ASDK_CAN_SUCCESS
    - @ref ASDK_CAN_ERROR_INVALID_CHANNEL
    - @ref ASDK_CAN_ERROR_READ_FAILED
*/
asdk_errorcode_t asdk_can_read(asdk_can_channel_t can_ch, uint8_t virtual_mailbox_no, asdk_can_message_t *can_message);

/** @} */ // end of asdk_can_fun_group

#endif /* ASDK_CAN_H */
