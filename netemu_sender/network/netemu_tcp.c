/**
 * @file
 * This module let's you create a connection by listening for or connecting to a host.
 */

#include "netemu_tcp.h"

#define LISTEN_BACKLOG	1024
#define BUFFER_SIZE		512

void _netemu_listener_listen(void* params);
void _netemu_tcp_connection_recv(void* params);
void _netemu_tcp_connection_notify(struct netemu_tcp_connection* receiver, char* data, size_t size);

struct netemu_tcp_connection* netemu_tcp_connection_new(netemu_sockaddr* addr, size_t addr_len) {
	NETEMU_SOCKET socket;
	struct netemu_tcp_connection* sender;
	sender = malloc(sizeof(struct netemu_tcp_connection));
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	if (socket == INVALID_SOCKET) {
		sender->error = netemu_get_last_error();
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;
	sender->listening = 0;

	return sender;
}

/**
 * This function creates a new threads and starts listening for incoming connections.
 */
void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener, void (*conAcceptedFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*)) {
	listener->conAcceptedFn = conAcceptedFn;
	netemu_thread_new(_netemu_listener_listen, (void*)listener);
}


void _netemu_listener_listen(void* params) {
	struct netemu_tcp_listener *listener;
	struct netemu_tcp_connection *connection;
	netemu_sockaddr *addr;
	int addr_len, error;
	NETEMU_SOCKET socket;

	addr_len = 0;
	listener = (struct netemu_tcp_listener*)params;
	listener->listening  = 1;

	error = netemu_listen(listener->socket,LISTEN_BACKLOG);
	if(error == -1) {
		listener->error = netemu_get_last_error();
		return;
	}
	while (1) {
		addr = malloc(sizeof(netemu_sockaddr));
		socket = netemu_accept(listener->socket,addr,&addr_len);
		connection = malloc(sizeof(struct netemu_tcp_connection));
		connection->addr = addr;
		connection->addr_len = addr_len;
		connection->socket = socket;
		connection->buffer_size = BUFFER_SIZE;
		listener->conAcceptedFn(listener,connection);
	}
}

void netemu_tcp_connection_register_recv_fn(struct netemu_tcp_connection* receiver, void (* listenerFn)(char*, size_t, struct netemu_tcp_connection*, void*), void* params) {
	struct netemu_tcp_connection_fn *receiver_fn;
	struct netemu_tcp_connection_fn *receiver_iter;
	receiver_fn = malloc(sizeof(struct netemu_tcp_connection_fn));
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

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con) {
	netemu_thread_new(_netemu_tcp_connection_recv, (void*)con);
}

void _netemu_tcp_connection_recv(void* params) {
	struct netemu_tcp_connection *receiver;
	int error;
	char *buffer;

	receiver = (struct netemu_tcp_connection*)params;
	buffer = malloc(sizeof(char)*receiver->buffer_size);

	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		error = netemu_recvfrom(receiver->socket, buffer, receiver->buffer_size, 0, NULL, 0);
		if (error == -1) {
			receiver->error = netemu_get_last_error();
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		_netemu_tcp_connection_notify(receiver, buffer, error);
		memset(buffer, 0, receiver->buffer_size);
		netemu_thread_mutex_release(receiver->lock);
	}
}

void _netemu_tcp_connection_notify(struct netemu_tcp_connection* receiver, char* data, size_t size) {
	struct netemu_tcp_connection_fn* receiver_fn;
	receiver_fn = receiver->receiver_fn;
	while(receiver_fn != NULL) {
		receiver_fn->listenerFn(data,size, receiver, receiver_fn->params);
		receiver_fn = receiver_fn->next;
	}
}

int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size) {
	int success;
	success = netemu_sendto(sender->socket,data,size,0,sender->addr,sender->addr_len);
	if(success == -1){
		sender->error = netemu_get_last_error();
	}
	return success;
}

int netemu_tcp_connection_connect(struct netemu_tcp_connection *sender) {
	int success;
	success = netemu_connect(sender->socket,sender->addr,sender->addr_len);
	if(success == -1){
		sender->error = netemu_get_last_error();
	}
	return success;
}

struct netemu_tcp_listener* netemu_tcp_listener_new(netemu_sockaddr* bind_addr, size_t addr_len) {
	NETEMU_SOCKET socket;
	struct netemu_tcp_listener* sender;
	sender = malloc(sizeof(struct netemu_tcp_listener));
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_STREAM);
	if (socket == INVALID_SOCKET) {
		sender->error = netemu_get_last_error();
	}
	sender->addr_len = addr_len;
	sender->addr = bind_addr;
	sender->socket = socket;
	sender->listening = 0;

	return sender;
}
