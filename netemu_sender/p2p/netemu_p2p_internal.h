/*
 * netemu_p2p_internal.h
 *
 *  Created on: 21 maj 2010
 *      Author: fabian
 */

#ifndef NETEMU_P2P_INTERNAL_H_
#define NETEMU_P2P_INTERNAL_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netlib_util.h"
#include "netemu_thread.h"

struct netemu_p2p_internal {
	struct netemu_tcp_listener *host;
	struct netemu_tcp_connection *login_connection;
	struct netemu_sender_collection *peers;
	struct netemu_list *login_callbacks;
	struct netemu_list *play_values_callbacks;
	struct netemu_list *game_created_callbacks;
	struct netemu_list *join_callbacks;
	struct netemu_list *player_joined_callbacks;
	struct netemu_list *player_ready_callbacks;
	struct netemu_list *game_started_callbacks;
	struct netemu_list *all_ready_callbacks;
	struct netemu_list *users;
	struct netemu_list *game_status_updated_callbacks;
	struct netemu_list *games;
	char *values_to_send; /**< Contains cached values that should be sent the next time we send something. */
	int values_buffered; /**< The number of values that has been buffered for sending. */
	int sent_values; /**< values sent since last value was received. */
	char *values_received; /**< Contains values that have been received. */
	int frame_index; /**< The index of the frame to return to the user. */
	netemu_event play_values_event;
	struct netemu_receiver_buffer *receive_buffer;
	struct buffered_play_values *buffered_values;
	struct netemu_sender_buffer *send_buffer;
	void (*continueFn)(struct netemu_p2p_connection *connection); /**< Function for continuing work after a ready instruction has been added */
};


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_P2P_INTERNAL_H_ */
