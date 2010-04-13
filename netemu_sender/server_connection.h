#include "netlib_util.h"

struct server_connection {
	char *host;
	NETEMU_DWORD port;
};

void server_connection_send_chat_message();
