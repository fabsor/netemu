/*
 * application.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include "application.h"

sizes[] = {
	sizeof(struct user_left),
	sizeof(struct user_joined),
	sizeof(struct login_request),
	sizeof(struct login_success),
	sizeof(struct ping),
	sizeof(struct pong),
	sizeof(struct chat),
	sizeof(struct chat),
	0,
	sizeof(struct game_created),
	sizeof(struct player_left),
	sizeof(struct player_joined),
	sizeof(struct existing_player_list),
	sizeof(struct game_status_update),
	sizeof(struct kick_player),
	sizeof(struct game_closed),
	sizeof(struct game_start),
	sizeof(struct buffered_play_values),
	sizeof(struct intelligently_cached_buffered_play_values),
	sizeof(struct intelligently_cached_buffered_play_values), // Player dropped. There is no such struct, but this struct is equally sized.
	0,
	sizeof(struct login_status),
	sizeof(struct chat) // MOTD, its the same size as chat.
};

struct application_instruction* netemu_application_create_message(int message_type,char* user,void* instruction) {
	struct application_instruction* message;
	message = malloc(sizeof(struct application_instruction));
	message->id = message_type;
	message->body = instruction;
	return message;
}

void netemu_application_free_message(struct application_instruction* message) {
	free(message->body);
	free(message);
}

struct login_request* netemu_application_create_login_request(char appName[128], int connection) {
	struct login_request* request;
	request = malloc(sizeof(struct login_request));
	memcpy(request->name,appName,sizeof(char)*128);
	request->connection = connection;
	return request;
}

void netemu_application_free_login_request(struct login_request* request) {
	free(request->name);
	free(request);
}

//struct pong* foo(DWORD i) {
//	DWOR
//}