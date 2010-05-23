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

#include <stdio.h>
#include <stdlib.h>

#include "../util.h"
#include "../protocol/communication.h"

void send_communication_data(struct netemu_sender_udp* sender);
void communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver);

int com_hello_received;
int com_ping_received;

void test_server_communication() {
	struct netemu_receiver_udp* receiver;
	struct netemu_sender_udp* sender;
	receiver = netemu_util_prepare_receiver(CLIENT_PORT,communication_listener, NULL);
	sender = netemu_util_prepare_sender_on_socket(receiver->socket, SERVER_PORT);
	send_communication_data(sender);
	while(!com_hello_received && !com_ping_received);
}

void communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver) {
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

void send_communication_data(struct netemu_sender_udp* sender) {
	char* ping_message;
	char* hello_message;
	ping_message = netemu_communication_create_ping_message();
	if (strcmp(ping_message,"PING") != 0) {
		printf("ERROR: Ping message incorrect.");
		return;
	}
	netemu_util_send_data(sender, ping_message);
	free(ping_message);
	hello_message = netemu_communication_create_hello_message("0.83");
	netemu_util_send_data(sender, hello_message);
	free(hello_message);
}
