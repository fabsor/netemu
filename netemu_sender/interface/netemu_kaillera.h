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

struct netemu_info {
	struct netemu_sockaddr_in *addr;
	struct user* user;
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

void netemu_kaillera_network_init(struct netemu_sockaddr_in *addr, int addr_size);

int kaillera_communication_get_server_list(struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

struct netemu_info* kaillera_communication_connect(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username);

void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct netemu_info*));

int netemu_send_chat_message(struct netemu_info *connection, char *message);

int netemu_register_chat_callback(struct netemu_info *connection, chatFn);

int netemu_unregister_chat_callback(struct netemu_info *connection, chatFn);

int netemu_register_user_join_callback(struct netemu_info *connection, joinFn);

int netemu_unregister_user_join_callback(struct netemu_info *connection, joinFn);

int netemu_register_user_leave_callback(struct netemu_info *connection, leaveFn);

int netmeu_unregister_user_leave_callback(struct netemu_info *connection, leaveFn);

int netemu_disconnect(struct netemu_info *connection, char *message);

int netemu_create_game(struct netemu_info *connection, char *gamename, struct game** result);

void netemu_create_game_async(struct netemu_info *connection, char *gamename, gameCreatedFn fn);

int netemu_game_created_callback(struct netemu_info *connection, gameCreatedFn);

int netemu_join_game(struct netemu_info *connection, NETEMU_DWORD gameid);

struct game** netemu_get_game_list(struct netemu_info* connection, int *count);

int netemu_send_player_ready(struct netemu_info *connection);

int netemu_register_play_values_received_callback(struct netemu_info *connection, valuesReceivedFn fn);

int netemu_unregister_play_values_received_callback(struct netemu_info *connection, valuesReceivedFn fn);

void netemu_send_play_values(struct netemu_info* connection, int size, void* data);


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_KAILLERA_H_ */
