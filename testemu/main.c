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
	addr.addr = ADDR;
	addr.port = PORT;
	addr.family = NETEMU_AF_INET;
	netemu_init_network();
	kaillera_communication_get_server_list();
	kaillera_communication_connect(&addr);

}
