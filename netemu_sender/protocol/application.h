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
 * This file contains defines for id numbers for all packets for
 * both kaillera and netemu p2p. It also contains functions that both
 * protocols use.
 */
#pragma once
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "protocol.h"
#include "../network/net.h"

/* Below are the IDs for each instruction as they appear in the original Kaillera protocol and our modified P2P protocoll */

/*! P2P defines */
#define CREATE_P2P_GAME				30
#define JOIN_P2P_GAME				31
#define P2P_LEAVE_GAME 				32
#define KICK_P2P_PLAYER 			33
#define P2P_LOGIN_SUCCESS			34
#define P2P_LOGIN_REQUEST			35
#define P2P_READY					36
#define P2P_USER_JOIN				37
#define P2P_JOIN_HOST				38
#define P2P_PLAYER_READY			39
#define P2P_GAME_START				40
#define P2P_BUFFERED_PLAY_VALUES		41
#define P2P_CACHED_BUFFERED_PLAY_VALUES	42
#define P2P_PLAYER_JOIN_SUCCESS		43
#define P2P_LOGIN_DENIED			44


/*! Kaillera defines */
#define USER_LEAVE										0x01
#define USER_JOINED										0x02
#define LOGIN_REQUEST									0x03
#define LOGIN_SUCCESS									0x04
#define PING											0x05
#define PONG											0x06
#define PARTYLINE_CHAT									0x07
#define GAME_CHAT										0x08
#define CLIENT_TIMEOUT									0x09
#define CREATE_GAME										0x0a
#define PLAYER_JOINED									0x0c
#define EXISTING_PLAYERS_LIST							0x0d
#define GAME_STATUS_UPDATE								0x0e
#define PLAYER_KICK										0x0f
#define START_GAME										0x11
#define BUFFERED_PLAY_VALUES							0x12
#define INTELLIGENTLY_CACHED_N_BUFFERED_PLAY_VALUES		0x13
#define PLAYER_LEFT										0x14
#define PLAYER_READY									0x15
#define MOTD_CHAT										0x17
#define PLAYER_DROPPED									20 /* Jag har ingen internetanslutning = Jag kan inte komma �t hex-decimal konverteraren. */


int netemu_application_parse_tcp(NETEMU_SOCKET socket, netemu_connection_types type,  union netemu_connection_type connection, void* param);

int netemu_application_parse_udp(NETEMU_SOCKET socket, netemu_connection_types type,  union netemu_connection_type connection, void* param);

struct application_instruction* netemu_application_instruction_create();

struct application_instruction* netemu_application_instruction_parse(struct transport_instruction *instruction);

struct application_instruction* netemu_application_instruction_copy(struct application_instruction* instruction);

void netemu_application_instruction_destroy(struct application_instruction* message);
#endif /* APPLICATION_H_ */
