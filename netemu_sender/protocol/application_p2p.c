/*
 * application_p2p.c
 *
 *  Created on: 29 apr 2010
 *      Author: fabbe
 */
#define CREATE_P2P_GAME	25
#define JOIN_P2P_GAME	26
#define LEAVE_P2P_GAME 	27
#include "application.h"
#include "../netemu_util.h"
struct p2p_game {
	char* name;
	char* app_name;
	int user_count;
	struct user* creator;
	struct netemu_user_list *players;
};

struct p2p_join_game {
	char *name;
};

struct p2p_leave_game {
	char *name;
};

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct user* creator) {
	struct p2p_game *game;
	int size;
	game = malloc(sizeof(struct game));
	size = netemu_util_copy_string(&game->name,gamename);
	size += netemu_util_copy_string(&game->app_name,appname);
	game->creator = creator;
	instruction->body = game;
	instruction->id = CREATE_P2P_GAME;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_create_game_pack;
}



void netemu_application_p2p_join_game_add(struct application_instruction* instruction, char* name) {
	struct p2p_join_game *join_game;
	int size;
	join_game = malloc(sizeof(struct p2p_join_game));
	size = netemu_util_copy_string(&join_game->name,name);
	instruction->body = join_game;
	instruction->id = JOIN_P2P_GAME;
}

void netemu_application_leave_game_add(struct application_instruction* instruction) {
	instruction->body_size = 0;
	instruction->id = LEAVE_P2P_GAME;
	instruction->body = NULL;
	instruction->body_size = 0;
	instruction->packBodyFn = NULL;
}

void netemu_application_kick_player(struct application_instruction *instruction, char* player_name) {

}
