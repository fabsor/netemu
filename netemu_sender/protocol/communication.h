/*
 * application.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "netemu_socket.h"

enum {
	CONNECTION_ACCEPTED,
	CONNECTION_REJECTED_VER,
	CONNECTION_REJECTED_TOO,
	PING_RECEIVED
};

struct existing_game {
	char *gamename;
	char *address;
	char *player;
	char *clientname;
	int num_players;
	char *servername;
	char *location;
};

struct server {
	char *name;
	char *address;
	char *players;
	int games;
	char *version;
	char *location;
};

/**
 * Create an hello message that can be sent to the server.
 * @param int version the version of the protocol you want to tell the server that you have.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_hello_message(char* version);

/**
 * Create a ping message can be sent to the server.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_ping_message();

/**
 * Parse incoming messages from the server.
 * @return an int describing the server response. it could be one of:
 * CONNECTION_ACCEPTED the connection has been accepted. You should use netemu_protocol_parse_server_accept port to get the port
 * CONNECTION_REJECTED_VER the connection was rejected because of version concerns.
 * CONNECTION_REJECTED_TOO the connection was rejected because this server is full.
 * PING_RECEIVED The sent ping was received.
 */
int netemu_communication_parse_server_message(char* server_message);

/**
 * Parse the accept port out of a CONNECTION_ACCEPTED message.
 * @return the port number or -1 if the parsing failed.
 */
int netemu_communication_parse_server_accept_port(char* server_message);

char* netemu_communication_http_get(char* host, char* path);

int netemu_communication_parse_http(NETEMU_SOCKET socket, struct existing_game **games, struct server **servers);

#endif /* COMMUNICATION_H_ */
