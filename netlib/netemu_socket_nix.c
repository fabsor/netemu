#include "netemu_socket.h"
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
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
    NETEMU_SOCKET sock = socket(address_family,socket_type,0);
}

/* Binds a given NETEMU_SOCKET to an address. */
int netemu_bind(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
    return bind(socket,(struct sockaddr*)address,address_len);
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
    return sendto(socket,buffer,len,flags,(struct sockaddr*)dest_address,address_len);
    
}

/* Receives data on a connected socket. */
int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags) {
    return netemu_recv(socket,buffer,len,flags);
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
    
}

/* Returns the error code for the last error that occured */
int netemu_get_last_error() {
  return errno;
}

int netemu_get_addr_info(char* nodename, char* servicetype, const netemu_addrinfo* hints, netemu_addrinfo* result) {
    return getaddrinfo(nodename,servicetype,hints,&result);
}
