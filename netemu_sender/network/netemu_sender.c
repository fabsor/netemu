/*
 * netemu_sender.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */
#include "netemu_sender.h"
#include <stdlib.h>


int netemu_sender_send(struct netemu_sender* sender, char* data) {
	return netemu_sendto(sender->socket, data, 1, 0, sender->addr, sizeof(sender->addr));
}

struct netemu_sender* netemu_sender_new(netemu_sockaddr* addr) {
	NETEMU_SOCKET socket;
	struct netemu_sender* sender;
	sender = malloc(sizeof(struct netemu_sender));
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	sender->addr = addr;
	sender->socket = socket;
	return sender;
}

void netemu_sender_free(struct netemu_sender* sender) {
	netemu_shutdown(sender->socket,0);
	free(sender);
}
