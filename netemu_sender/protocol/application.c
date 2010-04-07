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
	int pos = 0;
	app_instruction = malloc(sizeof(struct application_instruction));
	memcpy(&app_instruction->id,instruction->instruction,sizeof(char));

	switch(app_instruction->id) {
		case PING:
			app_instruction->body_size = 0;
			break;
	}
	return app_instruction;
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
	int p;
	request = (struct pong*)instruction->body;
	buffer[0] = '\0';
	pos = buffer+sizeof(char);
	for(dword = 0; dword <= 3; dword++) {
		p = (sizeof(NETEMU_DWORD)*dword);
		memcpy((pos+(sizeof(NETEMU_DWORD)*dword)),&dword,sizeof(unsigned long));
	}
}

void netemu_application_free_login_request(struct login_request* request) {
	free(request->name);
	free(request);
}

//struct pong* foo(DWORD i) {
//	DWOR
//}
