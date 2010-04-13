#include "netemu_packet_buffer.h"
#include "netemu_list.h"

struct _netemu_packet_buffer_internal {
	int i;
};

hash_size _buffer_hash_function(void *key) {
}

struct netemu_packet_buffer netemu_packet_buffer_new() {
	struct netemu_packet_buffer *buffer;

	buffer = malloc(sizeof(struct netemu_packet_buffer));
	buffer->table = netemu_hashtbl_create(10, _buffer_hash_function);
}

void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction) {
	struct netemu_list *list;
	if((list = netemu_hashtbl_get(buffer->table, &instruction->id)) == NULL)
		list = netemu_list_new(sizeof(struct application_instruction*), 10);

	netemu_list_add(list, instruction);
	netemu_hashtbl_insert(buffer->table, &instruction->id, instruction);
}

struct netemu_list* netemu_packet_buffer_get(struct netemu_packet_buffer *buffer, int id) {
	return (struct netemu_list*)netemu_hashtbl_get(buffer->table, &id);
}

struct application_instruction* netemu_packet_buffer_peek(struct netemu_packet_buffer *buffer, int id) {
	struct netemu_list* list;
	if((list = netemu_hashtbl_get(buffer->table, &id)) == NULL) 
		return NULL;
	
	return (struct application_instruction*)netemu_list_get(list, list->count - 1);
}

struct application_instruction* netemu_packet_buffer_pop(struct netemu_packet_buffer *buffer, int id) {
	struct netemu_list* list;
	struct application_instruction *instruction;
	if((list = netemu_hashtbl_get(buffer->table, &id)) == NULL) 
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