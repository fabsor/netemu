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
