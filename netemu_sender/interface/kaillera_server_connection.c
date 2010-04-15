
#include "kaillera_server_connection.h"
#include "../netemu_resources.h"
#include "../network/netemu_sender.h"
#include "netemu_list.h"
#include "netemu_thread.h"

//void _server_connection_receive(char*, size_t, struct netemu_receiver*, void*);

struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	struct netemu_packet_buffer *receive_buffer;
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
	return netemu_sender_udp_send(client->sender,buffer.data,buffer.size);
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
	return netemu_sender_udp_send(client->sender ,buffer.data, buffer.size);
}

int server_connection_create_game(struct server_connection *connection, char *gamename, struct game* result) {
	netemu_mutex mutex;
	int error;
	time_t timestamp;
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1], *reply;

	mutex = netemu_thread_mutex_create();
	client = netemu_resources_get_client();
	client->sender->addr = connection->addr;

	messages[0] = netemu_application_create_message();
	netemu_application_create_game_add(messages[0], gamename);
	buffer = netemu_transport_pack(messages,1);

	timestamp = time(NULL);
	netemu_packet_buffer_register_wakeup_on_instruction(connection->_internal->receive_buffer, CREATE_GAME, timestamp, mutex);
	if((error = netemu_sender_udp_send(client->sender,buffer.data,buffer.size)) != 0)
		return error;
	
	netemu_thread_mutex_lock(mutex, NETEMU_INFINITE);
	netemu_thread_mutex_release(mutex);
	netemu_packet_buffer_pop(connection->_internal->receive_buffer, CREATE_GAME);
	netemu_application_free_message(messages[0]);
	netemu_thread_mutex_destroy(mutex);
}

struct server_connection *server_connection_new(char* user) {
	struct server_connection *connection;

	connection = (struct server_connection*)malloc(sizeof(struct server_connection));
	if(connection == NULL) {
		return NULL;
	}
	connection->user = user;
	connection->_internal = malloc(sizeof(struct _server_connection_internal));
	connection->_internal->chat_callback = netemu_list_new(sizeof(chatFn), 3);
	connection->_internal->game_created_callback = netemu_list_new(sizeof(gameCreatedFn), 3);
	connection->_internal->join_callback = netemu_list_new(sizeof(joinFn), 3);
	connection->_internal->leave_callback = netemu_list_new(sizeof(leaveFn), 3);
	//connection->_internal->receive_buffer = netemu_packet_buffer_new(
	//client = netemu_resources_get_client();
	//netemu_receiver_register_recv_fn(client->receiver, _server_connection_receive, NULL);
	return connection;
}

void _server_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params) {

}
