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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util.h"

void network_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void*);
void send_network_data(struct netemu_sender_udp* sender);
int net_ping = 0;
int net_hello = 0;

void test_network(){
	struct netemu_receiver_udp* receiver;
	struct netemu_sender_udp* sender;
	receiver = netemu_util_prepare_receiver(INTERNAL_PORT,network_listener, NULL);
	sender = netemu_util_prepare_sender(INTERNAL_PORT);
	send_network_data(sender);
	while(!net_ping && !net_hello);
	netemu_receiver_udp_destroy(receiver);
	netemu_sender_udp_free(sender);
	printf("network test finished\n");
}

void network_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args) {
	printf("%s",data);
	if(strstr("PING",data) != NULL)
		net_ping = 1;
	if(strstr("HELLO",data) != NULL)
		net_hello = 1;
}

void send_network_data(struct netemu_sender_udp* sender) {
	char* ping_message;
	char* hello_message;
	ping_message = "PING";
	netemu_util_send_data(sender, ping_message);
	hello_message = "HELLO";
	netemu_util_send_data(sender, hello_message);
}

