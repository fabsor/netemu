/*
 * netemu_p2p.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#include "netemu_server_connection.h"
#include "interface/netemu_p2p.h"
#include "network/netemu_tcp.h"
/**
 * Create a host connection.
 */
struct netemu_p2p* netemu_p2p_new(struct netemu_sockaddr_in *addr, int addr_size) {
	struct netemu_p2p *p2p;
	p2p = malloc(sizeof(struct netemu_p2p));
	p2p->host = malloc(sizeof(struct netemu_tcp_listener));
	p2p->host = netemu_tcp_listener_new(addr,addr_size);
	p2p->connection = netemu_server_connection_new("test","test");
	return p2p;

}

/**
 * Connect to a peer.
 */
int netemu_p2p_connect(struct netemu_p2p* p2p, struct netemu_sockaddr_in *addr, int addr_size) {
	int error;
	struct netemu_tcp_connection *connection;
	connection = malloc(sizeof(struct netemu_tcp_connection));
	connection->addr = netemu_prepare_net_addr(addr);
	connection->addr_len = addr_size;
	netemu_tcp_connection_register_recv_fn(p2p->connection,netemu_tcp_connection_receive,p2p->connection);
	error = netemu_tcp_connection_connect(connection);
}

