/*
 * application.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../netemu_util.h"
#include "netemu_socket.h"
#include "netlib_util.h"
#include "application_kaillera.h"
#include "application_p2p_internal.h"
#include "netlib_error.h"

void netemu_application_player_left_pack(struct application_instruction* instruction, char* buffer);
int _netemu_application_login_success_games_parse(struct login_success *success, char **data);
int _netemu_application_login_success_users_parse(struct login_success *success, char **data);

struct application_instruction* netemu_application_create_message() {
	struct application_instruction* message;
	if((message = malloc(sizeof(struct application_instruction))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	if((message->user = malloc(sizeof(char))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(message);
		return NULL;
	}
	message->packBodyFn = NULL;
	message->copyBodyFn = NULL;
	message->freeBodyFn = NULL;
	message->important = 0;
	*message->user = '\0';
	return message;
}

int netemu_application_chat_game_add(struct application_instruction *instruction, char *data, char *user) {
	struct chat *partyline_chat;
	int size;
	size = 0;
	if((partyline_chat = malloc(sizeof(struct chat))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	if((size += netemu_util_copy_string(&partyline_chat->message, data)) == NULL) {
		/* Error code already set in _netemu_application_copy_string */
		free(partyline_chat);
		return -1;
	}
	if((size += netemu_util_copy_string(&instruction->user, user)) == NULL) {
		/* Error code already set in _netemu_application_copy_string */
		free(partyline_chat->message);
		free(partyline_chat);
		return -1;
	}

	instruction->body = partyline_chat;
	instruction->body_size = size;
	instruction->id = GAME_CHAT;
	instruction->packBodyFn = netemu_application_chat_pack;
	return 0;
}

int netemu_application_chat_partyline_add(struct application_instruction *instruction, char *data, char* user) {
	struct chat *partyline_chat;
	int size;
	size = 0;
	if((partyline_chat = malloc(sizeof(struct chat))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	if((size += netemu_util_copy_string(&partyline_chat->message, data)) == NULL) {
		/* Error code already set in _netemu_application_copy_string */
		free(partyline_chat);
		return -1;
	}
	if((size += netemu_util_copy_string(&instruction->user, user)) == NULL) {
		/* Error code already set in _netemu_application_copy_string */
		free(partyline_chat->message);
		free(partyline_chat);
		return -1;
	}

	instruction->body = partyline_chat;
	instruction->body_size = size;
	instruction->id = PARTYLINE_CHAT;
	instruction->packBodyFn = netemu_application_chat_pack;
	return 0;
}

void netemu_application_chat_pack(struct application_instruction *instruction, char *buffer) {
	struct chat *chat_msg;

	chat_msg = (struct chat*)instruction->body;
	memcpy(buffer, chat_msg->message, strlen(chat_msg->message) + 1);
}

int netemu_application_chat_parse(struct application_instruction *instruction, char *data) {
	struct chat *chat_msg;

	if((chat_msg = malloc(sizeof(struct chat))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	if((chat_msg->message = netemu_util_parse_string(data)) == NULL) {
		/* Error code already set in parse_string */
		free(chat_msg);
		return -1;
	}
	instruction->body = data;
	return 0;
}

int netemu_application_login_success_parse(struct application_instruction *instruction, char *data) {
	struct login_success *success;

	if((success = malloc(sizeof(struct login_success))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	success->users_count = *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);
	success->games_count =  *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);

	if(_netemu_application_login_success_users_parse(success, &data) == -1) {
		free(success);
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	if(_netemu_application_login_success_games_parse(success, &data) == -1) {
		free(success->users);
		free(success);
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	instruction->body = success;
	return 0;
}

int _netemu_application_login_success_games_parse(struct login_success *success, char **data) {
	unsigned int i, j;

	if((success->games = malloc(sizeof(struct game*) * success->games_count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	for(i = 0; i < success->games_count; i++) {
		if((success->games[i] = malloc(sizeof(struct game))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free the games we have created thus far */
			for(j = 0; j < i; j++) {
				free(success->games[j]->users_count);
				free(success->games[j]->app_name);
				free(success->games[j]->name);
				free(success->games[j]);
			}
			free(success->games);
			return -1;
		}

		if((success->games[i]->name = netemu_util_parse_string(*data)) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free the games we have created thus far */
			for(j = 0; j < i; j++) {
				free(success->games[j]->users_count);
				free(success->games[j]->app_name);
				free(success->games[j]->name);
				free(success->games[j]);
			}
			free(success->games[i]);
			free(success->games);
			return -1;
		}
		*data += strlen(success->games[i]->name) + 1;

		success->games[i]->id = *((NETEMU_DWORD*)*data);
		*data += sizeof(NETEMU_DWORD);

		if((success->games[i]->app_name = netemu_util_parse_string(*data)) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free the games we have created thus far */
			for(j = 0; j < i; j++) {
				free(success->games[j]->users_count);
				free(success->games[j]->app_name);
				free(success->games[j]->name);
				free(success->games[j]);
			}
			free(success->games[i]->name);
			free(success->games[i]);
			free(success->games);
			return -1;
		}
		*data += strlen(success->games[i]->app_name) + 1;

		if((success->games[i]->users_count = netemu_util_parse_string(*data)) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free the games we have created thus far */
			for(j = 0; j < i; j++) {
				free(success->games[j]->users_count);
				free(success->games[j]->app_name);
				free(success->games[j]->name);
				free(success->games[j]);
			}
			free(success->games[i]->app_name);
			free(success->games[i]->name);
			free(success->games[i]);
			free(success->games);
			return -1;
		}
		*data += strlen(success->games[i]->users_count) + 1;

		success->games[i]->status = **data;
		*data += sizeof(char);
	}
	
	return 0;
}

int _netemu_application_login_success_users_parse(struct login_success *success, char **data) {
	unsigned int i, j;

	if((success->users = malloc(sizeof(struct user*) * success->users_count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	for(i = 0; i < success->users_count; i++) {
		if((success->users[i] = malloc(sizeof(struct user))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free memory of all users we have created thus far.*/
			for(j = 0; j < i; j++) {
				free(success->users[j]->username);
				free(success->users[j]);
			}
			free(success->users);
			return -1;
		}

		if((success->users[i]->username = netemu_util_parse_string(*data)) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			/* Free memory of all users we have created thus far.*/
			for(j = 0; j < i; j++) {
				free(success->users[j]->username);
				free(success->users[j]);
			}
			free(success->users[i]);
			free(success->users);
			return -1;
		}
		*data += strlen(success->users[i]->username) + 1;

		success->users[i]->ping = *((NETEMU_DWORD*)*data);
		*data += sizeof(NETEMU_DWORD);

		success->users[i]->status = **data;
		*data += sizeof(char);

		success->users[i]->id = *((NETEMU_WORD*)*data);
		*data += sizeof(NETEMU_WORD);

		success->users[i]->connection = **data;
		*data += sizeof(char);
	}
	return 0;
}

void netemu_application_existing_players_list_parse(struct application_instruction *instruction, char *data) {
	struct existing_player_list *list;
	unsigned int i;

	list = malloc(sizeof(struct existing_player_list));
	list->players_count = *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);
	list->players = malloc(sizeof(struct player) * list->players_count);

	for(i = 0; i < list->players_count; i++) {
		list->players[i].username = netemu_util_parse_string(data);
		data += strlen(list->players[i].username) + 1;

		list->players[i].ping = *((NETEMU_DWORD*)data);
		data += sizeof(NETEMU_DWORD);

		list->players[i].id = *((NETEMU_WORD*)data);
		data += sizeof(NETEMU_WORD);

		list->players[i].connection = *data;
		data += sizeof(char);
	}

	instruction->body = list;
}

void netemu_application_buffered_play_values_parse(struct application_instruction *instruction, char *data) {
	struct buffered_play_values *play_values;

	play_values = malloc(sizeof(struct buffered_play_values));

	play_values->size = *((NETEMU_WORD*)data);
	data += sizeof(NETEMU_WORD);

	play_values->values = malloc(play_values->size);
	memcpy(play_values->values, data, play_values->size);

	instruction->body = play_values;
}

void netemu_application_buffered_play_values_copy(struct buffered_play_values *target, struct buffered_play_values *values) {
	target->size = values->size;
	memcpy(target->values, values->values, values->size);
}

void netemu_application_intelligently_cached_play_values_parse(struct application_instruction *instruction, char *data) {
	struct intelligently_cached_buffered_play_values *cache;

	cache = malloc(sizeof(struct intelligently_cached_buffered_play_values));
	cache->index = *data;
}

void netemu_application_player_dropped_parse(struct application_instruction *instruction, char *data) {
	struct player_dropped *dropped;

	dropped = malloc(sizeof(struct player_dropped));

	dropped->player_number = *data;
	instruction->body = dropped;
}

void netemu_application_buffered_play_values_add(struct application_instruction *instruction, NETEMU_WORD size, char *data) {
	struct buffered_play_values *play_values;
	
	play_values = malloc(sizeof(struct buffered_play_values));

	play_values->size = size;
	play_values->values = malloc(play_values->size);
	memcpy(play_values->values, data, play_values->size);
	instruction->body_size = sizeof(NETEMU_WORD) + play_values->size;
	instruction->body = play_values;
	instruction->id = BUFFERED_PLAY_VALUES;
	instruction->packBodyFn = netemu_application_buffered_play_values_pack;
}

void netemu_application_intelligently_cached_play_values_add(struct application_instruction *instruction, char index) {
	struct intelligently_cached_buffered_play_values *play_values;

	play_values = malloc(sizeof(struct intelligently_cached_buffered_play_values));

	play_values->index = index;
	instruction->body_size = sizeof(char);
	instruction->body = play_values;
	instruction->id = INTELLIGENTLY_CACHED_N_BUFFERED_PLAY_VALUES;
	instruction->packBodyFn = netemu_application_intelligently_cached_play_values_pack;
}

void netemu_application_intelligently_cached_play_values_pack(struct application_instruction *instruction, char *buffer) {
	struct intelligently_cached_buffered_play_values *play_values;

	play_values = (struct intelligently_cached_buffered_play_values*)instruction->body;
	memcpy(buffer, &play_values->index, sizeof(char));
}

void netemu_application_buffered_play_values_pack(struct application_instruction *instruction, char *buffer) {
	struct buffered_play_values *play_values;

	play_values = (struct buffered_play_values*)instruction->body;
	memcpy(buffer, &play_values->size, sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(buffer, play_values->values, play_values->size);
}

void netemu_application_free_message(struct application_instruction* message) {
	/* TODO: Var inte detta ändrat redan? Så att vi inte frigör hela bodyn här, utan använder specifika free-metoder
	 * beroende på body-typ. Håll käften, jag gör vad jag vill! =) */
	free(message->body);
	free(message->user);
	free(message);
}

int netemu_application_login_request_add(struct application_instruction* instruction, char* appName, char* user, int connection) {
	struct login_request* request;
	int size, strlength;
	if((request = malloc(sizeof(struct login_request))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	size = netemu_util_copy_string(&request->name,appName);
	if(size == -1) {
		free(request);
		return -1;
	}
	strlength = netemu_util_copy_string(&instruction->user,user);
	if(strlength == -1) {
		free(request->name);
		free(request);
		return -1;
	}
	request->connection = connection;
	instruction->body_size = size + sizeof(char);
	instruction->body = request;
	instruction->id = LOGIN_REQUEST;
	instruction->packBodyFn = netemu_application_login_request_pack;
	return 0;
}

void netemu_application_pong_add(struct application_instruction* instruction) {
	struct pong* request;
	int size;
	size = sizeof(NETEMU_DWORD)*4;
	request = malloc(sizeof(struct pong));
	instruction->body = request;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_pong_pack;
	instruction->id = PONG;
}

void netemu_application_login_request_pack(struct application_instruction *instruction, char *buffer) {
	struct login_request* request;
	int size;
	request = (struct login_request*)instruction->body;
	memcpy(buffer,(void*)request->name,sizeof(char)*strlen(request->name)+1);
	size = sizeof(char)*strlen(request->name)+1;
	memcpy((buffer+size),(void*)&request->connection,sizeof(char));
}

void netemu_application_pong_pack(struct application_instruction *instruction, char *buffer) {
	struct pong* request;
	NETEMU_DWORD dword;
	request = (struct pong*)instruction->body;
	for (dword = 0; dword <= 3; dword++) {
		memcpy(buffer,&dword,sizeof(NETEMU_DWORD));
		buffer += sizeof(NETEMU_DWORD);
	}
}

void netemu_application_free_login_request(struct login_request* request) {
	free(request->name);
	free(request);
}

void netemu_application_user_leave_add(struct application_instruction* instruction, char* exit_message) {
	struct user_left *left_msg;
	int size;
	left_msg = malloc(sizeof(struct user_left));
	size = netemu_util_copy_string(&left_msg->exit_message,exit_message);
	left_msg->id = 0xFFFF;
	size += sizeof(NETEMU_WORD);
	instruction->body_size = size;
	instruction->body = left_msg;
	instruction->id = USER_LEAVE;
	instruction->packBodyFn = netemu_application_user_leave_pack;
}

void netemu_application_user_leave_pack(struct application_instruction *instruction, char *buffer) {
	struct user_left *left_msg;
	int pos;
	left_msg = (struct user_left*)instruction->body;
	memcpy(buffer,&left_msg->id,sizeof(NETEMU_WORD));
	pos = sizeof(NETEMU_WORD);
	netemu_util_pack_str(buffer+pos, left_msg->exit_message);
}

void netemu_application_user_leave_parse(struct application_instruction *instruction, char *buffer) {
	struct user_left *left;

	if((left = malloc(sizeof(struct user_left))) == NULL) {
		/* TODO: ALL functions really need to return an integer, so that we can return -1 when something goes wrong and 0 otherwise. */
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return;
	}

	left->id = *((NETEMU_WORD*)buffer);
	buffer += sizeof(NETEMU_WORD);
	left->exit_message = netemu_util_parse_string(buffer);
	instruction->body = left;
}

void netemu_application_create_game_add(struct application_instruction *instruction, char* gamename) {
	struct game_created *game;
	int size;
	game = malloc(sizeof(struct game));
	size = netemu_util_copy_string(&game->gameName,gamename);
	size += netemu_util_copy_string(&game->appName,"");
	game->id = 0xFFFF;
	game->wtf = 0xFFFF;
	size += sizeof(NETEMU_WORD)*2;
	instruction->body = game;
	instruction->id = CREATE_GAME;
	instruction->body_size = size;
	instruction->packBodyFn = netemu_application_create_game_pack;
}


void netemu_application_join_game_add(struct application_instruction *instruction, NETEMU_DWORD game_id, char connection) {
	struct player_joined *join;
	int size;
	join = malloc(sizeof(struct player_joined));
	join->user_id = 0xFFFF;
	join->game_id = game_id;
	join->ping = 0;
	join->connection = connection;
	size = netemu_util_copy_string(&join->username,"");

	instruction->id = PLAYER_JOINED;
	instruction->body = join;
	instruction->body_size = size + sizeof(NETEMU_DWORD)*2 + sizeof(NETEMU_WORD) + sizeof(char);
	instruction->packBodyFn = netemu_application_join_game_pack;

}

void netemu_application_add_join_game_total(struct application_instruction *instruction, NETEMU_DWORD game_id, char* username, NETEMU_DWORD ping, NETEMU_WORD user_id, char connection) {
	struct player_joined *join;
	int size;
	join = malloc(sizeof(struct player_joined));
	join->user_id = user_id;
	join->game_id = game_id;
	join->ping = ping;
	join->connection = connection;
	size = netemu_util_copy_string(&join->username,username);

	instruction->id = PLAYER_JOINED;
	instruction->body = join;
	instruction->body_size = size + sizeof(NETEMU_DWORD)*2 + sizeof(NETEMU_WORD) + sizeof(char);
	instruction->packBodyFn = netemu_application_join_game_pack;

}

void netemu_application_join_game_pack(struct application_instruction *instruction, char *buffer) {
	struct player_joined *join;
	join = (struct player_joined*)instruction->body;
	memcpy(buffer,&join->game_id,sizeof(NETEMU_DWORD));
	buffer += sizeof(NETEMU_DWORD);
	buffer += netemu_util_pack_str(buffer,join->username);
	memcpy(buffer,&join->ping,sizeof(NETEMU_DWORD));
	buffer += sizeof(NETEMU_DWORD);
	memcpy(buffer,&join->user_id,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(buffer,&join->connection,sizeof(char));
	buffer += sizeof(char);
}

void netemu_application_join_game_parse(struct application_instruction *instruction, char *data) {
	struct player_joined *join;
	join = malloc(sizeof(struct player_joined));
	memcpy(&join->game_id,data,sizeof(NETEMU_DWORD));
	data += sizeof(NETEMU_DWORD);
	data += netemu_util_copy_string(&join->username,data);
	memcpy(&join->ping,data,sizeof(NETEMU_DWORD));
	data += sizeof(NETEMU_DWORD);
	memcpy(&join->user_id,data,sizeof(NETEMU_WORD));
	data += sizeof(NETEMU_WORD);
	memcpy(&join->connection,data,sizeof(char));
	data += sizeof(char);
	instruction->body = join;
}

void netemu_application_create_game_pack(struct application_instruction *instruction, char *buffer) {
	struct game_created *game;
	int pos;
	game = (struct game_created*) instruction->body;
	pos = netemu_util_pack_str(buffer,game->gameName);
	pos += netemu_util_pack_str(buffer+pos,game->appName);
	memcpy(buffer+pos,&game->id,sizeof(NETEMU_WORD));
	pos += sizeof(NETEMU_WORD);
	memcpy(buffer+pos,&game->wtf,sizeof(NETEMU_WORD));
}


void netemu_application_create_game_parse(struct application_instruction *instruction, char* buffer) {
	struct game_created *game;

	game = malloc(sizeof(struct game_created));
	buffer += netemu_util_copy_string(&game->gameName,buffer);
	buffer += netemu_util_copy_string(&game->appName,buffer);
	memcpy(&game->id,buffer,sizeof(NETEMU_DWORD));
	buffer += sizeof(NETEMU_DWORD);
	//memcpy(&game->wtf,buffer,sizeof(NETEMU_WORD)); Maybe it isn't included?
	instruction->body = game;
}

void netemu_application_game_status_parse(struct application_instruction *instruction, char* buffer) {
	struct game_status_update* status;
	status = malloc(sizeof(struct login_status));
	memcpy(&status->id,buffer,sizeof(NETEMU_DWORD));
	buffer += sizeof(NETEMU_DWORD);
	memcpy(&status->status,buffer,sizeof(char));
	buffer += sizeof(char);
	memcpy(&status->num_players,buffer,sizeof(char));
	buffer += sizeof(char);
	memcpy(&status->max_players,buffer,sizeof(char));
	buffer += sizeof(char);
	instruction->body = status;
}

void netemu_application_user_joined_parse(struct application_instruction *instruction, char* buffer) {
	struct user_joined* status;
	int pos;
	status = malloc(sizeof(struct user_joined));
	memcpy(&status->id,buffer,sizeof(NETEMU_WORD));
	pos = sizeof(NETEMU_WORD);
	memcpy(&status->ping,buffer+pos,sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	memcpy(&status->connection,buffer+pos,sizeof(char));
	instruction->body = status;
	instruction->body_size = instruction->body_size;
}

void netemu_application_player_left_add(struct application_instruction* instruction) {
	struct player_left *left;
	left = malloc(sizeof(struct player_left));
	left->user_id = 0;
	instruction->body = left;
	instruction->body_size = sizeof(NETEMU_WORD);
	instruction->id = PLAYER_LEFT;
	instruction->packBodyFn = netemu_application_player_left_pack;
}

void netemu_application_player_left_pack(struct application_instruction* instruction, char* buffer) {
	struct player_left *left;
	left = (struct player_left*) instruction->body;
	memcpy(buffer,&left->user_id,sizeof(NETEMU_WORD));
}

void netemu_application_player_left_parse(struct application_instruction* instruction, char* buffer) {
	struct player_left *left;
	left = malloc(sizeof(struct player_left));
	memcpy(&left->user_id,buffer,sizeof(char));
}

void netemu_application_player_joined_parse(struct application_instruction* instruction, char* data) {
	struct player_joined *joined;
	
	joined = malloc(sizeof(struct player_joined));

	joined->game_id = *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);

	joined->username = netemu_util_parse_string(data);
	data += strlen(joined->username) + 1;

	joined->ping = *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);

	joined->user_id = *((NETEMU_WORD*)data);
	data += sizeof(NETEMU_WORD);

	joined->connection = *data;

	instruction->body = joined;
}

void netemu_application_kick_player_add(struct application_instruction* instruction, NETEMU_WORD user_id) {
	struct kick_player *kick;
	kick = malloc(sizeof(struct kick_player));
	kick->user_id = user_id;
	instruction->body_size = sizeof(NETEMU_DWORD);
	instruction->id = PLAYER_KICK;
	instruction->packBodyFn = netemu_application_kick_player_pack;
}

void netemu_application_kick_player_pack(struct application_instruction* instruction, char* buffer) {
	struct kick_player *kick;
	kick = (struct kick_player*) instruction->body;
	memcpy(buffer,&kick->user_id,sizeof(char));
}

void netemu_application_kick_player_parse(struct application_instruction* instruction, char* buffer) {
	struct kick_player *kick;
	kick = malloc(sizeof(struct kick_player));
	memcpy(&kick->user_id,buffer,sizeof(char));
}

void netemu_application_start_game_total_add(struct application_instruction* instruction, NETEMU_WORD time_band, char player_no, char max_players) {
	struct game_start *start;
	start = malloc(sizeof(struct game_start));
	start->time_band = time_band;
	start->player_no = player_no;
	start->max_players = max_players;
	instruction->body = start;
	instruction->body_size = sizeof(struct game_start);
	instruction->id = START_GAME;
	instruction->packBodyFn = netemu_application_kick_player_pack;
}

void netemu_application_start_game_add(struct application_instruction* instruction) {
	struct game_start *start;
	start = malloc(sizeof(struct game_start));
	start->time_band = 0xFFFF;
	start->player_no = 0xFF;
	start->max_players = 0xFF;
	instruction->body = start;
	instruction->body_size = sizeof(struct game_start);
	instruction->id = START_GAME;
	instruction->packBodyFn = netemu_application_start_game_pack;
}

void netemu_application_start_game_pack(struct application_instruction* instruction, char* buffer) {
	struct game_start *start;
	start = (struct game_start*) instruction->body;
	memcpy(buffer,&start->time_band,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(buffer,&start->player_no,sizeof(char));
	buffer += sizeof(char);
	memcpy(buffer,&start->max_players,sizeof(char));
	buffer += sizeof(char);
}

void netemu_application_client_timeout_request_add(struct application_instruction* instruction) {
	instruction->body_size = 0;
	instruction->id = CLIENT_TIMEOUT;
	instruction->packBodyFn = NULL;
}

void netemu_application_start_game_parse(struct application_instruction* instruction, char* buffer) {
	struct game_start *start;
	start = malloc(sizeof(struct game_start));
	memcpy(&start->time_band,buffer,sizeof(NETEMU_WORD));
	buffer += sizeof(NETEMU_WORD);
	memcpy(&start->player_no,buffer,sizeof(char));
	buffer += sizeof(char);
	memcpy(&start->max_players,buffer,sizeof(char));
	buffer += sizeof(char);

	instruction->body = start;
}

void netemu_application_player_ready_add(struct application_instruction* instruction) {
	instruction->id = PLAYER_READY;
	instruction->body_size = 0;
	instruction->body = NULL;
	instruction->packBodyFn = NULL;
}

struct application_instruction* netemu_application_instruction_copy(struct application_instruction* instruction) {
	struct application_instruction *copy;
	copy = malloc(sizeof(struct application_instruction));
	if(instruction->body_size > 0 && instruction->body != NULL) {
		copy->body = malloc(sizeof(instruction->body_size));
		memcpy(copy->body, instruction->body, instruction->body_size);
	}
	else {
		copy->body = NULL;
	}
	copy->user = malloc(sizeof(char)*(strlen(instruction->user)+1));
	copy->id = instruction->id;
	copy->important = instruction->important;
	copy->timestamp = instruction->timestamp;
	copy->packBodyFn = instruction->packBodyFn;
	copy->body_size = instruction->body_size;

	strcpy(copy->user,instruction->user);
	return copy;

}
