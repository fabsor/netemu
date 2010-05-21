/**
 * @file
 * This header file defines the different responses that can be used to respond to instructions.
 */

#ifndef RESPONSES_H_
#define RESPONSES_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../network/receiver_buffer.h"

void _netemu_respond_to_login_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_ping(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_user_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_user_leave(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_game_created(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_player_joined(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_buffered_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_player_list(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_game_status_update(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_chat(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_cached_play_values(struct netemu_receiver_buffer *buffer, struct netemu_receiver_buffer_item *item, void *arg);
void _netemu_respond_to_player_ready(struct netemu_receiver_buffer *buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_respond_to_game_started(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

#ifdef	__cplusplus
}
#endif


#endif /* RESPONSES_H_ */
