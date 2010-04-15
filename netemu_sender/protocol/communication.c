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
#include "netemu_socket.h"
#include "netemu_list.h"

#define HTTP_BUFFER_SIZE 512


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
	char *format;
	char *buffer;
	unsigned int size;

	format = "GET %s HTTP/1.0\r\nHost:%s\r\n\r\n";
	/* Allocate enough memory. */
	size = strlen(format) - 4 + 
		strlen(host) + 
		strlen(path);
	buffer = (char*)malloc(size);
	sprintf(buffer, format, path, host);

	return buffer;
}

int netemu_communication_parse_http(NETEMU_SOCKET socket, struct waiting_games** games) {
	char *receive_buffer, *temp_buffer;
	int received;
	int buffer_size;
	struct netemu_list *list;
	int i;
	int size;
	list = netemu_list_new(sizeof(char), 6000);
	buffer_size = 0;
	receive_buffer = malloc(HTTP_BUFFER_SIZE);
	size = SIZE_MAX;
	do{
		received = netemu_recv(socket, receive_buffer, HTTP_BUFFER_SIZE, 0);
		if(received < 0) {
			buffer_size = netemu_get_last_error();
			break;
		}
		else
		{
			/* We can not pass receive_buffer to addrange,
			 *since that will corrupt the list when we modify the receive_buffer.
			 * To solve this, we supply the list with a copy of our data. */
			//temp_buffer = (char*)malloc(received);
			//memcpy(temp_buffer, receive_buffer, received);
			//netemu_list_addrange(list, temp_buffer, received);

			for(i = 0; i < received; i++) {
				netemu_list_add(list, &receive_buffer[i]);
			}
		}

		if(list->count > 3) {
			if((char)netemu_list_get(list, list->count - 1) == 0x0a &&
				(char)netemu_list_get(list, list->count - 2) == 0x0d &&
				(char)netemu_list_get(list, list->count - 3) == 0x0a &&
				(char)netemu_list_get(list, list->count - 4) == 0x0d) {
					break;
			}
		}
	}while(1);

	return received < 0 ? -1 : 0;
}