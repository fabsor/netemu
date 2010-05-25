/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#include "communication.h"
#include "netlib_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netemu_socket.h"
#include "../structures/netemu_list.h"
#include "netlib_error.h"
#include "../structures/netemu_stringbuilder.h"
#include <time.h>

#define HTTP_BUFFER_SIZE			512
#define HTTP_HEADER_END				"\r\n\r\n"
#define GAME_STRING_SEPARATOR		'|'
#define PARSE_STRING_MAX_LENGTH		512

/**
 * This struct contains internal data related to servers in the
 * kaillera master server list.
 * @ingroup netemu_communication
 */
struct _server_internal {
	struct netemu_sender_udp *sender;
	time_t ping_timestamp;
	void (* pingReceivedCallback)(struct server *server);
	struct netemu_receiver *receiver;
};

int _netemu_get_http_response(NETEMU_SOCKET socket, struct netemu_stringbuilder *builder);
char *_netemu_parse_game_list(char *input, struct netemu_list *games);
char *_netemu_parse_response_string(char *input, char **output, char terminator);
char *_netemu_parse_server_list(char *response_body, struct netemu_list *servers);

/**
 * Ping a server. This is not a blocking call.
 * @ingroup netemu_communication
 * @param server the server to ping.
 * @param pingReceivedCallback the callback which should be called after the server has been pinged.
 * @return 0 if everything went fine, -1 on failure.
 * @todo This function does not work yet. We need a clever way to ping servers.
 */
int netemu_communication_ping_server(struct server *server, void (* pingReceivedCallback)(struct server *server)) {
	netemu_sockaddr_in addr;
	char *address, *ip, *ping;
	int ip_len, port_len;
	short port;
	/* Parse the address string into one IP string and a port int */
	address = server->address;
	ip_len = 0;
	while(*address++ != ':')
		ip_len++;

	port_len = strlen(server->address) - ip_len;

	ip = malloc(ip_len + 1);
	memcpy(ip, server->address, ip_len);
	ip[ip_len] = '\0';

	port = atoi(address);

	/* Connect */
	addr.sin_addr.s_addr = netemu_inet_addr(ip);
	addr.sin_port = netemu_htons(port);
	addr.sin_family = NETEMU_AF_INET;

	server->_internal->sender = netemu_sender_udp_new(netemu_util_copy_addr((netemu_sockaddr*)&addr,sizeof(addr)), sizeof(addr));

	ping = netemu_communication_create_ping_message();
	/* Store current time, so we can calculate roundtrip time when we receive the pong. */
	server->_internal->ping_timestamp = time(NULL);
	netemu_sender_udp_send(server->_internal->sender, ping, strlen(ping)+1);
	return 0;
}

/**
 * Create an hello message that can be sent to the server.
 * @ingroup netemu_communication
 * @param int version the version of the protocol you want to tell the server that you have.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_hello_message(char* version) {
	char* hello_message;
	char* hello;
	int hello_message_size;
	hello = "HELLO";

	hello_message_size = strlen(hello) + strlen(version) + 1;
	if((hello_message = malloc(hello_message_size)) == NULL)
		return NULL;
	memcpy(hello_message, hello, strlen(hello));
	memcpy(hello_message + strlen(hello), version, strlen(version));
	
	hello_message[hello_message_size - 1] = '\0';
	return hello_message;
}

/**
 * Create a ping message can be sent to the server.
 * @ingroup netemu_communication
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_ping_message() {
	char* msg;
	char* ret;
	msg = "PING";
	ret = malloc(sizeof(char)*strlen(msg)+1);
	ret = memcpy(ret,msg,sizeof(char)*strlen(msg)+1);
	return ret;
}

/**
 * Parse incoming messages from the server.
 * @ingroup netemu_communication
 * @return an int describing the server response. it could be one of:
 * CONNECTION_ACCEPTED the connection has been accepted. You should use netemu_protocol_parse_server_accept port to get the port
 * CONNECTION_REJECTED_VER the connection was rejected because of version concerns.
 * CONNECTION_REJECTED_TOO the connection was rejected because this server is full.
 * PING_RECEIVED The sent ping was received.
 */
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

/**
 * Parse the accept port out of a CONNECTION_ACCEPTED message.
 * @ingroup netemu_communication
 * @return the port number or -1 if the parsing failed.
 */
int netemu_communication_parse_server_accept_port(char* server_message) {
	int start,len;
	char* port;

	start = strlen("HELLOD00D");
	len = strlen(server_message)+1;
	port = (server_message+start);

	return atoi(port);
}

/**
 * Create a http get request.
 * @ingroup netemu_communication
 * @param host the host
 * @param path the path
 * @return the http get request.
 */
char* netemu_communication_http_get(char* host, char* path) {
	char *format;
	char *buffer;
	unsigned int size;

	format = "GET %s HTTP/1.0\r\nHost:%s\r\n\r\n";
	/* Allocate enough memory. */
	size = strlen(format) - 4 + 
		strlen(host) + 
		strlen(path) + 1;
	if((buffer = (char*)malloc(size)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	sprintf(buffer, format, path, host);

	return buffer;
}

/**
 * Parse an http response
 * @ingroup netemu_communication
 * @param socket the socket from which the request should be received.
 * @param games the active games. Just send in a pointer here, and it will be filled automaticly.
 * @param gamecount the number of games. It will be filled by the function.
 * @param servers The actual server. The pointer will be directed to a block containing the server.
 * @param servercount the number of servers. It will be filled with the appropriate number.
 * @return 0 if everything went OK, -1 on failure.
 */
int netemu_communication_parse_http(NETEMU_SOCKET socket, struct existing_game ***games, int *gamecount, struct server ***servers, int *servercount) {
	struct netemu_stringbuilder *builder;
	struct netemu_list *game_list, *server_list;
	char *response_body, *serverlist;
	struct existing_game **eg;
	int return_value, i;

	builder = netemu_stringbuilder_new(1024);
	if(builder == NULL)
		return -1;

	return_value = _netemu_get_http_response(socket, builder);
	if(return_value != 0) {
		netemu_stringbuilder_free(builder);
		return -1;
	}

	response_body = strstr(builder->string, HTTP_HEADER_END);
	if(response_body == NULL) {
		netlib_set_last_error(NETEMU_EINVALIDSERVERLIST);
		netemu_stringbuilder_free(builder);
		return -1;
	}

	game_list = netemu_list_create(128, FALSE);
	if(game_list == NULL) {
		netemu_stringbuilder_free(builder);
		return -1;
	}

	response_body += strlen(HTTP_HEADER_END);
	response_body = _netemu_parse_game_list(response_body, game_list);
	if(response_body == NULL) {
		for(i = 0; i < game_list->count; i++)
			free(netemu_list_get(game_list, i));
		netemu_stringbuilder_free(builder);
		netemu_list_destroy(game_list);
		return -1;
	}


	server_list = netemu_list_create(128, FALSE);
	if(server_list == NULL) {
		for(i = 0; i < game_list->count; i++)
			free(netemu_list_get(game_list, i));
		netemu_stringbuilder_free(builder);
		netemu_list_destroy(game_list);
	}

	response_body = _netemu_parse_server_list(response_body, server_list);
	if(response_body == NULL) {
		/* TODO: The strings inside the game and server structs are not being freed! */
		for(i = 0; i < server_list->count; i++)
			free(netemu_list_get(server_list, i));
		for(i = 0; i < game_list->count; i++)
			free(netemu_list_get(game_list, i));
		netemu_stringbuilder_free(builder);
		netemu_list_destroy(game_list);
		netemu_list_destroy(server_list);
		return -1;
	}

	return_value = netemu_list_copy(game_list, (void***)games);
	if(return_value != 0) {
		for(i = 0; i < server_list->count; i++)
			free(netemu_list_get(server_list, i));
		for(i = 0; i < game_list->count; i++)
			free(netemu_list_get(game_list, i));
		netemu_stringbuilder_free(builder);
		netemu_list_destroy(game_list);
		netemu_list_destroy(server_list);
		return -1;
	}

	return_value = netemu_list_copy(server_list, (void***)servers);
	if(return_value != 0) {
		for(i = 0; i < server_list->count; i++)
			free(netemu_list_get(server_list, i));
		for(i = 0; i < game_list->count; i++)
			free(netemu_list_get(game_list, i));
		free(*games);
		netemu_stringbuilder_free(builder);
		netemu_list_destroy(game_list);
		netemu_list_destroy(server_list);
		return -1;
	}

	*gamecount = game_list->count;
	*servercount = server_list->count;

	netemu_stringbuilder_free(builder);
	netemu_list_destroy(game_list);
	netemu_list_destroy(server_list);

	return 0;
}

char *_netemu_parse_game_list(char* response_body, struct netemu_list *game_list) {
	struct existing_game *game;
	char *return_value;
	char *player_int;

	while(response_body && *response_body != '\n') {
		if((game = malloc(sizeof(struct existing_game))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			return NULL;
		}

		/* Game name */
		if((response_body = _netemu_parse_response_string(response_body, &game->gamename, GAME_STRING_SEPARATOR)) == NULL) {
			break;
		}

		/* IP address */
		if((response_body = _netemu_parse_response_string(response_body, &game->address, GAME_STRING_SEPARATOR)) == NULL) {
			break;
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

		if(netemu_list_add(game_list, game) != 0) {
			response_body = NULL;
			break;
		}
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
		if((serv = (struct server*)malloc(sizeof(struct server))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			return NULL;
		}
		
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

		if(netemu_list_add(servers, serv) != 0) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			response_body = NULL;
			break;
		}
	}

	return response_body;
}

int _netemu_get_http_response(NETEMU_SOCKET socket, struct netemu_stringbuilder *builder) {
	char *receive_buffer;
	int received, error;
	
	error = 0;
	if((receive_buffer = (char*)malloc(HTTP_BUFFER_SIZE)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	do{
		received = netemu_recv(socket, receive_buffer, HTTP_BUFFER_SIZE, 0);
		if(received < 0) {
			error = netlib_get_last_platform_error();
			free(receive_buffer);

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
	/* Search forward in the memory block for the terminator string
	 * The loop will break if we have reached the end of the string, denoted by a null byte.*/
	while(input[string_length] != terminator && 
		input[string_length] != '\0') {
			string_length++;
	}

	/* If we reached the end of the string instead of the terminating character,
	 * the master server returned an invalid response and we need to return NULL */
	if(input[string_length] == '\0') {
		netlib_set_last_error(NETEMU_EINVALIDSERVERLIST);
		return NULL;
	}
		

	if((*output = (char*)malloc(string_length + 1)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	memcpy(*output, input, string_length);
	(*output)[string_length] = '\0';

	return input + string_length + 1;
}
