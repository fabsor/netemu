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
 * @defgroup netemu_p2p Netemu P2P
 * The netemu_p2p module implements the netemu_p2p interface.
 * It contains all external code that can be used by clients for
 * using the netemu_p2p protocol.
 */

/**
 * @defgroup netemu_p2p_callbacks Netemu P2P callbacks.
 * Callbacks that can be registered which is associated with the netemu p2p module.
 */

#pragma once
#ifndef NETEMU_P2P_H_
#define NETEMU_P2P_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../protocol/application_p2p.h"
#include "netlib_network.h"
#include "netemu_kaillera.h"
/**
 * This struct is the base of the netemu_p2p module.
 */
struct netemu_p2p {
	struct p2p_user *user; /**< The current user. */
	struct p2p_game *current_game; /**< The current game. NULL if you're not in a game. */
	struct netemu_p2p_internal* _internal;
	char* cloud_name; /**< The name of the cloud. */
};

/**
 * @todo
 * We really need functions indicating when something go wrong too.
 */

typedef void (*connectedFn)(struct netemu_p2p *, int);
typedef void (*joinHostFn)(struct netemu_p2p *, int);
typedef void (*p2pValuesReceivedFn)(struct netemu_p2p *, char* values, int size);
typedef void (*p2pGameCreatedFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pGameStartedFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pUserJoinedFn)(struct netemu_p2p *, struct p2p_user *user);
typedef void (*p2pPlayerReadyFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pAllReadyFn)(struct netemu_p2p *, struct p2p_game *game);
typedef void (*p2pPlayerJoinedFn)(struct netemu_p2p *, struct p2p_game *game, struct p2p_user*);

/**
 * Call this function to initialize the network.
 * You only need to call this once.
 * @ingroup netemu_p2p
 */
int netemu_p2p_initialize();

/**
 * Create a new netemu_p2p instance.
 * This function creates a new p2p instance.
 * @ingroup netemu_p2p
 * @param username the username that you want to use in this p2p instance.
 * @param application name the name of the application.
 * @param connection_quality The connection quality that you want to use. The connection quality
 * decides how fast you send data packets.\n <b>Example:</b>\n
 * If you have a connection_quality of 1, then you will send one play value, then wait for all others.\n
 * If you have a connection quality of 2, then you will send two play values, then wait for all others.\n
 * etc.
 * @return An instance of the netemu_p2p module if everything went fine, NULL otherwise.
 * @todo This function needs error handling
 */
struct netemu_p2p* netemu_p2p_create(char* username, char* emulatorname, char connection_quality);

/**
 * Connect to a host. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte order.
 */
int netemu_p2p_connect(struct netemu_p2p* p2p, NETEMU_DWORD in_addr, unsigned short port,  NETEMU_DWORD connect_addr, unsigned short connect_port);

/**
 * Connect asyncronously to a cloud. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @todo This function needs to be refactored to use addresses and ports as long and short instead of structs.
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte orde
 */
int netemu_p2p_connect_async(struct netemu_p2p* p2p, netlib_sockaddr_in *in_addr, int in_addr_size,  netlib_sockaddr_in *connect_addr, int connect_addr_size, connectedFn callback);

/**
 * Host a new P2P cloud.
 * This will let other users connect to you on the designated port. By calling this function,
 * you will initiate a new netemu cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param address the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 */
int netemu_p2p_host(struct netemu_p2p* p2p, NETEMU_DWORD address, unsigned short port, char* cloudname);

/**
 * Create a new game. This function will create a new game which will be broadcasted to all
 * peers. This is a non-blocking call, since there's no requirement (or a way to check) that
 * all other users have received the notification that the game have been created.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param gamename the name of the game you want to create
 * @param connection_quality the quality of the connection for this game.
 * @param emulator_value_size This is the size of the play values for one player in the game.
 * No value sent over the network in this game can be larger than this.
 * @param result the created game.
 * @return The return value can be:
 * - 0 if all went well
 * - -1 if something went wrong. This generally indicates that you already have created a game.
 */
int netemu_p2p_create_game(struct netemu_p2p *connection, char *gamename, char connection_quality, int emulator_value_size, struct p2p_game** result);

/**
 * @todo
 * Should netemu_p2p_login be in the public interface??
 */
void netemu_p2p_login(struct netemu_p2p *p2p);

/**
 * Indicate that you are ready to play. In order for the game to start, you must call this function.
 * This function will send a player ready instruction to all players, and this will
 * cause them to make register the address to which they can contact you wile playing.
 * If you are the creator of the game, you don't have to call this function, since netemu_p2p_start_game will
 * do everything this function does for you. Also note that you have to recieve the start game instruction before
 * calling this function. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the other players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 0 if everything went OK
 * - -1 if you're not in a game, if you're the creator of this game or if the game hasn't started.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_player_ready(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

/**
 * Join an existing game. This is a blocking call that will connect to the creator of the
 * game if there isn't a connection already, and then send a "Join game" instruction.
 * @param connection an instance of the netemu_p2p module.
 * @param game the game you want to join. Use netemu_get_game_list to get a game to join.
 * @return the return value can be:
 * - 0 if everything went alright.
 * - -1 if you're already in a game.
 * - -2 if you try to join a game which you have created.
 * @todo we need more errors here for:
 * - if you can't connect to a host.
 * - if the host refuses you.
 */
int netemu_p2p_join_game(struct netemu_p2p *connection, struct p2p_game *game);

int netemu_p2p_join_game_async(struct netemu_p2p *connection, struct p2p_game *game, joinFn fn, void* user_data);

/**
 * Start a game. In order for the game to start, you must call this function.
 * This function will send a start game instruction to all players, and this will
 * cause them to make the necessary arrangement for playing the game such as binding to
 * UDP ports. If you are the creator of the game, the only thing you have to do is
 * call this function and it will be implicated that you are ready to play. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 0 if everything went OK
 * - -1 if you're not in a game or if you're not the owner.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_start_game(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port);

/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p* info, int *count);

/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p* info, int *count);

/**
 * Send play values to all clients.
 * This function will block if you haven't received play values from your co-players, and
 * will handle all the synchronization for you.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param data the data to send. The block must contain data with the size of emulator_value_size, and be big enough to contain
 * data from all players.
 * @return 0 if everything went alright.
 * @todo Error handling here.
 */
int netemu_p2p_send_play_values(struct netemu_p2p* info, void* data);

/**
 * Register a callback to be called when play values have been recieved.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 * @todo We probably don't need a callback for play_values_received.
 */
int netemu_p2p_register_play_values_received_callback(struct netemu_p2p *connection, p2pValuesReceivedFn callback, void *user_data);

/**
 * Register a callback to be called when a game has been created.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_game_created_callback(struct netemu_p2p *connection, p2pGameCreatedFn callback, void *user_data);

/**
 * Register a callback to be called when a user has joined the cloud.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_user_joined_callback(struct netemu_p2p *connection, p2pUserJoinedFn callback, void *user_data);

/**
 * Register a callback to be called when a game has started.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_game_started_callback(struct netemu_p2p *connection, p2pGameStartedFn callback, void *user_data);

/**
 * Register a callback to be called when a player has joined the game you're in.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_player_joined_callback(struct netemu_p2p *connection, p2pPlayerJoinedFn callback, void *user_data);

/**
 * Register a callback to be called when a player is ready.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_player_ready_callback(struct netemu_p2p *connection, p2pPlayerReadyFn callback, void *user_data);

/**
 * Register a callback to be called when all players are ready to play.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p *connection, p2pAllReadyFn callback, void *user_data);

/**
 * @todo
 * Add unregister_callback functions for all callbacks.
 */
#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_P2P_H_ */
