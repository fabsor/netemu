/**
 * @file
 * This header file contains functions for using the kaillera protocol.
 */

/**
 * @defgroup netemu_kaillera netemu_kaillera module
 * This module contains everything external developers need to
 * interact with kaillera servers.
 */

#ifndef NETEMU_KAILLERA_H_
#define NETEMU_KAILLERA_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../protocol/application_kaillera.h"
#include "../protocol/communication.h"
#include "netemu_socket.h"

/*! Error returned if you're in a game already. */
#define NETEMU_EINKAILLERAGAME 50001

typedef struct _netemu_info_internal *server_connection_internal;

/**
 * This struct is the base for the netemu_kaillera module
 * @ingroup netemu_kaillera
 */
struct netemu_kaillera {
	struct user* user; /**< The user which this instance of the module is logged in as. */
	int game_count; /**< The number of games on the server. */
	int user_count; /**< The number of users on the server. */
	char *emulator_name; /**< The name of the emulator */
	char *username; /**< The username */
	struct game *current_game; /**< The game this instance currently participates in. */
	NETEMU_WORD player_id; /**< the player id */
	server_connection_internal _internal; /**< Internal members that is used internally. Do not try to modify! */
};

typedef void (* kailleraGameCreatedFn)(struct netemu_kaillera *info, struct game* new_game, void *user_data);
typedef void (* kailleraPlayerJoinFn)(struct netemu_kaillera *info, struct player_joined *result);
typedef void (* valuesReceivedFn)(struct netemu_kaillera *info, struct buffered_play_values *result, void *user_data);
typedef void (* gameStatusUpdatedFn)(struct netemu_kaillera *info, struct game *game, struct game_status_update *update, void *user_data);
typedef void (* gameCreatedFn)(struct netemu_kaillera *info, struct game* new_game, void *user_data);
typedef void (* gameStartedFn)(struct netemu_kaillera *info, struct game* game, struct game_start *start, void* user_data);
typedef void (* playerJoinFn)(struct netemu_kaillera *info, struct player_joined *result);
typedef void (* joinFn)(struct netemu_kaillera *info, char *user, NETEMU_DWORD ping, char connection, void *user_data);
typedef void (* playerReadyFn)(struct netemu_kaillera *info);
typedef void (* chatFn)(char *user, char *message, void *user_data);
typedef void (* leaveFn)(NETEMU_WORD id, char *user, char *exit_message, void *user_data);
typedef void (* cachedValuesReceivedFn)(struct intelligently_cached_buffered_play_values *result, void *user_data);
typedef void (* connectionAcquiredFn) (struct netemu_kaillera *info, int status, void *user_data);

/* We must store the function pointers in structs since ISO C99 does not allow void* to be typecasted to function pointers. */
/**
 * This union contains the different callback functions that can be used and registered.
 * @ingroup netemu_kaillera
 */
union callback_fn {
	chatFn chat_fn;
	joinFn join_fn;
	leaveFn leave_fn;
	kailleraGameCreatedFn game_created_fn;
	kailleraPlayerJoinFn player_join_fn;
	valuesReceivedFn values_received_fn;
	cachedValuesReceivedFn cached_values_received_fn;
	playerJoinFn player_joined_fn;
	playerReadyFn player_ready_fn;
	gameStatusUpdatedFn status_update_fn;
	gameStartedFn game_started_fn;
	connectionAcquiredFn connection_acquired_fn;
};
/**
 * This struct represents a callback
 * @ingroup netemu_kaillera
 */
struct callback {
	void *user_data;
	short disposable;
	union callback_fn *fn;
};

typedef struct server kaillera_server;
typedef struct existing_game kaillera_existing_game;

struct netemu_kaillera *netemu_kaillera_create(char* user, char* emulator_name, int conneciton_quality);

void netemu_kaillera_network_init(netemu_sockaddr_in *addr, int addr_size);

int kaillera_communication_get_server_list(const char *address, struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

/* void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server))); */

struct netemu_kaillera* netemu_kaillera_connect(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port, NETEMU_DWORD server_address, unsigned short server_port);

int netemu_kaillera_connect_async(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port,
		NETEMU_DWORD server_address, unsigned short server_port, connectionAcquiredFn connectionFn, void* user_data);

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

int netemu_kaillera_disconnect(struct netemu_kaillera *connection, char *message);

int netemu_kaillera_create_game(struct netemu_kaillera *connection, char *gamename, struct game** result);

void netemu_kaillera_create_game_async(struct netemu_kaillera *connection, char *gamename, kailleraGameCreatedFn fn);

int netemu_kaillera_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn);

int netemu_kaillera_join_game(struct netemu_kaillera *connection, NETEMU_DWORD gameid);

int netemu_kaillera_join_game_async(struct netemu_kaillera *info, NETEMU_DWORD gameid, joinFn fn);

struct game** netemu_kaillera_get_game_list(struct netemu_kaillera* connection, int *count);

struct user** netemu_kaillera_get_user_list(struct netemu_kaillera* info, int *count);

int netemu_kaillera_send_player_ready(struct netemu_kaillera *connection);

int netemu_register_play_values_received_callback(struct netemu_kaillera *connection, valuesReceivedFn fn, void *user_data);

int netemu_unregister_play_values_received_callback(struct netemu_kaillera *connection, valuesReceivedFn fn);

int netemu_kaillera_send_play_values(struct netemu_kaillera* info, int size, void* data);

int netemu_register_cached_values_received_callback(struct netemu_kaillera *connection, cachedValuesReceivedFn fn, void *user_data);

int netemu_kaillera_start_game(struct netemu_kaillera *info);

int netemu_kaillera_start_game_async(struct netemu_kaillera *info, gameStartedFn fn);

int netemu_register_game_started_callback(struct netemu_kaillera *connection, gameStartedFn callback, void *user_data);

int netemu_kaillera_leave_game(struct netemu_kaillera *info);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_KAILLERA_H_ */
