/**
 * @file
 * This file contains functions for handling communication layer stuff in kaillera.
 */

/**
 * @defgroup netemu_communication Kaillera communbication layer
 * Code related to the kaillera communication layer.
 */

#include "protocol/communication.h"
#include <stdlib.h>
#include "netemu_info.h"
#include "network/netemu_tcp.h"
#include "netemu_resources.h"
#include "network/netemu_sender_udp.h"
#include "network/netemu_receiver.h"
#include "netemu_thread.h"
#include "netemu_util.h"
#include "netlib_error.h"
#include "netemu_socket.h"
#include "network/netemu_sender_buffer.h"
#include "interface/netemu_kaillera.h"
#include "network/netemu_net.h"
#include "network/netemu_sender_buffer.h"
#include "protocol/application.h"

/* These defines are temporary and let's you connect to the official kaillera server list. */
#define DOMAIN	"www.kaillera.com"
#define SERVER	"kaillera.com"
#define PATH	"/raw_server_list2.php?wg=1&version=0.9"
#define VERSION	"0.83"

/**
 * This struct defines a communication callback. It is for initiating a connection
 * synchronously or asynchornously to a server.
 * @ingroup netemu_kaillera
 * @ingroup netemu_communication
 */
struct communication_callback {
	NETEMU_BOOL async; /**< Wheter or not this callback is handled asynchronously. */
	NETEMU_BOOL response_received;
	int port; /**< the server port */
	netemu_sockaddr_in *addr; /**< The address to the server. */
	connectionAcquiredFn fn;
	struct netemu_kaillera *connection;
	netemu_event instruction_received_event;
	void *arg;
};

int netemu_kaillera_communication_listener(NETEMU_SOCKET socket, netemu_connection_types type, union netemu_connection_type connection, void* args);
void kaillera_communication_listener_async(char* data, size_t size, struct netemu_receiver_udp* receiver, void* args);
void _netemu_kaillera_communication_login(void *param);

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

/**
 * Connect to a kaillera server. This is a blocking call.
 * @ingroup netemu_kaillera
 * @ingroup netemu_communication
 * @param connection the instance of the netemu_kaillera module.
 * @param local_address the address to bind to. It must be in network byte order.
 * @param local_port the port to bind to. It muste be in network byte order.
 * @param server_address the address of the server. It muste be in network byte order.
 * @param server_port the server port. It must be in network byte order.
 */
struct netemu_kaillera* netemu_kaillera_connect(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port, NETEMU_DWORD server_address, unsigned short server_port) {
	struct netemu_client *client;
	char* hello;
	struct communication_callback callback;
	int connection_attempts;
	netemu_sockaddr_in in_addr;

	/* We set the port to -1, indicating that we don't know the port number yet. */
	callback.port = -1;
	/* This is not an async call. */
	callback.async = -1;
	callback.response_received = FALSE;
	callback.instruction_received_event = netemu_thread_event_create();
	client = netemu_resources_get_client();
	in_addr.sin_addr.s_addr = local_address;
	in_addr.sin_port = local_port;
	in_addr.sin_family = NETEMU_AF_INET;

	if(client == NULL)
		return NULL;

	client->receiver = netemu_receiver_udp_new((struct sockaddr*)&in_addr, sizeof(in_addr));
	if(client->receiver == NULL)
		return NULL;
	netemu_receiver_udp_start_receiving(client->receiver, netemu_kaillera_communication_listener, &callback);

	in_addr.sin_addr.s_addr = server_address;
	in_addr.sin_port = server_port;
	in_addr.sin_family = NETEMU_AF_INET;

	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, &in_addr, sizeof(in_addr));
	if(client->sender == NULL) {
		netemu_receiver_udp_free(client->receiver);
		return NULL;
	}

	if((hello = netemu_communication_create_hello_message(VERSION)) == NULL) {
		netemu_sender_udp_free(client->sender);
		netemu_receiver_udp_free(client->receiver);
		return NULL;
	}

	connection_attempts = 0;

	/* TODO: Fix better constants here. */
	while(connection_attempts < 5 && !callback.response_received) {
		netemu_util_send_data(client->sender,hello);
		if(netemu_thread_event_wait(callback.instruction_received_event, 3) == NETEMU_WAIT_TIMEOUT) {
			connection_attempts++;
		}
	}
	free(hello);
	while(callback.port == -1);
	netemu_receiver_udp_stop_receiving(client->receiver);
	in_addr.sin_port = netemu_htons(callback.port);
	client->sender->addr = netemu_util_copy_addr((netemu_sockaddr*)&in_addr,sizeof(in_addr));

	netemu_receiver_udp_start_receiving(client->receiver, netemu_application_parse_udp, connection->_internal->receive_buffer);
	netemu_kaillera_login(connection);
	return connection;
}

int netemu_kaillera_connect_async(struct netemu_kaillera *connection, NETEMU_DWORD local_address, unsigned short local_port,
		NETEMU_DWORD server_address, unsigned short server_port, connectionAcquiredFn connectionFn, void* user_data) {
	struct netemu_client *client;
	struct communication_callback *callback;
	netemu_sockaddr_in in_addr;

	callback = malloc(sizeof(struct communication_callback));
	/* We set the port to -1, indicating that we don't know the port number yet. */
	callback->port = -1;
	/* This is not an async call. */
	callback->async = TRUE;
	callback->response_received = FALSE;
	callback->connection = connection;
	callback->fn = connectionFn;
	callback->arg = user_data;
	callback->instruction_received_event = netemu_thread_event_create();
	client = netemu_resources_get_client();
	in_addr.sin_addr.s_addr = local_address;
	in_addr.sin_port = local_port;
	in_addr.sin_family = NETEMU_AF_INET;

	if(client == NULL)
		return -1;

	client->receiver = netemu_receiver_udp_new((struct sockaddr*)&in_addr, sizeof(in_addr));
	if(client->receiver == NULL)
		return -1;

	in_addr.sin_addr.s_addr = server_address;
	in_addr.sin_port = server_port;
	in_addr.sin_family = NETEMU_AF_INET;

	client->sender = netemu_util_prepare_sender_on_socket_at_addr(client->receiver->socket, &in_addr, sizeof(in_addr));

	if(client->sender == NULL) {
		netemu_receiver_udp_free(client->receiver);
		return -1;
	}
	netemu_thread_new(_netemu_kaillera_communication_login, callback);

	return 0;
}

void _netemu_kaillera_communication_login(void *param) {
	struct netemu_client *client;
	struct netemu_kaillera* connection;
	int connection_attempts;
	netemu_sockaddr_in addr;
	struct communication_callback *callback;
	char *hello;
	client = netemu_resources_get_client();
	callback = param;
	connection_attempts = 0;
	connection = callback->connection;
	/* if this is not an async callback, then we should not be here. */
	if(!callback->async)
		return;

	if((hello = netemu_communication_create_hello_message(VERSION)) == NULL) {
		netemu_sender_udp_free(client->sender);
		netemu_receiver_udp_free(client->receiver);
		return;
	}

	netemu_receiver_udp_start_receiving(client->receiver, netemu_kaillera_communication_listener, callback);
	/* TODO: Fix better constants here. */
	while(connection_attempts < 5 && !callback->response_received) {
		netemu_util_send_data(client->sender,hello);
		if(netemu_thread_event_wait(callback->instruction_received_event, 3) == NETEMU_WAIT_TIMEOUT) {
			connection_attempts++;
		}
	}
	free(hello);
	netemu_receiver_udp_stop_receiving(client->receiver);
	((netemu_sockaddr_in*)client->sender->addr)->sin_port = netemu_htons(callback->port);
	netemu_receiver_udp_start_receiving(client->receiver, netemu_application_parse_udp, connection->_internal->receive_buffer);
	netemu_kaillera_login(connection);
	callback->fn(callback->connection, 1, callback->arg);
}

int netemu_kaillera_communication_listener(NETEMU_SOCKET socket, netemu_connection_types type, union netemu_connection_type connection, void* args) {
	struct communication_callback *callback;
	int result;
	int error;
	int ret;
	char* buffer;

	ret = 1;
	callback = (struct communication_callback*)args;
	buffer = malloc(128);
	error = netemu_recvfrom(socket, buffer, 128, 0, NULL, 0);
	if(error == -1) {
		error = netlib_get_last_error();
		return -1;
	}
	result = netemu_communication_parse_server_message(buffer);
	if(result == CONNECTION_ACCEPTED) {
		callback->port = netemu_communication_parse_server_accept_port(buffer);
		ret = 0;
	}
	else if(result == CONNECTION_REJECTED_TOO) {
		ret = 0;
	}
	callback->response_received = TRUE;
	netemu_thread_event_signal(callback->instruction_received_event);
	free(buffer);
	return ret;
}
