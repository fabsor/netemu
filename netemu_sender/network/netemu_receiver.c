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

struct netemu_receiver_params {
	struct netemu_receiver* receiver;
	void *params;
};

/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
void netemu_receiver_recv(void* params);
void _netemu_receiver_notify(struct netemu_receiver* receiver, char* data, size_t size);
void free_receiver(struct netemu_receiver*);

struct netemu_receiver* netemu_receiver_new(netemu_sockaddr* addr, int addr_len, int buffer_size) {
	struct netemu_receiver* receiver;
	int bind_error;
	receiver = malloc(sizeof(struct netemu_receiver));
	receiver->buffer_size = buffer_size;
	receiver->addr = addr;
	receiver->addr_len = addr_len;
	receiver->socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	receiver->receiver_fn = NULL;

	if(receiver->socket == INVALID_SOCKET) {
		receiver->error = netemu_get_last_error();

	}
	bind_error = netemu_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if(bind_error == -1) {
		receiver->error = netemu_get_last_error();
	}


	return receiver;
}

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_start_listening(struct netemu_receiver* receiver){
	netemu_thread_new(netemu_receiver_recv, (void*)receiver);
}


/*! Called to receive data. */
void netemu_receiver_recv(void* params) {
	struct netemu_receiver_params* parameters;
	struct netemu_receiver* receiver;
	char *buffer;
	int error;
	int bind_error;
	receiver = (struct netemu_receiver*)params;

	buffer = malloc(sizeof(char)*receiver->buffer_size);
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock);
		error = netemu_recvfrom(receiver->socket, buffer, receiver->buffer_size, 0, NULL, 0);
		if (error == -1) {
			receiver->error = netemu_get_last_error();
			printf("Receive error: %i\n", receiver->error);
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		_netemu_receiver_notify(receiver, buffer, error);
		memset(buffer, NULL, receiver->buffer_size);
		netemu_thread_mutex_release(receiver->lock);
	}
	free_receiver(receiver);
	//netemu_thread_exit();
}

void free_receiver(struct netemu_receiver* receiver) {
	struct netemu_receiver_fn *receiver_fn, *receiver_next;
	
	netemu_free(receiver->socket);
	free(receiver->addr);
	receiver_fn = receiver->receiver_fn;
	while (receiver_fn != NULL) {
		receiver_next = receiver_fn->next;
		free(receiver_fn);
		receiver_fn = receiver_next;
	}
	free(receiver);
}

void _netemu_receiver_notify(struct netemu_receiver* receiver, char* data, size_t size) {
	struct netemu_receiver_fn* receiver_fn;
	receiver_fn = receiver->receiver_fn;
	while(receiver_fn != NULL) {
		receiver_fn->listenerFn(data,size, receiver, receiver_fn->params);
		receiver_fn = receiver_fn->next;
	}
}

/**
 * Register a function that will act as a listener. The function will be called when data is received.
 * The function must be thread safe.
 */
void netemu_receiver_register_recv_fn(struct netemu_receiver* receiver, void (* listenerFn)(char*, size_t, struct netemu_receiver*, void*), void* params) {
	struct netemu_receiver_fn *receiver_fn;
	struct netemu_receiver_fn *receiver_iter;
	receiver_fn = malloc(sizeof(struct netemu_receiver_fn));
	receiver_fn->listenerFn = listenerFn;
	receiver_fn->next = NULL;
	receiver_fn->params = params;

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

void netemu_receiver_free(struct netemu_receiver* receiver) {
	netemu_closesocket(receiver->socket);
}

