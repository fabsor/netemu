#include "netemu_socket.h"

int netemu_init_network() {
    int errcode;
    WSADATA wsaData;
    int retval;

    retval = 0;
    errcode = WSAStartup(MAKEWORD(2, 2), &wsaData);

    /* WSAStartup returns the errorcode directly if something went wrong.
     * For consistency, we'll set that to be the last error code and return -1. */
    if(errcode != 0) {
        WSASetLastError(errcode);
        retval = -1;
    }

    return retval;
}

int netemu_cleanup() {
    return WSACleanup();
}

NETEMU_SOCKET netemu_socket(int address_family, int socket_type) {
    return socket(address_family, socket_type, 0);
}

int netemu_bind(NETEMU_SOCKET socket, const struct sockaddr *address, socklen_t address_len) {
    return bind(socket, address, address_len);
}

int netemu_listen(NETEMU_SOCKET socket, int backlog) {
    return listen(socket, backlog);
}

NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, struct sockaddr *address, socklen_t *address_len) {
    return accept(socket, address, address_len);
}

int netemu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags) {
    return send(socket, buffer, len, flags);
}

int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const struct sockaddr *dest_address, socklen_t address_len) {
    return sendto(socket, buffer, len, flags, dest_address, address_len);
}

int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags) {
    return recv(socket, buffer, len, flags);
}

int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, struct sockaddr *address, socklen_t *address_len) {
    return recvfrom(socket, buffer, len, flags, address, address_len);
}

int netemu_shutdown(NETEMU_SOCKET socket, int how) {
    return shutdown(socket, how);
}

int netemu_closesocket(NETEMU_SOCKET socket) {
    return closesocket(socket);
}

int netemu_get_last_error() {
    return WSAGetLastError();
}

int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result) {
	/*return getaddrinfo(nodename, servicetype, hints, result);*/
}

netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr) {
	struct sockaddr_in* in_addr;
    in_addr = malloc(sizeof(struct sockaddr_in));
    in_addr->sin_port = netaddr->port;
    in_addr->sin_family = netaddr->family;
    in_addr->sin_addr.s_addr = netaddr->addr;
    
    return (netemu_sockaddr*) in_addr;
}

unsigned long netemu_inet_addr(char* addr) {
	return inet_addr(addr);
}