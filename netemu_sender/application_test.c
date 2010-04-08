/*
 * application_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include <stdlib.h>
#include <stdio.h>
#include "test_util.h"
#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include "protocol/application.h"
#include "protocol/transport.h"
#include "protocol/communication.h"


void application_listener(char* data, size_t size, struct netemu_receiver* receiver);
void send_hello(struct netemu_sender *sender);
void test_login_request(struct netemu_sender* sender);
void test_pong(struct netemu_sender* sender);
void test_send_leave(struct netemu_sender *sender);
void test_create_game(struct netemu_sender* sender);
int port = 0;
int login_accepted = 0;
int ping_received = 0;
struct netemu_sender* new_sender;
NETEMU_WORD user_id;
void run_application_tests() {
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	receiver = prepare_receiver(CLIENT_PORT,application_listener);
	sender = prepare_sender_on_socket(receiver->socket, SERVER_PORT);
	send_hello(sender);
	while(port == 0);
	new_sender = prepare_sender_on_socket(receiver->socket, port);
	test_login_request(new_sender);
	while(ping_received = 0);
	test_pong(new_sender);
	while(user_id == 0);
	test_create_game(new_sender);
	test_send_leave(new_sender);
}

void send_hello(struct netemu_sender *sender) {
	char* hello_message;
	struct user_joined* joined;
	hello_message = netemu_communication_create_hello_message("0.83");
	send_data(sender, hello_message);
	free(hello_message);
}

void application_listener(char* data, size_t size, struct netemu_receiver* receiver) {
	int status,i;
	struct transport_packet *packet;
	struct user_joined* joined;
	struct application_instruction *instruction;
	if (port == 0) {
		status = netemu_communication_parse_server_message(data);
		if(status == 0) {
			port = netemu_communication_parse_server_accept_port(data);
		}
	}
	else {
		packet = netemu_transport_unpack(data);
		for (i = 0; i < packet->count; i++) {
			instruction = netemu_application_parse_message(packet->instructions[i]);
			if(instruction->id == PING) {
				printf("PING! Sending PONG!\n");
				test_pong(new_sender);
			}
			else if(instruction->id == USER_JOINED) {
				joined = (struct user_joined*)instruction->body;
				user_id = joined->id;
				printf("Joined: %s\n", joined->user);
			}
			else if(instruction->id == LOGIN_SUCCESS) {
				printf("LOGIN SUCCESS\n");
			}
			ping_received = 1;
		}
	}
	//printf("%s", data);
}

/**
 * Test sending a login request.
 */
void test_login_request(struct netemu_sender* sender) {
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	netemu_application_add_login_request(messages[0],"netemu","haha",1);
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(sender,buffer.data,buffer.size);

}

void test_send_leave(struct netemu_sender *sender) {
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	netemu_application_add_leave(messages[0],"leavin.");
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(sender,buffer.data,buffer.size);
}

void test_pong(struct netemu_sender* sender) {
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	netemu_application_add_pong(messages[0]);
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(sender,buffer.data,buffer.size);
}

void test_create_game(struct netemu_sender* sender) {
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	netemu_application_add_create_game(messages[0],"thegame");
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(sender,buffer.data,buffer.size);
}
