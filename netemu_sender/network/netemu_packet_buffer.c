#include "netemu_packet_buffer.h"
#include "netemu_list.h"

struct _netemu_packet_buffer_wakeup_info {
	netemu_event eventhandle;
	time_t age;
	struct netemu_packet_buffer_item* item;
	struct _netemu_packet_buffer_wakeup_info *next;
	struct _netemu_packet_buffer_wakeup_info *prev;
	int FOO;
};

struct _netemu_packet_buffer_notify_info {
	bufferListenerFn fn;
	void* arg;
	struct _netemu_packet_buffer_notify_info *next;
	struct _netemu_packet_buffer_notify_info *prev;
};

struct _netemu_packet_buffer_internal {
	struct netemu_list* instructions_to_add;
	netemu_mutex add_mutex;
	netemu_mutex wakeup_mutex;
	netemu_mutex fn_mutex;
	NETEMU_HASHTBL *registered_wakeups;
	NETEMU_HASHTBL *registered_fns;
	netemu_event event;
};


void _netemu_packet_buffer_internal_add(struct netemu_packet_buffer *buffer, struct application_instruction* instruction);
void _netemu_packet_buffer_perform_notify(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item);
void _netemu_packet_buffer_perform_wakeup(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item);
void _netemu_packet_buffer_update(void* args);
struct _netemu_packet_buffer_wakeup_info* _netemu_packet_buffer_register_wakeup_on_instruction(struct netemu_packet_buffer *buffer, int instruction_id, time_t age, netemu_event eventhandle);

struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size) {
	struct netemu_packet_buffer *buffer;

	buffer = malloc(sizeof(struct netemu_packet_buffer));
	buffer->_internal = malloc(sizeof(struct _netemu_packet_buffer_internal));
	buffer->_internal->registered_wakeups = netemu_hashtbl_create(23, def_hashfunc_char, comparator_char);
	buffer->_internal->registered_fns = netemu_hashtbl_create(23, def_hashfunc_char, comparator_char);
	buffer->_internal->add_mutex = netemu_thread_mutex_create();
	buffer->_internal->wakeup_mutex = netemu_thread_mutex_create();
	buffer->_internal->fn_mutex = netemu_thread_mutex_create();
	buffer->_internal->instructions_to_add = netemu_list_new(20, TRUE);
	buffer->_internal->event = netemu_thread_event_create();
	netemu_thread_new(_netemu_packet_buffer_update,buffer);
	return buffer;
}


void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, netemu_connection_types type,  union netemu_connection_type connection) {
	struct netemu_packet_buffer_item *item;
	item = malloc(sizeof(struct netemu_packet_buffer_item));
	item->instruction = instruction;
	item->connection = connection;
	item->type = type;
	netemu_thread_mutex_lock(buffer->_internal->add_mutex, NETEMU_INFINITE);
	netemu_list_add(buffer->_internal->instructions_to_add, item);
	netemu_thread_event_signal(buffer->_internal->event);
	netemu_thread_mutex_release(buffer->_internal->add_mutex);
}

void netemu_packet_buffer_add_instruction_received_fn(struct netemu_packet_buffer *buffer, char instruction, bufferListenerFn fn, void* arg) {
	struct _netemu_packet_buffer_notify_info *info, *existing_info;
	info = malloc(sizeof(struct _netemu_packet_buffer_notify_info));
	info->fn = fn;
	info->arg = arg;
	info->next = NULL;
	netemu_thread_mutex_lock(buffer->_internal->fn_mutex,NETEMU_INFINITE);
	if((existing_info = netemu_hashtbl_get(buffer->_internal->registered_fns, &instruction, sizeof(char))) == NULL) {
		info->prev = NULL;
		netemu_hashtbl_insert(buffer->_internal->registered_fns, &instruction, sizeof(char), info);
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
}

void _netemu_packet_buffer_update(void* args) {
	struct netemu_packet_buffer *buffer;
	struct netemu_list* itemsToAdd;
	struct netemu_list* itemsToNotify;
	netemu_mutex lock;
	int i;
	buffer = (struct netemu_packet_buffer*)args;
	itemsToAdd = buffer->_internal->instructions_to_add;
	itemsToNotify = netemu_list_new(10, TRUE);
	lock = buffer->_internal->add_mutex;

	while(1) {
		if(itemsToAdd->count == 0) {
			netemu_thread_event_wait(buffer->_internal->event);
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
				_netemu_packet_buffer_perform_notify(buffer,itemsToNotify->elements[i]);
				_netemu_packet_buffer_perform_wakeup(buffer,itemsToNotify->elements[i]);
				 //free(itemsToNotify->elements[i]);
			}
			/* Finally clear everything out. */
			netemu_list_clear(itemsToNotify);
		}
	}
}

struct netemu_packet_buffer_item* netemu_packet_buffer_wait_for_instruction(struct netemu_packet_buffer* buffer, int instruction_id, time_t timestamp) {
	netemu_event eventhandle;
	struct _netemu_packet_buffer_wakeup_info* info;
	struct netemu_packet_buffer_item* item;

	eventhandle = netemu_thread_event_create();
	info = _netemu_packet_buffer_register_wakeup_on_instruction(buffer, instruction_id, timestamp, eventhandle);
	netemu_thread_event_wait(eventhandle);
	netemu_thread_event_destroy(eventhandle);
	item = info->item;
	free(info);
	return item;
}

struct _netemu_packet_buffer_wakeup_info* _netemu_packet_buffer_register_wakeup_on_instruction(struct netemu_packet_buffer *buffer, int instruction_id, time_t age, netemu_event eventhandle) {
	struct _netemu_packet_buffer_wakeup_info *wakeup, *existing_wakeup;
	wakeup = malloc(sizeof(struct _netemu_packet_buffer_wakeup_info));
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

void _netemu_packet_buffer_perform_wakeup(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item) {
	struct _netemu_packet_buffer_wakeup_info *wakeup, *nextwakeup;
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

void _netemu_packet_buffer_perform_notify(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item) {
	struct _netemu_packet_buffer_notify_info *notify, *nextnotify;
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
