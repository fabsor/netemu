/*
 * communication_state.h
 *
 *  Created on: 12 apr 2010
 *      Author: fabbe
 */

#ifndef COMMUNICATION_STATE_H_
#define COMMUNICATION_STATE_H_
#include <stdlib.h>
#include "netemu_socket.h"
#include "state.h"
#include "../network/netemu_receiver.h"
#include "../protocol/communication.h"
#include "../network/netemu_sender.h"
/**
 * Set the host you want to connect to. The state will now try to establish the connection and connect to the host if possible.
 */
void communication_state_set_host(struct netemu_state *state, struct netemu_sockaddr_in* clientaddr, struct netemu_sockaddr_in* hostaddr);

/**
 * Start function for this state. Set this as starting point when you set the state.
 */
void communication_state_start(struct netemu_state *state);

/**
 * Update function for this state.
 */
void communication_state_update(struct netemu_state *state);

void communication_state_send_ping(struct netemu_state *state);



#endif /* COMMUNICATION_STATE_H_ */
