/*
 * main.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
#include <stdlib.h>
#include <stdio.h>
#include "kaillera_communication.h"
#include "netemu_client.h"

#define EMUNAME		"testemu"
#define PLAYERNAME	"foobar"

#define ADDR	INADDR_ANY
#define PORT	netemu_htons(27888)

char* games[] = {"Foo", "Bar"};
#define NO_GAMES	2

int main() {
	struct netemu_sockaddr_in addr;
	struct netemu_emulator_info *info;
	struct server_connection* connection;
	kaillera_server **servers;
	kaillera_existing_game **existing_games;
	struct game *result;
	int i;

	addr.addr = ADDR;
	addr.port = PORT;
	addr.family = NETEMU_AF_INET;
	netemu_init_network();
	//info = netemu_client_new(EMUNAME,games);
	//kaillera_communication_get_server_list(&servers, &games);
	connection = kaillera_communication_connect(&addr,sizeof(addr),EMUNAME,PLAYERNAME);
	for(i = 0; i < NO_GAMES; i++) {
		server_connection_create_game(connection,games[i],&result);
		printf("FIRST!");
	}
	return 0;
}
