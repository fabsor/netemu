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

/**
 * @file
 * This file contains the implementation of the netemu_sender_buffer module.
 */
#include "../protocol/application_kaillera.h"
#include "../protocol/transport.h"
#include "tcp.h"
#include "../structures/netemu_hashtbl.h"
#include "sender_udp.h"
#include "sender_collection.h"
#include "sender_buffer.h"
#include "netlib_error.h"

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
	buffer->send_lock = netlib_thread_mutex_create();
	buffer->running = 1;
	buffer->itemsToAdd = netemu_list_create(10,1);
	buffer->event = netlib_thread_event_create();
	/* Start a new thread. */
	netlib_thread_new(_netemu_sender_buffer_update,buffer);
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
			netlib_thread_event_wait(buffer->event, NETLIB_INFINITE);
		}

		current_time = time(NULL);
		if (buffer->instructions->count > 0 /*&& (buffer->instructions->count > buffer->preferred_no_packets || buffer->send == 1 || (current_time - buffer->last_send ) > PACKET_SEND_INTERVAL)*/) {
			netlib_thread_mutex_lock(buffer->send_lock, NETLIB_INFINITE);
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
						if(instructions[j]->id == 31)
							printf("hej");
					}
					packet_buffer = netemu_transport_pack(instructions, items->count);
					netemu_sender_buffer_send(type,connection, packet_buffer.data,
							packet_buffer.size);
				}
			}
			netemu_hashtbl_clear(itemsToSend);
			netlib_thread_mutex_release(buffer->send_lock);
			buffer->send = 0;
			buffer->last_send = current_time;
		}
	}
}

int netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction, netemu_connection_types type, union netemu_connection_type recipient) {
	struct netemu_sender_buffer_item *item;
	struct netemu_list *list;
	void* key;
	/* Don't accept any new items when we're in lockdown. */
	if(buffer->locked) {
		netlib_set_last_error(NETEMU_SENDER_BUFFER_EBUFFERLOCKED);
		return -1;
	}
	if((item = malloc(sizeof(struct netemu_sender_buffer_item))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	item->recipient_type = type;
	item->recipient = recipient;
	item->instruction = instruction;

	switch(type) {
		case TCP_CONNECTION:
			key = recipient.connection;
			break;
		case UDP_SENDER:
			key = recipient.udp_sender;
			break;
		case CONNECTION_COLLECTION:
			key = recipient.collection;
			break;
	}

	if (instruction->important) {
		buffer->send = 1;
	}
	netlib_thread_mutex_lock(buffer->send_lock, NETLIB_INFINITE);
	if((list = netemu_hashtbl_get(buffer->instructions,key,sizeof(void*))) == NULL) {
		list = netemu_list_create(10,0);
		if(list == NULL) {
			free(item);
			return -1;
		}
		if(netemu_hashtbl_insert(buffer->instructions,key,sizeof(void*),list) == -1) {
			netemu_list_destroy(list);
			free(item);
			return -1;
		}
	}
	if(netemu_list_add(list,item) == -1) {
		netemu_list_destroy(list);
		free(item);
		return -1;
	}
	netlib_thread_event_signal(buffer->event);
	netlib_thread_mutex_release(buffer->send_lock);
	return 0;
}

void netemu_sender_buffer_lock(struct netemu_sender_buffer *buffer) {
	/* Make sure we don't interfere with anything. */
	netlib_thread_mutex_lock(buffer->send_lock, NETLIB_INFINITE);
	/* Flush all instructions that have been queued. */
	netemu_list_clear(buffer->itemsToAdd);
	buffer->locked = TRUE;
	netlib_thread_mutex_release(buffer->send_lock);
}

void netemu_sender_buffer_unlock(struct netemu_sender_buffer *buffer) {
	/* Make sure we don't interfere with anything. */
	netlib_thread_mutex_lock(buffer->send_lock, NETLIB_INFINITE);
	buffer->locked = FALSE;
	netlib_thread_mutex_release(buffer->send_lock);
}

void netemu_sender_buffer_send(netemu_connection_types type, union netemu_connection_type recipient, char* data, int size) {
	switch(type) {
		case UDP_SENDER:
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

