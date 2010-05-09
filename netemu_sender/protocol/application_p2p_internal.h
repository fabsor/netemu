/**
 * @file
 * This header file contains functions and structs that are used by the p2p application layer and should not be exposed to the public.
 */

#ifndef APPLICATION_P2P_INTERNAL_H_
#define APPLICATION_P2P_INTERNAL_H_

#include "application_p2p.h"
#include "netemu_thread.h"
/* TODO: This can't be here but I don't have the time to play nice right now =)*/
struct p2p_user_internal {
	struct netemu_tcp_connection *connection;
	struct netemu_sender_udp *sender; /**< The sender for udp packages */
	struct netemu_receiver_udp *receiver; /**< The receiver for udp packages*/
};

struct p2p_game_internal {
	struct netemu_sender_collection *tcp_collection; /**< This is used for status update messages */
	struct netemu_sender_collection *udp_collection; /**< This is used for play values */
	NETEMU_WORD ready_count;
	netemu_mutex game_lock;
};

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname, struct p2p_user* creator);

void netemu_application_p2p_leave_game_add(struct application_instruction* instruction);

void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_success_add(struct application_instruction *instruction,struct p2p_user *host, struct netemu_list *users, struct netemu_list *games);

void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name);

void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer);

void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, netemu_sockaddr *addr, size_t addr_size, char* username, char* appname, int connection);

void netemu_application_p2p_ready_add(struct application_instruction *instruction);

int netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user);

int netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game);

void netemu_application_p2p_create_game_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_join_host_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_join_host_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_start_game_add(struct application_instruction *instruction, netemu_sockaddr* addr, size_t addr_size);

void netemu_application_p2p_start_game_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_ready_add(struct application_instruction *instruction, netemu_sockaddr* addr, size_t addr_size);

struct p2p_game_internal *netemu_application_p2p_create_game_internal();

struct p2p_user_internal* netemu_application_p2p_create_user_internal();

#endif /* APPLICATION_P2P_INTERNAL_H_ */
