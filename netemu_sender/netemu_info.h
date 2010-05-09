/*
 * server_connection.h
 *
 *  Created on: 5 apr 2010
 *      Author: emil
 */

#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "interface/netemu.h"
#include "netemu_socket.h"
#include "netlib_util.h"
#include "protocol/application_kaillera.h"
#include "network/netemu_receiver.h"
#include "network/netemu_tcp.h"

struct _netemu_info_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	struct netemu_list *play_values_callback;
	struct netemu_list *users;
	struct netemu_list *games;
	NETEMU_BOOL has_id;
	short game_create_requested;
	struct buffered_play_values *buffered_values;
	struct netemu_packet_buffer *receive_buffer;
	struct netemu_sender_buffer *send_buffer;

};

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
	void (*continueFn)(struct netemu_p2p_connection *connection); /**< Function for continuing work after a ready instruction has been added */
};


struct netemu_info *netemu_info_new(char* user, char* emulator_name, struct netemu_sender_buffer* buffer);
void netemu_udp_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params);
void netemu_tcp_connection_receive(char* data, size_t size, struct netemu_tcp_connection* receiver, void* params);
int netemu_kaillera_login(struct netemu_info* connection);

#ifdef	__cplusplus
}
#endif

#endif /* SERVER_CONNECTION_H_ */
