/*
 * application.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include "application.h"

struct protocol_message* netemu_application_create_message(int message_type,char* user,void* instruction,size_t instruction_size) {
	struct protocol_message* message;
	message = malloc(sizeof(struct protocol_message));
	message->id = message_type;
	message->total_size = instruction_size;
	message->body = instruction;

	return message;
}

void netemu_application_free_message() {

}

struct login_request* netemu_application_create_login_request(char appName[128], int connection) {
	struct login_request* request;
	request = malloc(sizeof(struct login_request));
	memcpy(request->name,appName,sizeof(char)*128);
	request->connection = connection;
	return request;
}

void netemu_application_free_login_request(struct login_request* request) {

}
