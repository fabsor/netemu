/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "protocol/communication.h"
#include "interface/kaillera_server_connection.h"
#include "network/netemu_tcp.h"
#include "netemu_resources.h"
#include "network/netemu_sender.h"
#include "network/netemu_receiver.h"
#include "netemu_thread.h"
#include "netemu_util.h"
#include "netemu_socket.h"
#define DOMAIN	"www.kaillera.com"
#define SERVER	"kaillera.com"
#define PATH	"/raw_server_list2.php?wg=1&version=0.9"
#define VERSION	"0.83"


void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);

struct server_connection_callback {
	void (*ConnectionReceivedFn)(int status, struct server_connection*);
};

struct netemu_communication_server* kaillera_communication_get_server_list() {
	struct netemu_tcp_connection *sender;
	struct netemu_addrinfo *info;
	struct netemu_addrinfo hints;
	char* request;
	char* buffer;
	int error;

    hints.ai_family = NETEMU_AF_UNSPEC;
    hints.ai_socktype = NETEMU_SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	netemu_get_addr_info(DOMAIN,"80",NULL,&info);
	sender = netemu_tcp_connection_new(info->addr,info->addrlen);
	request = netemu_communication_http_get(SERVER,PATH);
	netemu_tcp_connection_send(sender,request,strlen(request));
	buffer = malloc(1024);
	error = netemu_recv(sender->socket,buffer,1024,0);

}

struct server_connection* kaillera_communication_connect(struct netemu_sockaddr_in *addr, int addr_size, char* emulator_name, char* username) {
	struct netemu_client *client;
	struct server_connection *connection;
	char* hello;
	int result;

	result = -1;

	client = netemu_resources_get_client();
	client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener,&result);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr, addr_size);

	hello = netemu_communication_create_hello_message(VERSION);
	netemu_util_send_data(client->sender,hello);
	while(result == -1);
	free(hello);
	addr->port = netemu_htons(result);
	free(client->sender->addr);
	netemu_receiver_udp_clear_listeners(client->receiver);
	client->sender->addr = netemu_prepare_net_addr(addr);
	connection = server_connection_new(username,emulator_name);
	return connection;
}
/*
void kaillera_communication_connect_async(struct netemu_sockaddr_in *addr, void (*ConnectionReceivedFn)(int status, struct server_connection*)) {
	struct netemu_client *client;
	char* hello;
	int result = -1;
	struct server_connection_callback *callback;
	callback = malloc(sizeof(struct server_connection_callback));
	client = netemu_resources_get_client();
	callback->ConnectionReceivedFn = ConnectionReceivedFn;
	client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener_async,&result);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr);
	hello = netemu_communication_create_hello_message(VERSION);
	free(hello);
}
*/
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args) {
	int result;
	int port;
	struct server_connection *connection;
	struct server_connection_callback *callback;

	callback = (struct server_connection_callback*)args;
	result = netemu_communication_parse_server_message(data);
	if (result == CONNECTION_ACCEPTED) {
		port = netemu_communication_parse_server_accept_port(data);
		connection = malloc(sizeof(connection));
		callback->ConnectionReceivedFn(result, connection);
	}
	else {
		callback->ConnectionReceivedFn(result, NULL);
	}

	free(callback);
	free(data);
}

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args) {
	int *port;
	int result;
	port = (int *)args;
	result = netemu_communication_parse_server_message(data);

	if(result == CONNECTION_ACCEPTED) {
		*port = netemu_communication_parse_server_accept_port(data);
	}
}

