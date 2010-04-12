
struct server_connection {
	char *host;
	DWORD port;
};

void server_connection_send_chat_message();