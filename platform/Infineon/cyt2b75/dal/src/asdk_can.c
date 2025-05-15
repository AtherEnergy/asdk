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
    asdk_can.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_can.c

    @Created on
    Aug 07, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the CAN module for CYT2B75 microcontroller.

*/

/*
 * Polling based:
   * Transmission:
   * Reception:

 * Interrupt based:
   * Transmission:
   * Reception:
*/

/*
    to do:
    1. [done] std id filtering.
    2. [] ext id support.
    3. [] ext id filtering.
    5. [] new features:
          * dedicated tx mailbox, requires id:mailbox mapping.
          * dedicated rx mailbox, requires id:mailbox mapping.
    6. [] enhancement:
        [] * use remaining mailboxes for tx as tx reserve pool
    7. [] handle can fd.
*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

#include "asdk_can.h"    // ASDK CAN APIs
#include "asdk_clock.h"  // CYT2B75 DAL clock APIs
#include "asdk_pinmux.h" // CYT2B75 DAL pinmux APIs

/* sdk includes ****************************** */

#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "canfd/cy_canfd.h"    // CYT2B75 CAN driver APIs
#include "sysint/cy_sysint.h"  // CYT2B75 system Interrupt APIs

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define CAN_PINMUX_COUNT 2 /*!< Number of pins required by CAN peripheral */

#define CAN_PEIPHERAL_CLOCK_HZ 40000000 /*!< CAN peripheral frequency. Using the best available frequency, \
                                             refer Figure 23-15 of the TRM for more information. */

#define CAN_HW_TX_MAILBOX_MAX (32)      /*!< Number of dedicated Tx buffers */
#define CAN_HW_RX_MAILBOX_MAX (64)      /*!< Number of dedicated Rx buffers */
#define CAN_HW_RX_FIFO_SIZE (64)        /*!< Size of the Rx FIFO */
#define CAN_HW_FILTER_ELEMENT_MAX (128) /*!< Size of the H/W filter table */

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*!
 * @brief An enumerator to represent the hardware CAN modules as per
 * microcontroller. Refer the TRM of the microcontroller for more
 * information.
 *
 * Implements : asdk_cyt2b75_hw_can_module_t
 */
typedef enum
{
    CYT2B75_CANFD_0 = 0, /*!< CANFD 0 module. */
    CYT2B75_CANFD_1,     /*!< CANFD 1 module. */
    CYT2B75_CANFD_MAX,   /*!< Total number of CAN modules. */
} asdk_cyt2b75_hw_can_module_t;

/*!
 * @brief An enumerator to represent the hardware CAN channels per
 * CAN module. Refer the TRM of the microcontroller for more
 * information.
 *
 * Implements : asdk_cyt2b75_hw_can_channel_t
 */
typedef enum
{
    CYT2B75_CAN_CH_0 = 0, /*!< Channel 0. */
    CYT2B75_CAN_CH_1,     /*!< Channel 1. */
    CYT2B75_CAN_CH_2,     /*!< Channel 2. */
    CYT2B75_CAN_CH_MAX,   /*!< Total number of channels. */
} asdk_cyt2b75_hw_can_channel_t;

/*!
 * @brief An enumerator to identify the type of baudrate. Required for
 * controllers with CAN-FD support.
 *
 * Implements : asdk_can_baudrate_type_t
 */
typedef enum
{
    ASDK_CAN_BAUDRATE_TYPE_NOMINAL = 0, /*!< Baudrate for classic CAN and for the
                                             arbitration phase of CAN-FD frame. */
    ASDK_CAN_BAUDRATE_TYPE_DATA,        /*!< Baudrate for the data phase of CAN-FD frame. */
} asdk_can_baudrate_type_t;

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*!
 * @brief An data structure to hold the module, channel and base address
 * of a CAN channel. Required for using the driver APIs.
 *
 * Implements : asdk_cyt2b75_can_map_t
 */
typedef struct
{
    asdk_cyt2b75_hw_can_module_t module_no;    /*!< The CAN module to which the channel belongs. */
    asdk_cyt2b75_hw_can_channel_t ch_no;       /*!< The CAN channel in use. */
    cy_pstc_canfd_type_t cyt_can_base_address; /*!< The base address of the CAN module. */
} asdk_cyt2b75_can_map_t;

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static inline asdk_errorcode_t __asdk_set_can_max_dlc(asdk_can_dlc_t max_dlc, cy_stc_canfd_config_t *cyt_config);
static inline asdk_errorcode_t __asdk_set_can_baudrate(asdk_can_baudrate_t baudrate, cy_stc_canfd_config_t *cyt_config);
static inline asdk_errorcode_t __asdk_set_can_data_baudrate(asdk_can_data_baudrate_t data_baudrate, cy_stc_canfd_config_t *cyt_config);
static inline asdk_errorcode_t __asdk_set_can_bit_timing(asdk_can_baudrate_type_t type, asdk_can_bit_time_settings_t bit_time, cy_stc_canfd_config_t *cyt_config);
static inline void __asdk_set_can_bitrate_config(asdk_can_controller_mode_t mode, asdk_can_bitrate_config_t bitrate_config, cy_stc_canfd_config_t *cyt_config);
static inline asdk_errorcode_t __asdk_set_can_hw_filter(asdk_can_id_t id_type, asdk_can_hw_filter_t hw_filter, cy_stc_canfd_config_t *cyt_config);

// ISR handlers
static void asdk_cyt2b75_can0_isr(void);
static void asdk_cyt2b75_can1_isr(void);
static void asdk_cyt2b75_can2_isr(void);
static void asdk_cyt2b75_can3_isr(void);
static void asdk_cyt2b75_can4_isr(void);
static void asdk_cyt2b75_can5_isr(void);

// Callback handlers
static void __asdk_cyt2b75_can_tx_handler(void);
static void __asdk_cyt2b75_can_rx_handler(bool bRxFifoMsg, uint8_t u8MsgBufOrRxFifoNum, cy_stc_canfd_msg_t *pstcCanFDmsg);
static void __asdk_cyt2b75_can_rxfifo_top_handler(uint8_t u8FifoNum, uint8_t u8BufferSizeInWord, uint32_t *pu32RxBuf);
static void __asdk_cyt2b75_can_error_handler(cy_en_canfd_bus_error_t volatile errorcode);
static void __asdk_cyt2b75_can_status_handler(cy_en_canfd_bus_error_status_t enCanFDStatus);

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

// Configuration
static cy_stc_sysint_irq_t irq_cfg = {0};
static asdk_clock_peripheral_t can_clock = {0};

// Filters
static cy_stc_id_filter_t std_id_filter[CAN_HW_FILTER_ELEMENT_MAX] = {0};
static cy_stc_extid_filter_t ext_id_filter[CAN_HW_FILTER_ELEMENT_MAX] = {0};

// Callbacks
static asdk_can_callback_t can_callback = NULL;

// DAL buffers
static cy_stc_canfd_msg_t cyt2b75_can_rxfifo_msg = {0};
static asdk_can_message_t can_rx_buffer = {0};
static asdk_can_message_t can_tx_buffer = {0};

// Default assumptions
// static uint8_t tx_mailbox = 0;
static asdk_can_channel_t active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED; // store instance of can while processing interrupt

/* static const variables */

// Map CAN channels as independent can modules
static const asdk_cyt2b75_can_map_t can_map[ASDK_CAN_MODULE_CAN_CH_MAX] = {
    [ASDK_CAN_MODULE_CAN_CH_0] = {CYT2B75_CANFD_0, CYT2B75_CAN_CH_0, CANFD0_CH0},
    [ASDK_CAN_MODULE_CAN_CH_1] = {CYT2B75_CANFD_0, CYT2B75_CAN_CH_1, CANFD0_CH1},
    [ASDK_CAN_MODULE_CAN_CH_2] = {CYT2B75_CANFD_0, CYT2B75_CAN_CH_2, CANFD0_CH2},
    [ASDK_CAN_MODULE_CAN_CH_3] = {CYT2B75_CANFD_1, CYT2B75_CAN_CH_0, CANFD1_CH0},
    [ASDK_CAN_MODULE_CAN_CH_4] = {CYT2B75_CANFD_1, CYT2B75_CAN_CH_1, CANFD1_CH1},
    [ASDK_CAN_MODULE_CAN_CH_5] = {CYT2B75_CANFD_1, CYT2B75_CAN_CH_2, CANFD1_CH2},
};

/* CYT2B75 CAN pin mapping

|   CANFD| CH|                         Tx|                         Rx|
|--------|---|---------------------------|---------------------------|
| CANFD_0|  0| MCU_PIN_6,            P2.0| MCU_PIN_7,            P2.1|
| CANFD_0|  0| MCU_PIN_35,           P8.0| MCU_PIN_36,           P8.1|
| CANFD_0|  1| MCU_PIN_4,            P0.2| MCU_PIN_5,            P0.3|
| CANFD_0|  1| MCU_PIN_NOT_DEFINED,  P4.3| MCU_PIN_NOT_DEFINED,  P4.4|
| CANFD_0|  2| MCU_PIN_20,           P6.2| MCU_PIN_21,           P6.3|
| CANFD_0|  2| MCU_PIN_45,          P12.0| MCU_PIN_46,          P12.1|
| CANFD_1|  0| MCU_PIN_60,          P14.0| MCU_PIN_61,          P14.1|
| CANFD_1|  0| MCU_PIN_NOT_DEFINED, P23.0| MCU_PIN_NOT_DEFINED, P23.1|
| CANFD_1|  1| MCU_PIN_64,          P17.0| MCU_PIN_65,          P17.1|
| CANFD_1|  1| MCU_PIN_91,          P22.0| MCU_PIN_92,          P22.1|
| CANFD_1|  2| MCU_PIN_73,          P18.6| MCU_PIN_74,          P18.7|
| CANFD_1|  2| MCU_PIN_NOT_DEFINED, P20.3| MCU_PIN_NOT_DEFINED, P20.4|*/

// IRQ numbers
static const cy_en_intr_t can_isr_src[ASDK_CAN_MODULE_CAN_CH_MAX] = {
    /* Using interrupt LINE0 */
    canfd_0_interrupts0_0_IRQn,
    canfd_0_interrupts0_1_IRQn,
    canfd_0_interrupts0_2_IRQn,
    canfd_1_interrupts0_0_IRQn,
    canfd_1_interrupts0_1_IRQn,
    canfd_1_interrupts0_2_IRQn};

// ISR handlers
static const cy_systemIntr_Handler can_isr[ASDK_CAN_MODULE_CAN_CH_MAX] = {
    asdk_cyt2b75_can0_isr,
    asdk_cyt2b75_can1_isr,
    asdk_cyt2b75_can2_isr,
    asdk_cyt2b75_can3_isr,
    asdk_cyt2b75_can4_isr,
    asdk_cyt2b75_can5_isr,
};

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

static inline asdk_errorcode_t __asdk_set_can_max_dlc(asdk_can_dlc_t max_dlc, cy_stc_canfd_config_t *cyt_config)
{
    asdk_errorcode_t dlc_status = ASDK_CAN_SUCCESS;

    switch (max_dlc)
    {
    case ASDK_CAN_DLC_8:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_8;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_8;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_8;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_8;
        break;

    case ASDK_CAN_DLC_12:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_12;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_12;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_12;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_12;
        break;

    case ASDK_CAN_DLC_16:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_16;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_16;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_16;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_16;
        break;

    case ASDK_CAN_DLC_20:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_20;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_20;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_20;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_20;
        break;

    case ASDK_CAN_DLC_24:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_24;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_24;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_24;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_24;
        break;

    case ASDK_CAN_DLC_32:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_32;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_32;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_32;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_32;
        break;

    case ASDK_CAN_DLC_48:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_48;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_48;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_48;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_48;
        break;

    case ASDK_CAN_DLC_64:
        cyt_config->rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_64;
        cyt_config->txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_64;
        cyt_config->rxFifo0DataSize = CY_CANFD_BUFFER_DATA_SIZE_64;
        cyt_config->rxFifo1DataSize = CY_CANFD_BUFFER_DATA_SIZE_64;
        break;

    default:
        dlc_status = ASDK_CAN_ERROR_INVALID_DLC;
        break;
    }

    return dlc_status;
}

static inline asdk_errorcode_t __asdk_set_can_baudrate(asdk_can_baudrate_t baudrate, cy_stc_canfd_config_t *cyt_config)
{
    asdk_errorcode_t baudrate_status = ASDK_CAN_SUCCESS;

    /* CAN Clock = 40 MHz */

    switch (baudrate)
    {
    case ASDK_CAN_BAUDRATE_125K:
        // 40M / 125K = 320
        // 320 / (prescaler+1) = 40
        cyt_config->bitrate.prescaler = 7; // 40tq per bit_time
        break;

    case ASDK_CAN_BAUDRATE_250K:
        // 40M / 250K = 160
        // 160 / (prescaler+1) = 40
        cyt_config->bitrate.prescaler = 3; // 40tq per bit_time
        break;

    case ASDK_CAN_BAUDRATE_500K:
        // 40M / 500K = 80
        // 80 / (prescaler+1) = 40
        cyt_config->bitrate.prescaler = 1; // 40tq per bit_time
        break;

    case ASDK_CAN_BAUDRATE_1M:
        // 40M / 1M = 40
        // 40 / (prescaler+1) = 40
        cyt_config->bitrate.prescaler = 0; // 40tq per bit_time
        break;

    default:
        baudrate_status = ASDK_CAN_ERROR_INVALID_BAUDRATE;
    }

    return baudrate_status;
}

static inline asdk_errorcode_t __asdk_set_can_data_baudrate(asdk_can_data_baudrate_t data_baudrate, cy_stc_canfd_config_t *cyt_config)
{
    asdk_errorcode_t data_baudrate_status = ASDK_CAN_SUCCESS;

    /* CAN Clock = 40 MHz */

    switch (data_baudrate)
    {
    case ASDK_CAN_DATA_BAUDRATE_500K:
        // 40M / 500K = 80
        // 80 / (prescaler+1) = 40
        cyt_config->fastBitrate.prescaler = 1; // 40tq per bit_time
        break;

    case ASDK_CAN_DATA_BAUDRATE_1M:
        // 40M / 1M = 40
        // 40 / (prescaler+1) = 40
        cyt_config->fastBitrate.prescaler = 0; // 40tq per bit_time
        break;

    case ASDK_CAN_DATA_BAUDRATE_2M:
        // 40M / 2M = 20
        // 20 / (prescaler+1) = 20
        cyt_config->fastBitrate.prescaler = 0; // 20tq per bit_time
        break;

    case ASDK_CAN_DATA_BAUDRATE_4M:
        // 40M / 4M = 10
        // 10 / (prescaler+1) = 10
        cyt_config->fastBitrate.prescaler = 0; // 10tq per bit_time
        break;

    case ASDK_CAN_DATA_BAUDRATE_5M:
        // 40M / 5M = 8
        // 8 / (prescaler+1) = 8
        cyt_config->fastBitrate.prescaler = 0; // 8tq per bit_time
        break;

    case ASDK_CAN_DATA_BAUDRATE_8M:
        // 40M / 8M = 5
        // 5 / (prescaler+1) = 8
        cyt_config->fastBitrate.prescaler = 0; // 5tq per bit_time
        break;

    default:
        data_baudrate_status = ASDK_CAN_ERROR_INVALID_DATA_BAUDRATE;
        break;
    }

    return data_baudrate_status;
}

static inline asdk_errorcode_t __asdk_set_can_bit_timing(asdk_can_baudrate_type_t type, asdk_can_bit_time_settings_t bit_time, cy_stc_canfd_config_t *cyt_config)
{
    asdk_errorcode_t bit_time_status = ASDK_CAN_SUCCESS;

    /* Note: Refer Table 23-7 of Techinical Reference Manual v09_00
     phase_segment_1 = prop_segment + phase_segment_1 (in CYT2B75) */

    if (ASDK_CAN_BAUDRATE_TYPE_NOMINAL == type)
    {
        cyt_config->bitrate.timeSegment1 = bit_time.prop_segment + bit_time.phase_segment1 - 1;
        cyt_config->bitrate.timeSegment2 = bit_time.phase_segment2 - 1;
        cyt_config->bitrate.syncJumpWidth = bit_time.sync_jump_width;
    }
    else // ASDK_CAN_BAUDRATE_TYPE_DATA
    {
        cyt_config->fastBitrate.timeSegment1 = bit_time.prop_segment + bit_time.phase_segment1 - 1;
        cyt_config->fastBitrate.timeSegment2 = bit_time.phase_segment2 - 1;
        cyt_config->fastBitrate.syncJumpWidth = bit_time.sync_jump_width;
    }

    return bit_time_status;
}

static inline void __asdk_set_can_bitrate_config(asdk_can_controller_mode_t mode, asdk_can_bitrate_config_t bitrate_config, cy_stc_canfd_config_t *cyt_config)
{
    if (ASDK_CAN_MODE_STANDARD == mode)
    {
        // nominal = common baudrate for entire can frame
        __asdk_set_can_baudrate(bitrate_config.can.baudrate, cyt_config);
        __asdk_set_can_bit_timing(ASDK_CAN_BAUDRATE_TYPE_NOMINAL, bitrate_config.can.bit_time, cyt_config);
    }
    else // ASDK_CAN_MODE_FD
    {
        // nominal = baudrate for arbitration phase (Arbitration + Control[FDF,res] + CRC_Delimiter + ACK)
        __asdk_set_can_baudrate(bitrate_config.canfd.nominal_baudrate, cyt_config);
        __asdk_set_can_bit_timing(ASDK_CAN_BAUDRATE_TYPE_NOMINAL, bitrate_config.canfd.nominal_bit_time, cyt_config);

        // data = baudrate for the data phase (Control[BRS,ESI,DLC] + DATA + CRC)
        __asdk_set_can_baudrate(bitrate_config.canfd.data_baudrate, cyt_config);
        __asdk_set_can_bit_timing(ASDK_CAN_BAUDRATE_TYPE_DATA, bitrate_config.canfd.data_bit_time, cyt_config);
    }
}

static inline asdk_errorcode_t __asdk_set_can_hw_filter(asdk_can_id_t id_type, asdk_can_hw_filter_t hw_filter, cy_stc_canfd_config_t *cyt_config)
{
    asdk_errorcode_t hw_filter_status = ASDK_CAN_SUCCESS;

    /* New feature:
       Designated mailboxes (Tx & Rx) for critical signals */

    if (NULL != hw_filter.tx_mailboxes) // optional
    {
    }
    else
    {
        // use mailbox 0 for Tx
        cyt_config->noOfTxBuffers = 1;
    }

    if (NULL != hw_filter.rx_mailboxes) // optional
    {
    }
    else
    {
        // using fifo
        cyt_config->noOfRxBuffers = 0;
    }

    /* Legacy feature:
       1. Use Rx FIFO for reception.
       2. Use single mailbox for transmission.
       3. When the tx mailbox is busy, returns as busy in can_services.c file.

       Enhancement:
       3. When the tx mailbox is busy:
         + Use the next available mailbox from the transmit reserve pool.
           If all mailboxes of the pool are busy, return as busy in can_services.c file.
           Else, use it for the current transmission. */

    // Rx FIFO without filtering, accept all
    if (NULL == hw_filter.rx_fifo_acceptance_filter.can_ids)
    {
        if (ASDK_CAN_ID_STANDARD == id_type)
        {
            cyt_config->sidFilterConfig.numberOfSIDFilters = 1;
            cyt_config->sidFilterConfig.sidFilter = std_id_filter;

            std_id_filter[0].sfid1 = 0x00;
            std_id_filter[0].sfid2 = 0x00;
            std_id_filter[0].sfec = CY_CANFD_ID_FILTER_ELEMNT_CONFIG_SET_PIORITY_STORE_RXFIFO0;
            std_id_filter[0].sft = (uint32_t)CY_CANFD_STD_ID_FILTER_TYPE_CLASSIC;
        }
        else // ASDK_CAN_ID_EXTENDED
        {
            cyt_config->extidFilterConfig.numberOfEXTIDFilters = 1;
            cyt_config->extidFilterConfig.extidFilter = ext_id_filter;

            // todo: handle ext id
        }
    }
    else // Rx FIFO with filtering
    {
        if (ASDK_CAN_ID_STANDARD == id_type)
        {
            cyt_config->sidFilterConfig.numberOfSIDFilters = hw_filter.rx_fifo_acceptance_filter.length;
            cyt_config->sidFilterConfig.sidFilter = std_id_filter;

            for (uint8_t i = 0; i < hw_filter.rx_fifo_acceptance_filter.length; i++)
            {
                std_id_filter[i].sfid1 = hw_filter.rx_fifo_acceptance_filter.can_ids[i];
                std_id_filter[i].sfid2 = 0x7FF;
                std_id_filter[i].sfec = CY_CANFD_ID_FILTER_ELEMNT_CONFIG_SET_PIORITY_STORE_RXFIFO0;
                std_id_filter[i].sft = (uint32_t)CY_CANFD_STD_ID_FILTER_TYPE_CLASSIC;
            }
        }
        else // ASDK_CAN_ID_EXTENDED
        {
            cyt_config->extidFilterConfig.numberOfEXTIDFilters = hw_filter.rx_fifo_acceptance_filter.length;
            cyt_config->extidFilterConfig.extidFilter = ext_id_filter;

            // todo: handle ext id
        }
    }

    // reject non-matching and remote frames
    cyt_config->globalFilterConfig.nonMatchingFramesStandard = CY_CANFD_REJECT_NON_MATCHING; // Reject none match IDs
    cyt_config->globalFilterConfig.nonMatchingFramesExtended = CY_CANFD_REJECT_NON_MATCHING; // Reject none match IDs
    cyt_config->globalFilterConfig.rejectRemoteFramesStandard = true;                        // No remote frame
    cyt_config->globalFilterConfig.rejectRemoteFramesExtended = true;                        // No remote frame

    // do not use RxFIFO_1
    cyt_config->rxFifo1Config.numberOfFifoElements = 0;

    // use RxFIFO_0
    cyt_config->rxFifo0Config.mode = CY_CANFD_FIFO_MODE_BLOCKING;
    cyt_config->rxFifo0Config.watermark = 0u;
    cyt_config->rxFifo0Config.numberOfFifoElements = CAN_HW_RX_FIFO_SIZE;
    cyt_config->rxFifo0Config.topPointerLogicEnabled = true;

    // use remaining mailboxes Tx
    // Enhancement: use leftover mailboxes for Tx operation
    // enable_tx_pool();

    return hw_filter_status;
}

/* ISR handlers */

static void asdk_cyt2b75_can0_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_0;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

static void asdk_cyt2b75_can1_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_1;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

static void asdk_cyt2b75_can2_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_2;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

static void asdk_cyt2b75_can3_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_3;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

static void asdk_cyt2b75_can4_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_4;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

static void asdk_cyt2b75_can5_isr(void)
{
    active_interrupt_can_instance = ASDK_CAN_MODULE_CAN_CH_5;
    Cy_CANFD_IrqHandler(can_map[active_interrupt_can_instance].cyt_can_base_address);
    active_interrupt_can_instance = ASDK_CAN_MODULE_NOT_DEFINED;
}

/* SDK callback handlers */

static void __asdk_cyt2b75_can_tx_handler(void)
{
    /* To identify which mailbox has completed transmission
       the driver must indicate the mailbox as callback
       parameter.

       SDK driver doesn't support it yet. So to enable
       such feature the driver must be modified.

       Hence, can_message = NULL */

    if (can_callback != NULL)
    {
        can_callback((uint8_t)active_interrupt_can_instance, ASDK_CAN_TX_COMPLETE_EVENT, &can_tx_buffer);
    }
}

static void __asdk_cyt2b75_can_rx_handler(bool bRxFifoMsg, uint8_t u8MsgBufOrRxFifoNum, cy_stc_canfd_msg_t *pstcCanFDmsg)
{
    if (can_callback != NULL)
    {
        can_rx_buffer.can_id = pstcCanFDmsg->idConfig.identifier;
        can_rx_buffer.dlc = pstcCanFDmsg->dataConfig.dataLengthCode;
        can_rx_buffer.message = (uint8_t *)pstcCanFDmsg->dataConfig.data;

        can_callback((uint8_t)active_interrupt_can_instance, ASDK_CAN_RX_EVENT, &can_rx_buffer);
    }
}

static void __asdk_cyt2b75_can_rxfifo_top_handler(uint8_t u8FifoNum, uint8_t u8BufferSizeInWord, uint32_t *pu32RxBuf)
{
    Cy_CANFD_ExtractMsgFromRXBuffer((cy_stc_canfd_rx_buffer_t *)pu32RxBuf,
                                    &cyt2b75_can_rxfifo_msg);

    if (can_callback != NULL)
    {
        can_rx_buffer.can_id = cyt2b75_can_rxfifo_msg.idConfig.identifier;
        can_rx_buffer.dlc = cyt2b75_can_rxfifo_msg.dataConfig.dataLengthCode;
        can_rx_buffer.message = (uint8_t *)cyt2b75_can_rxfifo_msg.dataConfig.data;

        can_callback((uint8_t)active_interrupt_can_instance, ASDK_CAN_RX_EVENT, &can_rx_buffer);
    }
}

static void __asdk_cyt2b75_can_error_handler(cy_en_canfd_bus_error_t volatile errorcode)
{
    /* Tx error and Rx error events are not reported by SDK */
    asdk_can_event_t error_event = ASDK_CAN_ERROR_EVENT;

    if (errorcode == 5)
    {
        error_event = ASDK_CAN_BUS_OFF_EVENT;
    }

    if (errorcode == 7)
    {
        // Error Passive, for LDU wake-up requirement
        error_event = ASDK_CAN_RX_ERROR_EVENT;
    }

    if (can_callback != NULL)
    {
        can_callback((uint8_t)active_interrupt_can_instance, error_event, NULL);
    }
}

static void __asdk_cyt2b75_can_status_handler(cy_en_canfd_bus_error_status_t enCanFDStatus)
{
}



/*!This function initializes the given CAN channel.*/
asdk_errorcode_t asdk_can_init(uint8_t can_ch, asdk_can_config_t *can_config)
{
    cy_en_canfd_status_t cyt_can_status = CY_CANFD_SUCCESS;
    asdk_errorcode_t can_init_status = ASDK_CAN_SUCCESS;
    cy_stc_canfd_config_t cyt_can_config = {0};

    /* Validate CAN channel */
    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    /* pin mux settings */

    asdk_pinmux_config_t can_pin_mux[] = {
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN, ASDK_PINMUX_CAN_SUBFUN_TX, can_map[can_ch].module_no /*module_no*/), .MCU_pin_num = can_config->mcu_pins.tx_pin, .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG},
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_CAN, ASDK_PINMUX_CAN_SUBFUN_RX, can_map[can_ch].module_no /*module_no*/), .MCU_pin_num = can_config->mcu_pins.rx_pin, .pull_configuration = ASDK_GPIO_PULL_TYPE_HIGHZ},
    };

    can_init_status = asdk_set_pinmux(can_pin_mux, CAN_PINMUX_COUNT);

    if (ASDK_PINMUX_SUCCESS != can_init_status)
    {
        // pin_mux error
        return can_init_status;
    }

    /* clock settings */

    can_clock.ch_no = can_map[can_ch].ch_no;
    can_clock.module_no = can_map[can_ch].module_no;
    can_clock.peripheral_type = ASDK_CAN;
    can_clock.target_frequency = CAN_PEIPHERAL_CLOCK_HZ;

    can_init_status = asdk_clock_enable(&can_clock, NULL);

    if (ASDK_CLOCK_SUCCESS != can_init_status)
    {
        // clock error
        return can_init_status;
    }

    /* can controller settings */

    // handle can mode type
    cyt_can_config.canFDMode = (can_config->controller_settings.mode == ASDK_CAN_MODE_FD);

    // handle max dlc
    can_init_status = __asdk_set_can_max_dlc(can_config->controller_settings.max_dlc, &cyt_can_config);

    if (ASDK_CAN_SUCCESS != can_init_status)
    {
        // ASDK_CAN_ERROR_INVALID_DLC
        return can_init_status;
    }

    // handle baudrate & bit timing segments
    __asdk_set_can_bitrate_config(can_config->controller_settings.mode, can_config->controller_settings.bitrate_config, &cyt_can_config);

    /* hw filter settings */

    can_init_status = __asdk_set_can_hw_filter(can_config->controller_settings.can_id_type, can_config->hw_filter, &cyt_can_config);

    if (ASDK_CAN_SUCCESS != can_init_status)
    {
        return can_init_status;
    }

    // assign callbacks
    cyt_can_config.txCallback = __asdk_cyt2b75_can_tx_handler;
    cyt_can_config.rxCallback = __asdk_cyt2b75_can_rx_handler;
    cyt_can_config.rxFifoWithTopCallback = __asdk_cyt2b75_can_rxfifo_top_handler;
    cyt_can_config.errorCallback = __asdk_cyt2b75_can_error_handler;
    cyt_can_config.statusCallback = __asdk_cyt2b75_can_status_handler; // Un-supported now

    /* initialize interrupt */

    if (can_config->controller_settings.interrupt_config.use_interrupt)
    {
        /* CAN interrupt configuration */
        irq_cfg.sysIntSrc = can_isr_src[can_ch]; /* Use interrupt LINE0 */
        irq_cfg.intIdx = CPUIntIdx3_IRQn;
        irq_cfg.isEnabled = true;

        /* Initialize CAN interrupt */
        Cy_SysInt_InitIRQ(&irq_cfg);
        Cy_SysInt_SetSystemIrqVector(can_isr_src[can_ch], can_isr[can_ch]);
        NVIC_SetPriority(CPUIntIdx3_IRQn, can_config->controller_settings.interrupt_config.priority);
        NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
        NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    }

    /* unused settings */

    // do not use ext id
    cyt_can_config.extidFilterConfig.numberOfEXTIDFilters = 0;

    // do not use transceiver delay compensation
    cyt_can_config.tdcConfig.tdcEnabled = false;
    cyt_can_config.tdcConfig.tdcOffset = 0;
    cyt_can_config.tdcConfig.tdcFilterWindow = 0;

    /* initialize CAN */

    cyt_can_status = Cy_CANFD_Init(can_map[can_ch].cyt_can_base_address, &cyt_can_config);

    if (CY_CANFD_SUCCESS != cyt_can_status)
    {
        return ASDK_CAN_ERROR_INIT_FAILED;
    }

    // ASDK_CAN_SUCCESS
    return can_init_status;
}


/*!This function de-initializes the given CAN channel*/
asdk_errorcode_t asdk_can_deinit(uint8_t can_ch)
{
    asdk_errorcode_t can_deinit_status = ASDK_CAN_SUCCESS;
    cy_en_canfd_status_t cy_can_status = CY_CANFD_SUCCESS;

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    cy_can_status = Cy_CANFD_DeInit(can_map[can_ch].cyt_can_base_address);

    if (cy_can_status != CY_CANFD_SUCCESS)
    {
        can_deinit_status = ASDK_CAN_ERROR_DEINIT_FAILED;
    }

    return can_deinit_status;
}

/*!This function registers the CAN service level event handler callback.*/
asdk_errorcode_t asdk_can_install_callback(asdk_can_callback_t callback)
{
    if (NULL == callback)
    {
        return ASDK_CAN_ERROR_NULL_PTR;
    }
    else
    {
        can_callback = callback;
    }

    return ASDK_CAN_SUCCESS;
}

/*!This function puts the CAN module to sleep mode*/
asdk_errorcode_t asdk_can_sleep(uint8_t can_ch)
{
    return ASDK_CAN_ERROR_HW_FEATURE_NOT_SUPPORTED;
}

/*! This function checks whether the given mailbox for transmission is
  busy or not.*/
asdk_errorcode_t asdk_can_is_tx_busy(uint8_t can_ch, uint8_t virtual_mailbox_no, bool *status)
{
    *status = false;

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    *status = (can_map[can_ch].cyt_can_base_address->M_TTCAN.unTXBRP.u32Register >> virtual_mailbox_no) & 1;

    return ASDK_CAN_SUCCESS;
}

/*!  This function fills the given message in a mailbox which is configured
  for transmission.*/
asdk_errorcode_t asdk_can_write(uint8_t can_ch, uint8_t virtual_mailbox_no, asdk_can_message_t *can_message)
{
    asdk_errorcode_t can_write_status = ASDK_CAN_SUCCESS;
    cy_en_canfd_status_t cy_can_status = CY_CANFD_SUCCESS;
    cy_stc_canfd_msg_t stcMsg = {0};

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    stcMsg.canFDFormat = false;
    stcMsg.idConfig.extended = false;
    stcMsg.idConfig.identifier = can_message->can_id;
    stcMsg.dataConfig.dataLengthCode = can_message->dlc;
    memcpy(stcMsg.dataConfig.data, can_message->message, stcMsg.dataConfig.dataLengthCode);

    cy_can_status = Cy_CANFD_UpdateAndTransmitMsgBuffer(can_map[can_ch].cyt_can_base_address, virtual_mailbox_no, &stcMsg);
    if (cy_can_status != CY_CANFD_SUCCESS)
    {
        can_write_status = ASDK_CAN_ERROR_WRITE_FAILED;
    }

    can_tx_buffer.can_id = can_message->can_id;
    can_tx_buffer.dlc = can_message->dlc;
    can_tx_buffer.message = can_message->message;
    // memcpy(&can_tx_buffer.message, can_message->message, can_message->dlc);

    return can_write_status;
}

/*!  This function fills the given message buffer if a new message is available
  in the given mailbox.*/
asdk_errorcode_t asdk_can_read(uint8_t can_ch, uint8_t virtual_mailbox_no, asdk_can_message_t *can_message)
{
    /* to do: implement for polling purpose only, must be called by can_service_iteration from can_services.c file */

    return ASDK_CAN_ERROR_READ_FAILED;
}

/* static functions ************************** */
