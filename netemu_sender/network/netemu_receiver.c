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

#define LISTEN_BACKLOG	1024

struct netemu_receiver_tcp_recv_params {
	struct netemu_receiver_tcp* tcp_receiver;
	NETEMU_SOCKET socket;
};

/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
void _netemu_receiver_tcp_recv(void* params);
void netemu_receiver_recv(void* params);
void _netemu_receiver_udp_notify(struct netemu_receiver_udp* receiver, char* data, size_t size);
void _netemu_receiver_tcp_notify(struct netemu_receiver_tcp* receiver, NETEMU_SOCKET socket, char* data, size_t size);
void _netemu_receiver_listen(void* params);
void free_receiver(struct netemu_receiver_udp*);

struct netemu_receiver_udp* netemu_receiver_udp_new(netemu_sockaddr* addr, int addr_len, int buffer_size) {
	struct netemu_receiver_udp* receiver;
	int bind_error;
	receiver = malloc(sizeof(struct netemu_receiver_udp));
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

struct netemu_receiver_tcp* netemu_receiver_tcp_new(netemu_sockaddr* addr, int addr_len, int buffer_size) {
	struct netemu_receiver_tcp* receiver;
	int bind_error;
	receiver = malloc(sizeof(struct netemu_receiver_udp));
	receiver->buffer_size = buffer_size;
	receiver->addr = addr;
	receiver->addr_len = addr_len;
	receiver->socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_STREAM);
	receiver->receiver_fn = NULL;
	if (receiver->socket == INVALID_SOCKET) {
		receiver->error = netemu_get_last_error();

	}
	bind_error = netemu_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if (bind_error == -1) {
		receiver->error = netemu_get_last_error();
	}
	return receiver;
}

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver) {
	netemu_thread_new(netemu_receiver_recv, (void*)receiver);
}

/**
 * This function creates a new threads and starts listening for incoming connections.
 */
void netemu_receiver_tcp_start_listening(struct netemu_receiver_tcp *receiver) {
	netemu_thread_new(_netemu_receiver_listen, (void*)receiver);
}

void _netemu_receiver_listen(void* params) {
	struct netemu_receiver_tcp *receiver;
	NETEMU_SOCKET socket;
	receiver = (struct netemu_receiver_tcp*)params;
	netemu_listen(receiver->socket,LISTEN_BACKLOG);

	while (1) {
		socket = netemu_accept(receiver->socket,receiver->addr,receiver->addr_len);
		netemu_thread_new(_netemu_receiver_tcp_recv, (void*)receiver);
	}
}

void _netemu_receiver_tcp_recv(void* params) {
	struct netemu_receiver_tcp *receiver;
	NETEMU_SOCKET socket;
	int error;
	struct netemu_receiver_tcp_recv_params* recv_params;
	char *buffer;
	recv_params = (struct netemu_receiver_tcp_recv_params*)params;
	receiver = recv_params->tcp_receiver;
	socket = recv_params->socket;

	buffer = malloc(sizeof(char)*receiver->buffer_size);
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		error = netemu_recvfrom(receiver->socket, buffer, receiver->buffer_size, 0, NULL, 0);
		if (error == -1) {
			receiver->error = netemu_get_last_error();
			printf("Receive error: %i\n", receiver->error);
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		_netemu_receiver_tcp_notify(receiver, buffer, socket, error);
		memset(buffer, 0, receiver->buffer_size);
		netemu_thread_mutex_release(receiver->lock);
	}
}

/*! Called to receive data. */
void netemu_receiver_recv(void* params) {
	struct netemu_receiver_udp* receiver;
	char *buffer;
	int error;
	receiver = (struct netemu_receiver_udp*)params;

	buffer = malloc(sizeof(char)*receiver->buffer_size);
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		error = netemu_recvfrom(receiver->socket, buffer, receiver->buffer_size, 0, NULL, 0);
		if (error == -1) {
			receiver->error = netemu_get_last_error();
			printf("Receive error: %i\n", receiver->error);
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		_netemu_receiver_udp_notify(receiver, buffer, error);
		memset(buffer, 0, receiver->buffer_size);
		netemu_thread_mutex_release(receiver->lock);
	}
	free_receiver(receiver);
}

void free_receiver(struct netemu_receiver_udp* receiver) {
	struct netemu_receiver_tcp_fn *receiver_fn, *receiver_next;
	free(receiver->addr);
	receiver_fn = receiver->receiver_fn;
	while (receiver_fn != NULL) {
		receiver_next = receiver_fn->next;
		free(receiver_fn);
		receiver_fn = receiver_next;
	}
	free(receiver);
}

void _netemu_receiver_tcp_notify(struct netemu_receiver_tcp* receiver, NETEMU_SOCKET socket, char* data, size_t size) {
	struct netemu_receiver_tcp_fn* receiver_fn;
	receiver_fn = receiver->receiver_fn;
	while(receiver_fn != NULL) {
		receiver_fn->listenerFn(data, size, receiver, socket, receiver_fn->params);
		receiver_fn = receiver_fn->next;
	}
}

void _netemu_receiver_udp_notify(struct netemu_receiver_udp* receiver, char* data, size_t size) {
	struct netemu_receiver_udp_fn* receiver_fn;
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
void netemu_receiver_udp_register_recv_fn(struct netemu_receiver_udp* receiver, void (* listenerFn)(char*, size_t, struct netemu_receiver_udp*, void*), void* params) {
	struct netemu_receiver_udp_fn *receiver_fn;
	struct netemu_receiver_udp_fn *receiver_iter;
	receiver_fn = malloc(sizeof(struct netemu_receiver_udp_fn));
	receiver_fn->listenerFn = listenerFn;
	receiver_fn->next = NULL;
	receiver_fn->params = params;

	if (receiver->receiver_fn == NULL) {
		receiver->receiver_fn = receiver_fn;
	}
	else {
		receiver_iter = receiver->receiver_fn;
		while (receiver_iter->next != NULL) {
			receiver_iter = receiver_iter->next;
		}
		receiver_iter->next = receiver_fn;
	}
}

void netemu_receiver_udp_free(struct netemu_receiver_udp* receiver) {
	netemu_closesocket(receiver->socket);
}
