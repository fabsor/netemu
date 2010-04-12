/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "../network/netemu_receiver.h"
#include "../protocol/communication.h"
#include "../network/netemu_sender.h"
#include "state.h"

#define DEFAULT_PORT_RANGE_START 	35700
#define DEFAULT_PORT_RANGE_END 		35800
#define DEFAULT_BUFFER_SIZE			256
#define VERSION						"0.83"
enum {
	COMMUNICATION_IDLE,
	COMMUNICATION_CONNECTING,
	COMMUNICATION_CONNECTED
};


struct netemu_communication_state {
	unsigned int host;
	unsigned short port;
	struct netemu_receiver* receiver;
	struct netemu_receiver* private_receiver;
	struct netemu_sender* sender;
	int error;
	unsigned short status;
};

void communication_state_listener(char* data, size_t size, struct netemu_receiver* receiver, void* args);
/**
 * Set the host you want to connect to. The state will now try to establish the connection and connect to the host if possible.
 */
void communication_state_set_host(struct netemu_state *state, struct netemu_sockaddr_in* clientaddr, struct netemu_sockaddr_in* hostaddr) {
	struct netemu_communication_state *com_state;
	struct netemu_receiver *receiver;
	struct netemu_receiver *sender;
	com_state = (struct netemu_communication_state*)state->state;

	if(clientaddr->port == 0) {
		clientaddr->port = netemu_htons(DEFAULT_PORT_RANGE_START);
	}
	com_state->port = hostaddr->port;
	com_state->host = hostaddr->addr;
	receiver = netemu_receiver_new(netemu_prepare_net_addr(clientaddr),sizeof(struct netemu_sockaddr_in),DEFAULT_BUFFER_SIZE);
	netemu_receiver_register_recv_fn(receiver,communication_state_listener, (void*)com_state);
	sender = netemu_sender_new_on_socket(netemu_prepare_net_addr(hostaddr),receiver->socket,sizeof(struct netemu_sockaddr_in));
	com_state->status = COMMUNICATION_CONNECTING;
}

void communication_state_listener(char* data, size_t size, struct netemu_receiver* receiver, void* args) {
	int msg;
	struct netemu_communication_state *state;
	state = (struct netemu_communication_state *)args;
	msg = netemu_communication_parse_server_message(data);
	switch(msg) {
		case CONNECTION_ACCEPTED:
			state->port = netemu_communication_parse_server_accept_port(data);
			break;
		case PING_RECEIVED:
			break;
		default:
			state->error = msg;
			break;
	}
	free(data);
}
/*
TODO: Move this to application state instead.
void _communication_create_new_sender(struct netemu_communication_state *state, int port) {
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	addr_in.addr = state->host;
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = netemu_htons(port);
	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new_on_socket(addr,state->receiver->socket,sizeof(addr_in));
	return sender;
}
*/
/**
 * Initiate this state.
 */
void communication_state_start(struct netemu_state *state) {
	struct netemu_communication_state *com_state;
	com_state = malloc(sizeof(struct netemu_communication_state));
	com_state->status = COMMUNICATION_IDLE;
	state->state = (void*)com_state;
}

/**
 * Perform operations relevant to this state.
 */
void communication_state_update(struct netemu_state *state) {

}

void _communication_state_connect(struct netemu_communication_state *state) {
	char *message;
	message = netemu_communication_create_hello_message(VERSION);
	netemu_sender_send(state->sender,message,strlen(message)+1);
	state->status = COMMUNICATION_CONNECTING;
}

struct netemu_receiver* communication_state_get_receiver() {

}

/**
 * Send a ping to the server.
 */
void communication_state_send_ping(struct netemu_state *state);
