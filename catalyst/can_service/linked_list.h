#ifndef UTILS_QUEUE_API_LINKED_LIST_H_
#define UTILS_QUEUE_API_LINKED_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct __linked_list_item {
	void *priv;
	struct __linked_list_item *next;
} linked_list_item_t;

typedef struct __linked_list_data {
	linked_list_item_t *head;
	linked_list_item_t *tail;
	uint16_t count;
	//mutex;
} linked_list_t;

void linked_list_init(linked_list_t *list);
void linked_list_deinit(linked_list_t *list);
void linked_list_push(linked_list_t *list, linked_list_item_t *item);
linked_list_item_t *linked_list_peek(linked_list_t *list);
void linked_list_pop(linked_list_t *list);



#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UTILS_QUEUE_API_LINKED_LIST_H_ */
