/*
 * server_connection.h
 *
 *  Created on: 5 apr 2010
 *      Author: emil
 */

#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netemu_socket.h"
#include "netlib_util.h"
#include "../protocol/application.h"

typedef struct _server_connection_internal *server_connection_internal;
typedef void (* chatFn)(char *user, char *message);
typedef void (* joinFn)(char *user);
typedef void (* leaveFn)(char *user);
typedef void (* createGameFn)(int success, struct game* result);
typedef void (* gameCreatedFn)(struct game* new_game);
typedef void (* playerJoinFn)(struct player_joined *result);

struct server_connection {
	struct netemu_sockaddr_in *addr;
	char *user;
	server_connection_internal _internal;
};

int server_connection_send_chat_message(struct server_connection *connection, char *message);

int server_connection_register_chat_callback(struct server_connection *connection, chatFn);

int server_connection_unregister_chat_callback(struct server_connection *connection, chatFn);

int server_connection_register_user_join_callback(struct server_connection *connection, joinFn);

int server_connection_unregister_user_join_callback(struct server_connection *connection, joinFn);

int server_connection_register_user_leave_callback(struct server_connection *connection, leaveFn);

int server_connection_unregister_user_leave_callback(struct server_connection *connection, leaveFn);

int server_connection_disconnect(struct server_connection *connection, char *message);

int server_connection_create_game(struct server_connection *connection, char *gamename, struct game* result);

int server_connection_create_game_async(struct server_connection *connection, char *gamename, createGameFn);

int server_connection_register_game_created_callback(struct server_connection *connection, gameCreatedFn);

int server_connection_join_game(struct server_connection *connection, NETEMU_DWORD gameid, struct player_joined *result);

int server_connection_join_game_async(struct server_connection *connection, NETEMU_DWORD gameid, playerJoinFn);

struct server_connection *server_connection_new(char* username);

#ifdef	__cplusplus
}
#endif

#endif /* SERVER_CONNECTION_H_ */
