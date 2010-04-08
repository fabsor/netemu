/*
 * application.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include "netemu_socket.h"
#include "netemu_util.h"
#include "application.h"


int _netemu_application_copy_string(char** dest, char* src);
char* parse_string(char* data);
int _netemu_application_pack_str(char* buffer, char* str);


struct application_instruction* netemu_application_create_message(int message_type,void* body, int body_size, void (*packBodyFn)(struct application_instruction* instruction, char* buffer)) {
	struct application_instruction* message;
	message = malloc(sizeof(struct application_instruction));
	message->id = message_type;
	message->body = body;
	message->body_size = body_size;
	message->packBodyFn = packBodyFn;
	return message;
}

struct application_instruction* netemu_application_parse_message(struct transport_instruction *instruction) {
	struct application_instruction *app_instruction;

	char *data, *user;
	int pos = 0;
	app_instruction = malloc(sizeof(struct application_instruction));
	memcpy(&app_instruction->id,instruction->instruction,sizeof(char));

	data = (char*)instruction->instruction;

	user = parse_string(++data);
	data += strlen(user) + 1;

	switch(app_instruction->id) {
		case PING:
			app_instruction->body_size = 0;
			break;
		case LOGIN_SUCCESS:
			netemu_application_parse_login_success(app_instruction, data, user);
			break;
		case USER_JOINED:
			netemu_application_parse_user_joined(app_instruction,data, user);
			break;
		case PARTYLINE_CHAT:
		case GAME_CHAT:
		case MOTD_CHAT:
			netemu_application_parse_chat(app_instruction, data, user);
			break;

	}
	return app_instruction;
}

char* parse_string(char* data) {
	int str_len;
	char* char_data;
	char* string;

	str_len = strlen(data);
	string = malloc(str_len);
	strcpy(string, data);
}

void netemu_application_parse_chat(struct application_instruction *instruction, char *data, char *user) {
	struct chat *partyline_chat;

	partyline_chat = malloc(sizeof(struct chat));
	partyline_chat->message = parse_string(data);
}

void netemu_application_parse_login_success(struct application_instruction *instruction, char *data, char *user) {
	struct login_success *success;
	int i;

	success = malloc(sizeof(struct login_success));

	success->users_count = *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);
	success->games_count =  *((NETEMU_DWORD*)data);
	data += sizeof(NETEMU_DWORD);

	success->users = malloc(sizeof(struct user) * success->users_count);
	success->games = malloc(sizeof(struct game) * success->games_count);

	for(i = 0; i < success->users_count; i++) {
		success->users[i].username = parse_string(data);
		data += strlen(success->users[i].username) + 1;

		success->users[i].ping = *((NETEMU_DWORD*)data);
		data += sizeof(NETEMU_DWORD);

		success->users[i].status = *data;
		data += sizeof(char);

		success->users[i].id = *((NETEMU_WORD*)data);
		data += sizeof(NETEMU_WORD);

		success->users[i].connection = *data;
		data += sizeof(char);
	}

	for(i = 0; i < success->games_count; i++) {
		success->games[i].name = parse_string(data);
		data += strlen(success->games[i].name) + 1;

		success->games[i].id = *((NETEMU_DWORD*)data);
		data += sizeof(NETEMU_DWORD);

		success->games[i].app_name = parse_string(data);
		data += strlen(success->games[i].app_name) + 1;

		success->games[i].users_count = parse_string(data);
		data += strlen(success->games[i].users_count) + 1;

		success->games[i].status = *data;
		data += sizeof(char);
	}

	instruction->body = success;
}

void netemu_application_free_message(struct application_instruction* message) {
	free(message->body);
	free(message);
}

struct login_request* netemu_application_create_login_request(char* appName, char* user, int connection, int *size) {
	struct login_request* request;
	int appname_len;
	int user_len;
	appname_len = sizeof(char)*strlen(appName)+1;
	user_len = sizeof(char)*strlen(user) + 1;
	request = malloc(sizeof(struct login_request));
	request->name = malloc(appname_len);
	request->user = malloc(user_len);
	memcpy(request->name, appName, appname_len);
	memcpy(request->user, user, user_len);
	*size = appname_len + user_len + sizeof(char);
	request->connection = connection;
	return request;
}

struct pong* netemu_application_create_pong(int *size) {
	struct pong* request;
	*size = sizeof(struct pong);
	request = malloc(*size);
	return request;
}

void netemu_application_login_request_pack(struct application_instruction *instruction, char *buffer) {
	struct login_request* request;
	int size;
	request = (struct login_request*)instruction->body;
	size = sizeof(char)*strlen(request->user)+1;
	memcpy(buffer,(void*)request->user,size);
	memcpy((buffer+size),(void*)request->name,sizeof(char)*strlen(request->name)+1);
	size += sizeof(char)*strlen(request->name)+1;
	memcpy((buffer+size),(void*)&request->connection,sizeof(char));
}

void netemu_application_pong_pack(struct application_instruction *instruction, char *buffer) {
	struct pong* request;
	char* pos;
	NETEMU_DWORD dword;
	request = (struct pong*)instruction->body;
	buffer[0] = '\0';
	pos = buffer+sizeof(char);
	for(dword = 0; dword <= 3; dword++) {
		memcpy((pos+(sizeof(NETEMU_DWORD)*dword)),&dword,sizeof(unsigned long));
	}
}

void netemu_application_free_login_request(struct login_request* request) {
	free(request->name);
	free(request);
}

struct user_left* netemu_application_create_leave(char* exit_message, int *size) {
	struct user_left *left_msg;
	left_msg = malloc(sizeof(struct user_left));
	*size = sizeof(char);
	*size += _netemu_application_copy_string(&left_msg->exit_message,exit_message);
	left_msg->id = 0xFFFF;
	*size += sizeof(NETEMU_WORD);
	return left_msg;
}

int _netemu_application_copy_string(char** dest, char* src) {
	int size;
	size = sizeof(char)*strlen(src) + 1;
	*dest = malloc(size);
	strcpy(*dest,src);
	return size;
}

void netemu_application_leave_pack(struct application_instruction *instruction, char *buffer) {
	struct user_left *left_msg;
	int pos;
	char null = '\0';
	left_msg = (struct user_left*)instruction->body;
	memcpy(buffer,&null,sizeof(char));
	pos = sizeof(char);
	memcpy(buffer+pos,&instruction->id,sizeof(NETEMU_WORD));
	pos += sizeof(NETEMU_WORD);
	_netemu_application_pack_str(buffer+pos, left_msg->exit_message);
}

int _netemu_application_pack_str(char* buffer, char* str) {
	int size;
	size = sizeof(char)*(strlen(str)+1);
	memcpy(buffer,(void*)str,size);
	return size;
}


struct login_status * netemu_application_parse_login_status(struct transport_instruction *instruction) {
	struct login_status* status;
	status = malloc(sizeof(struct login_status));

}

void netemu_application_parse_user_joined(struct application_instruction *instruction, char* buffer, char *user) {
	struct user_joined* status;
	int pos;
	status = malloc(sizeof(struct user_joined));
	status->user = user;
	memcpy(&status->id,buffer,sizeof(NETEMU_WORD));
	pos = sizeof(NETEMU_WORD);
	memcpy(&status->ping,buffer+pos,sizeof(NETEMU_DWORD));
	pos += sizeof(NETEMU_DWORD);
	memcpy(&status->connection,buffer+pos,sizeof(char));
	instruction->body = status;
	instruction->body_size = instruction->body_size;
}



