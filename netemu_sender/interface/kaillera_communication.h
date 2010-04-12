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
#include "../network/netemu_receiver.h"
#include "../protocol/communication.h"
#include "../network/netemu_sender.h"


struct netemu_communication_server {
	char *name;
	struct netemu_sockaddr_in addr;
	struct netemu_communication_server *next;
};

struct netemu_communication_server* kaillera_communication_get_server_list();

/**
 *
 */
void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

void kaillera_communication_connect(struct netemu_sockaddr_in *addr);

void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr);




#endif /* COMMUNICATION_STATE_H_ */
