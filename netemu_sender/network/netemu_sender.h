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
	int error;
};



int netemu_sender_send(struct netemu_sender* sender, char* data, int size);

struct netemu_sender* netemu_sender_new(netemu_sockaddr* addr);

void netemu_sender_free(struct netemu_sender* sender);

#endif /* SENDER_H_ */
