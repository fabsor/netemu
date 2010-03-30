/*
 * application.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

enum {
	CONNECTION_ACCEPTED,
	CONNECTION_REJECTED_VER,
	CONNECTION_REJECTED_TOO,
	PING_RECEIVED,
};

char* create_hello_message();

char* create_ping_message();

char* create_get_message();

int parse_server_message(char* server_message);

int parse_server_accept_port(char* server_message);

#endif /* APPLICATION_H_ */
