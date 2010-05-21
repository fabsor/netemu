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
#include "netemu_net.h"
/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */

void netemu_receiver_recv(void* params);
void _netemu_receiver_udp_notify(struct netemu_receiver_udp* receiver, char* data, size_t size);
void _netemu_receiver_listen(void* params);
void free_receiver(struct netemu_receiver_udp*);

struct netemu_receiver_udp* netemu_receiver_udp_new(netemu_sockaddr* addr, int addr_len) {
	struct netemu_receiver_udp* receiver;
	int bind_error;
	if((receiver = malloc(sizeof(struct netemu_receiver_udp))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	receiver->addr = netemu_util_copy_addr(addr,addr_len);

	receiver->addr_len = addr_len;
	receiver->socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	receiver->fn = NULL;
	receiver->listening = FALSE;
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
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, parseReceivedDataFn fn, void* param) {
	if(!receiver->listening) {
		receiver->fn = fn;
		receiver->received_data_param = param;
		netemu_thread_new(netemu_receiver_recv, (void*)receiver);
	}
}

/*! Called to receive data. */
void netemu_receiver_recv(void* params) {
	struct netemu_receiver_udp* receiver;
	struct transport_instruction *instruction;
	struct transport_packet *packet;
	union netemu_connection_type type;
	int error, i;
	receiver = (struct netemu_receiver_udp*)params;
	receiver->listening = 1;
	/*TODO: We need to figure out how to fix this, it's ugly to include a null. */
	type.udp_sender = receiver;
	receiver->lock = netemu_thread_mutex_create();
	while (receiver->listening) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		error = receiver->fn(receiver->socket, UDP_RECEIVER, type, receiver->received_data_param);
		if (error == -1) {
			receiver->error = netlib_get_last_platform_error();
			netemu_thread_mutex_release(receiver->lock);
			receiver->listening = 0;
			break;
		}
		else if(error == 0) {
			receiver->listening = 0;
		}
		netemu_thread_mutex_release(receiver->lock);
	}
	netemu_thread_mutex_destroy(receiver->lock);
}

/**
 * Stop receiving.
 */
void netemu_receiver_udp_stop_receiving(struct netemu_receiver_udp *receiver) {
	if(receiver->listening) {
		/* Let's not interfere  with any ongoing parsings. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		receiver->listening = 0;
		netemu_thread_mutex_release(receiver->lock);
	}
}


void netemu_receiver_udp_free(struct netemu_receiver_udp* receiver) {
	netemu_receiver_udp_stop_receiving(receiver);
	netemu_closesocket(receiver->socket);
	free(receiver->addr);
}
