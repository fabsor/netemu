/*
 * application_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */

#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include "test_util.h"
#include "protocol/application.h"
#include "protocol/transport.h"
#include "protocol/communication.h"


void application_listener(char* data, size_t size, struct netemu_receiver* receiver);
void send_hello(struct netemu_sender *sender);
void test_login_request(struct netemu_sender* sender);

int port = 0;
int login_accepted = 0;

void run_application_tests() {
	struct netemu_receiver* receiver;
	struct netemu_sender* sender, *new_sender;
	receiver = prepare_receiver(CLIENT_PORT,application_listener);
	sender = prepare_sender_on_socket(receiver->socket, SERVER_PORT);
	send_hello(sender);
	while(port == 0);
	new_sender = prepare_sender_on_socket(receiver->socket, port);
	test_login_request(new_sender);
	while(1);
}

void send_hello(struct netemu_sender *sender) {
	char* hello_message;

	hello_message = netemu_communication_create_hello_message("0.83");
	send_data(sender, hello_message);
	free(hello_message);
}

void application_listener(char* data, size_t size, struct netemu_receiver* receiver) {
	int status,i;
	struct transport_packet *packet;
	if (port == 0) {
		status = netemu_communication_parse_server_message(data);
		if(status == 0) {
			port = netemu_communication_parse_server_accept_port(data);
		}
	}
	else {
		packet = netemu_transport_unpack(data);
		for (i = 0; i < packet->count; i++) {
			netemu_application_parse_message(packet->instructions[i]);
		}
	}
	printf("%s", data);
}

/**
 * Test sending a login request.
 */
void test_login_request(struct netemu_sender* sender) {
	struct login_request* request;
	struct transport_packet_buffer buffer;
	struct protocol_message *messages[1];
	int hej = LOGIN_REQUEST;
	int size;

	request = netemu_application_create_login_request("netemu",1,&size);
	messages[0] = netemu_application_create_message(hej,"haha",(void*)request,size,netemu_application_login_request_pack);
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(sender,buffer.data,buffer.size);

}
