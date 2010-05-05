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
#include "application.h"
#include "../netemu_util.h"


typedef struct p2p_user_internal* p2p_user_internal;
/* TODO: This can't be here but I don't have the time to play nice right now =)*/
struct p2p_user_internal {
	struct netemu_tcp_connection *connection;
	struct netemu_sender_udp *sender;
};


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

void netemu_application_p2p_leave_game_add(struct application_instruction* instruction);

void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_success_add(struct application_instruction *instruction,struct p2p_user *host, struct netemu_list *users, struct netemu_list *games);

void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name);

void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer);

void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, netemu_sockaddr *addr, size_t addr_size, char* username, char* appname, int connection);

void netemu_application_p2p_ready_add(struct application_instruction *instruction);

int netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user);

int netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game);

void netemu_application_p2p_create_game_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_add(struct application_instruction *instruction, struct p2p_user *user);

struct p2p_user_internal* netemu_application_p2p_create_user_internal();
#ifdef	__cplusplus
}
#endif


#endif /* APPLICATION_P2P_H_ */
