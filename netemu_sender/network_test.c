/*
 * network_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "netemu_util.h"

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
	netemu_receiver_udp_free(receiver);
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

