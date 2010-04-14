/*
 * communicationhandler.h
 *
 *  Created on: 3 apr 2010
 *      Author: fabian
 */

#include <stdlib.h>
#include "network/netemu_receiver.h"
#include "protocol/communication.h"
#include "network/netemu_sender.h"
#include "netemu_resources.h"
#include "netemu_util.h"
#include "interface/server_connection.h"
#include "netemu_socket.h"
#define DOMAIN	"google.com"
#define SERVER	"http://kaillera.com/"
#define PATH	"raw_server_list2.php?wg=1&version=0.9"

#define VERSION						"0.83"

void kaillera_communication_listener(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);

struct server_connection_callback {
	void (*ConnectionReceivedFn)(int status, struct server_connection*);
};

struct netemu_communication_server* kaillera_communication_get_server_list() {
	struct netemu_sender_tcp *sender;
	struct netemu_addrinfo *info;
	char* request;
	char* buffer;
	int error;
	buffer = malloc(sizeof(1024));
	netemu_get_addr_info(DOMAIN,"80",NULL,&info);

	sender = netemu_receiver_tcp_new(&info->addr,info->addrlen,1024);
	request = netemu_communication_http_get(SERVER,PATH);

	netemu_sender_tcp_send(sender,request,strlen(request)+1);
	error = netemu_recv(sender->socket,buffer,1024,0);

}

struct server_connection* kaillera_communication_connect(struct netemu_sockaddr_in *addr) {
	struct netemu_client *client;
	struct server_connection *connection;
	char* hello;
	int result = -1;
	client = netemu_resources_get_client();
	client->receiver = netemu_util_prepare_receiver(CLIENT_PORT,kaillera_communication_listener,&result);
	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, addr);
	hello = netemu_communication_create_hello_message(VERSION);
	netemu_util_send_data(client->sender,hello);

	while(result == -1);
	free(hello);
	connection = malloc(sizeof(connection));
	/* TODO: Fix more data when it becomes available. */
	return connection;
}

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
	int *result;
	result = (int *)args;
	*result = netemu_communication_parse_server_message(data);
	free(data);
}

