/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "network/netemu_receiver.h"
#include "protocol/communication.h"
#include "network/netemu_sender.h"
#include "netemu_resources.h"

#define VERSION						"0.83"

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver* receiver, void* args);
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver* receiver, void* args);

int kaillera_communication_connect(struct netemu_sockaddr_in *addr) {
	struct netemu_client *client;
	char* hello;
	int result = -1;
	client = netemu_resources_get_client();

	client->receiver = prepare_receiver(CLIENT_PORT);
	client->sender = prepare_sender_on_socket_at_addr(client->receiver->socket, addr);
	hello = netemu_communication_create_hello_message(VERSION);
	netemu_receiver_register_recv_fn(client->receiver,kaillera_communication_listener,&result);
	while(result != -1);

	free(hello);
	return result;
}

void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr, void (*ConnectionReceivedFn)(int port)) {
	struct netemu_client *client;
	char* hello;
	int result = -1;
	client = netemu_resources_get_client();

	client->receiver = prepare_receiver(CLIENT_PORT);
	client->sender = prepare_sender_on_socket_at_addr(client->receiver->socket, addr);
	hello = netemu_communication_create_hello_message(VERSION);
	netemu_receiver_register_recv_fn(client->receiver,kaillera_communication_listener_async,&result);
	free(hello);
}

void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver* receiver, void* args) {
	int result;
	int port;
	void (*ConnectionReceivedFn)(int port);
	result = netemu_communication_parse_server_message(data);
	ConnectionReceivedFn = args;
	if(result == CONNECTION_ACCEPTED) {
		port = netemu_communication_parse_server_accept_port(data);
		ConnectionReceivedFn(port);
	}
	free(data);
}

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver* receiver, void* args) {
	int *result;
	result = (int *)args;
	*result = netemu_communication_parse_server_message(data);
	free(data);
}
