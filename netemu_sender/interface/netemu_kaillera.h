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

typedef void (* kailleraGameCreatedFn)(struct game* new_game, void *user_data);
typedef void (* kailleraPlayerJoinFn)(struct player_joined *result);

typedef void (* gameCreatedFn)(struct game* new_game);
typedef void (* playerJoinFn)(struct player_joined *result);


/* We must store the function pointers in structs since ISO C99 does not allow void* to be typecasted to function pointers. */
union callback_fn {
	chatFn chat_fn;
	joinFn join_fn;
	leaveFn leave_fn;
	kailleraGameCreatedFn game_created_fn;
	kailleraPlayerJoinFn player_join_fn;
	valuesReceivedFn values_received_fn;
	cachedValuesReceivedFn cached_values_received_fn;
};

struct callback {
	void *user_data;
	short disposable;
	union callback_fn *fn;
};


struct netemu_kaillera_connection {
	struct netemu_info *info;
};

typedef struct server kaillera_server;
typedef struct existing_game kaillera_existing_game;

void netemu_kaillera_network_init(netemu_sockaddr_in *addr, int addr_size);

int kaillera_communication_get_server_list(const char *address, struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

struct netemu_info* kaillera_communication_connect(netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username);


void kaillera_communication_connect_async(netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct netemu_info*, void *arg), void* arg);

int netemu_send_chat_message(struct netemu_info *connection, char *message);

int netemu_register_chat_callback(struct netemu_info *connection, chatFn, void *user_data);

int netemu_unregister_chat_callback(struct netemu_info *connection, chatFn);

int netemu_register_user_join_callback(struct netemu_info *connection, joinFn, void *user_data);

int netemu_unregister_user_join_callback(struct netemu_info *connection, joinFn);

int netemu_register_user_leave_callback(struct netemu_info *connection, leaveFn, void *user_data);

int netmeu_unregister_user_leave_callback(struct netemu_info *connection, leaveFn);

int netemu_register_game_created_callback(struct netemu_info *connection, kailleraGameCreatedFn, void *user_data);

int netemu_unregister_game_created_callback(struct netemu_info *connection, kailleraGameCreatedFn);

int netemu_disconnect(struct netemu_info *connection, char *message);

int netemu_kaillera_create_game(struct netemu_info *connection, char *gamename, struct game** result);

void netemu_kaillera_create_game_async(struct netemu_info *connection, char *gamename, kailleraGameCreatedFn fn);

int netemu_kaillera_game_created_callback(struct netemu_info *connection, kailleraGameCreatedFn);

int netemu_kaillera_join_game(struct netemu_info *connection, NETEMU_DWORD gameid);

struct game** netemu_kaillera_get_game_list(struct netemu_info* connection, int *count);

struct user** netemu_kaillera_get_user_list(struct netemu_info* info, int *count);

int netemu_kaillera_send_player_ready(struct netemu_info *connection);

int netemu_register_play_values_received_callback(struct netemu_info *connection, valuesReceivedFn fn, void *user_data);

int netemu_unregister_play_values_received_callback(struct netemu_info *connection, valuesReceivedFn fn);

int netemu_register_cached_values_received_callback(struct netemu_info *connection, cachedValuesReceivedFn fn, void *user_data);

void netemu_kaillera_send_play_values(struct netemu_info* connection, int size, void* data);

int netemu_kaillera_start_game(struct netemu_info *info);


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_KAILLERA_H_ */
