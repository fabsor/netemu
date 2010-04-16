#include "netemu_packet_buffer.h"
#include "netemu_list.h"

void _netemu_packet_buffer_perform_wakeup(struct netemu_packet_buffer* buffer, struct application_instruction *instruction);

struct _netemu_packet_buffer_wakeup_info {
	netemu_mutex mutex;
	time_t age;
	struct _netemu_packet_buffer_wakeup_info *next;
	struct _netemu_packet_buffer_wakeup_info *prev;
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
	NETEMU_HASHTBL *registered_wakeups;
	NETEMU_HASHTBL *registered_fns;
};


void _netemu_packet_buffer_internal_add(struct netemu_packet_buffer *buffer, struct application_instruction* instruction);
void _netemu_packet_buffer_perform_notify(struct netemu_packet_buffer* buffer, struct application_instruction *instruction);
void _netemu_packet_buffer_update(void* args);


struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size, int receiving, int sending) {
	struct netemu_packet_buffer *buffer;

	buffer = malloc(sizeof(struct netemu_packet_buffer));
	buffer->table = netemu_hashtbl_create(size, def_hashfunc_int,comparator_int);
	buffer->receiving = receiving;
	buffer->sending = sending;
	buffer->_internal = malloc(sizeof(struct _netemu_packet_buffer_internal));
	buffer->_internal->registered_wakeups = netemu_hashtbl_create(23, def_hashfunc_int, comparator_int);
	buffer->_internal->registered_fns = netemu_hashtbl_create(size, def_hashfunc_int, comparator_int);
	buffer->_internal->add_mutex = netemu_thread_mutex_create();
	buffer->_internal->instructions_to_add = netemu_list_new(20);
	netemu_thread_new(_netemu_packet_buffer_update,buffer);
	return buffer;
}


void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction) {
	netemu_thread_mutex_lock(buffer->_internal->add_mutex, NETEMU_INFINITE);
	netemu_list_add(buffer->_internal->instructions_to_add, instruction);
	netemu_thread_mutex_release(buffer->_internal->add_mutex);
}

struct netemu_list* netemu_packet_buffer_get(struct netemu_packet_buffer *buffer, char id) {
	return (struct netemu_list*)netemu_hashtbl_get(buffer->table, &id, sizeof(char));
}

struct application_instruction* netemu_packet_buffer_peek(struct netemu_packet_buffer *buffer, char id) {
	struct netemu_list* list;
	if((list = netemu_hashtbl_get(buffer->table, &id, sizeof(char))) == NULL) 
		return NULL;
	
	return (struct application_instruction*)netemu_list_get(list, list->count - 1);
}

struct application_instruction* netemu_packet_buffer_pop(struct netemu_packet_buffer *buffer, char id) {
	struct netemu_list* list;
	struct application_instruction *instruction;
	if((list = netemu_hashtbl_get(buffer->table, &id, sizeof(char))) == NULL) 
		return NULL;
	
	if(list->count == 0)
		return NULL;
	instruction = (struct application_instruction*)netemu_list_get(list, list->count - 1);
	netemu_list_remove_at(list, list->count - 1);

	return instruction;
}

struct transport_packet* netemu_packet_buffer_pack(struct netemu_packet_buffer *buffer) {
	struct transport_packet* packet;
	struct application_instruction **instructions;
	instructions = malloc(sizeof(struct application_instruction**)*buffer->table->count);
}

void netemu_packet_buffer_clear(struct netemu_packet_buffer *buffer) {
	netemu_hashtbl_clear(buffer->table);
}

void netemu_packet_buffer_add_instruction_received_fn(struct netemu_packet_buffer *buffer, int instruction, bufferListenerFn fn, void* arg) {
	struct _netemu_packet_buffer_notify_info *info, *existing_info;
	info = malloc(sizeof(struct _netemu_packet_buffer_notify_info));
	info->fn = fn;
	info->arg = arg;
	info->next = NULL;

	if((existing_info = netemu_hashtbl_get(buffer->_internal->registered_fns, &instruction, sizeof(char))) == NULL) {
		info->prev = NULL;
		netemu_hashtbl_insert(buffer->_internal->registered_fns, &instruction, sizeof(char), info);
	}
	else {
		while(existing_info->next != NULL) {
			existing_info = existing_info->next;
		}
		info->prev = existing_info;
		info->next = info;
	}
}

void _netemu_packet_buffer_update(void* args) {
	struct netemu_packet_buffer *buffer;
	struct netemu_list* itemsToAdd;
	netemu_mutex lock;
	int i;
	buffer = (struct netemu_packet_buffer*)args;
	itemsToAdd = buffer->_internal->instructions_to_add;
	lock = buffer->_internal->add_mutex;

	while(1) {
		if(itemsToAdd->count > 0) {
			netemu_thread_mutex_lock(lock,NETEMU_INFINITE);
			for (i = 0; i < itemsToAdd->count; i++) {
				_netemu_packet_buffer_internal_add(buffer,itemsToAdd->elements[i]);
				if(buffer->receiving) {
					/* wakeup and notify */
					_netemu_packet_buffer_perform_notify(buffer,itemsToAdd->elements[i]);
					_netemu_packet_buffer_perform_wakeup(buffer,itemsToAdd->elements[i]);
				}
			}
			netemu_list_clear(itemsToAdd);
			netemu_thread_mutex_release(lock);
		}
	}
}

void _netemu_packet_buffer_internal_add(struct netemu_packet_buffer *buffer, struct application_instruction* instruction) {
	struct netemu_list *list;
	if((list = netemu_hashtbl_get(buffer->table, &instruction->id, sizeof(char))) == NULL)
		list = netemu_list_new(10);

	netemu_list_add(list, instruction);
	netemu_hashtbl_insert(buffer->table, &instruction->id, sizeof(char), instruction);
}

void netemu_packet_buffer_register_wakeup_on_instruction(struct netemu_packet_buffer *buffer, int instruction_id, time_t age, netemu_mutex mutex) {
	struct _netemu_packet_buffer_wakeup_info *wakeup, *existing_wakeup;
	wakeup = malloc(sizeof(struct _netemu_packet_buffer_wakeup_info));
	wakeup->age = age;
	wakeup->mutex = mutex;
	wakeup->next = NULL;

	netemu_thread_mutex_lock(mutex,NETEMU_INFINITE);

	if((existing_wakeup = netemu_hashtbl_get(buffer->_internal->registered_wakeups, &instruction_id, sizeof(char))) == NULL) {
		wakeup->prev = NULL;
		netemu_hashtbl_insert(buffer->_internal->registered_wakeups, &instruction_id, sizeof(char), wakeup);
	}
	else {
		while(existing_wakeup->next != NULL) {
			existing_wakeup = existing_wakeup->next;
		}
		wakeup->prev = existing_wakeup;
		existing_wakeup->next = wakeup;
	}
}

void _netemu_packet_buffer_perform_wakeup(struct netemu_packet_buffer* buffer, struct application_instruction *instruction) {
	struct _netemu_packet_buffer_wakeup_info *wakeup, *nextwakeup;
	if((wakeup = netemu_hashtbl_get(buffer->_internal->registered_wakeups, &instruction->id, sizeof(char))) != NULL) {
		while(wakeup != NULL) {
			if(wakeup->age <= instruction->timestamp) {
				nextwakeup = wakeup->next;
				netemu_thread_mutex_release(wakeup->mutex);
				wakeup->prev->next = wakeup->next;
				wakeup->next->prev = wakeup->prev;
				free(wakeup);
				wakeup = nextwakeup;
			}
			else {
				wakeup = wakeup->next;
			}
		}
		wakeup = NULL;
	}
}

void _netemu_packet_buffer_perform_notify(struct netemu_packet_buffer* buffer, struct application_instruction *instruction) {
	struct _netemu_packet_buffer_notify_info *notify, *nextnotify;
	if((notify = netemu_hashtbl_get(buffer->_internal->registered_fns, &instruction->id, sizeof(char))) != NULL) {
		while(notify != NULL) {
				nextnotify = notify->next;
				notify->fn(buffer,instruction,notify->arg);
				notify = nextnotify;
		}
	}
}
