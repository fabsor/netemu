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
netemu_event event;
void run_p2p_join_test() {
	netemu_sockaddr_in hostaddr, joinaddr;
	struct netemu_p2p_connection *p2p;
	struct p2p_game *game;

	event = netemu_thread_event_create();
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
	netemu_p2p_create_game(p2p, "TheGame",&game);

	netemu_thread_event_wait(event);

	netemu_sockaddr_in addr;
	addr.sin_family = NETEMU_AF_INET;
	addr.sin_addr.s_addr = ADDR;
	addr.sin_port = netemu_htons(40000);
	printf("A new player has joined the game! OK!\n Starting the game...\n");
	netemu_p2p_start_game(p2p,(netemu_sockaddr*)&addr,sizeof(addr));

	netemu_thread_event_wait(event);

	printf("OK!\nNow let's have some fun shall we? Let's send some data to the other player...");
	netemu_p2p_send_play_values(p2p, strlen("Right,Left And Right again")+1, "Right,Left And Right again");
}

void p2p_join_register_callbacks(struct netemu_p2p_connection *connection) {
	netemu_p2p_register_player_joined_callback(connection, p2p_join_player_join_callback);
	netemu_p2p_register_all_players_ready_callback(connection, p2p_join_all_ready_callback);
}

void p2p_join_player_join_callback(struct netemu_p2p_connection *connection, struct p2p_game *game, struct p2p_user *user) {
	netemu_thread_event_signal(event);
}

void p2p_join_all_ready_callback(struct netemu_p2p_connection *connection, struct p2p_game *game) {
	netemu_thread_event_signal(event);
}
