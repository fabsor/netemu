/*
 * network_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "test_util.h"

void network_listener(char* data, size_t size, struct netemu_receiver* receiver);
void send_network_data(struct netemu_sender* sender);
int net_ping = 0;
int net_hello = 0;

void test_network(){
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	receiver = prepare_receiver(INTERNAL_PORT,network_listener);
	sender = prepare_sender(INTERNAL_PORT);
	send_network_data(sender);
	while(!net_ping && !net_hello);
	netemu_receiver_free(receiver);
	netemu_sender_free(sender);
	printf("network test finished\n");
}

void network_listener(char* data, size_t size, struct netemu_receiver* receiver) {
	printf("%s",data);
	if(strstr("PING",data) != NULL)
		net_ping = 1;
	if(strstr("HELLO",data) != NULL)
		net_hello = 1;
}

void send_network_data(struct netemu_sender* sender) {
	char* ping_message;
	char* hello_message;
	ping_message = "PING";
	send_data(sender, ping_message);
	hello_message = "HELLO";
	send_data(sender, hello_message);
}

