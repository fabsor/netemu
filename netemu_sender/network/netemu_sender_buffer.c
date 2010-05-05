/*
 * netemu_sender_buffer.c
 *
 *  Created on: 16 apr 2010
 *      Author: fabian
 */
#include "../protocol/application_kaillera.h"
#include "../protocol/transport.h"
#include "netemu_tcp.h"
#include "../structures/netemu_hashtbl.h"
#include "netemu_sender_udp.h"
#include "netemu_sender_collection.h"
#include "netemu_sender_buffer.h"

struct netemu_sender_buffer_item {
	netemu_connection_types recipient_type;
	union netemu_connection_type recipient;
	struct application_instruction *instruction;
};

void _netemu_sender_buffer_update(void* arg);
void netemu_sender_buffer_send(netemu_connection_types type, union netemu_connection_type recipient, char* data, int size);

struct netemu_sender_buffer* netemu_sender_buffer_new(const short preferred_no_packets,
		const short preferred_delay_time) {
	struct netemu_sender_buffer *buffer;
	buffer = (struct netemu_sender_buffer*) malloc(
			sizeof(struct netemu_sender_buffer));
	buffer->instructions = netemu_hashtbl_create(10, def_hashfunc_int, comparator_int);
	buffer->preferred_no_packets = preferred_no_packets;
	buffer->preferred_delay_time = preferred_delay_time;
	buffer->send_lock = netemu_thread_mutex_create();
	buffer->running = 1;
	buffer->event = netemu_thread_event_create();
	/* Start a new thread. */
	netemu_thread_new(_netemu_sender_buffer_update,buffer);
	return buffer;
}

void _netemu_sender_buffer_update(void* arg) {
	struct netemu_sender_buffer *buffer;
	struct application_instruction** instructions;
	struct netemu_hashtbl *itemsToSend;
	struct netemu_list *items;
	struct transport_packet_buffer packet_buffer;
	struct netemu_sender_buffer_item *item;
	union netemu_connection_type connection;
	netemu_connection_types type;
	int i,j, count;
	time_t current_time;

	buffer = (struct netemu_sender_buffer*) arg;
	itemsToSend = buffer->instructions;
	while (buffer->running) {
		if(buffer->instructions->count == 0) {
			netemu_thread_event_wait(buffer->event);
		}

		current_time = time(NULL);
		if (buffer->instructions->count > 0 /*&& (buffer->instructions->count > buffer->preferred_no_packets || buffer->send == 1 || (current_time - buffer->last_send ) > PACKET_SEND_INTERVAL)*/) {
			netemu_thread_mutex_lock(buffer->send_lock, NETEMU_INFINITE);
			count = itemsToSend->count;
			for (i = 0; i < itemsToSend->keys->count; i++) {
				items = netemu_hashtbl_get(itemsToSend,itemsToSend->keys->elements[i], sizeof(struct netemu_list*));
				if(items != NULL) {
					instructions = malloc(sizeof(struct application_instruction*)*items->count);
					type = ((struct netemu_sender_buffer_item*)items->elements[0])->recipient_type;
					connection = ((struct netemu_sender_buffer_item*)items->elements[0])->recipient;
					for(j = 0; j < items->count; j++) {
						item = (struct netemu_sender_buffer_item*)items->elements[j];
						instructions[j] = item->instruction;
					}
					packet_buffer = netemu_transport_pack(instructions, items->count);
					netemu_sender_buffer_send(type,connection, packet_buffer.data,
							packet_buffer.size);
				}
			}
			netemu_hashtbl_clear(itemsToSend);
			netemu_thread_mutex_release(buffer->send_lock);
			buffer->send = 0;
			buffer->last_send = current_time;
		}
	}
}

void netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction, netemu_connection_types type, union netemu_connection_type recipient) {
	struct netemu_sender_buffer_item *item;
	struct netemu_list *list;
	void* key;

	item = malloc(sizeof(struct netemu_sender_buffer_item));
	item->recipient_type = type;
	item->recipient = recipient;
	item->instruction = instruction;

	switch(type) {
		case TCP_CONNECTION:
			key = recipient.connection;
			break;
		case UDP_CONNECTION:
			key = recipient.udp_sender;
			break;
		case CONNECTION_COLLECTION:
			key = recipient.collection;
			break;
	}

	if (instruction->important) {
		buffer->send = 1;
	}
	netemu_thread_mutex_lock(buffer->send_lock, NETEMU_INFINITE);
	if((list = netemu_hashtbl_get(buffer->instructions,key,sizeof(void*))) == NULL) {
		list = netemu_list_new(10,0);
		netemu_hashtbl_insert(buffer->instructions,key,sizeof(void*),list);
	}
	netemu_list_add(list,item);
	netemu_thread_event_signal(buffer->event);
	netemu_thread_mutex_release(buffer->send_lock);
}

void netemu_sender_buffer_send(netemu_connection_types type, union netemu_connection_type recipient, char* data, int size) {
	switch(type) {
		case UDP_CONNECTION:
			netemu_sender_udp_send(recipient.udp_sender,data,size);
			break;
		case TCP_CONNECTION:
			netemu_tcp_connection_send(recipient.connection, data, size);
			break;
		case CONNECTION_COLLECTION:
			netemu_sender_collection_send_data(recipient.collection, data, size);
			break;
	}
}

