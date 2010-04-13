#include "netemu_packet_buffer.h"
#include "netemu_list.h"

void _netemu_packet_buffer_perform_wakeup(struct netemu_packet_buffer* buffer, struct application_instruction *instruction);

struct _netemu_packet_buffer_wakeup_info {
	struct netemu_mutex *mutex;
	time_t age;
	struct _netemu_packet_buffer_wakeup_info *next;
	struct _netemu_packet_buffer_wakeup_info *prev;
};

struct _netemu_packet_buffer_internal {
	NETEMU_HASHTBL *registered_wakeups;
};



hash_size _buffer_hash_function(void *key) {

}

struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size) {
	struct netemu_packet_buffer *buffer;

	buffer = malloc(sizeof(struct netemu_packet_buffer));
	buffer->table = netemu_hashtbl_create(size, _buffer_hash_function);
	buffer->_internal = malloc(sizeof(struct _netemu_packet_buffer_internal));
	buffer->_internal->registered_wakeups = netemu_hashtbl_create(23, _buffer_hash_function);
	return buffer;
}

void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction) {
	struct netemu_list *list;
	if((list = netemu_hashtbl_get(buffer->table, &instruction->id, sizeof(char))) == NULL)
		list = netemu_list_new(sizeof(struct application_instruction*), 10);
	
	netemu_list_add(list, instruction);
	netemu_hashtbl_insert(buffer->table, &instruction->id, sizeof(char), instruction);
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

void netemu_packet_buffer_clear(struct netemu_packet_buffer *buffer) {
	netemu_hashtbl_clear(buffer->table);
}

void netemu_packet_buffer_register_wakeup_on_instruction(struct netemu_packet_buffer *buffer, char instruction_id, time_t age, struct netemu_mutex *mutex) {
	struct _netemu_packet_buffer_wakeup_info *wakeup, *existing_wakeup;
	wakeup = malloc(sizeof(struct _netemu_packet_buffer_wakeup_info));
	wakeup->age = age;
	wakeup->mutex = mutex;
	wakeup->next = NULL;

	netemu_thread_mutex_lock(mutex);

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
	}
}