/**
 * @file
 * This file contains functions for handling p2p protocol messages.
 */

#include "netlib_util.h"
#include "../netemu_util.h"
#include "application_p2p.h"

int _netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user);
int _netemu_application_p2p_pack_user(char* buffer, struct p2p_user *user);
int _netemu_application_p2p_parse_user(char* buffer, struct p2p_user *user, NETEMU_BOOL parse_user);

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct p2p_user* creator) {
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

void netemu_application_p2p_leave_game_add(struct application_instruction* instruction) {
	instruction->body_size = 0;
	instruction->id = LEAVE_P2P_GAME;
	instruction->body = NULL;
	instruction->body_size = 0;
	instruction->packBodyFn = NULL;
}

void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer) {
	struct p2p_login_success* success;
	int i;

	success = malloc(sizeof(struct p2p_login_success));
	memcpy(&success->users_count,buffer,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(&success->games_count,buffer,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);

	for(i = 0; i < success->users_count; i++) {
		buffer += _netemu_application_p2p_pack_user(buffer,&success->users[i]);
	}

	for(i = 0; i < success->games_count; i++) {
		success->games[i].name = netemu_util_parse_string(buffer);
		buffer += strlen(success->games[i].name)+1;
		success->games[i].app_name = netemu_util_parse_string(buffer);
		buffer += strlen(success->games[i].app_name)+1;
		memcpy(&success->games[i].user_count, buffer, sizeof(NETEMU_DWORD));
		buffer += sizeof(NETEMU_DWORD);
		buffer += _netemu_application_p2p_parse_user(buffer,success->games[i].creator, 1);
	}
}

void netemu_application_p2p_login_success_add(struct application_instruction *instruction, const unsigned int noUsers, struct p2p_user *users, const unsigned int noGames, struct p2p_game *games) {
	struct p2p_login_success *success;
	struct p2p_game *game;
	struct p2p_user *user;
	int size, game_size, user_size, i;

	success = malloc(sizeof(struct p2p_login_success));
	game_size = sizeof(struct p2p_game)*noGames;
	user_size = sizeof(struct p2p_user)*noUsers;
	success->users = malloc(game_size);

	success->games = malloc(user_size);
	success->users_count = noUsers;
	success->games_count = noGames;

	for(i = 0; i < noUsers; i++) {
		user = success->users+(sizeof(struct p2p_user)*i);
		size += _netemu_application_p2p_copy_user(user,&users[i]) - sizeof(char*);
	}

	for(i = 0; i < noGames; i++) {
		game = success->games+(sizeof(struct p2p_game)*i);
		size += netemu_util_copy_string(&game->app_name, games[i].app_name);
		size += netemu_util_copy_string(&game->name, games[i].name);
		game->user_count = games[i].user_count;
		game->players = NULL;
		size += _netemu_application_p2p_copy_user(game->creator,games[i].creator) - sizeof(char*);
	}

	instruction->body = success;
	instruction->id = P2P_LOGIN_SUCCESS;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_login_success_pack;
}

void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer) {
	struct p2p_login_success* success;
	int i;
	success = (struct p2p_login_success*)instruction->body;
	memcpy(buffer,&success->users_count,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(buffer,&success->games_count,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);

	for(i = 0; i < success->users_count; i++) {
		buffer += _netemu_application_p2p_pack_user(buffer,&success->users[i]);
	}

	for(i = 0; i < success->games_count; i++) {
		buffer += netemu_util_pack_str(buffer,success->games[i].name);
		buffer += netemu_util_pack_str(buffer,success->games[i].app_name);
		memcpy(buffer, &success->games[i].user_count, sizeof(NETEMU_DWORD));
		buffer += sizeof(NETEMU_DWORD);
		buffer += _netemu_application_p2p_pack_user(buffer,success->games[i].creator);
	}

}

int _netemu_application_p2p_pack_user(char* buffer, struct p2p_user *user) {
	int pos;
	memcpy(buffer,&user->addr_size, sizeof(size_t));
	pos = sizeof(size_t);
	memcpy(buffer+pos, &user->addr, sizeof(netemu_sockaddr));
	pos = sizeof(netemu_sockaddr);
	memcpy(buffer+pos, &user->connection, sizeof(char));
	pos++;
	if(user->name != NULL)
		pos += netemu_util_pack_str(buffer,user->name);

	memcpy(buffer+pos,&user->ping,sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	return pos;
}

int _netemu_application_p2p_parse_user(char* buffer, struct p2p_user *user, NETEMU_BOOL parse_user) {
	int pos;
	memcpy(&user->addr_size, buffer, sizeof(size_t));
	pos = sizeof(size_t);
	memcpy(user->addr, buffer+pos, user->addr_size);
	pos = sizeof(netemu_sockaddr);
	memcpy(&user->connection, buffer+pos, sizeof(char));
	pos++;
	if(parse_user) {
		user->name= netemu_util_parse_string(buffer);
		pos += strlen(user->name)+1;
	}
	memcpy(&user->ping, buffer+pos, sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	return pos;
}

int _netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user) {
	int size;
	size = sizeof(struct p2p_user);
	memcpy(target,user,size);
	/* ? */
	if(user->name != NULL)
		size += netemu_util_copy_string(&target->name,user->name);

	size += netemu_util_copy_string(&target->app_name,user->app_name);
	return size;
}

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, netemu_sockaddr *addr, size_t addr_size, char* username, char* appname, int connection) {
	struct p2p_user *user;
	int size;
	user = malloc(sizeof(struct p2p_user));
	user->addr = malloc(addr_size);
	memcpy(user->addr,addr,addr_size);
	size = addr_size + sizeof(size_t);
	netemu_util_copy_string(&instruction->user,username);
	user->addr_size = addr_size;
	user->name = NULL;
	user->connection = connection;
	size += netemu_util_copy_string(&user->app_name,appname);
	size += 1;

	instruction->body = user;
	instruction->id = P2P_LOGIN_REQUEST;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_login_request_pack;
}

void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer) {
	struct p2p_user *user;
	user = (struct p2p_user*)instruction->body;
	_netemu_application_p2p_pack_user(buffer,user);
}

void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer) {
	struct p2p_user *user;
	user = malloc(sizeof(struct p2p_user));
	_netemu_application_p2p_parse_user(buffer,user,0);
}

void netemu_application_p2p_user_join_add(struct application_instruction *instruction) {

}

void netemu_application_p2p_ready_add(struct application_instruction *instruction) {
	instruction->body = NULL;
	instruction->id = P2P_READY;
	instruction->body_size = 0;
	instruction->packBodyFn = NULL;
}

void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name) {
	struct p2p_kick_player *kick;
	int size;
	kick = malloc(sizeof(struct p2p_kick_player));
	size = netemu_util_copy_string(&kick->name,player_name);
	instruction->body = kick;
	instruction->id = KICK_P2P_PLAYER;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_create_game_pack;
}
