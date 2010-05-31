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

#ifndef NETEMU_SENDER_UDP_H_
#define NETEMU_SENDER_UDP_H_

#include "netlib_network.h"

/*! This struct describes a sender. */
struct netemu_sender_udp{
	NETLIB_SOCKET socket;
	netlib_sockaddr* addr;
	int addr_len;
	int error;
};

int netemu_sender_udp_send(struct netemu_sender_udp* sender, char* data, int size);

struct netemu_sender_udp* netemu_sender_udp_new(netlib_sockaddr* addr, int addr_len);

struct netemu_sender_udp* netemu_sender_udp_new_on_socket(netlib_sockaddr* addr, NETLIB_SOCKET socket, int addr_len);

void netemu_sender_free(struct netemu_sender_udp* sender);

void netemu_sender_udp_destroy(struct netemu_sender_udp* sender);

#endif /* SENDER_UDP_H_ */
