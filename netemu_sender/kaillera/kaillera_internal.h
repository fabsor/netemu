/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef KAILLERA_INTERNAL_H_
#define KAILLERA_INTERNAL_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "netemu_thread.h"
#include "../protocol/application_kaillera.h"
struct _netemu_info_internal {
	netemu_event connected_event;
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *player_join_callback;
	struct netemu_list *game_created_callback;
	struct netemu_list *play_values_callback;
	struct netemu_list *player_ready_callback;
	struct netemu_list *cached_values_callback;
	struct netemu_list *game_status_updated_callbacks;
	struct netemu_list *game_started_callbacks;
	struct netemu_list *users;
	struct netemu_list *games;
	struct netemu_list *callbacks_to_remove; /**< used by the various callback functions in order to be able to remove callbacks */
	struct netemu_list *received_play_values;
	NETEMU_BOOL has_id;
	short game_create_requested;
	char *values_to_send; /**< Contains cached values that should be sent the next time we send something. */
	int time_band;	/**< Determines how many values we can send before we need to have received a value from the server. */
	int connection_quality; /**< The quality of the connection, which determines how often we will send instuctions. */
	int values_buffered; /**< The number of values that has been buffered for sending. */
	int sent_values; /**< values sent since last value was received. */
	struct buffered_play_values cached_values[256]; /**< a cache that will contain 256 cached values. */
	short cached_count; /**< The current number of cached values.  */
	int cache_index; /**< The index in the cache buffer. */
	int frame_index; /**< The index of the frame to return to the user. */
	netemu_event play_values_event;
	int player_no;
	struct netemu_receiver_buffer *receive_buffer;
	struct netemu_sender_buffer *send_buffer;
	netemu_event send_timeout;
};

#ifdef	__cplusplus
}
#endif


#endif /* KAILLERA_INTERNAL_H_ */
