/*
 * netemu_receiver.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#include "netemu_receiver.h"
#include "netemu_thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
void netemu_receiver_recv(void* params);

struct netemu_receiver* netemu_receiver_new(netemu_sockaddr* addr, int buffer_size) {
	struct netemu_receiver* receiver;
	receiver = malloc(sizeof(struct netemu_receiver));
	receiver->buffer_size = buffer_size;
	receiver->addr = addr;
	return receiver;
}

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_start_listening(struct netemu_receiver* receiver){
	netemu_thread identifier;
	netemu_thread_new(&identifier, netemu_receiver_recv, (void*)receiver);
}


/*! Called to receive data. */
void netemu_receiver_recv(void* params) {
	struct netemu_receiver* receiver;
	char *buffer;
	int error;
	int bind_error;
	receiver = (struct netemu_receiver*)params;
	buffer = malloc(sizeof(char)*receiver->buffer_size);
	receiver->socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	if(receiver->socket == INVALID_SOCKET) {
		receiver->error = netemu_get_last_error();
		netemu_thread_exit();
		return;
	}

	bind_error = netemu_bind(receiver->socket,receiver->addr,sizeof(*receiver->addr));
	if(bind_error == -1) {
		receiver->error = netemu_get_last_error();
		netemu_thread_exit();
		return;
	}
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock);
		error = netemu_recv(receiver->socket, buffer, receiver->buffer_size, 0);
		if (error) {
			receiver->error = netemu_get_last_error();
			break;
		}
		netemu_thread_mutex_release(receiver->lock);
	}
	netemu_thread_exit();
}

void _netemu_receiver_notify(struct netemu_receiver* receiver, char* data) {
	struct netemu_receiver_fn* receiver_fn;
	receiver_fn = receiver->receiver_fn;
	receiver_fn->listenerFn(data,receiver->buffer_size, receiver);
	receiver_fn = receiver_fn->next;
}

/**
 * Register a function that will act as a listener. The function will be called when data is received.
 * The function must be thread safe.
 */
void netemu_receiver_register_recv_fn(struct netemu_receiver* receiver, void (* listenerFn)(char*, size_t, struct netemu_receiver*)) {
	struct netemu_receiver_fn *receiver_fn;
	struct netemu_receiver_fn *receiver_iter;
	receiver_fn = malloc(sizeof(struct netemu_receiver_fn));
	receiver_fn->listenerFn = listenerFn;

	if (receiver->receiver_fn == NULL) {
		receiver->receiver_fn = receiver_fn;
	}
	else {
		while (receiver_iter->next != NULL) {
			receiver_iter = receiver_iter->next;
		}
		receiver_iter->next = receiver_fn;
	}
}

