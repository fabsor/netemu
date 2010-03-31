/*
 * sender.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef SENDER_H_
#define SENDER_H_

#include "netemu_socket.h"

/*! This struct describes a sender. */
struct netemu_sender{
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
};


int netemu_sender_send(struct netemu_sender *sender, char *data);

struct netemu_sender* netemu_sender_new(char* host, int port);

void netemu_sender_free(struct netemu_sender* sender);

#endif /* SENDER_H_ */