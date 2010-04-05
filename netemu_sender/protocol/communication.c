/*
 * communication.c
 *
 *  Created on: 2 apr 2010
 *      Author: fabian
 */

#include "communication.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>


char* netemu_communication_create_hello_message(float version) {
	char str_version[5];
	char* msg;
	char* hello;
	hello = "HELLO";
	snprintf(str_version, 5, "%f", version);
	msg = malloc(sizeof(hello)+sizeof(version));
	strcpy(msg,hello);
	return strcat(msg,str_version);
}

char* netemu_communication_create_ping_message() {
	return "PING";
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
	return 0;
}
