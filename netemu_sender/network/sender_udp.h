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
struct netemu_sender_udp{
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	int addr_len;
	int error;
};

int netemu_sender_udp_send(struct netemu_sender_udp* sender, char* data, int size);

struct netemu_sender_udp* netemu_sender_udp_new(netemu_sockaddr* addr, int addr_len);

struct netemu_sender_udp* netemu_sender_udp_new_on_socket(netemu_sockaddr* addr, NETEMU_SOCKET socket, int addr_len);

void netemu_sender_free(struct netemu_sender_udp* sender);

void netemu_sender_udp_free(struct netemu_sender_udp* sender);

#endif /* SENDER_H_ */