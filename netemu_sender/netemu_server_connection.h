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

#include "interface/netemu_kaillera.h"
#include "netemu_socket.h"
#include "netlib_util.h"
#include "protocol/application.h"
#include "network/netemu_receiver.h"
#include "network/netemu_tcp.h"

struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	struct netemu_list *play_values_callback;
	struct netemu_list *users;
	struct netemu_list *games;
	short game_create_requested;
	struct buffered_play_values *buffered_values;
	struct netemu_packet_buffer *receive_buffer;
	struct netemu_sender_buffer *send_buffer;

};

struct server_connection *netemu_server_connection_new(char* user, char* emulator_name, struct netemu_sender_buffer* buffer);
void netemu_udp_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params);
void netemu_tcp_connection_receive(char* data, size_t size, struct netemu_tcp_connection* receiver, void* params);
int server_connection_login(struct server_connection* connection);

#ifdef	__cplusplus
}
#endif

#endif /* SERVER_CONNECTION_H_ */
