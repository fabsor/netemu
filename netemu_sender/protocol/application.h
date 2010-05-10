/*
 * application.h
 *
 *  Created on: May 10, 2010
 *      Author: emil
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

/*! P2P defines */
#define CREATE_P2P_GAME		30
#define JOIN_P2P_GAME		31
#define LEAVE_P2P_GAME 		32
#define KICK_P2P_PLAYER 	33
#define P2P_LOGIN_SUCCESS	34
#define P2P_LOGIN_REQUEST	35
#define P2P_READY			36
#define P2P_USER_JOIN		37
#define P2P_JOIN_HOST		38
#define P2P_PLAYER_READY	39
#define P2P_GAME_START		40


/* Kaillera defines */
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



int netemu_application_parse(NETEMU_SOCKET socket, void* params);

struct application_instruction* netemu_application_parse_message(struct transport_instruction *instruction);

#endif /* APPLICATION_H_ */
