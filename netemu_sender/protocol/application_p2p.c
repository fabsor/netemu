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
void netemu_application_p2p_user_join_pack(struct application_instruction *instruction, char *buffer);
void netemu_application_p2p_player_join_pack(struct application_instruction *instruction, char *buffer);
void* netemu_application_p2p_create_game_copy(struct application_instruction *instruction);
void netemu_application_p2p_free_game(struct p2p_game *game, NETEMU_BOOL free_pointer);
void netemu_application_p2p_free_user(struct p2p_user *user, NETEMU_BOOL free_pointer);
void* netemu_application_p2p_login_request_copy(struct application_instruction *instruction);
void netemu_application_p2p_login_success_free(struct application_instruction *instruction);
void* netemu_application_p2p_login_success_copy(struct application_instruction *instruction);

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct p2p_user* creator) {
	struct p2p_game *game;
	int size;
	game = malloc(sizeof(struct p2p_game));
	size = netemu_util_copy_string(&game->name,gamename);
	size += netemu_util_copy_string(&game->app_name,appname);
	game->creator = malloc(sizeof(struct p2p_user));
	size += netemu_application_p2p_copy_user(game->creator,creator);
	game->user_count = 1;
	game->players = NULL;
	size += sizeof(NETEMU_WORD);
	instruction->body = game;
	instruction->id = CREATE_P2P_GAME;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_create_game_pack;
	instruction->copyBodyFn = netemu_application_p2p_create_game_copy;
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
	instruction->packBodyFn = netemu_application_p2p_create_game_parse;
}

void* netemu_application_p2p_create_game_copy(struct application_instruction *instruction) {
	struct p2p_game *game;
	game = malloc(sizeof(struct p2p_game));
	netemu_application_p2p_copy_game(game,instruction->body);
	return game;
}

void netemu_application_p2p_create_game_free(struct application_instruction *instruction) {
	netemu_application_p2p_free_game(instruction->body, 0);
}


void netemu_application_p2p_free_game(struct p2p_game *game, NETEMU_BOOL free_pointer) {
	int i;
	free(game->app_name);
	free(game->name);
	netemu_application_p2p_free_user(game->creator, 1);
	for(i = 0; i < game->user_count; i++)  {
		netemu_application_p2p_free_user(&game->players[i],0);
	}
	free(game->players);
	if(free_pointer)
		free(game);
}

void netemu_application_p2p_free_user(struct p2p_user *user, NETEMU_BOOL free_pointer) {
	free(user->app_name);
	if (user->name != NULL) {
		free(user->name);
	}
	free(user->addr);
	if(free_pointer)
		free(user);
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
	game->creator = malloc(sizeof(struct p2p_user));
	pos += _netemu_application_p2p_parse_user(buffer+pos,game->creator, 1);


	if (game->user_count > 1) {
		game->players = malloc(sizeof(struct p2p_user)*game->user_count);
		for(i = 0; i < game->user_count; i++) {
			pos += _netemu_application_p2p_parse_user(buffer+pos, &game->players[i], 1);
		}
	}
	else {
		game->players = NULL;
	}
	return pos;
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

	if (success->games_count > 0) {
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
	instruction->copyBodyFn = netemu_application_p2p_login_success_copy;
	instruction->freeBodyFn = netemu_application_p2p_login_success_free;
}

int netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game) {
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
	return size;
}

void netemu_application_p2p_login_success_add(struct application_instruction *instruction, struct netemu_list *users, struct netemu_list *games) {
	struct p2p_login_success *success;
	int size, i;

	success = malloc(sizeof(struct p2p_login_success));
	success->users_count = users->count;
	success->games_count = games->count;
	size = sizeof(NETEMU_WORD)*2;
	if(success->users_count == 0) {
		success->users = NULL;
	}
	else {
		success->users = malloc(users->count*sizeof(struct p2p_user));
		for(i = 0; i < users->count; i++) {
			size += netemu_application_p2p_copy_user(&success->users[i],users->elements[i]);
		}
	}
	if (success->games_count == 0) {
		success->games = NULL;
	}
	else {
		success->games = malloc(games->count*sizeof(struct p2p_game));
		for(i = 0; i < games->count; i++) {
			size += netemu_application_p2p_copy_game(&success->games[i],games->elements[i]);
		}
	}

	instruction->body = success;
	instruction->id = P2P_LOGIN_SUCCESS;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_login_success_pack;
	instruction->copyBodyFn = netemu_application_p2p_login_success_copy;
	instruction->freeBodyFn = netemu_application_p2p_login_success_free;
}

void* netemu_application_p2p_login_success_copy(struct application_instruction *instruction) {
	struct p2p_login_success *copy;
	struct p2p_login_success *original;
	int i;

	original = instruction->body;

	copy = malloc(sizeof(struct p2p_login_success));
	copy->users_count = original->users_count;
	copy->games_count = original->games_count;
	if(copy->users_count == 0) {
		copy->users = NULL;
	}
	else {
		copy->users = malloc(copy->users_count*sizeof(struct p2p_user));
		for(i = 0; i < copy->users_count; i++) {
			netemu_application_p2p_copy_user(&copy->users[i],&original->users[i]);
		}
	}
	if (copy->games_count == 0) {
		copy->games = NULL;
	}
	else {
		copy->games = malloc(copy->games_count*sizeof(struct p2p_game));
		for(i = 0; i < copy->games_count; i++) {
			netemu_application_p2p_copy_game(&copy->games[i],&original->games[i]);
		}
	}
	return copy;
}

void netemu_application_p2p_login_success_free(struct application_instruction *instruction) {
	struct p2p_login_success *body;
	int i;
	body = instruction->body;
	for(i = 0; i < body->users_count; i++) {
		netemu_application_p2p_free_user(&body->users[i],0);
	}
	free(body->users);
	for(i = 0; i < body->games_count; i++) {
		netemu_application_p2p_free_game(&body->games[i],0);
	}
	free(body->games);
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
	memcpy(buffer+pos, &user->addr, user->addr_size);
	pos += user->addr_size;
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
	user->addr = malloc(user->addr_size);
	memcpy(user->addr, buffer+pos, user->addr_size);
	pos += user->addr_size;
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

	target->addr = malloc(user->addr_size);
	memcpy(target->addr,user->addr,user->addr_size);
	target->addr_size = user->addr_size;

	size = target->addr_size + sizeof(size_t);
	target->ping = user->ping;
	size += sizeof(NETEMU_DWORD);
	/* ? */
	if(user->name != NULL) {
		size += netemu_util_copy_string(&target->name,user->name);
	}
	else {
		target->name = NULL;
	}
	target->connection = user->connection;
	size++;
	size += netemu_util_copy_string(&target->app_name,user->app_name);
	return size;
}

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, netemu_sockaddr *addr, size_t addr_size, char* username, char* appname, int connection) {
	struct p2p_user *user;
	int size;
	user = malloc(sizeof(struct p2p_user));
	user->addr = malloc(addr_size);
	memcpy(user->addr,addr,addr_size);
	user->addr_size = addr_size;
	size = addr_size + sizeof(size_t);
	netemu_util_copy_string(&instruction->user,username);
	user->name = NULL;
	user->connection = connection;
	size += 1;
	size += netemu_util_copy_string(&user->app_name,appname);

	instruction->body = user;
	instruction->id = P2P_LOGIN_REQUEST;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_p2p_login_request_pack;
	instruction->copyBodyFn = netemu_application_p2p_login_request_copy;
}

void* netemu_application_p2p_login_request_copy(struct application_instruction *instruction) {
	struct p2p_user *user;
	user = malloc(sizeof(struct p2p_user));
	netemu_application_p2p_copy_user(user, instruction->body);
	return user;
}

void netemu_application_p2p_login_request_free(struct application_instruction *instruction) {
	struct p2p_user *user;
	user = instruction->body;
	netemu_application_p2p_free_user(user,1);
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
	instruction->packBodyFn = netemu_application_p2p_user_join_pack;
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

void netemu_application_p2p_player_join_add(struct application_instruction *instruction, struct p2p_user *user) {
	instruction->body = malloc(sizeof(struct p2p_user));
	instruction->body_size = netemu_application_p2p_copy_user(instruction->body, user);
	instruction->id = JOIN_P2P_GAME;
	instruction->packBodyFn = netemu_application_p2p_player_join_pack;
}

void netemu_application_p2p_player_join_pack(struct application_instruction *instruction, char *buffer) {
	_netemu_application_p2p_pack_user(buffer,(struct p2p_user*) instruction->body);
}

void netemu_application_p2p_player_join_parse(struct application_instruction *instruction, char *buffer) {
	struct p2p_user *user;
	user = malloc(sizeof(struct p2p_user));
	instruction->body_size = _netemu_application_p2p_parse_user(buffer,user,0);
	instruction->body = user;
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
