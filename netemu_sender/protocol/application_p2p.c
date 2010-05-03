/**
 * @file
 * This file contains functions for handling p2p protocol messages.
 */

#include "netlib_util.h"
#include "../netemu_util.h"
#include "application_p2p.h"

int _netemu_application_p2p_pack_user(char* buffer, struct p2p_user *user);
int _netemu_application_p2p_parse_user(char* buffer, struct p2p_user *user, NETEMU_BOOL parse_user);
int _netemu_application_p2p_pack_game(char *buffer, struct p2p_game *game);
void netemu_application_p2p_create_game_pack(struct application_instruction *instruction, char *buffer);
int _netemu_application_p2p_parse_game(char *buffer, struct p2p_game *game);

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct p2p_user* creator) {
	struct p2p_game *game;
	int size;
	game = malloc(sizeof(struct game));
	size = netemu_util_copy_string(&game->name,gamename);
	size += netemu_util_copy_string(&game->app_name,appname);
	game->creator = malloc(sizeof(struct p2p_user*));
	size += netemu_application_p2p_copy_user(game->creator,creator);
	game->user_count = 1;
	size += sizeof(NETEMU_WORD);
	instruction->body = game;
	instruction->id = CREATE_P2P_GAME;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_create_game_pack;
}

void netemu_application_p2p_create_game_pack(struct application_instruction *instruction, char *buffer) {
	struct p2p_game *game;
	game = (struct p2p_game*)instruction->body;
	_netemu_application_p2p_pack_game(buffer,game);
}

void netemu_application_p2p_create_game_parse(struct application_instruction *instruction, char *buffer) {
	struct p2p_game *game;
	int pos;
	game = malloc(sizeof(struct p2p_game));
	pos = _netemu_application_p2p_parse_game(buffer,game);
	instruction->body = game;
	instruction->body_size = pos;
}

int _netemu_application_p2p_pack_game(char *buffer, struct p2p_game *game) {
	int i, pos;
	pos = netemu_util_pack_str(buffer,game->name);
	pos += netemu_util_pack_str(buffer+pos,game->app_name);
	memcpy(buffer+pos, &game->user_count, sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	pos += _netemu_application_p2p_pack_user(buffer+pos,game->creator);

	if(game->players != NULL) {
		for(i = 0; i < game->user_count; i++) {
			pos += _netemu_application_p2p_pack_user(buffer+pos,&game->players[i]);
		}
	}
	return pos;
}

int _netemu_application_p2p_parse_game(char *buffer, struct p2p_game *game) {
	int i, pos;

	game->name = netemu_util_parse_string(buffer);
	pos = strlen(game->name)+1;
	game->app_name = netemu_util_parse_string(buffer+pos);
	pos += strlen(game->app_name)+1;
	memcpy(&game->user_count, buffer+pos, sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	pos += _netemu_application_p2p_parse_user(buffer+pos,game->creator, 1);

	if(game->players != NULL) {
		for(i = 0; i < game->user_count; i++) {
			pos += _netemu_application_p2p_parse_user(buffer+pos, &game->players[i], 1);
		}
	}
	return pos;
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
	if (success->users_count > 0) {
		success->users = malloc(sizeof(struct p2p_user)*success->users_count);
	}
	else {
		success->users = NULL;
	}

	if (success->games_count) {
		success->games = malloc(sizeof(struct p2p_game)*success->games_count);
	}
	else {
		success->games = NULL;
	}
	for(i = 0; i < success->users_count; i++) {
		buffer += _netemu_application_p2p_parse_user(buffer,&success->users[i],1);
	}
	for(i = 0; i < success->games_count; i++) {
		buffer += _netemu_application_p2p_parse_game(buffer, &success->games[i]);
	}
	instruction->body = success;
}

void netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game) {
	int size, i;
	size = netemu_util_copy_string(&target->app_name,game->app_name);
	size += netemu_util_copy_string(&target->name, game->name);
	target->user_count = game->user_count;
	size += sizeof(NETEMU_DWORD);
	target->players = game->players;
	if(target->players != NULL) {
		target->players = malloc(sizeof(struct p2p_user)*game->user_count);
		for(i = 0; i < game->user_count; i++) {

			size += netemu_application_p2p_copy_user(&target->players[i],&game->players[i]);
		}
	}
	size += netemu_application_p2p_copy_user(target->creator,game->creator);
}

void netemu_application_p2p_login_success_add(struct application_instruction *instruction, struct netemu_list *users, struct netemu_list *games) {
	struct p2p_login_success *success;
	struct p2p_game *game;
	struct p2p_user *user;
	int size, i;

	success = malloc(sizeof(struct p2p_login_success));
	success->users = malloc(users->count*sizeof(struct p2p_user));

	success->games = malloc(games->count*sizeof(struct p2p_game));
	success->users_count = users->count;
	success->games_count = games->count;

	for(i = 0; i < users->count; i++) {
		user = success->users+(sizeof(struct p2p_user)*i);
		size += netemu_application_p2p_copy_user(user,users->elements[i]);
	}

	for(i = 0; i < games->count; i++) {
		game = success->games+(sizeof(struct p2p_game)*i);
		netemu_application_p2p_copy_game(game,games->elements[i]);

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
		buffer += _netemu_application_p2p_pack_game(buffer,&success->games[i]);
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
		pos += netemu_util_pack_str(buffer+pos,user->name);

	pos += netemu_util_pack_str(buffer+pos,user->app_name);
	memcpy(buffer+pos,&user->ping,sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	return pos;
}

int _netemu_application_p2p_parse_user(char* buffer, struct p2p_user *user, NETEMU_BOOL parse_user) {
	int pos;
	memcpy(&user->addr_size, buffer, sizeof(size_t));
	pos = sizeof(size_t);
	user->addr = malloc(sizeof(user->addr_size));
	memcpy(user->addr, buffer+pos, user->addr_size);
	pos = sizeof(netemu_sockaddr);
	memcpy(&user->connection, buffer+pos, sizeof(char));
	pos++;
	if(parse_user) {
		user->name= netemu_util_parse_string(buffer+pos);
		pos += strlen(user->name)+1;
	}
	user->app_name = netemu_util_parse_string(buffer+pos);
	pos += strlen(user->app_name)+1;
	memcpy(&user->ping, buffer+pos, sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	return pos;
}

int netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user) {
	int size;

	target->addr = malloc(sizeof(user->addr_size));
	memcpy(target->addr,user->addr,user->addr_size);
	target->addr_size = user->addr_size;

	size = target->addr_size + sizeof(size_t);
	target->ping = user->ping;
	size += sizeof(NETEMU_DWORD);
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
	instruction->body_size = _netemu_application_p2p_parse_user(buffer,user,0);
	instruction->body = user;

}

void netemu_application_p2p_user_join_add(struct application_instruction *instruction, struct p2p_user *user) {
	struct p2p_user *copy;
	copy = malloc(sizeof(struct p2p_user));
	instruction->body_size = netemu_application_p2p_copy_user(copy,user);
	instruction->body = copy;
	instruction->id = JOIN_P2P_GAME;
}

void netemu_application_p2p_user_join_pack(struct application_instruction *instruction, char *buffer) {
	_netemu_application_p2p_pack_user(buffer,(struct p2p_user*) instruction->body);
}

void netemu_application_p2p_user_join_parse(struct application_instruction *instruction, char *buffer) {
	struct p2p_user *user;
	user = malloc(sizeof(struct p2p_user));
	instruction->body_size = _netemu_application_p2p_parse_user(buffer,user,0);
	instruction->body = user;
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
