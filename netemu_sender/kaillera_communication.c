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

	return result;
}

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver* receiver, void* args) {
	int result;
	result = (int *)args;
	result = netemu_communication_parse_server_message(data);
	free(data);
}
