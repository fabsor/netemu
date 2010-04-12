/*
 * netemu_resources.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */

struct netemu_client* netemu_resources_get_client() {
	if (client == NULL) {
		client = malloc(sizeof(struct netemu_client));
	}
	return client;
}

struct netemu_client* netemu_resources_free_client() {
	netemu_receiver_free(client->receiver);
	netemu_sender_free(client->sender);
	free(client);
}
