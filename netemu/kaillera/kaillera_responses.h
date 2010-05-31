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

/**
 * @file
 * This header file defines the different responses that can be used to respond to instructions.
 */
#pragma once
#ifndef NETEMU_KAILLERA_RESPONSES_H_
#define NETEMU_KAILLERA_RESPONSES_H_
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


#endif /* NETEMU_KAILLERA_RESPONSES_H_ */
