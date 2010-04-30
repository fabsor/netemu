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
#define CREATE_P2P_GAME		25
#define JOIN_P2P_GAME		26
#define LEAVE_P2P_GAME 		27
#define KICK_P2P_PLAYER 	27
#define P2P_LOGIN_SUCCESS	28
#define P2P_LOGIN_REQUEST	29
#define P2P_READY			30
#include "application.h"
#include "../netemu_util.h"

struct p2p_user {
	char *name;
	char *app_name;
	netemu_sockaddr *addr;
	size_t addr_size;
	NETEMU_DWORD ping;
	char connection;
};

struct p2p_game {
	char* name;
	char* app_name;
	NETEMU_WORD user_count;
	struct p2p_user* creator;
	struct p2p_user* players;
};

struct p2p_join_game {
	char *name;
};

struct p2p_kick_player {
	char *name;
};

struct p2p_login_success {
	NETEMU_WORD users_count;
	NETEMU_WORD games_count;
	struct p2p_user *users;
	struct p2p_game *games;
};

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct p2p_user* creator);

void netemu_application_p2p_join_game_add(struct application_instruction* instruction, char* name);

void netemu_application_p2p_leave_game_add(struct application_instruction* instruction);

void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_success_add(struct application_instruction *instruction, const unsigned int noUsers, struct p2p_user *users, const unsigned int noGames, struct p2p_game *games);

void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_add(struct application_instruction *instruction);

void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name);

void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer);

void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, netemu_sockaddr *addr, size_t addr_size, char* username, char* appname, int connection);

void netemu_application_p2p_ready_add(struct application_instruction *instruction);
#ifdef	__cplusplus
}
#endif


#endif /* APPLICATION_P2P_H_ */
