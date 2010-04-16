
#include "kaillera_server_connection.h"
#include "../protocol/application.h"
#include "../netemu_resources.h"
#include "../network/netemu_packet_buffer.h"
#include "../network/netemu_sender_buffer.h"
#include "../network/netemu_sender.h"
#include "../network/netemu_receiver.h"

#include "netemu_list.h"
#include "netemu_thread.h"

void _server_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params);
int server_connection_login(struct server_connection* connection);
int server_connection_wait_for_instruction(struct server_connection* connection, int instruction_id);
void respondToPing(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);

struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	struct netemu_packet_buffer *receive_buffer;
	struct netemu_sender_buffer *send_buffer;
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
	struct application_instruction *message, *reply;

	mutex = netemu_thread_mutex_create();
	client = netemu_resources_get_client();
	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);
	buffer = netemu_transport_pack(&message,1);

	timestamp = time(NULL);
	netemu_packet_buffer_register_wakeup_on_instruction(connection->_internal->receive_buffer, CREATE_GAME, timestamp, mutex);
	if((error = netemu_sender_udp_send(client->sender,buffer.data,buffer.size)) != 0)
		return error;
	
	netemu_thread_mutex_lock(mutex, NETEMU_INFINITE);
	netemu_thread_mutex_release(mutex);
	netemu_packet_buffer_pop(connection->_internal->receive_buffer, CREATE_GAME);
	netemu_application_free_message(message);
	netemu_thread_mutex_destroy(mutex);

	return 0;
}


struct server_connection *server_connection_new(char* user, char* emulator_name) {
	struct server_connection *connection;

	connection = (struct server_connection*)malloc(sizeof(struct server_connection));
	if(connection == NULL) {
		return NULL;
	}
	connection->user = user;
	connection->emulator_name = emulator_name;
	connection->_internal = malloc(sizeof(struct _server_connection_internal));
	connection->_internal->chat_callback = netemu_list_new(3);
	connection->_internal->game_created_callback = netemu_list_new(3);
	connection->_internal->join_callback = netemu_list_new(3);
	connection->_internal->leave_callback = netemu_list_new(3);
	connection->_internal->receive_buffer = netemu_packet_buffer_new(100);
	connection->_internal->send_buffer = netemu_sender_buffer_new(5,10);

	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,PING,respondToPing, connection);

	netemu_receiver_udp_register_recv_fn(netemu_resources_get_receiver(), _server_connection_receive, connection);
	server_connection_login(connection);
	return connection;
}

int server_connection_login(struct server_connection* connection) {
	struct application_instruction *message;
	struct netemu_sender_udp *sender;
	sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_login_request_add(message,connection->emulator_name,connection->user,1);
	message->important = 1;
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	server_connection_wait_for_instruction(connection,LOGIN_SUCCESS);
	netemu_packet_buffer_pop(connection->_internal->receive_buffer, LOGIN_SUCCESS);
	return 1;
}

int server_connection_wait_for_instruction(struct server_connection* connection, int instruction_id) {
	netemu_mutex mutex;
	time_t timestamp;
	mutex = netemu_thread_mutex_create();
	timestamp = time(NULL);
	netemu_packet_buffer_register_wakeup_on_instruction(connection->_internal->receive_buffer, CREATE_GAME, timestamp, mutex);
	netemu_thread_mutex_lock(mutex, NETEMU_INFINITE);
	netemu_thread_mutex_release(mutex);
	netemu_thread_mutex_destroy(mutex);
	return 1;
}

void respondToPing(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct application_instruction* pong;
	struct server_connection* connection;
	connection = (struct server_connection*)arg;
	pong = netemu_application_create_message();
	netemu_application_pong_add(pong);
	netemu_sender_buffer_add(connection->_internal->send_buffer,pong);
}

void _server_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params) {
	struct server_connection* connection;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	int i;
	connection = (struct server_connection*) params;
	packet = netemu_transport_unpack(data);
	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		netemu_packet_buffer_add(connection->_internal->receive_buffer,instruction);
	}
}
