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

/**
 * @file
 * This file contains the interface for netemu_p2p.
 */

/**
 * @defgroup netemu_p2p The netemu p2p module
 * The netemu_p2p module implements the netemu_p2p interface.
 * It contains all external code that can be used by clients for
 * using the netemu_p2p protocol.
 */

#pragma once
#ifndef NETEMU_P2P_H_
#define NETEMU_P2P_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../protocol/application_p2p.h"
#include "netemu_socket.h"
#include "netemu_kaillera.h"

struct netemu_p2p {
	struct p2p_user *user;
	struct p2p_game *current_game;
	struct netemu_p2p_internal* _internal;
	char* cloud_name;
};

typedef void (*connectedFn)(struct netemu_p2p *, int);
typedef void (*joinHostFn)(struct netemu_p2p *, int);
typedef void (*p2pValuesReceivedFn)(struct netemu_p2p *, char* values, int size);
typedef void (*p2pGameCreatedFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pGameStartedFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pUserJoinedFn)(struct netemu_p2p *, struct p2p_user *user);
typedef void (*p2pPlayerReadyFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pAllReadyFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pPlayerJoinedFn)(struct netemu_p2p *, struct p2p_game *game, struct p2p_user*);


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
	void *user_data;
};

/**
 * Call this function to initialize the network.
 * You only need to call this once.
 */
int netemu_p2p_initialize();

struct netemu_p2p* netemu_p2p_new(char* username, char* emulatorname, char connection_quality);

int netemu_p2p_connect(struct netemu_p2p* p2p, NETEMU_DWORD in_addr, unsigned short port,  NETEMU_DWORD connect_addr, unsigned short connect_port);

int netemu_p2p_connect_async(struct netemu_p2p* p2p, netemu_sockaddr_in *in_addr, int in_addr_size,  netemu_sockaddr_in *connect_addr, int connect_addr_size, connectedFn callback);

int netemu_p2p_host(struct netemu_p2p* p2p, NETEMU_DWORD address, unsigned short port, char* cloudname);

int netemu_p2p_create_game(struct netemu_p2p *connection, char *gamename, char connection_quality, int emulator_value_size, struct p2p_game** result);

void netemu_p2p_login(struct netemu_p2p *p2p);

int netemu_p2p_player_ready(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

int netemu_p2p_join_game(struct netemu_p2p *connection, struct p2p_game *game);

int netemu_p2p_start_game(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p* info, int *count);

struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p* info, int *count);

int netemu_p2p_send_play_values(struct netemu_p2p* info, void* data);

int netemu_p2p_register_play_values_received_callback(struct netemu_p2p *connection, p2pValuesReceivedFn callback, void *user_data);

int netemu_p2p_register_game_created_callback(struct netemu_p2p *connection, p2pGameCreatedFn callback, void *user_data);

int netemu_p2p_register_user_joined_callback(struct netemu_p2p *connection, p2pUserJoinedFn callback, void *user_data);

int netemu_p2p_register_game_started_callback(struct netemu_p2p *connection, p2pGameStartedFn callback, void *user_data);

int netemu_p2p_register_player_joined_callback(struct netemu_p2p *connection, p2pPlayerJoinedFn callback, void *user_data);

int netemu_p2p_register_player_ready_callback(struct netemu_p2p *connection, p2pPlayerReadyFn callback, void *user_data);

int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p *connection, p2pAllReadyFn callback, void *user_data);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_P2P_H_ */
