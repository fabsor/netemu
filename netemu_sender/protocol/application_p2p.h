/*
 * application_p2p.h
 *
 *  Created on: 29 apr 2010
 *      Author: fabian
 */

#ifndef APPLICATION_P2P_H_
#define APPLICATION_P2P_H_

#ifdef	__cplusplus
extern "C" {
#endif
#define CREATE_P2P_GAME		30
#define JOIN_P2P_GAME		31
#define LEAVE_P2P_GAME 		32
#define KICK_P2P_PLAYER 	33
#define P2P_LOGIN_SUCCESS	34
#define P2P_LOGIN_REQUEST	35
#define P2P_READY			36
#define P2P_USER_JOIN		37
#define P2P_JOIN_HOST		38
#define P2P_PLAYER_READY	39
#define P2P_GAME_START		40
#include "application_kaillera.h"
#include "../netemu_util.h"


typedef struct p2p_user_internal* p2p_user_internal;
typedef struct p2p_game_internal* p2p_game_internal;

struct p2p_user {
	char *name;
	char *app_name;
	netemu_sockaddr *addr;
	size_t addr_size;
	NETEMU_DWORD ping;
	char connection;
	p2p_user_internal _internal; /**< Secret stuff =) */
};

struct p2p_game {
	char* name;
	char* app_name;
	NETEMU_WORD user_count;
	struct p2p_user* creator;
	struct p2p_user* players;
	NETEMU_BOOL started;
	p2p_game_internal _internal;
};

struct p2p_kick_player {
	char *name;
};

struct p2p_start_game {
	netemu_sockaddr* addr;
	size_t addr_size;
};

struct p2p_login_success {
	NETEMU_WORD users_count;
	NETEMU_WORD games_count;
	struct p2p_user *users;
	struct p2p_game *games;
};


#ifdef	__cplusplus
}
#endif


#endif /* APPLICATION_P2P_H_ */
