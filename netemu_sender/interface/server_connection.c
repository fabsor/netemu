﻿#include "server_connection.h"
#include "../netemu_resources.h"
#include "../network/netemu_sender.h"
#include "netemu_list.h"

void _server_connection_receive(char*, size_t, struct netemu_receiver*, void*);

struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
};

int server_connection_send_chat_message(struct server_connection *connection, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];

	client = netemu_resources_get_client();
	client->sender->addr = connection->addr;
	messages[0] = netemu_application_create_message();
	netemu_application_chat_partyline_add(messages[0], message, connection->user);
	buffer = netemu_transport_pack(messages,1);
	return netemu_sender_send(client->sender,buffer.data,buffer.size);
}

int server_connection_register_chat_callback(struct server_connection *connection, chatFn callback) {
	netemu_list_add(connection->_internal->chat_callback, callback);
	return 0;
}

int server_connection_unregister_chat_callback(struct server_connection *connection, chatFn callback) {
	return netemu_list_remove(connection->_internal->chat_callback, callback);
}

int server_connection_register_user_join_callback(struct server_connection *connection, joinFn callback) {
	netemu_list_add(connection->_internal->join_callback, callback);
	return 0;
}

int server_connection_unregister_user_join_callback(struct server_connection *connection, joinFn callback) {
	return netemu_list_remove(connection->_internal->join_callback, callback);
}

int server_connection_register_user_leave_callback(struct server_connection *connection, leaveFn callback) {
	netemu_list_add(connection->_internal->leave_callback, callback);
	return 0;
}

int server_connection_unregister_user_leave_callback(struct server_connection *connection, leaveFn callback) {
	return netemu_list_remove(connection->_internal->leave_callback, callback);
}

int server_connection_disconnect(struct server_connection *connection, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];

	client = netemu_resources_get_client();
	client->sender->addr = connection->addr;
	messages[0] = netemu_application_create_message();
	netemu_application_user_leave_add(messages[0], message);
	buffer = netemu_transport_pack(messages,1);
	return netemu_sender_send(client->sender ,buffer.data, buffer.size);
}

int server_connection_create_game(struct server_connection *connection, char *gamename, struct game* result) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];

	client = netemu_resources_get_client();
	client->sender->addr = connection->addr;

	messages[0] = netemu_application_create_message();
	netemu_application_create_game_add(messages[0], gamename);
	buffer = netemu_transport_pack(messages,1);
	netemu_sender_send(client->sender,buffer.data,buffer.size);
}

struct server_connection *server_connection_new(struct netemu_sockaddr_in *addr) {
	struct server_connection *connection;
	struct netemu_client *client;
	connection = malloc(sizeof(struct server_connection));
	connection->addr = addr;
	connection->user = NULL;
	connection->_internal = malloc(sizeof(struct _server_connection_internal));
	connection->_internal->chat_callback = netemu_list_new(sizeof(chatFn), 3);
	connection->_internal->game_created_callback = netemu_list_new(sizeof(gameCreatedFn), 3);
	connection->_internal->join_callback = netemu_list_new(sizeof(joinFn), 3);
	connection->_internal->leave_callback = netemu_list_new(sizeof(leaveFn), 3);

	client = netemu_resources_get_client();
	netemu_receiver_register_recv_fn(client->receiver, _server_connection_receive, NULL);
	return connection;
}

void _server_connection_receive(char* data, size_t size, struct netemu_receiver* receiver, void* params) {

}