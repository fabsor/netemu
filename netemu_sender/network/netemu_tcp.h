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

struct netemu_tcp_connection {
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	netemu_mutex lock;
	struct netemu_tcp_connection_fn *receiver_fn;
	int buffer_size;
	int addr_len;
	int fn_count;
	int listening;
	int error;
};

struct netemu_tcp_listener {
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	size_t addr_len;
	int listening;
	int error;
	void (*conAcceptedFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*);
};


struct netemu_tcp_connection_fn{
	void (* listenerFn)(char*, size_t, struct netemu_tcp_connection*, void*);
	void* params;
	struct netemu_tcp_connection_fn *next;
};

struct netemu_tcp_connection* netemu_tcp_connection_new(netemu_sockaddr* addr, size_t addr_len);

void netemu_tcp_listener_start_listening(struct netemu_tcp_listener *listener, void (*conAcceptedFn)(struct netemu_tcp_listener*, struct netemu_tcp_connection*));

void netemu_tcp_connection_register_recv_fn(struct netemu_tcp_connection* receiver, void (* listenerFn)(char*, size_t, struct netemu_tcp_connection*, void*), void* params);

void netemu_tcp_connection_start_receiving(struct netemu_tcp_connection* con);

int netemu_tcp_connection_send(struct netemu_tcp_connection* sender, char* data, int size);

struct netemu_tcp_listener* netemu_tcp_listener_new(netemu_sockaddr* bind_addr, size_t addr_len);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_TCP_H_ */
