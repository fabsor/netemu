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
 * This file contains the implementation of the netemu_receiver module.
 */

#include "receiver_udp.h"
#include "netlib_error.h"
#include <stdlib.h>
#include <stdio.h>
#include "../util.h"

void _netemu_receiver_udp_recv(void* params);
void _netemu_receiver_udp_notify(struct netemu_receiver_udp* receiver, char* data, size_t size);
void _netemu_receiver_listen(void* params);
void free_receiver(struct netemu_receiver_udp*);

/**
 * Creates a new instance of hte netemu_receiver_udp module
 * @ingroup netemu_receiver_udp
 * @param addr The address to bind to.
 * @param addr_len the size of the address.
 * @return NULL if something went wrong, 0 otherwise. Call netlib_get_last_error to get information
 * about the occurred error. Errors that can occur includes:
 * - NETEMU_ENOTENOUGHMEMORY
 * - NETEMU_INVALID_SOCKET
 * - All errors that can occur with netemu_bind @see netemu_bind
 */
struct netemu_receiver_udp* netemu_receiver_udp_create(netemu_sockaddr* addr, int addr_len) {
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

/**
 * Start receiving data with this instance of netemu_receiver.
 * @param receiver the instance of netemu_receiver_udp
 * @param fn the function which should handle and parse data recieved.
 * @param param any value you want to pass on to the responder function. Set it to NULL if you're not interested.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, parseReceivedDataFn fn, void* param) {
	if(!receiver->listening) {
		receiver->fn = fn;
		receiver->received_data_param = param;
		netemu_thread_new(_netemu_receiver_udp_recv, (void*)receiver);
	}
}

/**
 * Banana, banana, banana
 */
void _netemu_receiver_udp_recv(void* params) {
	struct netemu_receiver_udp* receiver;
	union netemu_connection_type type;
	int error;
	receiver = (struct netemu_receiver_udp*)params;
	receiver->listening = 1;
	type.udp_receiver = receiver;
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
 * Stop receiving data on this receiver. This function will block for a short time in order to let the receiving thread finish.
 * @param receiver the instance of netemu_receiver_udp
 */
void netemu_receiver_udp_stop_receiving(struct netemu_receiver_udp *receiver) {
	if(receiver->listening) {
		/* Let's not interfere  with any ongoing parsings. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		receiver->listening = 0;
		netemu_thread_mutex_release(receiver->lock);
	}
}

/**
 * Free resources used by this receiver and destroy it. You cannot use this
 * receiver after destroying it!!
 * @param receiver the instance of netemu_receiver_udp
 */
void netemu_receiver_udp_destroy(struct netemu_receiver_udp* receiver) {
	netemu_receiver_udp_stop_receiving(receiver);
	netemu_closesocket(receiver->socket);
	free(receiver->addr);
}
