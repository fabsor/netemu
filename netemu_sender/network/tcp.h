/*
 * netemu_tcp.h
 *
 *  Created on: 14 apr 2010
 *      Author: fabian
 */

#ifndef NETEMU_TCP_H_
#define NETEMU_TCP_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netemu_socket.h"
#include "netemu_thread.h"
#include "net.h"

struct netemu_tcp_connection {
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	netemu_mutex lock;
	void* data_param;
	int addr_len;
	parseReceivedDataFn fn;
	int listening;
	int error;
};

struct netemu_tcp_listener {
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
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

struct netemu_tcp_connection* netemu_tcp_connection_new(netemu_sockaddr* addr, size_t addr_len);

struct netemu_tcp_connection* netemu_tcp_connection_new_on_socket(NETEMU_SOCKET socket, netemu_sockaddr* addr, size_t addr_len);

void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener);

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con, parseReceivedDataFn parseFn, void *param);

int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size);

struct netemu_tcp_listener* netemu_tcp_listener_new(netemu_sockaddr* bind_addr, size_t addr_len);

void netemu_tcp_listener_register_new_connection_fn(struct netemu_tcp_listener* receiver, void (* listenerFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*, void*), void* params);

int netemu_tcp_connection_connect(struct netemu_tcp_connection *sender);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_TCP_H_ */
