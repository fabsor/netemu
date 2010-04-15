/*
 * netemu_client.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "netemu_socket.h"
#include "interface/netemu_client.h"
struct netemu_emulator_info* netemu_client_new(char* appName, char** gameList) {
	struct netemu_emulator_info *info;
	info = malloc(sizeof(struct netemu_emulator_info));
	netemu_init_network();
	info->appName = appName;
	info->gameList = gameList;
	return info;
}

void netemu_client_destroy(struct netemu_emulator_info* info) {

}
