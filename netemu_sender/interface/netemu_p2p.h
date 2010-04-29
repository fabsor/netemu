/*
 * netemu_client.h
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#ifndef NETEMU_CLIENT_H_
#define NETEMU_CLIENT_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "netemu_socket.h"
#include "netemu_kaillera.h"

typedef struct netemu_p2p_internal* netemu_p2p_internal;

struct p2p_player {
	struct netemu_sockaddr_in *addr;
	struct player* player_info;
};

struct netemu_p2p {
	struct netemu_tcp_listener *host;
	struct netemu_tcp_connection **connections;
	struct server_connection *connection;
	netemu_p2p_internal _internal;
	char* cloud_name;
};

struct netemu_p2p* netemu_p2p_new(char* username, char* emulatorname);

int netemu_p2p_connect(struct netemu_p2p* p2p, struct netemu_sockaddr_in *addr, int addr_size);

void netemu_p2p_host(struct netemu_p2p* p2p,struct netemu_sockaddr_in *addr, int addr_size, char* cloudname);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_CLIENT_H_ */
