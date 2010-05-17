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
#include "network/netemu_net.h"
#include "network/netemu_sender_buffer.h"
#include "protocol/application.h"
#define DOMAIN	"www.kaillera.com"
#define SERVER	"kaillera.com"
#define PATH	"/raw_server_list2.php?wg=1&version=0.9"
#define VERSION	"0.83"


struct communication_callback {
	int async;
	int port;
	netemu_sockaddr_in *addr;
	void (*ConnectionReceivedFn)(int status, struct netemu_kaillera*, void *arg);
	char* emulatorname;
	char* username;
	void *arg;
};

int kaillera_communication_listener(NETEMU_SOCKET socket, netemu_connection_types type, union netemu_connection_type connection, void* args);
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

struct netemu_kaillera* netemu_kaillera_connect(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port, NETEMU_DWORD server_address, unsigned short server_port) {
	struct netemu_client *client;
	char* hello;
	struct communication_callback callback;
	int connection_attempts;
	NETEMU_BOOL connection_success;
	netemu_sockaddr_in in_addr;
	callback.port = -1;
	callback.async = -1;
	client = netemu_resources_get_client();

	in_addr.sin_addr.s_addr = local_address;
	in_addr.sin_port = local_port;
	in_addr.sin_family = NETEMU_AF_INET;

	if(client == NULL)
		return NULL;

	client->receiver = netemu_receiver_udp_new((struct sockaddr*)&in_addr, sizeof(in_addr));
	if(client->receiver == NULL)
		return NULL;
	netemu_receiver_udp_start_receiving(client->receiver, kaillera_communication_listener, &callback);

	in_addr.sin_addr.s_addr = server_address;
	in_addr.sin_port = server_port;
	in_addr.sin_family = NETEMU_AF_INET;

	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, &in_addr, sizeof(in_addr));
	if(client->sender == NULL)
		return NULL;
	/* TODO: Free the receiver and sender incase of errors. Need new functions for this. */

	if((hello = netemu_communication_create_hello_message(VERSION)) == NULL)
		return NULL;

	/*connection_attempts = 0;
	while(connection_attempts < 5 && !connection_success) {*/
		netemu_util_send_data(client->sender,hello);
		/*if(netemu_thread_event_wait(connection->_internal->connected_event, 3) != NETEMU_WAIT_TIMEOUT) {
			connection_success = TRUE;
		}
	}*/

	free(hello);
	while(callback.port == -1);
	netemu_receiver_udp_stop_receiving(client->receiver);
	in_addr.sin_port = netemu_htons(callback.port);
	client->sender->addr = netemu_util_copy_addr((netemu_sockaddr*)&in_addr,sizeof(in_addr));

	netemu_receiver_udp_start_receiving(client->receiver, netemu_application_parse_udp, connection->_internal->receive_buffer);
	netemu_kaillera_login(connection);
	return connection;
}

void kaillera_communication_connect_async(netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username, void (*ConnectionReceivedFn)(int status, struct netemu_kaillera*, void *arg), void *arg) {
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
	//client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener,callback);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr, addr_size);

	hello = netemu_communication_create_hello_message(VERSION);
	netemu_util_send_data(client->sender,hello);
}

void _kaillera_communication_login(struct communication_callback *callback) {
	struct netemu_client *client;
	struct netemu_kaillera* connection;
	client = netemu_resources_get_client();
	callback->addr = netemu_htons(callback->port);
	client->sender->addr = (netemu_sockaddr*)callback->addr;
	/*connection = netemu_server_connection_new(callback->username,callback->username);*/
	callback->ConnectionReceivedFn(callback->port, connection, callback->arg);
	free(callback);
}

int kaillera_communication_listener(NETEMU_SOCKET socket, netemu_connection_types type, union netemu_connection_type connection, void* args) {
	struct communication_callback *callback;
	int result;
	int error;
	callback = (struct communication_callback*)args;
	char* buffer;

	buffer = malloc(128);
	/* We need to set this to nonblocking... */
	error = netemu_recvfrom(socket, buffer, 128, 0, NULL, 0);
	if(error == -1) {
		error = netlib_get_last_platform_error();
		if(error != 11) {
			return -1;
		}
	}
	result = netemu_communication_parse_server_message(buffer);

	if(result == CONNECTION_ACCEPTED) {
		callback->port = netemu_communication_parse_server_accept_port(buffer);
		if(callback->async != -1) {
			_kaillera_communication_login(callback);
		}
		return 0;
	}
	return 1;
}



/*void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args) {
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
}*/
