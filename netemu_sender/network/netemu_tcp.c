/**
 * @file
 * This module let's you create a connection by listening for or connecting to a host.
 */

#include "netemu_tcp.h"
#include "netlib_error.h"
#define LISTEN_BACKLOG	1024
#define BUFFER_SIZE		512

void _netemu_tcp_listener_listen(void* params);
void _netemu_tcp_connection_recv(void* params);
void _netemu_tcp_connection_notify(struct netemu_tcp_connection* receiver, char* data, size_t size);
void _netemu_tcp_listener_notify(struct netemu_tcp_listener *listener, struct netemu_tcp_connection *connection);
void netemu_tcp_listener_listen(void* params);

struct netemu_tcp_connection* netemu_tcp_connection_new(netemu_sockaddr* addr, size_t addr_len) {
	NETEMU_SOCKET socket;
	struct netemu_tcp_connection* sender;
	if((sender = malloc(sizeof(struct netemu_tcp_connection))) == NULL) {
		//netlib_set_last_error(NETMEU_ENOTENOUGHMEMORY);
		return NULL;
	}

	socket = netemu_socket(NETEMU_AF_INET, NETEMU_SOCK_STREAM);
	if (socket == NETEMU_INVALID_SOCKET) {
		/* TODO: Need to retrieve the platforms last error here and set it as our last error. Will use NETEMU_UNKNOWNERROR in the meantime. */
		//netlib_set_last_error(NETEMU_EUNKNOWNERROR);
		/* sender->error = netlib_get_last_error(); */
		return NULL;
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;
	sender->listening = 0;
	sender->receiver_fn = NULL;
	sender->buffer_size = 512;
	return sender;
}

struct netemu_tcp_connection* netemu_tcp_connection_new_on_socket(NETEMU_SOCKET socket, netemu_sockaddr* addr, size_t addr_len) {
	struct netemu_tcp_connection* sender;
	sender = malloc(sizeof(struct netemu_tcp_connection));
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;
	sender->listening = 0;
	sender->receiver_fn = NULL;
	sender->socket = socket;
	sender->buffer_size = 512;
	return sender;
}

/**
 * This function creates a new threads and starts listening for incoming connections.
 */
void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener) {
	netemu_thread_new(_netemu_tcp_listener_listen, (void*)listener);
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
		error = netemu_recv(receiver->socket, buffer, receiver->buffer_size, 0);
		if (error == -1) {
			receiver->error = netlib_get_last_error();
			netemu_thread_mutex_release(receiver->lock);
			break;
		}
		else if (error == 0) {
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
	return success;
}

int netemu_tcp_connection_connect(struct netemu_tcp_connection *sender) {
	int success;
	success = netemu_connect(sender->socket,sender->addr,sender->addr_len);
	return success;
}

struct netemu_tcp_listener* netemu_tcp_listener_new(netemu_sockaddr* bind_addr, size_t addr_len) {
	NETEMU_SOCKET socket;
	struct netemu_tcp_listener* sender;
	sender = malloc(sizeof(struct netemu_tcp_listener));
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_STREAM);
	if (socket == NETEMU_INVALID_SOCKET) {
		//sender->error = netemu_get_last_error();
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
	NETEMU_SOCKET socket;
	netemu_sockaddr *addr;
	receiver = (struct netemu_tcp_listener*)params;
	error = netemu_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if(error == -1) {
		error = netlib_get_last_error();
	}
	error = netemu_listen(receiver->socket,5);
	if(error == -1) {
		error = netlib_get_last_error();
	}
	while (1) {
		socket = netemu_accept_inet(receiver->socket,&addr,&addr_len);
		if (socket == NETEMU_INVALID_SOCKET) {
			free(addr);
			//receiver->error = netemu_get_last_error();
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
