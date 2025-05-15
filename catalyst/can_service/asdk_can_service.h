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
    asdk_can_service.h

    @path
    inc/asdk_can_service.h

    @Created on
    Oct 17, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the CAN_SERVICE DAL module of asdk ( Ather SDK )
*/

#ifndef ASDK_CAN_SERVICE_H
#define ASDK_CAN_SERVICE_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

#include <stdint.h>
#include <stdbool.h>

/* asdk includes ***************************** */

#include "asdk_error.h"
#include "linked_list.h"

/* dal includes ****************************** */

#include "asdk_can.h"

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

typedef enum
{
    ASDK_CAN_SERVICE_TX_Q = 0,
    ASDK_CAN_SERVICE_RX_Q = 1,
} asdk_can_service_q_t;

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

asdk_errorcode_t asdk_can_service_init(uint8_t can_ch, asdk_can_config_t can_config);
asdk_errorcode_t asdk_can_service_install_callback(asdk_can_callback_t user_callback);
asdk_errorcode_t asdk_can_service_send(uint8_t can_ch, asdk_can_message_t *msg);
asdk_errorcode_t asdk_can_service_send_iteration(uint8_t can_ch);
asdk_errorcode_t asdk_can_service_receive_iteration(uint8_t can_ch);
asdk_errorcode_t asdk_can_service_get_q_address(uint8_t can_ch, asdk_can_service_q_t tx_q_type, linked_list_t **reserve_q, linked_list_t **pending_q);

#endif /* ASDK_CAN_SERVICE_H */
