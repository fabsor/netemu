/*
 * application.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_
#include "transport.h"
#include "netemu_util.h"
/* Size of the application_instruction struct excluding the body. */
#define APPLICATION_INSTRUCTION_SIZE	33;

#define USER_LEAVE		0x01
#define LOGIN_REQUEST	0x03
#define LOGIN_SUCCESS	0x04
#define PING			0x05
#define PONG			0x06
#define USER_JOINED		0x02

/*! A message to be sent to the server. */
struct application_instruction {
	char id; /* 1...23 */
	void *body;
	int body_size;
	/* Since we dont know the actual size, this is probably the best option, unfortunately. */
	void (*packBodyFn)(struct application_instruction* instruction, char* buffer);
};

struct login_request {
	char *user;
	char *name;
	short connection;
};

struct user {
	char username[32];
	unsigned int ping;
	short status;
	int id;
	short connection;
};

struct player {
	char username[32];
	unsigned int ping;
	int word;
	short connection;
};

struct login_success {
	unsigned int users_count;
	unsigned int games_count;
	struct user *users;
	struct game *games;
};

struct login_status {
	NETEMU_WORD id;
	char message[128];
};

struct ping {
	int pbody[3];
};

struct pong {
	char pbody[17];
};

/* TODO: What about empty structs? do we need them? 
struct client_timeout {

};
*/
struct user_joined {
	char* user;
	NETEMU_WORD id;
	NETEMU_DWORD ping;
	char connection;
};

struct game {
	char name[128];
	NETEMU_DWORD id;
	char app_name[128];
	char users_count[10];
	char status;
};

struct user_left {
	char *user;
	NETEMU_WORD id;
	char *exit_message;
};

struct game_created {
	char gameName[128];
	char appName[128];
	unsigned int id;
};

struct game_closed {
	unsigned int id;
};

struct game_status_update {
	unsigned int id;
	short status;
	int num_players;
	int max_players;
};

struct player_joined {
	unsigned int game_id;
	char username[32];
	unsigned int ping;
	int user_id;
	short connection;
};

struct player_left {
	int user_id;
};

struct existing_player_list {
	unsigned int players_count;
	struct player *players;
};

struct kick_player {
	int user_id;
};

struct game_start {
	int time_band;
	short player_no;
	short max_players;
};
/*
struct player_ready{

};
*/
struct buffered_play_values {
	int size;
	void** values;
	int player_values_size;
	int emulator_word_size;
};

struct intelligently_cached_buffered_play_values {
	char index;
};

struct chat {
	char message[128];
};


struct application_instruction* netemu_application_create_message(int message_type, void* body, int body_size, void (*packBodyFn)(struct application_instruction* instruction, char* buffer));

struct login_request* netemu_application_create_login_request(char* appName, char* user, int connection, int *size);

void netemu_application_login_request_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_pong_pack(struct application_instruction *instruction, char *buffer);

struct pong* netemu_application_create_pong(int *size);

struct user_left* netemu_application_create_leave(char* user, NETEMU_WORD id, char* exit_message, int *size);

void netemu_application_leave_pack(struct application_instruction *instruction, char *buffer);

struct application_instruction* netemu_application_parse_message(struct transport_instruction *instruction);

void netemu_application_parse_user_joined(struct application_instruction *instruction, char* buffer);

#endif /* APPLICATION_H_ */
