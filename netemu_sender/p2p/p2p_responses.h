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
 * This header file contains responder functions for netemu p2p protocol.
 */

/**
 * @defgroup p2p_responses netemu p2p responder functions
 * Responder functions to different instructions.
 */
#pragma once
#ifndef NETEMU_P2P_RESPONSES_H_
#define NETEMU_P2P_RESPONSES_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "../interface/netemu_p2p.h"
#include "../network/receiver_buffer.h"

/**
 * Register responders that will be used by this module.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param p2p the netemu_p2p instance.
 */
void _netemu_p2p_register_responders(struct netemu_p2p_connection *p2p);

/**
 * This function responds to all "User Join" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_user_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "Ready" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_ready(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "Game started" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_game_started(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "Join host" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_join_host(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "player join" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_player_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function handles all incoming "Game Created"-instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_game_created(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void netemu_p2p_respond_to_play_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void netemu_p2p_respond_to_cached_play_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "Login success" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_login_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

/**
 * This function responds to all "Login request" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_login_request(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

void _netemu_p2p_respond_to_player_join_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_P2P_RESPONSES_H_ */
