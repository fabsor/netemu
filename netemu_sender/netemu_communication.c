/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "protocol/communication.h"
#include "netemu_info.h"
#include "network/netemu_tcp.h"
#include "netemu_resources.h"
#include "network/netemu_sender_udp.h"
#include "network/netemu_receiver.h"
#include "netemu_thread.h"
#include "netemu_util.h"
#include "netemu_socket.h"
#include "network/netemu_sender_buffer.h"
#include "interface/netemu_kaillera.h"
#include "network/netemu_sender_buffer.h"

#define DOMAIN	"www.kaillera.com"
#define SERVER	"kaillera.com"
#define PATH	"/raw_server_list2.php?wg=1&version=0.9"
#define VERSION	"0.83"


struct communication_callback {
	int async;
	int port;
	netemu_sockaddr_in *addr;
	void (*ConnectionReceivedFn)(int status, struct netemu_info*, void *arg);
	char* emulatorname;
	char* username;
	void *arg;
};

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);
void _kaillera_communication_login(struct communication_callback *callback);

int kaillera_communication_get_server_list(const char *address, struct server ***servers, int *servercount, struct existing_game ***games, int *gamecount) {
	struct netemu_tcp_connection *sender;
	struct netemu_addrinfo *info;
	char* request;
	int errcode;
	/* TODO: We need to write a function for destroying and freeing netemu_tcp_connections. */
	errcode = netemu_get_addr_info(address,"80",NULL,&info);
	if(errcode != 0)
		return -1;

	if((request = netemu_communication_http_get(SERVER,PATH)) == NULL)
		return -1;

	sender = netemu_tcp_connection_new(info->addr,info->addrlen);
	if(sender == NULL) {
		free(request);
		return -1;
	}

	errcode = netemu_tcp_connection_connect(sender);
	if(errcode != 0) {
		free(request);
		return -1;
	}

	errcode = netemu_tcp_connection_send(sender,request,strlen(request));
	if(errcode < 0) {
		free(request);
		return -1;
	}

	errcode = netemu_communication_parse_http(sender->socket, games, gamecount, servers, servercount);
	if(errcode != 0) {
		free(request);
		return -1;
	}
	return 0;
}

struct netemu_info* kaillera_communication_connect(netemu_sockaddr_in *addr, int addr_size, char* emulatorname, char* username) {
	struct netemu_client *client;
	struct netemu_info *connection;
	char* hello;
	struct communication_callback callback;
	struct netemu_sender_buffer *buffer;
	union netemu_connection_type *type;
	callback.port = -1;
	callback.async = -1;
	client = netemu_resources_get_client();
	client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener,&callback);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr, addr_size);
	hello = netemu_communication_create_hello_message(VERSION);
	netemu_util_send_data(client->sender,hello);
	free(hello);
	while(callback.port == -1);

	addr->sin_port = netemu_htons(callback.port);
	netemu_receiver_udp_clear_listeners(client->receiver);
	client->sender->addr = (struct sockaddr*)addr;
	type = malloc(sizeof(union netemu_connection_type));
	type->udp_sender = client->sender;
	buffer = netemu_sender_buffer_new(5,10);
	connection = netemu_server_connection_new(username,emulatorname,buffer);
	netemu_receiver_udp_register_recv_fn(client->receiver,netemu_udp_connection_receive,connection);
	server_connection_login(connection);
	return connection;
}

void kaillera_communication_connect_async(netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct netemu_info*, void *arg), void *arg) {
	struct netemu_client *client;
	char* hello;
	struct communication_callback *callback;

	netemu_sockaddr_in *addr_cpy;
	char* user_cpy;
	char* emulator_cpy;

	/* Copy data, so the paramaters can be freed by the calling thread. */
	addr_cpy = malloc(sizeof(netemu_sockaddr_in));
	emulator_cpy = malloc(sizeof(char)*(strlen(emulator_name)+1));
	user_cpy = malloc(sizeof(char)*(strlen(username)+1));

	memcpy(addr_cpy,addr,addr_size);
	strcpy(user_cpy,username);
	strcpy(emulator_cpy,emulator_name);

	callback = malloc(sizeof(struct communication_callback*));
	callback->port = -1;
	callback->async = 1;
	callback->addr = addr_cpy;
	callback->emulatorname = emulator_cpy;
	callback->username = user_cpy;
	callback->ConnectionReceivedFn = ConnectionReceivedFn;
	callback->arg = arg;
	client = netemu_resources_get_client();
	client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener,callback);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr, addr_size);

	hello = netemu_communication_create_hello_message(VERSION);
	netemu_util_send_data(client->sender,hello);
}

void _kaillera_communication_login(struct communication_callback *callback) {
	struct netemu_client *client;
	struct netemu_info* connection;
	client = netemu_resources_get_client();
	netemu_receiver_udp_clear_listeners(client->receiver);
	callback->addr = netemu_htons(callback->port);
	client->sender->addr = (netemu_sockaddr*)callback->addr;
	/*connection = netemu_server_connection_new(callback->username,callback->username);*/
	callback->ConnectionReceivedFn(callback->port, connection, callback->arg);
	free(callback);
}

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args) {
	struct communication_callback *callback;
	int result;
	callback = (struct communication_callback*)args;
	result = netemu_communication_parse_server_message(data);

	if(result == CONNECTION_ACCEPTED) {
		callback->port = netemu_communication_parse_server_accept_port(data);
		if(callback->async != -1) {
			_kaillera_communication_login(callback);
		}
	}
}
