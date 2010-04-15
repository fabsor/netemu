/*
 * main.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
#include <stdlib.h>
#include <stdio.h>
#include "netemu_socket.h"
#include "kaillera_communication.h"


#define EMUNAME		"testemu"
#define PLAYERNAME	"foobar"

#define ADDR	netemu_htonl(NETEMU_INADDR_LOOPBACK)
#define PORT	netemu_htons(27888)

char* games[] = {"Foo", "Bar"};


int main() {
	struct netemu_sockaddr_in addr;
	//struct netemu_stringbuilder *builder;
	//char *text;
	//text = malloc(4);
	//text[0] = 'E';
	//text[1] = 'm';
	//text[2] = 'i';
	//text[3] = 'l';
	//builder = netemu_stringbuilder_new(8);
	//netemu_stringbuilder_append_chars(builder, text, 4);
	//text[0] = 'G';
	//text[1] = 'r';
	//text[2] = 'ö';
	//text[3] = 't';
	//netemu_stringbuilder_append_chars(builder, text, 4);
	addr.addr = ADDR;
	addr.port = PORT;
	addr.family = NETEMU_AF_INET;
	netemu_init_network();
	kaillera_communication_get_server_list();
	kaillera_communication_connect(&addr);

}
