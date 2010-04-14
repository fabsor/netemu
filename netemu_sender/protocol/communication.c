/*
 * communication.c
 *
 *  Created on: 2 apr 2010
 *      Author: fabian
 */

#include "communication.h"
#include "netlib_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* netemu_communication_create_hello_message(char* version) {
	char* hello_message;
	char* hello;
	int hello_message_size;
	hello = "HELLO";

	hello_message_size = strlen(hello) + strlen(version) + 1;
	hello_message = malloc(hello_message_size);
	memcpy(hello_message, hello, strlen(hello));
	memcpy(hello_message + strlen(hello), version, strlen(version));
	
	hello_message[hello_message_size - 1] = '\0';
	return hello_message;
}

char* netemu_communication_create_ping_message() {
	char* msg;
	char* ret;
	msg = "PING";
	ret = malloc(sizeof(char)*strlen(msg)+1);
	ret = memcpy(ret,msg,sizeof(char)*strlen(msg)+1);
	return ret;
}

int netemu_communication_parse_server_message(char* server_message) {
	if(strstr(server_message,"VER") != NULL) {
		return CONNECTION_REJECTED_VER;
	}
	if(strstr(server_message,"VER") != NULL) {
		return CONNECTION_REJECTED_TOO;
	}
	if(strstr(server_message, "HELLOD00D") != NULL) {
		return CONNECTION_ACCEPTED;
	}
	if(strstr(server_message, "PONG") != NULL) {
		return PING_RECEIVED;
	}

	return -1;
}

int netemu_communication_parse_server_accept_port(char* server_message) {
	int start,len;
	char* port;

	start = strlen("HELLOD00D");
	len = strlen(server_message)+1;
	port = (server_message+start);

	return atoi(port);
}

char* netemu_communication_http_get(char* host, char* path) {
	char *get_ins, *get;
	unsigned int size;

	get_ins = "GET %s HTTP/1.1\nHost: %s";
	/* Allocate enough memory. */
	size = sizeof(char)*(strlen(get_ins)+1)+
			sizeof(char)*(strlen(host)+1)+sizeof(char)*(strlen(path)+1);
	get = malloc(size);

	strcat(get, "GET ");
	strcat(get,path);
	strcat(get," HTTP/1.1\nHost: ");
	strcat(get,host);

	return get;
}
