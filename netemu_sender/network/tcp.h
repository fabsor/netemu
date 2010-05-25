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
#pragma once
#ifndef NETEMU_TCP_H_
#define NETEMU_TCP_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netlib_network.h"
#include "net.h"
#include "netlib_thread.h"

struct netemu_tcp_connection {
	NETLIB_SOCKET socket;
	netlib_sockaddr* addr;
	netemu_mutex lock;
	void* data_param;
	int addr_len;
	parseReceivedDataFn fn;
	int listening;
	int error;
};

struct netemu_tcp_listener {
	NETLIB_SOCKET socket;
	netlib_sockaddr* addr;
	size_t addr_len;
	int listening;
	struct netemu_tcp_new_connection_fn *listener_fn;
	int error;
};

struct netemu_tcp_new_connection_fn {
	void (* listenerFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*, void*);
	void* params;
	struct netemu_tcp_new_connection_fn *next;
};

struct netemu_tcp_connection* netemu_tcp_connection_new(netlib_sockaddr* addr, size_t addr_len);

struct netemu_tcp_connection* netemu_tcp_connection_new_on_socket(NETLIB_SOCKET socket, netlib_sockaddr* addr, size_t addr_len);

void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener);

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con, parseReceivedDataFn parseFn, void *param);

int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size);

struct netemu_tcp_listener* netemu_tcp_listener_new(netlib_sockaddr* bind_addr, size_t addr_len);

void netemu_tcp_listener_register_new_connection_fn(struct netemu_tcp_listener* receiver, void (* listenerFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*, void*), void* params);

int netemu_tcp_connection_connect(struct netemu_tcp_connection *sender);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_TCP_H_ */
