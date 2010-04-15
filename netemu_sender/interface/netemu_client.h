/*
 * netemu_client.h
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */

#ifndef NETEMU_CLIENT_H_
#define NETEMU_CLIENT_H_

struct  netemu_emulator_info {
	char *appName;
	char **gameList;
	char *userName;
};

struct netemu_emulator_info* netemu_client_new(char* appName, char** gameList);
void netemu_client_destroy(struct netemu_emulator_info* info);

#endif /* NETEMU_CLIENT_H_ */
