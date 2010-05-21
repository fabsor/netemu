/**
 * @file
 * This file implements the netemu_p2p interface.
 * Basicly this file contains that glues the underlying packet handling
 * and the interface functions together.
 */

/**
 * @defgroup netemu_p2p The netemu p2p module
 * The netemu_p2p module implements the netemu_p2p interface.
 * It contains all external code that can be used by clients for
 * using the netemu_p2p protocol.
 */

#include "netemu_info.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_packet_buffer.h"
#include "network/netemu_sender_collection.h"
#include "network/netemu_packet_buffer.h"
#include "interface/netemu_p2p.h"
#include "netemu_info.h"
#include "protocol/application_p2p.h"
#include "network/netemu_tcp.h"
#include "netemu_list.h"
#include "protocol/application.h"
#include "protocol/application_p2p_internal.h"

/* Function declarations */
void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection);
void _netemu_p2p_send_login_success(struct netemu_p2p_connection *info, struct netemu_tcp_connection *recipient);
void _netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);
void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void _netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_send_user_joined(struct netemu_p2p_connection *connection, struct p2p_user *user);
void _netemu_p2p_respond_to_player_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void _netemu_p2p_respond_to_game_created(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
int _netemu_p2p_addr_compare(const void *arg1, const void *arg2);
int _netemu_p2p_user_compare(const void *arg1, const void *arg2);
int _netemu_p2p_game_compare(const void *arg1, const void *arg2);
struct netemu_tcp_connection *_netemu_p2p_connect_to(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size);
void _netemu_p2p_add_player(struct p2p_game * game, struct p2p_user *player);
void netemu_p2p_join_host(struct netemu_p2p_connection *p2p, struct p2p_user *user, struct netemu_tcp_connection *connection);
void _netemu_p2p_respond_to_join_host(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_game_started(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void _netemu_p2p_send_login_request(struct netemu_p2p_connection *p2p);
void netemu_p2p_respond_to_ready(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_play_values(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
struct netemu_tcp_connection *_netemu_p2p_connect_to_async(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size);
NETEMU_BOOL _netemu_p2p_player_exists(struct p2p_game *game, struct p2p_user *player);
void netemu_process_user_value(struct netemu_p2p_connection *info, struct p2p_user *player);
int netemu_p2p_receive_play_values(struct netemu_p2p_connection *info);
void netemu_p2p_respond_to_cached_play_values(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void _netemu_p2p_register_responders(struct netemu_p2p_connection *p2p);
/* End of function declarations */

/**
 * Create a new netemu_p2p instance.
 * This function creates a new p2p instance.
 * @ingroup netemu_p2p
 * @param username the username that you want to use in this p2p instance.
 * @param application name the name of the application.
 * @param connection_quality The connection quality that you want to use. The connection quality
 * decides how fast you send data packets.\n <b>Example:</b>\n
 * If you have a connection_quality of 1, then you will send one play value, then wait for all others.\n
 * If you have a connection quality of 2, then you will send two play values, then wait for all others.\n
 * etc.
 */
struct netemu_p2p_connection* netemu_p2p_new(char* username, char* emulatorname, char connection_quality) {
	struct netemu_p2p_connection *p2p;
	union netemu_connection_type *type;

	type = malloc(sizeof(union netemu_connection_type));
	type->collection = netemu_sender_collection_new();
	p2p = malloc(sizeof(struct netemu_p2p_connection));

	p2p->user = malloc(sizeof(struct p2p_user));
	p2p->user->name = username;
	p2p->user->app_name = emulatorname;
	p2p->user->connection = connection_quality;
	p2p->user->ping = 0;
	p2p->user->_internal = netemu_application_p2p_create_user_internal();
	p2p->current_game = NULL;
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	p2p->_internal->users = netemu_list_new(10,TRUE);
	p2p->_internal->games = netemu_list_new(10,TRUE);
	p2p->_internal->send_buffer = netemu_sender_buffer_new(5,20);
	p2p->_internal->receive_buffer = netemu_packet_buffer_new(100);
	netemu_list_add(p2p->_internal->users, p2p->user);
	p2p->_internal->login_connection = NULL;
	p2p->_internal->continueFn = NULL;
	p2p->_internal->login_callbacks = netemu_list_new(2,0);
	p2p->_internal->play_values_callbacks = netemu_list_new(2,0);
	p2p->_internal->game_created_callbacks = netemu_list_new(2,0);
	p2p->_internal->join_callbacks = netemu_list_new(2,0);
	p2p->_internal->game_started_callbacks = netemu_list_new(2,0);
	p2p->_internal->all_ready_callbacks = netemu_list_new(2,0);
	p2p->_internal->player_joined_callbacks = netemu_list_new(2,0);


	p2p->_internal->values_to_send = NULL;
	p2p->_internal->values_buffered = 0;
	p2p->_internal->sent_values = 0;
	p2p->_internal->frame_index = 0;

	netemu_list_register_sort_fn(p2p->_internal->games, _netemu_p2p_game_compare);
	netemu_list_register_sort_fn(p2p->_internal->users, _netemu_p2p_user_compare);
	_netemu_p2p_register_responders(p2p);
	p2p->_internal->peers = type->collection;
	return p2p;
}

/**
 * Register responders that will be used by this module.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p instance.
 */
void _netemu_p2p_register_responders(struct netemu_p2p_connection *p2p) {
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_LOGIN_REQUEST, netemu_p2p_respond_to_login_request, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_LOGIN_SUCCESS, _netemu_p2p_respond_to_login_success, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_USER_JOIN, netemu_p2p_respond_to_user_join, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, CREATE_P2P_GAME, _netemu_p2p_respond_to_game_created, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, JOIN_P2P_GAME, _netemu_p2p_respond_to_player_join, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_JOIN_HOST, _netemu_p2p_respond_to_join_host, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_GAME_START, netemu_p2p_respond_to_game_started, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_PLAYER_READY, netemu_p2p_respond_to_game_started, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_READY, netemu_p2p_respond_to_ready, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_BUFFERED_PLAY_VALUES, netemu_p2p_respond_to_play_values, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_CACHED_BUFFERED_PLAY_VALUES, netemu_p2p_respond_to_cached_play_values, p2p);
}

/**
 * Establish a new connection to a host.
 * @param listener the tcp listener you want to use.
 * @param connection the connection to the player you want to
 * establish a connection to.
 */
void _netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params) {
	struct netemu_p2p_connection *p2p;
	p2p = (struct netemu_p2p_connection*)params;
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers, connection);
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp ,p2p->_internal->receive_buffer);
	_netemu_p2p_send_ready(p2p->_internal->send_buffer,connection);
}

void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	instruction = netemu_application_create_message();
	netemu_application_p2p_ready_add(instruction);
	type.connection = connection;
	netemu_sender_buffer_add(buffer, instruction,TCP_CONNECTION, type);
}

/**
 * Host a new P2P cloud.
 * This will let other users connect to you on the designated port. By calling this function,
 * you will initiate a new netemu cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param address the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 */
int netemu_p2p_host(struct netemu_p2p_connection* p2p, NETEMU_DWORD address, unsigned short port, char* cloudname) {
	int size;
	netemu_sockaddr* addr;
	addr = (netemu_sockaddr*)netemu_util_create_addr(address, port, &size);
	p2p->_internal->host = netemu_tcp_listener_new(addr, size);
	if(p2p->_internal->host->error != 0) {
		return -1;
	}
	p2p->user->addr = address;
	p2p->user->port = port;
	p2p->cloud_name = cloudname;

	netemu_tcp_listener_register_new_connection_fn(p2p->_internal->host,_netemu_p2p_new_connection,p2p);
	netemu_tcp_listener_start_listening(p2p->_internal->host);
	return 1;
}
/**
 * Connect to a host. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte order.
 */
int netemu_p2p_connect(struct netemu_p2p_connection* p2p, NETEMU_DWORD in_addr, unsigned short port,  NETEMU_DWORD connect_addr, unsigned short connect_port) {
	int error, size;
	struct netemu_tcp_connection *connection;
	netemu_sockaddr *saddr;

	error = 1;
	saddr = (netemu_sockaddr*)netemu_util_create_addr(connect_addr,connect_port,&size);
	netemu_p2p_host(p2p, in_addr,port,"thecloud");
	connection = _netemu_p2p_connect_to(p2p,saddr,size);
	netemu_p2p_login(p2p);
	return error;
}

/**
 * Connect asyncronously to a cloud. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @todo This function needs to be refactored to use addresses and ports as long and short instead of structs.
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte orde
 */
int netemu_p2p_connect_async(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *in_addr, int in_addr_size,  netemu_sockaddr_in *connect_addr, int connect_addr_size, connectedFn callback) {
	int error;
	struct netemu_tcp_connection *connection;
	union p2p_callback_fn *fn;
	fn = malloc(sizeof(union p2p_callback_fn));
	fn->connectFn = callback;
	error = 0;
	netemu_list_add(p2p->_internal->login_callbacks,fn);
	netemu_p2p_host(p2p, in_addr, in_addr_size,"thecloud");
	connection = _netemu_p2p_connect_to_async(p2p, netemu_util_copy_addr((netemu_sockaddr*)connect_addr,connect_addr_size), connect_addr_size);
	netemu_p2p_login(p2p);
	return error;
}

/**
 * Login to the cloud. In order to participate, you need to be logged in. After logging in,
 * you receive a list of games and users, and you will be notified about new games and new users.
 * @ingroup netemu_p2p
 * @param p2p the netemu module instance.
 */
void netemu_p2p_login(struct netemu_p2p_connection *p2p) {
	struct application_instruction *message;
	struct netemu_packet_buffer_item *item;
	union netemu_connection_type type;
	time_t timestamp;
	type.collection = p2p->_internal->peers;
	message = netemu_application_create_message();
	netemu_application_p2p_login_request_add(message,p2p->user->addr,p2p->user->port,p2p->user->name,p2p->user->app_name, 1);
	message->important = 1;
	timestamp = time(NULL);

	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,CONNECTION_COLLECTION,type);
	item = netemu_packet_buffer_wait_for_instruction(p2p->_internal->receive_buffer, P2P_LOGIN_SUCCESS, timestamp);
}

/**
 * Join a host. In order to be able to use a connection to a user, you must send this instruction.
 * This will make the host aware about your existence and it will send data to you. Note that this is
 * not the same as logging in, since this only make the host aware of your connection.
 * If you do not send a join_host instruction to every user you connect to, it will automaticly discard your
 * connection.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param user the user you want to identify yourself as.
 * @param connection the connection you want to send the instruction on.
 */
void netemu_p2p_join_host(struct netemu_p2p_connection *p2p, struct p2p_user *user, struct netemu_tcp_connection *connection) {
	struct application_instruction *message;
	union netemu_connection_type type;
	time_t timestamp;
	message = netemu_application_create_message();
	netemu_application_p2p_join_host_add(message, user);
	type.connection = connection;
	message->important = 1;
	timestamp = time(NULL);
	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,TCP_CONNECTION,type);
}
/**
 * Connect to a specific player.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param addr the sockaddr to connect to.
 * @param the size of the struct.
 */
struct netemu_tcp_connection *_netemu_p2p_connect_to(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size) {
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	int error;

	connection = netemu_tcp_connection_new(connect_addr,connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	error = netemu_tcp_connection_connect(connection);
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp, p2p->_internal->receive_buffer);
	timestamp = time(NULL);
	netemu_packet_buffer_wait_for_instruction(p2p->_internal->receive_buffer, P2P_READY, timestamp);
	return connection;
}

/**
 * Connect asyncronously.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param addr the sockaddr to connect to.
 * @param the size of the struct.
 */
struct netemu_tcp_connection *_netemu_p2p_connect_to_async(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size) {
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	int error;

	connection = netemu_tcp_connection_new(connect_addr,connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	p2p->_internal->continueFn = _netemu_p2p_send_login_request;
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp, p2p->_internal->receive_buffer);
	error = netemu_tcp_connection_connect(connection);

	timestamp = time(NULL);
	return connection;
}

/**
 * Send a login request.
 * This function just sends a login request to the host, and will not block and
 * wait for a response.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 */
void _netemu_p2p_send_login_request(struct netemu_p2p_connection *p2p) {
	struct application_instruction *message;
	union netemu_connection_type type;
	time_t timestamp;
	type.collection = p2p->_internal->peers;
	message = netemu_application_create_message();
	netemu_application_p2p_login_request_add(message,p2p->user->addr,p2p->user->port,p2p->user->name,p2p->user->app_name, 1);
	message->important = 1;
	timestamp = time(NULL);
	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,CONNECTION_COLLECTION,type);
}
/**
 * Send a login success instruction.
 * @ingroup netemu_p2p
 * @param info the netemu_p2p instance.
 * @param recipient the recipient.
 */
void _netemu_p2p_send_login_success(struct netemu_p2p_connection *info, struct netemu_tcp_connection *recipient) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	type.connection = recipient;
	instruction = netemu_application_create_message();
	netemu_application_p2p_login_success_add(instruction,info->user, info->_internal->users,info->_internal->games);
	netemu_sender_buffer_add(info->_internal->send_buffer,instruction,TCP_CONNECTION,type);
}

/**
 * Create a new game. This function will create a new game which will be broadcasted to all
 * peers. This is a non-blocking call, since there's no requirement (or a way to check) that
 * all other users have received the notification that the game have been created.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param gamename the name of the game you want to create
 * @param connection_quality the quality of the connection for this game.
 * @param emulator_value_size This is the size of the play values for one player in the game.
 * No value sent over the network in this game can be larger than this.
 * @param result the created game.
 * @return The return value can be:
 * - 1 if all went well
 * - -1 if something went wrong. This generally indicates that you already have created a game.
 */
int netemu_p2p_create_game(struct netemu_p2p_connection *connection, char *gamename, char connection_quality, int emulator_value_size, struct p2p_game** result) {
	struct p2p_game* game;
	struct application_instruction* instruction;
	union netemu_connection_type type;

	if(connection->current_game == NULL)
		return -1;

	type.collection = connection->_internal->peers;
	instruction = netemu_application_create_message();
	connection->user->_internal->player_no = 1;
	/* We add our new game to the list. */
	netemu_application_p2p_create_game_add(instruction,gamename,connection->user->app_name,connection->user, connection_quality, emulator_value_size);
	game = (struct p2p_game *)instruction->body;
	/* We have created a game, so we will set this to our current game. */
	connection->current_game = game;
	connection->_internal->play_values_event = netemu_thread_event_create();
	netemu_list_add(connection->_internal->games, game);
	game->_internal = netemu_application_p2p_create_game_internal();
	game->_internal->tcp_collection = netemu_sender_collection_new();
	game->_internal->udp_collection = netemu_sender_collection_new();
	game->creator->_internal->values_received = TRUE;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 1;
}
/**
 * Join an existing game. This is a blocking call that will connect to the creator of the
 * game if there isn't a connection already, and then send a "Join game" instruction.
 * @param connection an instance of the netemu_p2p module.
 * @param game the game you want to join. Use netemu_get_game_list to get a game to join.
 * @return the return value can be:
 * - 1 if everything went alright.
 * - -1 if you're already in a game.
 * - -2 if you try to join a game which you have created.
 * @todo we need more errors here for:
 * - if you can't connect to a host.
 * - if the host refuses you.
 */
int netemu_p2p_join_game(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	netemu_sockaddr* addr;
	int size;

	/* You can't join a game if you're already in one.*/
	if(connection->current_game != NULL) {
		return -1;
	}

	/* We can't join a game created by you, since you're always part of your own game.*/
	if(_netemu_p2p_user_compare(connection->user,game->creator) == 0) {
		return -2;
	}

	/* You belong to this game now, at least locally. */
	connection->current_game = game;

	/*TODO: This is temporary, we need a better fix for this. */
	connection->user->_internal->player_no = 2;
	connection->user->_internal->values_received = TRUE;

	/* Add yourself to the player list. */
	_netemu_p2p_add_player(game,connection->user);

	/* Create a connection to the creator of this game if we don't have one already. */
	if(game->creator->_internal == NULL || (game->creator->_internal->connection == NULL)) {
		game->creator->_internal = netemu_application_p2p_create_user_internal();
		addr = (netemu_sockaddr*)netemu_util_create_addr(game->creator->addr,game->creator->port,&size);
		game->creator->_internal->connection = _netemu_p2p_connect_to(connection,addr,size);
		netemu_p2p_join_host(connection,game->creator, game->creator->_internal->connection);
	}

	/*Create internal stuff*/
	game->_internal = netemu_application_p2p_create_game_internal();
	game->_internal->tcp_collection = netemu_sender_collection_new();
	game->_internal->udp_collection = netemu_sender_collection_new();

	netemu_sender_collection_add_tcp_sender(game->_internal->tcp_collection, game->creator->_internal->connection);
	connection->_internal->play_values_event = netemu_thread_event_create();
	/* Send join game instruction to the creator. */
	type.connection = game->creator->_internal->connection;
	instruction = netemu_application_create_message();
	netemu_application_p2p_player_join_add(instruction, connection->user);
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, TCP_CONNECTION, type);
	return 1;
}

/**
 * Start a game. In order for the game to start, you must call this function.
 * This function will send a start game instruction to all players, and this will
 * cause them to make the necessary arrangement for playing the game such as binding to
 * UDP ports. If you are the creator of the game, the only thing you have to do is
 * call this function and it will be implicated that you are ready to play. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 1 if everything went OK
 * - -1 if you're not in a game or if you're not the owner.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_start_game(struct netemu_p2p_connection *connection, NETEMU_DWORD listen_addr, unsigned short listen_port) {
	struct netemu_receiver_udp *receiver;
	struct application_instruction *instruction;
	union netemu_connection_type type;
	netemu_sockaddr_in addr;
	int cmp;
	/*We can't start a game if a game is not created first, and you are the owner of the game.*/
	if(connection->current_game == NULL || (cmp = _netemu_p2p_user_compare(connection->current_game->creator, connection->user)) != 0)
		return -1;

	/* Create an addr_in  */
	addr.sin_addr.s_addr = listen_addr;
	addr.sin_family = NETEMU_AF_INET;
	addr.sin_port = listen_port;

	receiver = netemu_receiver_udp_new((netemu_sockaddr*)&addr,sizeof(addr));
	netemu_receiver_udp_start_receiving(receiver, netemu_application_parse_udp,connection->_internal->receive_buffer);
	connection->user->_internal->receiver = receiver;
	connection->current_game->creator->_internal = connection->user->_internal;
	connection->_internal->values_received = malloc((connection->current_game->emulator_value_size*
			connection->current_game->user_count)*connection->current_game->connection_quality);
	instruction = netemu_application_create_message();
	netemu_application_p2p_start_game_add(instruction, listen_addr, listen_port);
	/* This instruction will be broadcasted to all, in order for all clients to register that the game has started. */
	type.collection = connection->_internal->peers;
	connection->current_game->_internal->ready_count++;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 1;
}
/**
 * Indicate that you are ready to play. In order for the game to start, you must call this function.
 * This function will send a player ready instruction to all players, and this will
 * cause them to make register the address to which they can contact you wile playing.
 * If you are the creator of the game, you don't have to call this function, since netemu_p2p_start_game will
 * do everything this function does for you. Also note that you have to recieve the start game instruction before
 * calling this function. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the other players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 1 if everything went OK
 * - -1 if you're not in a game, if you're the creator of this game or if the game hasn't started.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_player_ready(struct netemu_p2p_connection *connection, NETEMU_DWORD listen_addr, unsigned short listen_port) {
	struct netemu_receiver_udp *receiver;
	struct application_instruction *instruction;
	union netemu_connection_type type;
	netemu_sockaddr_in addr;
	/* We can't join a game if a game is not created and started first, and you are not the owner of the game. */
	if(connection->current_game == NULL || (_netemu_p2p_user_compare(connection->current_game->creator, connection->user)) == -1)
		return -1;

	addr.sin_addr.s_addr = listen_addr;
	addr.sin_family = NETEMU_AF_INET;
	addr.sin_port = listen_port;

	receiver = netemu_receiver_udp_new((netemu_sockaddr*)&addr,sizeof(addr));
	netemu_receiver_udp_start_receiving(receiver, netemu_application_parse_udp, connection->_internal->receive_buffer);
	connection->user->_internal->receiver = receiver;
	instruction = netemu_application_create_message();
	netemu_application_p2p_player_ready_add(instruction, listen_addr, listen_port);
	connection->current_game->_internal->ready_count++;
	if(connection->current_game->_internal->ready_count >= connection->current_game->user_count) {
		connection->current_game->started = 1;
	}
	/* This instruction will be broadcasted to all game members */
	type.collection = connection->current_game->_internal->tcp_collection;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 1;
}
/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p_connection* info, int *count) {
	struct p2p_game** games;
	netemu_list_copy(info->_internal->games,(void***)&games);
	*count = info->_internal->games->count;
	return games;
}
/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p_connection* info, int *count) {
	struct p2p_user** users;
	netemu_list_copy(info->_internal->users,(void***)&users);
	*count = info->_internal->users->count;
	return users;
}

/**
 * This function responds to all "Login success" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_p2p_connection *info;
	struct p2p_login_success *success;
	struct p2p_user *user;
	struct p2p_game *game;
	int i;
	info = (struct netemu_p2p_connection *)arg;
	success = (struct p2p_login_success*)item->instruction->body;
	for(i = 0; i < success->users_count; i++) {
		user = malloc(sizeof(struct p2p_user));
		netemu_application_p2p_copy_user(user, &success->users[i]);
		netemu_list_add(info->_internal->users,user);

		/* The first user in the list is always the host, so lets add the contact information we know is here. */
		if(i == 0 && item->type == TCP_CONNECTION) {
			user->_internal = netemu_application_p2p_create_user_internal();
			user->_internal->connection = item->connection.connection;
		}
	}
	for(i = 0; i < success->games_count; i++) {
		game = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(game, &success->games[i]);
		netemu_list_add(info->_internal->users,user);
	}
	/* Notify any listeners about successful login. */
	for(i = 0; i < info->_internal->login_callbacks->count; i++) {
		((union p2p_callback_fn*)info->_internal->login_callbacks->elements[i])->connectFn(info, 1);
	}
}
/**
 * This function responds to all "Login request" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int i;
	connection = (struct netemu_p2p_connection *)arg;
	type.collection = connection->_internal->peers;
	user = malloc(sizeof(struct p2p_user));
	netemu_application_p2p_copy_user(user,(struct p2p_user*)item->instruction->body);
	netemu_util_copy_string(&user->name, item->instruction->user);
	user->_internal = netemu_application_p2p_create_user_internal();
	switch(item->type) {
		case TCP_CONNECTION:
			user->_internal->connection = item->connection.connection;
			break;
		case UDP_SENDER:
			user->_internal->sender = item->connection.udp_sender;
			break;
		default:
			break;
	}

	_netemu_p2p_send_login_success(connection,item->connection.connection);
	netemu_p2p_send_user_joined(connection, user);
	netemu_list_add(connection->_internal->users,user);

	/* Notify listeners about successful join */
	for(i = 0; i < connection->_internal->join_callbacks->count; i++) {
		((struct p2p_callback*)connection->_internal->join_callbacks->elements[i])->fn.userJoinedFn(connection, user);
	}
}

/**
 * This function responds to all "User joined" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_send_user_joined(struct netemu_p2p_connection *connection, struct p2p_user *user) {
	struct application_instruction *instruction;
	union netemu_connection_type type;

	type.collection = connection->_internal->peers;
	instruction = netemu_application_create_message();
	netemu_application_p2p_user_join_add(instruction, user);
	netemu_sender_buffer_add(connection->_internal->send_buffer,instruction,CONNECTION_COLLECTION,type);
}

/**
 * This function responds to all "User Join" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int i;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if(netemu_list_contains(connection->_internal->users,user) == -1) {
		type.collection = connection->_internal->peers;
		netemu_list_add(connection->_internal->users,user);
		netemu_sender_buffer_add(connection->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);
		for(i = 0; i < connection->_internal->join_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->join_callbacks->elements[i])->fn.userJoinedFn(connection, user);
		}
	}
}

/**
 * This function responds to all "Ready" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_ready(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;

	connection = (struct netemu_p2p_connection*)arg;
	if(connection->_internal->continueFn != NULL) {
		connection->_internal->continueFn(connection);
	}

}

/**
 * This function responds to all "Game started" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_game_started(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_start_game *game;
	struct netemu_p2p_connection* connection;
	struct netemu_sender_udp *sender;
	struct sockaddr *addr;
	int i, size;
	NETEMU_BOOL user_joined;
	user_joined = 0;
	connection = (struct netemu_p2p_connection*)arg;
	if(connection->current_game == NULL || connection->current_game->started == 1 || connection->current_game->received_start_signal == TRUE) {
		return;
	}
	connection->current_game->received_start_signal = TRUE;
	connection->_internal->values_received = malloc((connection->current_game->emulator_value_size*
			connection->current_game->user_count)*connection->current_game->connection_quality);

	game = (struct p2p_start_game*)item->instruction->body;
	addr = (netemu_sockaddr*) netemu_util_create_addr(game->addr,game->port,&size);
	sender = netemu_sender_udp_new(addr, size);
	netemu_sender_collection_add_udp_sender(connection->current_game->_internal->udp_collection, sender);
	connection->current_game->_internal->ready_count++;

	if(connection->current_game->_internal->ready_count >= connection->current_game->user_count) {
		connection->current_game->started = 1;
		for(i = 0; i < connection->_internal->all_ready_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->all_ready_callbacks->elements[i])->fn.allReadyFn(connection, connection->current_game);
		}
	}

	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(connection->current_game->players[i].addr == game->addr && connection->current_game->creator->port == game->port) {
				user_joined = 1;
				break;
		}

	}
	if(user_joined) {
		for(i = 0; i < connection->_internal->player_ready_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->player_ready_callbacks->elements[i])->fn.gameStartedFn(connection, connection->current_game);
		}
	}
	else {
		for(i = 0; i < connection->_internal->game_started_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->game_started_callbacks->elements[i])->fn.gameStartedFn(connection, connection->current_game);
		}
	}


}

/**
 * This function responds to all "Join host" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_join_host(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct p2p_user *registered_user;
	int index;
	struct netemu_p2p_connection* connection;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if((index = netemu_list_contains(connection->_internal->users,user)) == -1) {
		return;
	}
	registered_user = connection->_internal->users->elements[index];
	if(registered_user->_internal == NULL) {
		registered_user->_internal = netemu_application_p2p_create_user_internal();
	}
	registered_user->_internal->connection = item->connection.connection;
}

/**
 * This function responds to all "player join" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_player_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	struct netemu_tcp_connection *player_con;
	int index, i, size;
	union netemu_connection_type type;
	netemu_sockaddr* addr;
	connection = (struct netemu_p2p_connection*)arg;
	if(connection->current_game != NULL) {
		user = (struct p2p_user*)item->instruction->body;
		/* Do we have a connection to this player? */
		if((index = netemu_list_contains(connection->_internal->users,user)) != -1) {
			user->_internal = ((struct p2p_user*)connection->_internal->users->elements[index])->_internal;
		}
		else {
			/* If the user is not in the list, we shouldn't have received this instruction */
			return;
		}
		/* Does this player already exist in our list? In that case we don't need to do anything further. */
		if(_netemu_p2p_player_exists(connection->current_game,user)) {
			return;
		}
		/* If that's not the case, it's time to correct that. */
		if(user->_internal == NULL || (user->_internal->connection == NULL )) {
			addr = (netemu_sockaddr*)netemu_util_create_addr(user->addr, user->port, &size);
			player_con = _netemu_p2p_connect_to(connection,addr,  size);
			user->_internal = netemu_application_p2p_create_user_internal();
			user->_internal->connection = player_con;
			netemu_sender_collection_add_tcp_sender(connection->_internal->peers, player_con);

			//netemu_p2p_send_user_joined(connection, connection->user);
		}
		netemu_sender_collection_add_tcp_sender(connection->current_game->_internal->tcp_collection, user->_internal->connection);
		_netemu_p2p_add_player(connection->current_game,user);
		type.connection = user->_internal->connection;
		netemu_sender_buffer_add(connection->_internal->send_buffer, item->instruction, TCP_CONNECTION, type);
		for(i = 0; i < connection->_internal->player_joined_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->player_joined_callbacks->elements[i])->fn.playerJoinedFn(connection,
					connection->current_game, user);
		}
	}
}

/**
 * Check if a player exists.
 * @todo Use binary search instead (micro-optimization)
 * @ingroup p2p_player
 * @param game the game to check in
 * @param player the player to check for.
 */
NETEMU_BOOL _netemu_p2p_player_exists(struct p2p_game *game, struct p2p_user *player) {
	int i;
	if(_netemu_p2p_user_compare(player,game->creator) == 0) {
		return 1;
	}
	if(game->_internal != NULL) {
		netemu_thread_mutex_lock(game->_internal->game_lock, NETEMU_INFINITE);
	}
	for(i = 0; i < game->user_count-1; i++) {
		if(_netemu_p2p_user_compare(player,&game->players[i]) == 0) {
			netemu_thread_mutex_release(game->_internal->game_lock);
			return 1;
		}
	}
	if(game->_internal != NULL) {
		netemu_thread_mutex_release(game->_internal->game_lock);
	}
	return 0;
}


/**
 * Add a player to the list of players in a game.
 * @ingroup p2p_player
 * @param game the game to which the player should be added.
 * @param player the player that should be added to the game.
 */
void _netemu_p2p_add_player(struct p2p_game * game, struct p2p_user *player) {
	struct p2p_user* new_player_list;
	int i;
	/* If we have an internal struct, then we have a lock which we can use. */
	if(game->_internal != NULL) {
		netemu_thread_mutex_lock(game->_internal->game_lock, NETEMU_INFINITE);
	}

	game->user_count++;
	/* The player number is the same as the current count. */
	player->_internal->player_no = game->user_count;
	/* Allocate enough space for the new player list, and copy the existing users.
	   TODO: Try to use realloc here and if that fails, use this method. */
	new_player_list = malloc(game->user_count*sizeof(struct p2p_user));
	for(i = 0; i < game->user_count-2; i++) {
		netemu_application_p2p_copy_user(&new_player_list[i],&game->players[i]);
	}
	netemu_application_p2p_copy_user(&new_player_list[game->user_count-2],player);
	free(game->players);
	game->players = new_player_list;
	/* Release the lock. */
	if(game->_internal != NULL) {
		netemu_thread_mutex_release(game->_internal->game_lock);
	}
}

/**
 * This function handles all incoming "Game Created"-instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_game_created(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_game *game, *copy;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int index, i;

	connection = (struct netemu_p2p_connection*)arg;
	game = (struct p2p_game*)item->instruction->body;
	/* We don't need to do anything if this game already exists in our list. */
	if(netemu_list_contains(connection->_internal->games,game) == -1) {
		/* If we have the creator in our list (which we should have), then we might be interested
		   in any internal data we have on that player. */
		if((index = netemu_list_contains(connection->_internal->users,game->creator)) != -1) {
			game->creator->_internal = ((struct p2p_user*)connection->_internal->users->elements[index])->_internal;
		}
		/* Add the game to our game list. */
		netemu_list_add(connection->_internal->games,game);
		/* Create a copy of the game, to preserve the instruction integrity. */
		copy = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(copy, game);
		/* Replace the body in the instruction with the copy. */
		item->instruction->body = copy;
		/* Now let's send this to all other players we are connected to. */
		type.collection = connection->_internal->peers;
		netemu_sender_buffer_add(connection->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);

		/* Notify any listeners about the new game. */
		for(i = 0; i < connection->_internal->game_created_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->game_created_callbacks->elements[i])->fn.gameCreatedFn(connection, game);
		}
	}
}
/**
 * Send play values to all clients.
 * This function will block if you haven't received play values from your co-players, and
 * will handle all the synchronization for you.
 * @ingroup p2p_responses
 */
int netemu_p2p_send_play_values(struct netemu_p2p_connection* info, void* data) {
	time_t timestamp;
	int size;
	struct application_instruction *message;
	union netemu_connection_type type;
	int cache_index;
	size = info->current_game->emulator_value_size;

	if(info->_internal->values_to_send == NULL) {
		info->_internal->values_to_send = malloc(info->current_game->connection_quality*size);
	}

	memcpy(info->_internal->values_to_send + (info->_internal->values_buffered * size), data, size);
	/* Copy data from the current frame index. */
	if(&info->_internal->values_received != NULL) {
		memcpy(data, info->_internal->values_received + (size*info->_internal->frame_index), size * info->current_game->user_count);
	}
	info->_internal->frame_index++;
	info->_internal->values_buffered++;

	/* We have used up all our frames, we need new ones. Let's send the data we collected and then receive data from other players. */
	if(info->_internal->values_buffered == info->current_game->connection_quality) {
		message = netemu_application_create_message();
		cache_index = netemu_p2p_value_in_cache(info->user, info->_internal->values_to_send, info->current_game->emulator_value_size);
		if(cache_index == -1) {
			netemu_application_p2p_buffered_play_values_add(message,info->user->_internal->player_no, size,data);
		}
		else {
			netemu_application_p2p_cached_play_values_add(message,info->user->_internal->player_no, cache_index);
		}
		type.collection = info->current_game->_internal->udp_collection;
		timestamp = time(NULL);
		netemu_sender_buffer_add(info->_internal->send_buffer,message, CONNECTION_COLLECTION, type);
		info->_internal->values_buffered = 0;
		netemu_p2p_receive_play_values(info);
		info->_internal->frame_index = 0;
	}
	return 1;
}

/**
 * Locate a value in the cache.
 * @param user in which cache the value should exist.
 * @param data the data which should be in the cache
 * @param size the size fo the data.
 * @return the index of the value if it was found, -1 otherwise.
 */
int netemu_p2p_value_in_cache(struct p2p_user *user, char* data, int size) {
	int i;
	for(i = 0; i < user->_internal->cache_index; i++) {
		if(memcmp(user->_internal->cache[i].values, data, size) == 0) {
			return i;
		}
	}
	return -1;
}

int netemu_p2p_receive_play_values(struct netemu_p2p_connection *info) {
	int i;
	struct p2p_user *player;

	/* Wait for values if there we haven't received values from all players. */
	while(!info->current_game->_internal->all_values_received) {
		netemu_thread_event_wait(info->_internal->play_values_event, NETEMU_INFINITE);
	}
	netemu_thread_mutex_lock(info->current_game->_internal->game_lock, NETEMU_INFINITE);
	netemu_process_user_value(info, info->current_game->creator);
	for(i = 0; i < info->current_game->user_count-1; i++) {
		player = &info->current_game->players[i];
		netemu_process_user_value(info, player);
	}
	info->current_game->_internal->all_values_received = FALSE;
	netemu_thread_mutex_release(info->current_game->_internal->game_lock);
	return 1;
}

void netemu_process_user_value(struct netemu_p2p_connection *info, struct p2p_user *player) {
	struct p2p_buffered_play_values *values;
	struct application_instruction *instruction;
	int index, i;
	instruction = player->_internal->play_values->elements[0];
	netemu_list_remove_at(player->_internal->play_values, 0);
	if(info->user->_internal->player_no != player->_internal->player_no) {
		if(instruction->id == P2P_CACHED_BUFFERED_PLAY_VALUES) {
			index = ((struct intelligently_cached_buffered_play_values*)instruction->body)->index;
		}
		else if(instruction->id == P2P_BUFFERED_PLAY_VALUES) {
			values = instruction->body;
			if(player->_internal->cache_index == 256) {
				for(i = 0; i < 255; i++) {
					player->_internal->cache[i] = player->_internal->cache[i+1];
				}
				netemu_application_p2p_buffered_play_values_copy(&player->_internal->cache[255], values);
				index = 255;
			}
			else {
				netemu_application_p2p_buffered_play_values_copy(&player->_internal->cache[player->_internal->cache_index], values);
				index = player->_internal->cache_index;
				player->_internal->cache_index++;

			}
		}
		/*
		for(i = 0; i < info->current_game->connection_quality; i++) {
			memcpy(info->_internal->values_received + (i * (info->current_game->emulator_value_size * info->current_game->user_count)) +
					(info->current_game->emulator_value_size*player->_internal->player_no-1),
							   player->_internal->cache[index].values,info->current_game->emulator_value_size);
		}
		*/
		player->_internal->values_received = FALSE;
	}

}

/**
 * Compare two p2p users.
 * @ingroup netemu_p2p
 * @param arg1 the first user
 * @param arg2 the second user
 * @return value bigger than 0 if the user 1 is bigger than the other one, 0 if their equal and less than zero if user 1 is less than user 2
 */
int _netemu_p2p_user_compare(const void *arg1, const void *arg2) {
	const struct p2p_user* user1, *user2;
	user1 = (const struct p2p_user*)arg1;
	user2 = (const struct p2p_user*)arg2;

	return (user1->addr-user2->addr) + (user1->port - user2->port);
}

int _netemu_p2p_game_compare(const void *arg1, const void *arg2) {
	const struct p2p_game* game1, *game2;
	game1 = (const struct p2p_game*)arg1;
	game2 = (const struct p2p_game*)arg2;

	return (game1->creator->addr-game2->creator->addr) + (game1->creator->port - game2->creator->port);
}

void netemu_p2p_respond_to_play_values(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;
	struct netemu_list *callbacks;
	struct p2p_buffered_play_values *values;
	int i;
	NETEMU_BOOL all_values_received;
	all_values_received = TRUE;
	connection = (struct netemu_p2p_connection*)arg;
	callbacks = connection->_internal->play_values_callbacks;
	values = item->instruction->body;
	netemu_thread_mutex_lock(connection->current_game->_internal->game_lock, NETEMU_INFINITE);
	if(values->player_no == 1) {
		netemu_list_add(connection->current_game->creator->_internal->play_values, item->instruction);
		connection->current_game->creator->_internal->values_received = TRUE;
	}
	else if(connection->current_game->creator->addr != connection->user->connection && connection->current_game->creator->port != connection->user->port && connection->current_game->creator->_internal->values_received == FALSE) {
		all_values_received = FALSE;
	}
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(connection->current_game->players[i]._internal->player_no == values->player_no && connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no) {
			netemu_list_add(connection->current_game->players[i]._internal->play_values, item->instruction);
			connection->current_game->players[i]._internal->values_received = TRUE;
		}
		else if(connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no && connection->current_game->players[i]._internal->values_received == FALSE) {
			all_values_received = FALSE;
		}
	}
	netemu_thread_mutex_release(connection->current_game->_internal->game_lock);
	if(all_values_received) {
		/* Signal anyone that waits for an event. */
		connection->current_game->_internal->all_values_received = TRUE;
		netemu_thread_event_signal(connection->_internal->play_values_event);
	}

	for(i = 0; i < callbacks->count; i++) {
		((struct p2p_callback*)callbacks->elements[i])->fn.valuesFn(connection, values->values, values->size);
	}
}

void netemu_p2p_respond_to_cached_play_values(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;
	int i;
	NETEMU_BOOL all_values_received;
	all_values_received = TRUE;
	connection = (struct netemu_p2p_connection*)arg;
	netemu_thread_mutex_lock(connection->current_game->_internal->game_lock, NETEMU_INFINITE);
	if(connection->current_game->creator->_internal->receiver == item->connection.udp_receiver) {
		netemu_list_add(connection->current_game->creator->_internal->play_values, item->instruction);
		connection->current_game->creator->_internal->values_received = TRUE;
	}
	else if(connection->current_game->creator->_internal->values_received == FALSE) {
		all_values_received = FALSE;
	}
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(connection->current_game->players[i]._internal->receiver == item->connection.udp_receiver) {
			netemu_list_add(connection->current_game->players[i]._internal->play_values, item->instruction);
			connection->current_game->players[i]._internal->values_received = TRUE;
		}
		if(connection->current_game->players[i]._internal->values_received == FALSE) {
			all_values_received = FALSE;
		}
	}
	netemu_thread_mutex_release(connection->current_game->_internal->game_lock);
	if(all_values_received) {
		/* Signal anyone that waits for an event. */
		connection->current_game->_internal->all_values_received = TRUE;
		netemu_thread_event_signal(connection->_internal->play_values_event);
	}
	/* Signal anyone that waits for an event. */
	netemu_thread_event_signal(connection->_internal->play_values_event);
}

int _netemu_p2p_addr_compare(const void *arg1, const void *arg2) {
	const netemu_sockaddr* addr1, *addr2;
	int cmp, i;
	addr1 = (const netemu_sockaddr*)arg1;
	addr2 = (const netemu_sockaddr*)arg2;
	cmp = 0;

	for(i = 0; i < 14; i++) {
		cmp = addr1->sa_data[i]-addr2->sa_data[i];
		if(cmp != 0)
			return cmp;
	}
	return cmp;
}

