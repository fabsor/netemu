#include "headers/netemu_socket.h"
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

int initialized;
/* Initializes the socket usage for the application on the platform. */
int netemu_init_network() {
    initialized = 1;
    return 0;
    
}

/* Creates a new NETEMU_SOCKET. */
NETEMU_SOCKET netemu_socket(int address_family, int socket_type) {
    return socket(address_family,socket_type,0);

}

/* Binds a given NETEMU_SOCKET to an address. */
int netemu_bind(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
    return bind(socket,address,address_len);
}

/* Places a socket in a state where it listens for incoming connection attempts. */
int netemu_listen(NETEMU_SOCKET socket, int backlog) {
    return listen(socket,backlog);
}

/* Accepts a connection attempt made on the listening socket. */
NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, netemu_sockaddr *address, socklen_t *address_len) {
    return accept(socket,address,address_len);
}

/* Sends data through a connected socket. */
int netmu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags) {
    return send(socket,buffer,len,flags);

}

/* Sends data to a specific destination. */
int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const netemu_sockaddr *dest_address, socklen_t address_len){
    return sendto(socket,buffer,len,flags,dest_address,address_len);
}

/* Receives data on a connected socket. */
int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags) {
    return recv(socket,buffer,len,flags);
}

/* Received a datagram and stores the sender address */
int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, netemu_sockaddr *address, socklen_t *address_len) {
    return recvfrom(socket,buffer,len,flags,address,address_len);
}

/* Disables send or receive on a socket. */
int netemu_shutdown(NETEMU_SOCKET socket, int how) {
    return shutdown(socket,how);
}

/* Frees the memory allocated for the socket */
int netemu_free(NETEMU_SOCKET socket) {
    return 1;
}

int netemu_closesocket(NETEMU_SOCKET socket) {
	return close(socket);
}

/* Returns the error code for the last error that occured */
int netemu_get_last_error() {
  return errno;
}

int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result) {
    return 0;
}

unsigned long netemu_inet_addr(char* addr) {
	return htonl(inet_addr(addr));
}

netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr){
    struct sockaddr_in* in_addr;
    in_addr = malloc(sizeof(struct sockaddr_in));
    in_addr->sin_port = netaddr->port;
    in_addr->sin_family = netaddr->family;
    in_addr->sin_addr.s_addr = netaddr->addr;
    
    return (netemu_sockaddr*) in_addr;
}

int netemu_connect(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
	return connect(socket,address,address_len);
}


/* Converts an unsigned long from host order to network order. */
unsigned long netemu_htonl(unsigned long value) {
	return htonl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned long netemu_ntohl(unsigned long value) {
	return ntohl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned long netemu_htons(unsigned long value) {
	return htons(value);
}
