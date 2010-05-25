/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#include "receiver_buffer.h"
#include "../structures/netemu_list.h"
#include "netlib_error.h"
#include "../protocol/application.h"

struct _netemu_receiver_buffer_wakeup_info {
	netemu_event eventhandle;
	time_t age;
	struct netemu_receiver_buffer_item* item;
	struct _netemu_receiver_buffer_wakeup_info *next;
	struct _netemu_receiver_buffer_wakeup_info *prev;
};

struct _netemu_receiver_buffer_notify_info {
	bufferListenerFn fn;
	void* arg;
	struct _netemu_receiver_buffer_notify_info *next;
	struct _netemu_receiver_buffer_notify_info *prev;
};
/**
 * This is the internal struct for the netmeu_receiver_buffer module
 * @ingroup netemu_receiver_buffer
 */
struct _netemu_receiver_buffer_internal {
	struct netemu_list* instructions_to_add;
	netemu_mutex add_mutex;
	netemu_mutex wakeup_mutex;
	netemu_mutex fn_mutex;
	NETEMU_HASHTBL *registered_wakeups;
	NETEMU_HASHTBL *registered_fns;
	netemu_event event;
};


void _netemu_receiver_buffer_internal_add(struct netemu_receiver_buffer *buffer, struct application_instruction* instruction_id);
void _netemu_receiver_buffer_perform_notify(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item);
void _netemu_receiver_buffer_perform_wakeup(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item);
void _netemu_receiver_buffer_update(void* args);
struct _netemu_receiver_buffer_wakeup_info* _netemu_receiver_buffer_register_wakeup_on_instruction(struct netemu_receiver_buffer *buffer, int instruction_id, time_t age, netemu_event eventhandle);

/**
 * Create a new instance of the receiver buffer module
 * @ingroup netemu_receiver_buffer
 * @param hash_size size the size of the hash table which will be used internally.
 * @return a netemu_receiver_buffer instance or NULL if something went wrong. Call netlib_get_last_error for more information
 * of the occurred error.
 */
struct netemu_receiver_buffer *netemu_receiver_buffer_create(hash_size size) {
	struct netemu_receiver_buffer *buffer;
	buffer = malloc(sizeof(struct netemu_receiver_buffer));
	if(buffer == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	buffer->_internal = malloc(sizeof(struct _netemu_receiver_buffer_internal));
	buffer->_internal->registered_wakeups = netemu_hashtbl_create(23, def_hashfunc_char, comparator_char);
	buffer->_internal->registered_fns = netemu_hashtbl_create(23, def_hashfunc_char, comparator_char);
	buffer->_internal->add_mutex = netemu_thread_mutex_create();
	buffer->_internal->wakeup_mutex = netemu_thread_mutex_create();
	buffer->_internal->fn_mutex = netemu_thread_mutex_create();
	buffer->_internal->instructions_to_add = netemu_list_create(20, TRUE);
	buffer->_internal->event = netemu_thread_event_create();
	netemu_thread_new(_netemu_receiver_buffer_update,buffer);
	return buffer;
}

/**
 * Add a received instruction to the buffer.
 * @param buffer an instance of the netemu_receiver_buffer module.
 * @param instruction
 * @return 0 if everything went fine, -1 if an error occurred. Call netlib_get_last_error for more information
 * of the occurred error.
 * The following errors can occur:
 * - NETEMU_EINVAL one of the arguments was invalid. Check that the pointers point to the right thing.
 * - NETEMU_ENOTENOUGHMEMORY The memory is out. You're screwed =)
 * - Any error related to mutex locks can also occur.
 */
int netemu_receiver_buffer_add(struct netemu_receiver_buffer *buffer, struct application_instruction *instruction,
		netemu_connection_types type,  union netemu_connection_type connection, netlib_sockaddr* addr, int addr_len) {
	struct netemu_receiver_buffer_item *item;
	/* If there are any nullpointers, we might as well stop right here. */
	if(buffer == NULL || instruction == NULL) {
		netlib_set_last_error(NETLIB_EINVAL);
		return -1;
	}
	item = malloc(sizeof(struct netemu_receiver_buffer_item));
	if(item == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	item->instruction = instruction;
	item->connection = connection;
	item->addr = addr;
	item->addr_size = addr_len;
	item->type = type;

	netemu_thread_mutex_lock(buffer->_internal->add_mutex, NETEMU_INFINITE);
	netemu_list_add(buffer->_internal->instructions_to_add, item);
	netemu_thread_event_signal(buffer->_internal->event);
	netemu_thread_mutex_release(buffer->_internal->add_mutex);
	return 0;
}

/**
 * Adds a listener function that listens for a particular instruction.
 * @ingroup netemu_receiver_buffer
 * @param buffer an instance of the netemu_receiver_buffer module.
 * @param instruction_id the instruction id.
 * @param fn the listener function that will be called when the instruction is received.
 * @param arg a user provided argument. Set to NULL if you're not interested.
 * @return 0 if everything went well, -1 on failure.
 * The following errors can occur:
 * - NETEMU_EINVAL if one of the arguments was invalid. Check that the pointers point to the right thing.
 * - NETEMU_ENOTENOUGHMEMORY if the memory is out. You're screwed =)
 * - Any error related to mutex locks can also occur.
 */
int netemu_receiver_buffer_add_instruction_received_fn(struct netemu_receiver_buffer *buffer, char instruction_id, bufferListenerFn fn, void* arg) {
	struct _netemu_receiver_buffer_notify_info *info, *existing_info;
	info = malloc(sizeof(struct _netemu_receiver_buffer_notify_info));
	if(info == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	info->fn = fn;
	info->arg = arg;
	info->next = NULL;
	netemu_thread_mutex_lock(buffer->_internal->fn_mutex,NETEMU_INFINITE);
	if((existing_info = netemu_hashtbl_get(buffer->_internal->registered_fns, &instruction_id, sizeof(char))) == NULL) {
		info->prev = NULL;
		netemu_hashtbl_insert(buffer->_internal->registered_fns, &instruction_id, sizeof(char), info);
		info->next = NULL;
	}
	else {
		while(existing_info->next != NULL) {
			existing_info = existing_info->next;
		}
		existing_info->next = info;
		info->prev = existing_info;
	}
	netemu_thread_mutex_release(buffer->_internal->fn_mutex);
	return 0;
}

/**
 * Primary function for the worker thread. This function goes through instructions that have come in,
 * notifies functions and wakeups sleeping threads that waits for the instruction
 * @ingroup netemu_receiver_buffer
 * @param args a user defined argument. This argument is always the instance of the netemu_receiver_buffer
 * since this is just an internal function.
 */
void _netemu_receiver_buffer_update(void* args) {
	struct netemu_receiver_buffer *buffer;
	struct netemu_list* itemsToAdd;
	struct netemu_list* itemsToNotify;
	netemu_mutex lock;
	int i;
	buffer = (struct netemu_receiver_buffer*)args;
	itemsToAdd = buffer->_internal->instructions_to_add;
	itemsToNotify = netemu_list_create(10, TRUE);
	lock = buffer->_internal->add_mutex;

	while(1) {
		if(itemsToAdd->count == 0) {
			netemu_thread_event_wait(buffer->_internal->event, NETEMU_INFINITE);
		}
		if(itemsToAdd->count > 0) {
			netemu_thread_mutex_lock(lock,NETEMU_INFINITE);
			/* We copy the contents of the array here in order to not lock the list longer than necessary. */
			for (i = 0; i < itemsToAdd->count; i++) {
				netemu_list_add(itemsToNotify, itemsToAdd->elements[i]);
			}
			netemu_list_clear(itemsToAdd);
			netemu_thread_mutex_release(lock);
			/* Then we use the newly created list to notify. */
			for(i = 0; i < itemsToNotify->count; i++) {
				/* wakeup and notify */
				_netemu_receiver_buffer_perform_notify(buffer,itemsToNotify->elements[i]);
				_netemu_receiver_buffer_perform_wakeup(buffer,itemsToNotify->elements[i]);
				/* TODO After we have fixed proper memory management, we need to clear out instructions here. */
			}
			/* Finally clear everything out. */
			netemu_list_clear(itemsToNotify);
		}
	}
}

/**
 * Wait for an instruction to come in. This is a blocking call that will wait for the buffer
 * to receive an instruction with the specified id. When such an instruction is received,
 * this function will return the instruction.
 * @todo Add a timeout parameter so that this function doesn't wait forever
 * @todo Make sure that the results are copied to the receiver, so that memory can be freed internally.
 * @todo fix error handling here.
 * @ingroup netemu_receiver_buffer
 * @param buffer an instance of the netemu_receiver_buffer module.
 * @param instruction_id the id of the instruction to wait for. Make sure that the id is a valid one or you will surely wait forever.
 * @param timestamp the timestamp when you started waiting. This will discard any instructions received before this timestamp.
 * @return a receiver_buffer_item containing the instruction, and where it came from if everything went all right, or NULL if something went wrong.
 */
struct netemu_receiver_buffer_item* netemu_receiver_buffer_wait_for_instruction(struct netemu_receiver_buffer* buffer, int instruction_id, time_t timestamp) {
	netemu_event eventhandle;
	struct _netemu_receiver_buffer_wakeup_info* info;
	struct netemu_receiver_buffer_item* item;

	eventhandle = netemu_thread_event_create();
	info = _netemu_receiver_buffer_register_wakeup_on_instruction(buffer, instruction_id, timestamp, eventhandle);
	netemu_thread_event_wait(eventhandle, NETEMU_INFINITE);
	netemu_thread_event_destroy(eventhandle);
	item = info->item;
	free(info);
	return item;
}
/**
 * Banana, banana, banana
 */
struct _netemu_receiver_buffer_wakeup_info* _netemu_receiver_buffer_register_wakeup_on_instruction(struct netemu_receiver_buffer *buffer, int instruction_id, time_t age, netemu_event eventhandle) {
	struct _netemu_receiver_buffer_wakeup_info *wakeup, *existing_wakeup;
	wakeup = malloc(sizeof(struct _netemu_receiver_buffer_wakeup_info));
	wakeup->age = age;
	wakeup->eventhandle = eventhandle;
	wakeup->next = NULL;
	netemu_thread_mutex_lock(buffer->_internal->wakeup_mutex, NETEMU_INFINITE);
	if((existing_wakeup = netemu_hashtbl_get(buffer->_internal->registered_wakeups, &instruction_id, sizeof(char))) == NULL) {
		wakeup->prev = NULL;
		wakeup->next = NULL;
		netemu_hashtbl_insert(buffer->_internal->registered_wakeups, &instruction_id, sizeof(char), wakeup);
	}
	else {
		while(existing_wakeup->next != NULL) {
			existing_wakeup = existing_wakeup->next;
		}
		wakeup->prev = existing_wakeup;
		wakeup->next = NULL;
		existing_wakeup->next = wakeup;
	}
	netemu_thread_mutex_release(buffer->_internal->wakeup_mutex);
	return wakeup;
}
/**
 * Banana, banana, banana
 */
void _netemu_receiver_buffer_perform_wakeup(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item) {
	struct _netemu_receiver_buffer_wakeup_info *wakeup, *nextwakeup;
	netemu_thread_mutex_lock(buffer->_internal->wakeup_mutex, NETEMU_INFINITE);
	if((wakeup = netemu_hashtbl_get(buffer->_internal->registered_wakeups, &item->instruction->id, sizeof(char))) != NULL) {
		while(wakeup != NULL) {
			if(wakeup->age <= item->instruction->timestamp) {
				wakeup->item = item;
				if(wakeup->prev != NULL)
					wakeup->prev->next = wakeup->next;
				if(wakeup->next != NULL) {
					wakeup->next->prev = wakeup->prev;
					nextwakeup = wakeup->next;
				}
				else {
					nextwakeup = NULL;
				}

				if(wakeup->next == NULL && wakeup->prev == NULL) {
					/* We remove the node completely, we don't want a bunch of null pointers in the hash table. */
					netemu_hashtbl_remove(buffer->_internal->registered_wakeups,&item->instruction->id,sizeof(char));
				}

				netemu_thread_event_signal(wakeup->eventhandle);
				wakeup = nextwakeup;
			}
			else {
				wakeup = wakeup->next;
			}
		}

		wakeup = NULL;
		/* Moved the above line to the loop above, since we just want to remove the entire linked list from the hashtable
		 * when the last wakeupinfo is removed from the linked list. */
		/* We remove the node completely, we don't want a bunch of null pointers in the hash table. */
		/*netemu_hashtbl_remove(buffer->_internal->registered_wakeups,&instruction->id,sizeof(char)); */
	}
	netemu_thread_mutex_release(buffer->_internal->wakeup_mutex);
}

/**
 * Banana, banana, banana
 */
void _netemu_receiver_buffer_perform_notify(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item) {
	struct _netemu_receiver_buffer_notify_info *notify, *nextnotify;
	netemu_thread_mutex_lock(buffer->_internal->fn_mutex, NETEMU_INFINITE);
	if((notify = netemu_hashtbl_get(buffer->_internal->registered_fns, &item->instruction->id, sizeof(char))) != NULL) {
		while(notify != NULL) {
				nextnotify = notify->next;
				notify->fn(buffer,item,notify->arg);
				notify = nextnotify;
		}
	}
	netemu_thread_mutex_release(buffer->_internal->fn_mutex);
}
