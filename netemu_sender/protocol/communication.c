/*
 * communication.c
 *
 *  Created on: 2 apr 2010
 *      Author: fabian
 */

#include "communication.h"
#include "netemu_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* netemu_communication_create_hello_message(char* version) {
	char* msg;
	char* hello;
	hello = "HELLO";
	msg = malloc(sizeof(hello)+sizeof(version));
	memcpy(msg,hello, strlen(hello));
	return strcat(msg,version);
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
	if(strstr("VER",server_message) != NULL) {
		return CONNECTION_REJECTED_VER;
	}
	if(strstr("TOO",server_message) != NULL) {
		return CONNECTION_REJECTED_TOO;
	}
	if(strstr("HELLOD00D",server_message) != NULL) {
		return CONNECTION_ACCEPTED;
	}
	if(strstr("PONG",server_message) != NULL) {
		return PING_RECEIVED;
	}

	return -1;
}

int netemu_communication_parse_server_accept_port(char* server_message) {
	int start,len;
	char* port;

	start = strlen("HELLOD00D")-1;
	len = strlen(server_message)+1;
	port = (server_message+start);

	return atoi(port);
}
