#include "netemu_socket.h"

struct server_connection {
	netemu_sockaddr_in *addr;
};

int server_connection_send_chat_message(server_connection *connection, char *message);

int server_connection_register_chat_callback(server_connection *connection, void (* chatFn)(char *user, char *message));