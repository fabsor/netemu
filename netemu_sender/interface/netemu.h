/**
 * netemu.h
 * This header file contains functions and structs that are used by both the p2p and and kaillera interfaces.
 */

#ifndef NETEMU_H_
#define NETEMU_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "netlib_util.h"
typedef void (* chatFn)(char *user, char *message, void *user_data);
typedef void (* joinFn)(char *user, void *user_data);
typedef void (* leaveFn)(char *user, void *user_data);
typedef void (* valuesReceivedFn)(struct buffered_play_values *result, void *user_data);

typedef struct _netemu_info_internal *server_connection_internal;

struct netemu_info {
	struct user* user;
	int game_count;
	int user_count;
	char *emulator_name;
	char *username;
	struct game *current_game;
	NETEMU_WORD player_id;
	server_connection_internal _internal;
};


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_H_ */
