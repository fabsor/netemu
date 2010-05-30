/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#include "sender_udp.h"
#include <stdlib.h>
#include "netlib_error.h"


int netemu_sender_udp_send(struct netemu_sender_udp* sender, char* data, int size) {
	int success;
	success = netlib_sendto(sender->socket, data, size, 0, sender->addr, sender->addr_len);
	if(success == -1){
		//sender->error = netemu_get_last_error();
	}
	return success;
}

struct netemu_sender_udp* netemu_sender_udp_new(netlib_sockaddr* addr, int addr_len) {
	NETLIB_SOCKET socket;
	struct netemu_sender_udp* sender;
	sender = malloc(sizeof(struct netemu_sender_udp));
	socket = netlib_socket(NETLIB_AF_INET,NETLIB_SOCK_DGRAM);
	if (socket == NETLIB_INVALID_SOCKET) {
		sender->error = netlib_get_last_platform_error();
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;

	return sender;
}

struct netemu_sender_udp* netemu_sender_udp_new_on_socket(netlib_sockaddr* addr, NETLIB_SOCKET socket, int addr_len) {
	struct netemu_sender_udp* sender;
	if((sender = malloc(sizeof(struct netemu_sender_udp))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	if (socket == NETLIB_INVALID_SOCKET) {
		return NULL;
	}
	sender->addr_len = addr_len;
	sender->addr = addr;
	sender->socket = socket;

	return sender;
}

void netemu_sender_udp_destroy(struct netemu_sender_udp* sender) {
	netlib_closesocket(sender->socket);
	free(sender->addr);
	free(sender);
}
