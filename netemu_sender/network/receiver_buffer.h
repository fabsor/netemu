/**
 * @file
 * This header represents a buffer module that can be used for packets.
 */

/**
 * @defgroup netemu_receiver_buffer Netemu Receiver Buffer
 * This module handles received packets and delegates them
 * to appropriate listener functions.
 */

#ifndef _NETEMU_PACKET_BUFFER_H
#define _NETEMU_PACKET_BUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif
	#include "../structures/netemu_hashtbl.h"
	#include "netemu_thread.h"
	#include "net.h"

	typedef struct _netemu_receiver_buffer_internal* netemu_receiver_buffer_internal;
	/**
	 * This struct contains information about a specific instruction and where it was received.
	 * @ingroup netemu_receiver_buffer
	 */
	struct netemu_receiver_buffer_item {
		struct application_instruction *instruction; /**< The instruction */
		union netemu_connection_type connection; /**< The connection from which the instruction was received */
		netemu_sockaddr *addr; /**< The address from the instruction was received. */
		int addr_size; /**< The address size. */
		netemu_connection_types type; /**< The type of connection. */
	};
	/**
	 * This struct represents the base of the netemu_receiver_buffer module.
	 * @ingroup netemu_receiver_buffer
	 */
	struct netemu_receiver_buffer {
		netemu_receiver_buffer_internal _internal;
	};

	/**
	 * This function declaration is designed to receive notifications about received instructions.
	 * @param buffer the netemu_receiver_buffer instance that contained it.
	 * @param item the item containing information about the instruction.
	 * @param arg user defined arguments.
	 */
	typedef void (*bufferListenerFn)(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

	/**
	 * Create a new instance of the receiver buffer module
	 * @ingroup netemu_receiver_buffer
	 * @param hash_size size the size of the hash table which will be used internally.
	 * @return a netemu_receiver_buffer instance or NULL if something went wrong. Call netlib_get_last_error for more information
	 * of the occurred error.
	 */
	struct netemu_receiver_buffer *netemu_receiver_buffer_create(hash_size size);

	/**
	 * Add a received instruction to the buffer.
	 * @param buffer an instance of the netemu_receiver_buffer module.
	 * @param instruction
	 * @return 0 if everything went fine, -1 if an error occurred. Call netlib_get_last_error for more information
	 * of the occurred error.
	 * The following errors can occur:
	 * - NETEMU_EINVAL one of the arguments was invalid. Check that the pointers point to the right thing.
	 * - NETEMU_ENOTENOUGHMEMORY The memory is out. You're screwed =)
	 * - Any error related to mutex locks can also occur.
	 */
	int netemu_receiver_buffer_add(struct netemu_receiver_buffer *buffer, struct application_instruction *instruction,
			netemu_connection_types type,  union netemu_connection_type connection, netemu_sockaddr* addr, int addr_size);

	/**
	 * Adds a listener function that listens for a particular instruction.
	 * @ingroup netemu_receiver_buffer
	 * @param buffer an instance of the netemu_receiver_buffer module.
	 * @param instruction_id the instruction id.
	 * @param fn the listener function that will be called when the instruction is received.
	 * @param arg a user provided argument. Set to NULL if you're not interested.
	 * @return 0 if everything went well, -1 on failure.
	 * The following errors can occur:
	 * - NETEMU_EINVAL if one of the arguments was invalid. Check that the pointers point to the right thing.
	 * - NETEMU_ENOTENOUGHMEMORY if the memory is out. You're screwed =)
	 * - Any error related to mutex locks can also occur.
	 */
	int netemu_receiver_buffer_add_instruction_received_fn(struct netemu_receiver_buffer *buffer, char instruction, bufferListenerFn fn, void* arg);

	/**
	 * Wait for an instruction to come in. This is a blocking call that will wait for the buffer
	 * to receive an instruction with the specified id. When such an instruction is received,
	 * this function will return the instruction.
	 * @todo Add a timeout parameter so that this function doesn't wait forever
	 * @todo Make sure that the results are copied to the receiver, so that memory can be freed internally.
	 * @todo fix error handling here.
	 * @ingroup netemu_receiver_buffer
	 * @param buffer an instance of the netemu_receiver_buffer module.
	 * @param instruction_id the id of the instruction to wait for. Make sure that the id is a valid one or you will surely wait forever.
	 * @param timestamp the timestamp when you started waiting. This will discard any instructions received before this timestamp.
	 * @return a receiver_buffer_item containing the instruction, and where it came from if everything went all right, or NULL if something went wrong.
	 */
	struct netemu_receiver_buffer_item* netemu_receiver_buffer_wait_for_instruction(struct netemu_receiver_buffer* buffer, int instruction_id, time_t timestamp);

	/**
	 * Remove a listener function so that it doesn't get called anymore when the instruction is received.
	 * @ingroup netemu_receiver_buffer
	 * @todo implement this function.
	 * @param buffer an instance of the netemu_receiver_buffer module.
	 * @param bufferListenerFn fn the function to remove.
	 */
	int netemu_receiver_buffer_remove_instruction_received_fn(struct netemu_receiver_buffer *buffer, bufferListenerFn fn);
#ifdef	__cplusplus
}
#endif

#endif
