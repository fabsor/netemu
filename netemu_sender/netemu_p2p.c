/*
 * netemu_p2p.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#include "netemu_server_connection.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_sender_collection.h"
#include "interface/netemu_p2p.h"
#include "network/netemu_tcp.h"
#include "netemu_list.h"

struct netemu_p2p_internal {
	struct netemu_list *peers;
};

void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);

/**
 * Create a host connection.
 */
struct netemu_p2p* netemu_p2p_new(struct netemu_sockaddr_in *addr, int addr_size, char* username, char* emulatorname) {
	struct netemu_p2p *p2p;
	struct netemu_sender_buffer *buffer;
	union netemu_sender_buffer_type *type;

	type = malloc(sizeof(union netemu_sender_buffer_type));
	type->collection = netemu_sender_collection_new();

	buffer = netemu_sender_buffer_new(BUFFER_SENDER_COLLECTION,type,5,20);

	p2p = malloc(sizeof(struct netemu_p2p));
	p2p->host = malloc(sizeof(struct netemu_tcp_listener));
	p2p->host = netemu_tcp_listener_new(netemu_prepare_net_addr(addr),addr_size);

	p2p->connection = netemu_server_connection_new(username,emulatorname,buffer);
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	p2p->_internal->peers = netemu_list_new(10,1);
	netemu_tcp_listener_register_new_connection_fn(p2p->host,netemu_p2p_new_connection,p2p);
	return p2p;
}

void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params) {
	struct netemu_p2p *p2p;
	p2p = (struct netemu_p2p*)params;
	netemu_list_add(p2p->_internal->peers, connection);
}

/**
 * Connect to a host.
 */
int netemu_p2p_connect(struct netemu_p2p* p2p, struct netemu_sockaddr_in *addr, int addr_size) {
	int error;
	struct netemu_tcp_connection *connection;
	connection = netemu_tcp_connection_new(netemu_prepare_net_addr(addr),addr_size);
	netemu_tcp_connection_register_recv_fn(connection, netemu_tcp_connection_receive, p2p->connection);
	error = netemu_tcp_connection_connect(connection);

	return error;
}
