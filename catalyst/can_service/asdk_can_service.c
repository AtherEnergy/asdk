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
    asdk_can_service.c

    @path
    platform/cyt2b75/dal/src/asdk_can_service.c

    @Created on
    Oct 17, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the CAN_SERVICE module for Ather SDK (asdk)

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <string.h>

/* asdk includes ***************************** */

#include "asdk_error.h"

/* middleware includes *********************** */

#include "asdk_can_service.h"
#include "can_service_q.h"

/* dal includes ****************************** */

#include "asdk_can.h"
#include "asdk_platform.h"

/* sdk includes ****************************** */

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define MAX_POOL_SIZE 40
#define MAX_STD_CANID 0x7FF

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

static can_data_t __can_tx_pool[ASDK_CAN_MODULE_CAN_CH_MAX][MAX_POOL_SIZE];
static can_data_t __can_rx_pool[ASDK_CAN_MODULE_CAN_CH_MAX][MAX_POOL_SIZE];

static linked_list_t can_tx_reserve_q[ASDK_CAN_MODULE_CAN_CH_MAX];
static linked_list_t can_tx_pending_q[ASDK_CAN_MODULE_CAN_CH_MAX];
static linked_list_t can_rx_reserve_q[ASDK_CAN_MODULE_CAN_CH_MAX];
static linked_list_t can_rx_pending_q[ASDK_CAN_MODULE_CAN_CH_MAX];

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

static asdk_can_callback_t service_user_callback = NULL;

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/* static functions ************************** */

void __asdk_can_service_callback_handler(uint8_t can_ch, asdk_can_event_t event, asdk_can_message_t *message)
{
    can_data_t *can_data;

    switch (event)
    {
    /* service rx event */
    case ASDK_CAN_RX_EVENT:
        can_data = can_get_can_data_from_q(&can_rx_reserve_q[can_ch]);
        if (NULL != can_data)
        {
            can_data->msg.can_id = message->can_id;
            can_data->msg.dlc = message->dlc;
            can_data->msg.extended_id = (message->can_id > MAX_STD_CANID);
            memcpy(can_data->msg.data, message->message, can_data->msg.dlc);
            can_put_can_data_into_q(&can_rx_pending_q[can_ch], can_data);
        }
        break;

    // propogate transmit complete event
    case ASDK_CAN_TX_COMPLETE_EVENT:
        if (NULL != service_user_callback)
        {
            service_user_callback(can_ch, event, message);
        }
        break;

    // propogate error events to application
    case ASDK_CAN_TX_ERROR_EVENT:
    case ASDK_CAN_RX_ERROR_EVENT:
    case ASDK_CAN_ERROR_EVENT:
    case ASDK_CAN_BUS_OFF_EVENT:
    case ASDK_CAN_SLEEP_EVENT:
    case ASDK_CAN_WAKE_UP_EVENT:
        if (NULL != service_user_callback)
        {
            service_user_callback(can_ch, event, NULL);
        }
        break;

    default:
        break;
    }
}

asdk_errorcode_t asdk_can_service_init(uint8_t can_ch, asdk_can_config_t can_config)
{
    asdk_errorcode_t service_init_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    /* initialize CAN driver */

    service_init_status = asdk_can_init(can_ch, &can_config);

    if (service_init_status != ASDK_CAN_SUCCESS)
    {
        return service_init_status;
    }

    /* initialize circular buffers for a given channel */

    linked_list_init(&can_tx_reserve_q[can_ch]);
    linked_list_init(&can_tx_pending_q[can_ch]);
    linked_list_init(&can_rx_pending_q[can_ch]);
    linked_list_init(&can_rx_reserve_q[can_ch]);

    for (uint8_t i = 0; i < MAX_POOL_SIZE; i++)
    {
        __can_tx_pool[can_ch][i].item.priv = (void *)&(__can_tx_pool[can_ch][i]);
        linked_list_push(&can_tx_reserve_q[can_ch], &(__can_tx_pool[can_ch][i].item));

        __can_rx_pool[can_ch][i].item.priv = (void *)&(__can_rx_pool[can_ch][i]);
        linked_list_push(&can_rx_reserve_q[can_ch], &(__can_rx_pool[can_ch][i].item));
    }

    return service_init_status;
}

asdk_errorcode_t asdk_can_service_install_callback(asdk_can_callback_t user_callback)
{
    asdk_errorcode_t assign_cb_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    /* assign CAN service user callback handler */

    if (NULL != user_callback)
    {
        service_user_callback = user_callback;
    }
    else
    {
        return ASDK_MW_CAN_SERVICE_ERROR_NULL_PTR;
    }

    /* assign service callback handler */

    // the callback is made from interrupt context if interrupt is enabled
    // otherwise it is made from iteration context if interrupt is disabled
    assign_cb_status = asdk_can_install_callback(&__asdk_can_service_callback_handler);

    if (assign_cb_status != ASDK_CAN_SUCCESS)
    {
        return assign_cb_status;
    }

    return assign_cb_status;
}

asdk_errorcode_t asdk_can_service_send(uint8_t can_ch, asdk_can_message_t *msg)
{
    asdk_errorcode_t service_send_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    can_data_t *can_data;

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    /* get free packet from reserve queue */

    can_data = can_get_can_data_from_q(&can_tx_reserve_q[can_ch]);

    if (can_data == NULL)
    {
        return ASDK_MW_CAN_SERVICE_INVALID_CAN_DATA;
    }

    /* push packet to transmit pending queue */

    can_data->msg.can_id = msg->can_id;
    can_data->msg.dlc = msg->dlc;
    can_data->msg.extended_id = (msg->can_id > MAX_STD_CANID);
    memcpy(can_data->msg.data, msg->message, can_data->msg.dlc);

    can_put_can_data_into_q(&can_tx_pending_q[can_ch], can_data);

    return service_send_status;
}

asdk_errorcode_t asdk_can_service_send_iteration(uint8_t can_ch)
{
    asdk_errorcode_t service_send_iteration_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    bool tx_status_busy = false;
    can_data_t *can_data = NULL;
    asdk_can_message_t tx_msg = {0};

    service_send_iteration_status = asdk_can_is_tx_busy(can_ch, 0, &tx_status_busy);

    if (ASDK_CAN_SUCCESS != service_send_iteration_status)
    {
        return service_send_iteration_status;
    }
    else if (tx_status_busy)
    {
        return ASDK_MW_CAN_SERVICE_TX_BUSY;
    }
    else
    {
        /* get data from transmit pending queue and send it on can bus */

        can_data = can_get_can_data_from_q(&can_tx_pending_q[can_ch]);

        if (can_data == NULL)
        {
            return ASDK_MW_CAN_SERVICE_INVALID_CAN_DATA;
        }

        tx_msg.can_id = can_data->msg.can_id;
        tx_msg.dlc = can_data->msg.dlc;
        tx_msg.message = can_data->msg.data;

        service_send_iteration_status = asdk_can_write(can_ch, 0, &tx_msg);

        // if (ASDK_IS_SUCCESS(ret_status))
        // {
        //     can_traffic.can_tx_loss++;
        // }
        // else
        // {
        //     can_traffic.can_tx_frequency++;
        // }

        /* clean up the CAN Packet */

        memset(&(can_data->msg), 0, sizeof(can_msg_t));

        /* push packet to transmit reserve queue */

        can_put_can_data_into_q(&can_tx_reserve_q[can_ch], can_data);
    }

    return service_send_iteration_status;
}

asdk_errorcode_t asdk_can_service_receive_iteration(uint8_t can_ch)
{
    asdk_errorcode_t service_receive_iteration_status = ASDK_MW_CAN_SERVICE_SUCCESS;
    can_data_t *can_data = NULL;
    asdk_can_message_t rx_msg;

    if (ASDK_CAN_MODULE_CAN_CH_MAX <= can_ch)
    {
        return ASDK_CAN_ERROR_INVALID_CHANNEL;
    }

    /* get data from receive pending queue */

    can_data = can_get_can_data_from_q(&can_rx_pending_q[can_ch]);

    if (can_data == NULL)
    {
        return ASDK_MW_CAN_SERVICE_INVALID_CAN_DATA;
    }

    /* callback to user with received message */

    rx_msg.can_id = can_data->msg.can_id;
    rx_msg.dlc = can_data->msg.dlc;
    rx_msg.message = can_data->msg.data;

    if (NULL != service_user_callback)
    {
        service_user_callback(can_ch, ASDK_CAN_RX_EVENT, &rx_msg);
    }

    /* clean up the CAN Packet */

    memset(&(can_data->msg), 0, sizeof(can_msg_t));
    can_put_can_data_into_q(&can_rx_reserve_q[can_ch], can_data);

    return service_receive_iteration_status;
}

asdk_errorcode_t asdk_can_service_get_q_address(uint8_t can_ch, asdk_can_service_q_t tx_q_type, linked_list_t **reserve_q, linked_list_t **pending_q)
{
    asdk_errorcode_t service_get_q_status = ASDK_MW_CAN_SERVICE_SUCCESS;

    return service_get_q_status;
}
