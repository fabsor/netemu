/**
 * @file
 * This file defines global resources used by the whole library.
 * @author Fabian Sörqvist <fabian.sorqvist@gmail.com>
 */

#ifndef NETEMU_RESOURCES_H_
#define NETEMU_RESOURCES_H_

/* Common defined values. */

#define CLIENT_PORT 	35888


/**
 * This struct contains resources commonly used by all parts of
 * the program.
 */
struct netemu_client {
	struct netemu_receiver_udp *receiver;
	struct netemu_sender_udp *sender;
};


/**
 * Get the client, containing a sender and receiver object.
 * @return the netemu_client that's currently in use.
 */
struct netemu_client* netemu_resources_get_client();

struct netemu_receiver_udp* netemu_resources_get_receiver();

struct netemu_sender_udp* netemu_resources_get_sender();

/**
 * Free the client memory. This will free the memory of the
 * sender and receiver contained in the struct as well.
 */
void netemu_resources_free_client();

#endif /* NETEMU_RESOURCES_H_ */
