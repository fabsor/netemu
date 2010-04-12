/**
 * @file
 * This file defines global resources used by the whole library.
 * @author Fabian Sörqvist <fabian.sorqvist@gmail.com>
 */

#ifndef NETEMU_RESOURCES_H_
#define NETEMU_RESOURCES_H_

struct netemu_client {
	struct netemu_receiver *receiver;
	struct netemu_sender *sender;

};

/* Global variable containing the shared client. */
struct netemu_client* client = NULL;

/**
 * Get the client, containing a sender and receiver object.
 */
struct netemu_client* netemu_resources_get_client();

/**
 * Free the client.
 */
struct netemu_client* netemu_resources_free_client();

#endif /* NETEMU_RESOURCES_H_ */
