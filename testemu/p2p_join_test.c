
/**
 * @file
 * This file contains tests for the joining part of the client.
 */
#include "netemu_p2p.h"
#include "constants.h"
#include "p2ptest.h"
#include "netemu_socket.h"
#include <stdlib.h>
#include <stdio.h>

int p2p_join_test_ready = 0;
void p2p_join_player_join_callback(struct netemu_p2p_connection *connection, struct p2p_game *game, struct p2p_user *user);
void p2p_join_all_ready_callback(struct netemu_p2p_connection *connection, struct p2p_game *game);
void p2p_join_register_callbacks(struct netemu_p2p_connection *connection);
void p2p_join_play_values_received_callback(struct netemu_p2p_connection *, char* values, int size);
netemu_event p2p_join_event;
int p2p_join_ready_to_send = 0;
void run_p2p_join_test() {
	struct netemu_p2p_connection *p2p;
	struct p2p_game *game;
	int n;
	char* data;
	data = malloc(512);
	memcpy(data, VALUE, strlen(VALUE)+1);
	p2p_join_event = netemu_thread_event_create();
	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME);
	printf("Registering callbacks...");
	p2p_join_register_callbacks(p2p);
	printf("OK!\Hosting on %d and connecting to %d...", P2P_JOIN_TEST_PORT, P2P_JOIN_TEST_CONNECT_PORT);

	if(netemu_p2p_connect(p2p,P2P_JOIN_TEST_BIND_ADDR,P2P_JOIN_TEST_PORT,P2P_JOIN_TEST_CONNECT_ADDR, P2P_JOIN_TEST_CONNECT_PORT)) {
		printf("OK!\n Creating a game...");
	}
	else {
		printf("Failed! Could not bind! Exiting...");
		return;
	}
	netemu_p2p_create_game(p2p, "TheGame",1,strlen(VALUE)+1,&game);
	netemu_thread_event_wait(p2p_join_event, NETEMU_INFINITE);
	printf("A new player has joined the game! OK!\n Starting the game...\n");
	netemu_p2p_start_game(p2p,ADDR,45400);
	while(!p2p_join_ready_to_send) {
		netemu_thread_event_wait(p2p_join_event, NETEMU_INFINITE);
	}
	for(n = 0; n < 10000; n++) {
		netemu_p2p_send_play_values(p2p, data);
	}
}

void p2p_join_register_callbacks(struct netemu_p2p_connection *connection) {
	netemu_p2p_register_player_joined_callback(connection, p2p_join_player_join_callback);
	netemu_p2p_register_all_players_ready_callback(connection, p2p_join_all_ready_callback);

}

void p2p_join_player_join_callback(struct netemu_p2p_connection *connection, struct p2p_game *game, struct p2p_user *user) {
	netemu_p2p_player_ready(connection, P2P_JOIN_TEST_BIND_ADDR, netemu_htons(40000));
}

void p2p_join_all_ready_callback(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	p2p_join_ready_to_send = 1;
	netemu_thread_event_signal(p2p_join_event);
}
