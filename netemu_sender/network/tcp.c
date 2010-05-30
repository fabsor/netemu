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
 * This module let's you create a connection by listening for or connecting to a host.
 */

#include "tcp.h"
#include "netlib_error.h"
#include "../protocol/transport.h"
#include "net.h"
#define LISTEN_BACKLOG	1024
#define BUFFER_SIZE		512

void _netemu_tcp_listener_listen(void* params);
void _netemu_tcp_connection_recv(void* params);
void _netemu_tcp_listener_notify(struct netemu_tcp_listener *listener, struct netemu_tcp_connection *connection);
void netemu_tcp_listener_listen(void* params);

struct netemu_tcp_connection* netemu_tcp_connection_new(netlib_sockaddr* addr, size_t addr_len) {
	NETLIB_SOCKET socket;
	struct netemu_tcp_connection* sender;
	if((sender = malloc(sizeof(struct netemu_tcp_connection))) == NULL) {
		/*netlib_set_last_error(NETMEU_ENOTENOUGHMEMORY);*/
		return NULL;
	}

	socket = netlib_socket(NETLIB_AF_INET, NETLIB_SOCK_STREAM);
	if (socket == NETLIB_INVALID_SOCKET) {
		/* TODO: Need to retrieve the platforms last error here and set it as our last error. Will use NETEMU_UNKNOWNERROR in the meantime. */
		/* sender->error = netlib_get_last_error(); */
		return NULL;
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;
	sender->receiving = 0;
	sender->fn = NULL;
	return sender;
}

struct netemu_tcp_connection* netemu_tcp_connection_new_on_socket(NETLIB_SOCKET socket, netlib_sockaddr* addr, size_t addr_len) {
	struct netemu_tcp_connection* sender;
	sender = malloc(sizeof(struct netemu_tcp_connection));
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;
	sender->receiving = 0;
	sender->fn = NULL;
	sender->socket = socket;
	return sender;
}

/**
 * This function creates a new threads and starts listening for incoming connections.
 */
void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener) {
	netlib_thread_new(_netemu_tcp_listener_listen, (void*)listener);
}


void netemu_tcp_listener_register_new_connection_fn(struct netemu_tcp_listener* receiver, void (* listenerFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*, void*), void* params) {
	struct netemu_tcp_new_connection_fn *receiver_fn;
	struct netemu_tcp_new_connection_fn *receiver_iter;
	receiver_fn = malloc(sizeof(struct netemu_tcp_new_connection_fn));
	receiver_fn->listenerFn = listenerFn;
	receiver_fn->next = NULL;
	receiver_fn->params = params;

	if (receiver->listener_fn == NULL) {
		receiver->listener_fn = receiver_fn;
	}
	else {
		receiver_iter = receiver->listener_fn;
		while (receiver_iter->next != NULL) {
			receiver_iter = receiver_iter->next;
		}
		receiver_iter->next = receiver_fn;
	}
}

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con, parseReceivedDataFn fn, void *param) {
	con->fn = fn;
	con->data_param = param;
	con->receiving = 1;
	netlib_thread_new(_netemu_tcp_connection_recv, (void*)con);
}

void _netemu_tcp_connection_recv(void* params) {
	struct netemu_tcp_connection *receiver;
	union netemu_connection_type type;
	int error;
	receiver = (struct netemu_tcp_connection*)params;
	type.connection = receiver;
	receiver->lock = netlib_thread_mutex_create();
	while (receiver->receiving) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netlib_thread_mutex_lock(receiver->lock, NETLIB_INFINITE);
		error = receiver->fn(receiver->socket, TCP_CONNECTION, type, receiver->data_param);
		if (error == -1) {
			receiver->error = netlib_get_last_error();
			netlib_thread_mutex_release(receiver->lock);
			break;
		}
		netlib_thread_mutex_release(receiver->lock);
	}
}

void netemu_tcp_connection_stop_receiving(struct netemu_tcp_connection *receiver) {
	receiver->receiving = -1;
}

int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size) {
	int success;
	success = netlib_send(sender->socket,data,size,0);

	return success;
}

int netemu_tcp_connection_connect(struct netemu_tcp_connection *sender) {
	int success;
	success = netlib_connect(sender->socket,sender->addr,sender->addr_len);
	return success;
}

void netemu_tcp_connection_destroy(struct netemu_tcp_connection *connection) {
	connection->receiving = -1;
	netlib_closesocket(connection->socket);
	netlib_thread_mutex_lock(connection->lock, NETLIB_INFINITE);
	netlib_thread_mutex_release(connection->lock);
	free(connection->addr);
	netlib_thread_mutex_destroy(connection->lock);
	free(connection);
}

struct netemu_tcp_listener* netemu_tcp_listener_new(netlib_sockaddr* bind_addr, size_t addr_len) {
	NETLIB_SOCKET socket;
	struct netemu_tcp_listener* sender;
	sender = malloc(sizeof(struct netemu_tcp_listener));
	sender->error = 0;
	socket = netlib_socket(NETLIB_AF_INET,NETLIB_SOCK_STREAM);

	if (socket == NETLIB_INVALID_SOCKET) {
		sender->error = netlib_get_last_error();
	}
	sender->addr_len = addr_len;
	sender->addr = bind_addr;
	sender->socket = socket;
	sender->listening = 0;
	sender->listener_fn = NULL;
	return sender;
}


void _netemu_tcp_listener_listen(void* params) {
	struct netemu_tcp_listener *receiver;
	int error;
	socklen_t addr_len;
	struct netemu_tcp_connection *connection;
	NETLIB_SOCKET socket;
	netlib_sockaddr *addr;
	receiver = (struct netemu_tcp_listener*)params;
	error = netlib_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if(error == -1) {
		error = netlib_get_last_error();
		return;
	}
	error = netlib_listen(receiver->socket,5);
	if(error == -1) {
		error = netlib_get_last_error();
		return;
	}
	while (1) {
		socket = netlib_accept_inet(receiver->socket,&addr,&addr_len);
		if (socket == NETLIB_INVALID_SOCKET) {
			free(addr);
			/*Do something interesting here.*/
			error = netlib_get_last_error();
		}
		else {
			connection = netemu_tcp_connection_new_on_socket(socket,addr,addr_len);
			_netemu_tcp_listener_notify(receiver,connection);
		}

	}
}

void _netemu_tcp_listener_notify(struct netemu_tcp_listener *listener, struct netemu_tcp_connection *connection) {
	struct netemu_tcp_new_connection_fn* listener_fn;
	listener_fn = listener->listener_fn;
	while(listener_fn != NULL) {
		listener_fn->listenerFn(listener,connection,listener_fn->params);
		listener_fn = listener_fn->next;
	}
}
