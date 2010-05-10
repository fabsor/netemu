/**
 * @file
 * This module let's you create a connection by listening for or connecting to a host.
 */

#include "netemu_tcp.h"
#include "netlib_error.h"
#include "../protocol/transport.h"
#include "netemu_packet_buffer.h"
#include "netemu_net.h"
#define LISTEN_BACKLOG	1024
#define BUFFER_SIZE		512

void _netemu_tcp_listener_listen(void* params);
void _netemu_tcp_connection_recv(void* params);
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
	return sender;
}

/**
 * This function creates a new threads and starts listening for incoming connections.
 */
void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener) {
	netemu_thread_new(_netemu_tcp_listener_listen, (void*)listener);
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

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con, struct netemu_packet_buffer *buffer) {
	con->buffer = buffer;
	netemu_thread_new(_netemu_tcp_connection_recv, (void*)con);
}

void _netemu_tcp_connection_recv(void* params) {
	struct netemu_tcp_connection *receiver;
	struct transport_packet *packet;
	union netemu_connection_type type;
	struct application_instruction *instruction;
	int error, i;
	NETEMU_DWORD temp;

	receiver = (struct netemu_tcp_connection*)params;
	type.connection = receiver;
	receiver->lock = netemu_thread_mutex_create();
	while (1) {
		/* We have to make sure that no one else is fiddling with our struct while we're receiving. */
		netemu_thread_mutex_lock(receiver->lock, NETEMU_INFINITE);
		packet = netemu_transport_unpack(receiver->socket);

		if (packet == NULL) {
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
}

/*
 	char count;
	int i, j;
	unsigned int pos;
	static int foo = 0;
	struct transport_packet* packet;
	struct transport_instruction* instruction;
	foo++;
	if((packet = malloc(sizeof(struct transport_packet))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	memcpy(&count,data,sizeof(char));
	packet->count = count;
	if((packet->instructions = malloc(sizeof(struct transport_instruction*)*count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(packet);
		return NULL;
	}
	pos = sizeof(char);
	for (i = 0; i < count; i++) {
		if((instruction = malloc(sizeof(struct transport_instruction))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);

			for(j = 0; j <= i; j++)
				free(instruction);

			free(packet->instructions);
			free(packet);
			return NULL;
		}

		memcpy(&instruction->serial, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);

		memcpy(&instruction->length, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		instruction->instruction = malloc(instruction->length);
		memcpy(instruction->instruction, data + pos,instruction->length);
		pos += instruction->length;
		packet->instructions[i] = instruction;

	}
	return packet;
 */


int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size) {
	int success;
	success = netemu_send(sender->socket,data,size,0);

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
	sender->error = 0;
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_STREAM);

	if (socket == NETEMU_INVALID_SOCKET) {
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
	NETEMU_SOCKET socket;
	netemu_sockaddr *addr;
	receiver = (struct netemu_tcp_listener*)params;
	error = netemu_bind(receiver->socket,receiver->addr,receiver->addr_len);
	if(error == -1) {
		error = netlib_get_last_error();
		return;
	}
	error = netemu_listen(receiver->socket,5);
	if(error == -1) {
		error = netlib_get_last_error();
		return;
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
