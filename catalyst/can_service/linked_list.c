#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "linked_list.h"

//////////////////////////////////////////////////////////Public Functions
void linked_list_init(linked_list_t *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
	//mutex_init
}

void linked_list_deinit(linked_list_t *list)
{
	if (list) {
		//mutex_deinit
		list->head = NULL;
		list->tail = NULL;
	}
}

void linked_list_push(linked_list_t *list, linked_list_item_t *item)
{
	if (list && item) {
		item->next = NULL;

		//Mutex_lock
		if (list->head == NULL) {
			// first item being added
			list->head = item;
			list->tail = item;

		} else {
			// there is already an item
			list->tail->next = item;
			list->tail = list->tail->next;
		}

		list->count++;
		//mutex_unlock
	}
}

linked_list_item_t *linked_list_peek(linked_list_t *list)
{
	linked_list_item_t *temp;

	if (!list) {
		//printf("WARN: Invalid arguments.\n");
		return NULL;
	}

	//mutex_lock
	temp = list->head;
	//mutex_unlock

	return temp;
}

void linked_list_pop(linked_list_t *list)
{
	if (!list) {
		//printf("WARN: Invalid arguments.\n");
		return;
	}

	//mutex_lock
	if (list->head != NULL) {
		list->head = list->head->next;
		if (list->count != 0)
			list->count--;

		if (list->count == 0) {
			// the last element was popped
			list->tail = NULL;
		}
	}

	//muex_unlock
}
