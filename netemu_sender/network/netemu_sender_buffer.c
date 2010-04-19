/*
 * netemu_sender_buffer.c
 *
 *  Created on: 16 apr 2010
 *      Author: fabian
 */
#include "../protocol/application.h"
#include "../protocol/transport.h"
#include "../netemu_resources.h"
#include "netemu_sender.h"
#include "netemu_sender_buffer.h"
void _netemu_sender_buffer_update(void* arg);

struct netemu_sender_buffer* netemu_sender_buffer_new(const short preferred_no_packets,
		const short preferred_delay_time) {
	struct netemu_sender_buffer *buffer;
	buffer = (struct netemu_sender_buffer*) malloc(
			sizeof(struct netemu_sender_buffer));
	buffer->instructions = netemu_list_new(LIST_START_SIZE);
	buffer->preferred_no_packets = preferred_no_packets;
	buffer->preferred_delay_time = preferred_delay_time;
	buffer->send_lock = netemu_thread_mutex_create();
	buffer->running = 1;
	/* Start a new thread. */
	netemu_thread_new(_netemu_sender_buffer_update,buffer);
	return buffer;
}

void netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction) {
	netemu_thread_mutex_lock(buffer->send_lock, NETEMU_INFINITE);
	netemu_list_add(buffer->instructions, instruction);
	if (instruction->important) {
		buffer->send = 1;
	}
	netemu_thread_mutex_release(buffer->send_lock);
}

void _netemu_sender_buffer_update(void* arg) {
	struct netemu_sender_buffer *buffer;
	struct application_instruction** instructions;
	struct netemu_list* itemsToSend;
	struct netemu_sender_udp *sender;
	struct transport_packet_buffer packet_buffer;
	int i;
	int count;
	buffer = (struct netemu_sender_buffer*) arg;
	itemsToSend = buffer->instructions;
	sender = netemu_resources_get_sender();
	instructions = malloc(sizeof(struct netemu_application_instruction*)
			* itemsToSend->count);

	while (buffer->running) {
		if (buffer->instructions->count > buffer->preferred_no_packets
				|| buffer->send == 1) {
			netemu_thread_mutex_lock(buffer->send_lock, NETEMU_INFINITE);
			count = itemsToSend->count;
			for (i = 0; i < itemsToSend->count; i++) {
				instructions[i] = itemsToSend->elements[i];
			}
			netemu_list_clear(itemsToSend);
			netemu_thread_mutex_release(buffer->send_lock);
			packet_buffer = netemu_transport_pack(instructions, count);
			netemu_sender_udp_send(sender, packet_buffer.data,
					packet_buffer.size);
			//free(packet_buffer.data);
			for(i = 0; i < count; i++) {
				netemu_application_free_message(instructions[i]);
			}
			buffer->send = 0;
		}
	}
}
