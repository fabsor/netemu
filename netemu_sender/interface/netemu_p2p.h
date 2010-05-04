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

#include "../protocol/application_p2p.h"
#include "netemu_socket.h"
#include "netemu_kaillera.h"

typedef struct netemu_p2p_internal* netemu_p2p_internal;

struct netemu_p2p_connection {
	struct netemu_info* info;
	struct p2p_user *user;
	struct p2p_game *current_game;
	netemu_p2p_internal _internal;
	char* cloud_name;
};

struct netemu_p2p_connection* netemu_p2p_new(char* username, char* emulatorname);

int netemu_p2p_connect(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *in_addr, int in_addr_size,  netemu_sockaddr_in *connect_addr, int connect_addr_size);

void netemu_p2p_host(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *addr, int addr_size, char* cloudname);

int netemu_p2p_create_game(struct netemu_p2p_connection *connection, char *gamename, struct game** result);

void netemu_p2p_login(struct netemu_p2p_connection *p2p);

int netemu_p2p_join_game(struct netemu_p2p_connection *connection, struct p2p_user *creator);

struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p_connection* info, int *count);

struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p_connection* info, int *count);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_CLIENT_H_ */
