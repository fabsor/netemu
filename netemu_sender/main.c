#include "netemu_socket.h"
#include "protocol/communication.h"
#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include <stdio.h>
#include <stdlib.h>

#define INTERNAL_PORT	27999
#define SERVER_PORT		27888
#define CLIENT_PORT 	35888

struct netemu_receiver* prepare_receiver(int port);
struct netemu_sender* prepare_sender(int port);
struct netemu_sender* prepare_sender_on_socket(NETEMU_SOCKET socket, int port);
void send_communication_data(struct netemu_sender* sender);
void send_data(struct netemu_sender* sender, char* data);
void test_server_communication();
void test_sender_receiver();

int main()
{
	netemu_init_network();
	test_server_communication();
	return 0;
}

void listener(char* data, size_t size, struct netemu_receiver* receiver) {
	printf("%s",data);
}

void test_server_communication() {
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	receiver = prepare_receiver(CLIENT_PORT);
	sender = prepare_sender_on_socket(receiver->socket, SERVER_PORT);
	send_communication_data(sender);
}

void test_sender_receiver(){
	struct netemu_receiver* receiver;
	struct netemu_sender* sender;
	receiver = prepare_receiver(INTERNAL_PORT);
	sender = prepare_sender(INTERNAL_PORT);
	send_communication_data(sender);
}

struct netemu_receiver* prepare_receiver(int port) {
	struct netemu_sockaddr_in addr_in;
	netemu_sockaddr *addr;
	struct netemu_receiver *receiver;
	addr_in.addr = netemu_htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = htons(port);

	addr = netemu_prepare_net_addr(&addr_in);
	receiver = netemu_receiver_new(addr,sizeof(addr_in),64);
	netemu_receiver_register_recv_fn(receiver, listener);
	netemu_receiver_start_listening(receiver);
	return receiver;
}

struct netemu_sender* prepare_sender(int port) {
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	addr_in.addr = netemu_htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = htons(port);

	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new(addr,sizeof(addr_in));

	return sender;
}

struct netemu_sender* prepare_sender_on_socket(NETEMU_SOCKET socket, int port) {
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	addr_in.addr = netemu_htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = htons(port);
	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new_on_socket(addr,socket,sizeof(addr_in));
	return sender;
}


void send_communication_data(struct netemu_sender* sender) {
	char* message;
	message = netemu_communication_create_ping_message();
	send_data(sender, message);
	message = netemu_communication_create_hello_message(0.83);
	send_data(sender, message);
}

void send_data(struct netemu_sender* sender, char* data) {
	int error;
	error = netemu_sender_send(sender,data,strlen(data)+1);
	if(error < 0) {
		printf("Error: %d\n", netemu_get_last_error());
	}
}
