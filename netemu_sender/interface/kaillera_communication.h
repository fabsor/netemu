/*
 * communication_state.h
 *
 *  Created on: 12 apr 2010
 *      Author: fabbe
 */

#ifndef COMMUNICATION_STATE_H_
#define COMMUNICATION_STATE_H_
#include <stdlib.h>
#include "netemu_socket.h"
#include "../network/netemu_receiver.h"
#include "../protocol/communication.h"
#include "../network/netemu_sender.h"
#include "kaillera_server_connection.h"

//struct netemu_communication_server {
//	char *name;
//	struct netemu_sockaddr_in addr;
//	struct netemu_communication_server *next;
//};

typedef struct server kaillera_server;
typedef struct existing_game kaillera_existing_game;

int kaillera_communication_get_server_list(struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount);

/**
 *
 */
void kaillera_communication_get_server_list_async(void (*listReceivedFn(struct netemu_communication_server *server)));

struct server_connection* kaillera_communication_connect(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username);

void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr, void (*ConnectionReceivedFn)(int status, struct server_connection*));

#endif /* COMMUNICATION_STATE_H_ */
