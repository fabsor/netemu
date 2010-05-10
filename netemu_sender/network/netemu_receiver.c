/*
 * netemu_receiver.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#include "netemu_receiver.h"
#include "netemu_thread.h"
#include "netlib_error.h"
#include <stdlib.h>
#include <stdio.h>
#include "../netemu_util.h"

/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */

void netemu_receiver_recv(void* params);
void _netemu_receiver_udp_notify(struct netemu_receiver_udp* receiver, char* data, size_t size);
void _netemu_receiver_listen(void* params);
void free_receiver(struct netemu_receiver_udp*);

struct netemu_receiver_udp* netemu_receiver_udp_new(netemu_sockaddr* addr, int addr_len, int buffer_size) {
	struct netemu_receiver_udp* receiver;
	int bind_error;
	if((receiver = malloc(sizeof(struct netemu_receiver_udp))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	receiver->buffer_size = buffer_size;
	receiver->addr = netemu_util_copy_addr(addr,addr_len);

	receiver->addr_len = addr_len;
	receiver->socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	receiver->receiver_fn = NULL;
	if(receiver->socket == NETEMU_INVALID_SOCKET) {
		free(receiver);
		return NULL;
	}
	bind_error = netemu_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if(bind_error == -1) {
		bind_error = netlib_get_last_platform_error();
		netemu_closesocket(receiver->socket);
		free(receiver);
		return NULL;
	}
	return receiver;
}

 /* This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, struct netemu_packet_buffer *buffer, parseReceivedDataFn fn, void* param) {
	receiver->buffer = buffer;
	receiver->fn = fn;
	receiver->received_data_param = param;
	netemu_thread_new(netemu_receiver_recv, (void*)receiver);
}

/*! Called to receive data. */
void netemu_receiver_recv(void* params) {
	struct netemu_receiver_udp* receiver;
	struct transport_instruction *instruction;
	struct transport_packet *packet;

	union netemu_connection_type type;
	char *buffer;
	int error, i;
	receiver = (struct netemu_receiver_udp*)params;

	buffer = malloc(sizeof(char)*receiver->buffer_size);
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		error = receiver->fn(receiver->socket, receiver->received_data_param);
		if (error == -1) {
			receiver->error = netlib_get_last_error();
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		for(i = 0; i < packet->count; i++) {
			instruction = netemu_application_parse_message(packet->instructions[i]);
			netemu_packet_buffer_add(receiver->buffer, instruction,TCP_CONNECTION,type);
		}

		netemu_thread_mutex_release(receiver->lock);
	}
	free_receiver(receiver);
}

void free_receiver(struct netemu_receiver_udp* receiver) {
	free(receiver->addr);
	netemu_closesocket(receiver->socket);
	free(receiver);
}

void netemu_receiver_udp_free(struct netemu_receiver_udp* receiver) {
	netemu_closesocket(receiver->socket);
}
