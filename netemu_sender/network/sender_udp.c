/*
 * netemu_sender.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */
#include "sender_udp.h"
#include <stdlib.h>
#include "netlib_error.h"


int netemu_sender_udp_send(struct netemu_sender_udp* sender, char* data, int size) {
	int success;
	success = netemu_sendto(sender->socket, data, size, 0, sender->addr, sender->addr_len);
	if(success == -1){
		//sender->error = netemu_get_last_error();
	}
	return success;
}

struct netemu_sender_udp* netemu_sender_udp_new(netemu_sockaddr* addr, int addr_len) {
	NETEMU_SOCKET socket;
	struct netemu_sender_udp* sender;
	sender = malloc(sizeof(struct netemu_sender_udp));
	socket = netemu_socket(NETEMU_AF_INET,NETEMU_SOCK_DGRAM);
	if (socket == NETEMU_INVALID_SOCKET) {
		sender->error = netlib_get_last_platform_error();
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;

	return sender;
}

struct netemu_sender_udp* netemu_sender_udp_new_on_socket(netemu_sockaddr* addr, NETEMU_SOCKET socket, int addr_len) {
	struct netemu_sender_udp* sender;
	if((sender = malloc(sizeof(struct netemu_sender_udp))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	if (socket == NETEMU_INVALID_SOCKET) {
		return NULL;
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;

	return sender;
}

void netemu_sender_udp_free(struct netemu_sender_udp* sender) {
	netemu_closesocket(sender->socket);
	free(sender->addr);
	free(sender);
}