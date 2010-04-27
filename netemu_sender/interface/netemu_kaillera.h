/**
 * @file
 * This header file contains functions for using the kaillera protocol.
 */

#ifndef NETEMU_KAILLERA_H_
#define NETEMU_KAILLERA_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../protocol/application.h"
#include "netemu_socket.h"
typedef struct _server_connection_internal *server_connection_internal;
typedef void (* chatFn)(char *user, char *message);
typedef void (* joinFn)(char *user);
typedef void (* leaveFn)(char *user);
typedef void (* gameCreatedFn)(struct game* new_game);
typedef void (* playerJoinFn)(struct player_joined *result);
typedef void (* valuesReceivedFn)(struct buffered_play_values *result);

/* We must store the function pointers in structs since ISO C99 does not allow void* to be typecasted to function pointers. */
union callback_fn {
	chatFn chat_fn;
	joinFn join_fn;
	leaveFn leave_fn;
	gameCreatedFn game_created_fn;
	playerJoinFn player_join_fn;
	valuesReceivedFn valuesReceivedFn;
};

struct callback {
	short disposable;
	union callback_fn *fn;
};

struct server_connection {
	struct netemu_sockaddr_in *addr;
	char *user;
	int game_count;
	int user_count;
	char *emulator_name;
	struct game *current_game;
	char has_id;
	NETEMU_WORD player_id;
	server_connection_internal _internal;
};

typedef struct server kaillera_server;
typedef struct existing_game kaillera_existing_game;

int kaillera_communication_get_server_list(struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

struct server_connection* kaillera_communication_connect(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username);

void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct server_connection*));

int netemu_send_chat_message(struct server_connection *connection, char *message);

int netemu_register_chat_callback(struct server_connection *connection, chatFn);

int netemu_unregister_chat_callback(struct server_connection *connection, chatFn);

int netemu_register_user_join_callback(struct server_connection *connection, joinFn);

int netemu_unregister_user_join_callback(struct server_connection *connection, joinFn);

int netemu_register_user_leave_callback(struct server_connection *connection, leaveFn);

int netmeu_unregister_user_leave_callback(struct server_connection *connection, leaveFn);

int netemu_disconnect(struct server_connection *connection, char *message);

int netemu_create_game(struct server_connection *connection, char *gamename, struct game** result);

void netemu_create_game_async(struct server_connection *connection, char *gamename, gameCreatedFn fn);

int netemu_game_created_callback(struct server_connection *connection, gameCreatedFn);

int netemu_join_game(struct server_connection *connection, NETEMU_DWORD gameid);

struct server_connection *netemu_server_connection_new(char* username, char* emulator_name);

struct game** netemu_get_game_list(struct server_connection* connection, int *count);

int netemu_send_player_ready(struct server_connection *connection);

int netemu_register_play_values_received_callback(struct server_connection *connection, valuesReceivedFn fn);

int netemu_unregister_play_values_received_callback(struct server_connection *connection, valuesReceivedFn fn);

void netemu_send_play_values(struct server_connection* connection, int size, void* data);


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_KAILLERA_H_ */
