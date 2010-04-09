/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include "../network/netemu_receiver.h"
#include "../network/netemu_sender.h"
#include "state.h"

#define DEFAULT_PORT_RANGE_START 	35700
#define DEFAULT_PORT_RANGE_END 		35800
#define DEFAULT_BUFFER_SIZE			256

enum {
	COMMUNICATION_IDLE,
	COMMUNICATION_CONNECTING,
	COMMUNICATION_CONNECTED
};

struct netemu_communication_state {
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	unsigned short status;
};

void communication_state_listener(char* data, size_t size, struct netemu_receiver* receiver);
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

	receiver = netemu_receiver_new(netemu_prepare_net_addr(clientaddr),sizeof(struct netemu_sockaddr_in),DEFAULT_BUFFER_SIZE);
	netemu_receiver_register_recv_fn(receiver,communication_state_listener);
	sender = netemu_sender_new_on_socket(netemu_prepare_net_addr(hostaddr),receiver->socket,sizeof(struct netemu_sockaddr_in));
	com_state->status = COMMUNICATION_CONNECTING;
}

void communication_state_listener(char* data, size_t size, struct netemu_receiver* receiver) {

}

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
	 if(state == COMMUNICATION_CONNECTING) {

	}
}

void communication_state_connect() {

}

/**
 * Send a ping to the server.
 */
void communication_state_send_ping(struct netemu_state *state);

/**
 * Send an hello message to the server.
 */
void communication_state_send_hello(struct netemu_state *state);
