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
#include "../structures/netemu_stringbuilder.h"

#define HTTP_BUFFER_SIZE			512
#define HTTP_HEADER_END				"\r\n\r\n"
#define GAME_STRING_SEPARATOR		'|'

int _netemu_get_http_response(NETEMU_SOCKET socket, struct netemu_stringbuilder *builder);
char *_netemu_parse_game_list(char *input, struct netemu_list *games);
char *_netemu_parse_response_string(char *input, char **output, char terminator);
char *_netemu_parse_server_list(char *response_body, struct netemu_list *servers);


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

int netemu_communication_parse_http(NETEMU_SOCKET socket, struct existing_game ***games, int *gamecount, struct server ***servers, int *servercount) {
	struct netemu_stringbuilder *builder;
	struct netemu_list *game_list, *server_list;
	char *response_body, *serverlist;
	struct existing_game **eg;
	int return_value;

	builder = netemu_stringbuilder_new(1024);

	return_value = _netemu_get_http_response(socket, builder);
	if(return_value != 0) {
		netemu_stringbuilder_free(builder);
		return -1;
	}

	response_body = strstr(builder->string, HTTP_HEADER_END);
	if(response_body == NULL) {
		netemu_stringbuilder_free(builder);
		return -1;
	}

	game_list = netemu_list_new(128);
	response_body += strlen(HTTP_HEADER_END);

	response_body = _netemu_parse_game_list(response_body, game_list);
	if(response_body == NULL) {
		netemu_stringbuilder_free(builder);
		netemu_list_free(game_list);
		return -1;
	}


	server_list = netemu_list_new(128);

	response_body = _netemu_parse_server_list(response_body, server_list);
	if(response_body == NULL) {
		netemu_stringbuilder_free(builder);
		netemu_list_free(game_list);
		netemu_list_free(server_list);
		return -1;
	}

	return_value = netemu_list_copy(game_list, (void***)games);
	if(return_value != 0) {
		netemu_stringbuilder_free(builder);
		netemu_list_free(game_list);
		netemu_list_free(server_list);
		return -1;
	}

	return_value = netemu_list_copy(server_list, (void***)servers);
	if(return_value != 0) {
		netemu_stringbuilder_free(builder);
		netemu_list_free(game_list);
		netemu_list_free(server_list);
		return -1;
	}

	*gamecount = game_list->count;
	*servercount = server_list->count;

	netemu_stringbuilder_free(builder);
	netemu_list_free(game_list);
	netemu_list_free(server_list);

	return 0;
}

char *_netemu_parse_game_list(char* response_body, struct netemu_list *game_list) {
	struct existing_game *game;
	char *return_value;
	char *player_int;
	int gg;
	while(*response_body != '\n') {
		game = malloc(sizeof(struct existing_game));

		/* Game name */
		if((response_body = _netemu_parse_response_string(response_body, &game->gamename, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}

		/* IP address */
		if((response_body = _netemu_parse_response_string(response_body, &game->address, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}
		if(strcmp(game->address, "194.177.99.213:27888") == 0) {
gg = 4;
		}
		/* Player name */
		if((response_body = _netemu_parse_response_string(response_body, &game->player, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}

		/* Client name */
		if((response_body = _netemu_parse_response_string(response_body, &game->clientname, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}

		/* Number of players */
		if((response_body = _netemu_parse_response_string(response_body, &player_int, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}
		else {
			game->num_players = atoi(player_int);
		}

		/* Server name */
		if((response_body = _netemu_parse_response_string(response_body, &game->servername, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}

		/* Server location */
		if((response_body = _netemu_parse_response_string(response_body, &game->location, GAME_STRING_SEPARATOR)) < 0) {
			break;
		}

		netemu_list_add(game_list, game);
	}
	/* Move past the linebreak denoting the end of the game list */
	if(response_body)
		response_body++;

	return response_body;
}

char *_netemu_parse_server_list(char *response_body, struct netemu_list *servers) {
	struct server *serv;
	char *game_int;

	while(*response_body != '\0') {
		serv = (struct server*)malloc(sizeof(struct server));
		
		if((response_body = _netemu_parse_response_string(response_body, &serv->name, '\n')) == NULL) {
			break;
		}

		if((response_body = _netemu_parse_response_string(response_body, &serv->address, ';')) == NULL) {
			break;
		}

		if((response_body = _netemu_parse_response_string(response_body, &serv->players, ';')) == NULL) {
			break;
		}

		if((response_body = _netemu_parse_response_string(response_body, &game_int, ';')) == NULL) {
			break;
		}
		else {
			serv->games = atoi(game_int);
		}

		if((response_body = _netemu_parse_response_string(response_body, &serv->version, ';')) == NULL) {
			break;
		}

		if((response_body = _netemu_parse_response_string(response_body, &serv->location, '\n')) == NULL) {
			break;
		}

		netemu_list_add(servers, serv);
	}

	return response_body;
}

int _netemu_get_http_response(NETEMU_SOCKET socket, struct netemu_stringbuilder *builder) {
	char *receive_buffer;
	int received, error;
	
	error = 0;
	receive_buffer = malloc(HTTP_BUFFER_SIZE);
	do{
		received = netemu_recv(socket, receive_buffer, HTTP_BUFFER_SIZE, 0);
		if(received < 0) {
			error = netemu_get_last_error();
			break;
		}
		else if(received == 0) {
			error = 0;
			break;
		}
		else {
			netemu_stringbuilder_append_chars(builder, receive_buffer, received);
		}
	}while(1);

	free(receive_buffer);
	return error;
}

char *_netemu_parse_response_string(char *input, char **output, char terminator) {
	NETEMU_DWORD string_length;
	char *string;

	string_length = 0;
	while(input[string_length] != terminator)
		string_length++;

	if((*output = (char*)malloc(string_length + 1)) == NULL) {
		return -1;
	}

	memcpy(*output, input, string_length);
	(*output)[string_length] = '\0';

	return input + string_length + 1;
}
