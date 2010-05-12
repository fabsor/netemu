/**
 * @file
 * This header file contains functions for using the kaillera protocol.
 */

#ifndef NETEMU_KAILLERA_H_
#define NETEMU_KAILLERA_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../protocol/application_kaillera.h"
#include "../protocol/communication.h"
#include "netemu.h"
#include "netemu_socket.h"

struct netemu_kaillera {
	struct user* user;
	int game_count;
	int user_count;
	char *emulator_name;
	char *username;
	struct game *current_game;
	NETEMU_WORD player_id;
	server_connection_internal _internal;
};

typedef void (* kailleraGameCreatedFn)(struct netemu_kaillera *info, struct game* new_game, void *user_data);
typedef void (* kailleraPlayerJoinFn)(struct netemu_kaillera *info, struct player_joined *result);
typedef void (* valuesReceivedFn)(struct netemu_kaillera *info, struct buffered_play_values *result, void *user_data);
typedef void (* gameStatusUpdatedFn)(struct netemu_kaillera *info, struct game *game, struct game_status_update *update, void *user_data);
typedef void (* gameCreatedFn)(struct netemu_kaillera *info, struct game* new_game);
typedef void (* playerJoinFn)(struct netemu_kaillera *info, struct player_joined *result);
typedef void (* joinFn)(struct netemu_kaillera *info, char *user, NETEMU_DWORD ping, char connection, void *user_data);
typedef void (* playerReadyFn)(struct netemu_kaillera *info);

/* We must store the function pointers in structs since ISO C99 does not allow void* to be typecasted to function pointers. */
union callback_fn {
	chatFn chat_fn;
	joinFn join_fn;
	leaveFn leave_fn;
	kailleraGameCreatedFn game_created_fn;
	kailleraPlayerJoinFn player_join_fn;
	valuesReceivedFn values_received_fn;
	playerJoinFn player_joined_fn;
	playerReadyFn playerReadyFn;
	gameStatusUpdatedFn status_update_fn;
};

struct callback {
	void *user_data;
	short disposable;
	union callback_fn *fn;
};

typedef struct server kaillera_server;
typedef struct existing_game kaillera_existing_game;

struct netemu_kaillera *netemu_kaillera_create(char* user, char* emulator_name);

void netemu_kaillera_network_init(netemu_sockaddr_in *addr, int addr_size);

int kaillera_communication_get_server_list(const char *address, struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

struct netemu_kaillera* netemu_kaillera_connect(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port, NETEMU_DWORD server_address, unsigned short server_port);

void kaillera_communication_connect_async(netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct netemu_kaillera*, void *arg), void* arg);

int netemu_send_chat_message(struct netemu_kaillera *connection, char *message);

int netemu_register_chat_callback(struct netemu_kaillera *connection, chatFn, void *user_data);

int netemu_unregister_chat_callback(struct netemu_kaillera *connection, chatFn);

int netemu_register_user_join_callback(struct netemu_kaillera *connection, joinFn, void *user_data);

int netemu_unregister_user_join_callback(struct netemu_kaillera *connection, joinFn);

int netemu_register_user_leave_callback(struct netemu_kaillera *connection, leaveFn, void *user_data);

int netemu_register_game_status_updated_callback(struct netemu_kaillera *connection, gameStatusUpdatedFn callback, void *user_data);

int netmeu_unregister_user_leave_callback(struct netemu_kaillera *connection, leaveFn);

int netemu_register_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn, void *user_data);

int netemu_unregister_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn);

int netemu_register_player_join_callback(struct netemu_kaillera *connection, playerJoinFn callback, void *user_data);

int netemu_disconnect(struct netemu_kaillera *connection, char *message);

int netemu_kaillera_create_game(struct netemu_kaillera *connection, char *gamename, struct game** result);

void netemu_kaillera_create_game_async(struct netemu_kaillera *connection, char *gamename, kailleraGameCreatedFn fn);

int netemu_kaillera_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn);

int netemu_kaillera_join_game(struct netemu_kaillera *connection, NETEMU_DWORD gameid);

struct game** netemu_kaillera_get_game_list(struct netemu_kaillera* connection, int *count);

struct user** netemu_kaillera_get_user_list(struct netemu_kaillera* info, int *count);

int netemu_kaillera_send_player_ready(struct netemu_kaillera *connection);

int netemu_register_play_values_received_callback(struct netemu_kaillera *connection, valuesReceivedFn fn, void *user_data);

int netemu_unregister_play_values_received_callback(struct netemu_kaillera *connection, valuesReceivedFn fn);

void netemu_kaillera_send_play_values(struct netemu_kaillera* connection, int size, void* data);

int netemu_kaillera_start_game(struct netemu_kaillera *info);


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_KAILLERA_H_ */
