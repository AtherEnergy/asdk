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
    can_service_q.h

    @path
    /platform/asdk/apis/can_service_q.h

    @Created on
    Nov 19, 2019

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief

*/

#ifndef ASDK_APIS_CAN_SERVICE_Q_H_
#define ASDK_APIS_CAN_SERVICE_Q_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdio.h>

#include "asdk_system.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define MAX_CAN_DATA_LENGTH  8

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
#define DISABLE_IRQ_SAVE() {asdk_sys_disable_interrupts(); }
#define ENABLE_IRQ_RESTORE() {asdk_sys_enable_interrupts(); }

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/


/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

typedef struct __can_msg {
	uint32_t can_id;
	uint8_t dlc;
	bool extended_id;
	uint8_t data[MAX_CAN_DATA_LENGTH];
} can_msg_t;

typedef struct __can_data {
	can_msg_t msg; //define in
	linked_list_item_t item;
} can_data_t;

static inline can_data_t *can_get_can_data_from_q(linked_list_t *list)
{
	linked_list_item_t *item;

	if (!list)
		return NULL;

	DISABLE_IRQ_SAVE();
	item = linked_list_peek(list);

	if (item != NULL)
		linked_list_pop(list);

	ENABLE_IRQ_RESTORE();

	if (item == NULL)
		return NULL;
	else
		return (can_data_t *)(item->priv);
}

static inline void can_put_can_data_into_q(linked_list_t *list, can_data_t *can_data)
{
	linked_list_item_t *item;

	if (!list || !can_data)
		return;

	item = &(can_data->item);
	DISABLE_IRQ_SAVE();
	linked_list_push(list, item);
	ENABLE_IRQ_RESTORE();
	can_data = NULL;
}

static inline bool task_can_is_q_empty(linked_list_t *list)
{
	linked_list_item_t *item;

	DISABLE_IRQ_SAVE();
	item = linked_list_peek(list);
	ENABLE_IRQ_RESTORE();

	if (item == NULL)
		return true;

	return false;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ASDK_APIS_CAN_SERVICE_Q_H_ */
