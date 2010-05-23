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

struct netemu_p2p_connection {
	struct p2p_user *user;
	struct p2p_game *current_game;
	struct netemu_p2p_internal* _internal;
	char* cloud_name;
};

typedef void (*connectedFn)(struct netemu_p2p_connection *, int);
typedef void (*joinHostFn)(struct netemu_p2p_connection *, int);
typedef void (*p2pValuesReceivedFn)(struct netemu_p2p_connection *, char* values, int size);
typedef void (*p2pGameCreatedFn)(struct netemu_p2p_connection *, struct p2p_game *game);
typedef void (*p2pGameStartedFn)(struct netemu_p2p_connection *, struct p2p_game *game);
typedef void (*p2pUserJoinedFn)(struct netemu_p2p_connection *, struct p2p_user *user);
typedef void (*p2pPlayerReadyFn)(struct netemu_p2p_connection *, struct p2p_game *game);
typedef void (*p2pAllReadyFn)(struct netemu_p2p_connection *, struct p2p_game *game);
typedef void (*p2pPlayerJoinedFn)(struct netemu_p2p_connection *, struct p2p_game *game, struct p2p_user*);


union p2p_callback_fn {
	connectedFn connectFn;
	p2pValuesReceivedFn valuesFn;
	p2pGameCreatedFn gameCreatedFn;
	p2pUserJoinedFn userJoinedFn;
	p2pGameStartedFn gameStartedFn;
	p2pPlayerReadyFn playerReadyFn;
	p2pAllReadyFn allReadyFn;
	p2pPlayerJoinedFn playerJoinedFn;
};

struct p2p_callback {
	short disposable;
	union p2p_callback_fn fn;
};


struct netemu_p2p_connection* netemu_p2p_new(char* username, char* emulatorname, char connection_quality);

int netemu_p2p_connect(struct netemu_p2p_connection* p2p, NETEMU_DWORD in_addr, unsigned short port,  NETEMU_DWORD connect_addr, unsigned short connect_port);

int netemu_p2p_connect_async(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *in_addr, int in_addr_size,  netemu_sockaddr_in *connect_addr, int connect_addr_size, connectedFn callback);

int netemu_p2p_host(struct netemu_p2p_connection* p2p, NETEMU_DWORD address, unsigned short port, char* cloudname);

int netemu_p2p_create_game(struct netemu_p2p_connection *connection, char *gamename, char connection_quality, int emulator_value_size, struct p2p_game** result);

void netemu_p2p_login(struct netemu_p2p_connection *p2p);

int netemu_p2p_player_ready(struct netemu_p2p_connection *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

int netemu_p2p_join_game(struct netemu_p2p_connection *connection, struct p2p_game *game);

int netemu_p2p_start_game(struct netemu_p2p_connection *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p_connection* info, int *count);

struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p_connection* info, int *count);

int netemu_p2p_send_play_values(struct netemu_p2p_connection* info, void* data);

int netemu_p2p_register_play_values_received_callback(struct netemu_p2p_connection *connection, p2pValuesReceivedFn callback);

int netemu_p2p_register_game_created_callback(struct netemu_p2p_connection *connection, p2pGameCreatedFn callback);

int netemu_p2p_register_user_joined_callback(struct netemu_p2p_connection *connection, p2pUserJoinedFn callback);

int netemu_p2p_register_game_started_callback(struct netemu_p2p_connection *connection, p2pGameStartedFn callback);

int netemu_p2p_register_player_joined_callback(struct netemu_p2p_connection *connection, p2pPlayerJoinedFn callback);

int netemu_p2p_register_player_ready_callback(struct netemu_p2p_connection *connection, p2pPlayerReadyFn callback);

int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p_connection *connection, p2pAllReadyFn callback);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_CLIENT_H_ */
