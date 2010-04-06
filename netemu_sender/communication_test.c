/*
 * communication_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include <stdio.h>
#include <stdlib.h>
#include "test_util.h"
#include "protocol/communication.h"

void send_communication_data(struct netemu_sender* sender);
void communication_listener(char* data, size_t size, struct netemu_receiver* receiver);

int com_hello_received;
int com_ping_received;

void test_server_communication() {
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	receiver = prepare_receiver(CLIENT_PORT,communication_listener);
	sender = prepare_sender_on_socket(receiver->socket, SERVER_PORT);
	send_communication_data(sender);
	while(!com_hello_received && !com_ping_received);
}

void communication_listener(char* data, size_t size, struct netemu_receiver* receiver) {
	int msg;
	printf("%s",data);
	/*
	if(strstr("PING",data) != NULL)
		com_ping_received = 1;
	if(strstr("HELLO",data) != NULL)
		com_hello_received = 1;
	 */

	msg = netemu_communication_parse_server_message(data);
	switch(msg) {
		case CONNECTION_REJECTED_VER: case CONNECTION_ACCEPTED: case CONNECTION_REJECTED_TOO:
			com_hello_received = 1;
			break;
		case PING_RECEIVED:
			com_ping_received = 1;

	}
}

void send_communication_data(struct netemu_sender* sender) {
	char* ping_message;
	char* hello_message;
	ping_message = netemu_communication_create_ping_message();
	if (strcmp(ping_message,"PING") != 0) {
		printf("ERROR: Ping message incorrect.");
		return;
	}
	send_data(sender, ping_message);
	free(ping_message);
	hello_message = netemu_communication_create_hello_message("0.83");
	send_data(sender, hello_message);
	free(hello_message);
}
