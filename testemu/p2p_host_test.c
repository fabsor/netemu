/**
 * @file
 * This file contains functions testing the hosting part of the p2p client.
 */
#include "netemu_p2p.h"
#include "constants.h"
#include "p2ptest.h"
#include "netemu_socket.h"
#include "netemu_thread.h"
#include <stdlib.h>
#include <stdio.h>

void p2p_host_register_callbacks(struct netemu_p2p_connection *connection);
void p2p_host_game_started_callback(struct netemu_p2p_connection *connection, struct p2p_game *game);
void p2p_host_game_created_callback(struct netemu_p2p_connection *connection, struct p2p_game *game);
void p2p_game_created_callback(struct netemu_p2p_connection *connection, struct p2p_game *game);
void p2p_host_user_joined_callback(struct netemu_p2p_connection *connection, struct p2p_user *user);
void p2p_host_all_ready_callback(struct netemu_p2p_connection *connection, struct p2p_game *game);
void p2p_host_play_values_received_callback(struct netemu_p2p_connection *, char* values, int size);
netemu_event p2p_host_event;
int p2p_host_test_ready = 0;
int p2p_host_ready_to_send = 0;

void run_p2p_host_test(int no_instructions, char connection) {
	struct netemu_p2p_connection *p2p;
	struct p2p_game** games;
	int n;
	char* data;
	data = malloc(512);
	memcpy(data, VALUE, strlen(VALUE)+1);
	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME, connection);
	printf("Registering callbacks...");
	p2p_host_register_callbacks(p2p);

	p2p_host_event = netemu_thread_event_create();
	printf("OK!\nHosting cloud on %d...", P2P_HOST_TEST_PORT);
	if(netemu_p2p_host(p2p, P2P_HOST_TEST_BIND_ADDR,P2P_HOST_TEST_PORT,CLOUD_NAME) == 0) {
		printf("OK!\n Waiting for incoming connections...");
	}
	else {
		printf("Failed! Could not bind! Exiting...");
		return;
	}
	while(!p2p_host_ready_to_send) {
		netemu_thread_event_wait(p2p_host_event, NETEMU_INFINITE);
	}
	for(n = 0; n < no_instructions; n++) {
		netemu_p2p_send_play_values(p2p, data);
	}
}

void p2p_host_register_callbacks(struct netemu_p2p_connection *connection) {
	netemu_p2p_register_user_joined_callback(connection, p2p_host_user_joined_callback, NULL);
	netemu_p2p_register_game_created_callback(connection, p2p_host_game_created_callback, NULL);
	netemu_p2p_register_game_started_callback(connection, p2p_host_game_started_callback, NULL);
	netemu_p2p_register_all_players_ready_callback(connection, p2p_host_all_ready_callback, NULL);
}

void p2p_host_game_created_callback(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	printf("A game has been created!\nTrying to join the game...");
	/* Join the game that was just created. */
	netemu_p2p_join_game(connection, game);
	printf("OK!\n Waiting for game start...");
}

void p2p_host_game_started_callback(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	printf("OK!\nThe game has started, trying to send player ready status...\n");
	netemu_p2p_player_ready(connection,ADDR,netemu_htons(40511));
	printf("OK!\nWaiting for all players to be ready...");
}

void p2p_host_all_ready_callback(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	printf("OK!\nNow let's have some fun shall we? Let's send some data to the other player...");
	p2p_host_ready_to_send = 1;
	netemu_thread_event_signal(p2p_host_event);
}

void p2p_host_user_joined_callback(struct netemu_p2p_connection *connection, struct p2p_user *user) {
	printf("A new user has joined! Hosting Seems OK!\n Listening for more players or new games...\n");
}
