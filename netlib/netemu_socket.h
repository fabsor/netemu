/* 
 * File:   netemu_socket.h
 * Author: emil
 *
 * Created on March 22, 2010, 1:22 PM
 */

#ifndef _NETEMU_SOCKET_H
#define	_NETEMU_SOCKET_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* Platform-specific socket typedefs.
     * Unix uses signed integers for the socket file descriptor,
     * while windows uses an unsigned integer. */
    #ifdef _NIX

    typedef int NETEMU_SOCKET;
    #define INVALID_SOCKET  (NETEMU_SOCKET)(-1)

    #else

    typedef unsigned int NETEMU_SOCKET;
    #define INVALID_SOCKET  (NETEMU_SOCKET)(~0)

    #endif

    /* Constants for the different address families that
     * can be specified when creating a new socket. */
    enum {
        AF_APPLETALK = 0,
        AF_BLUETOOTH,
        AF_INET,
        AF_INET6,
        AF_IPX,
        AF_IRDA,
        AF_UNSPEC
    };

    /* Constans for the different socket types that
     * can be specified when creating a new socket. */
    enum {
        SOCK_DGRAM,
        SOCK_RAW,
        SOCK_RDM,
        SOCK_STREAM
    };
    
    /* Constants defining flags for the send/recv calls. */
    enum {
        MSG_OOB,        // Process Out-of-band data (send/recv).
        MSG_DONTROUTE,  // Don't use local routing (send).
        MSG_PEEK,       // Peeks at incoming data (recv).
        MSG_WAITALL     // Incoming data will be received only when the buffer is full, or when the connection is closed or an error occured (recv).
    };

    /* Constants for the shutdown method. */
    enum {
        SHUT_RD,        // Disables further data receival on the socket.
        SHUT_WR,        // Disables further data sending from the socket.
        SHUT_RDWR       // Disables sending and receiving data on the socket.
    };

    /* Structure describing a generic socket address.
     * Hopefully this is identical on both the unix and win32 platform, not yet verified though. */
    struct sockaddr {
        unsigned short sa_family;   // THe address family of the socket.
        char sa_data[14];           // Address info.
    };

    /* Type defining size of socket info */
    typedef unsigned int socklen_t;


    /* Method definitions */

    /* Initializes the socket usage for the application on the platform. */
    int init_network();

    /* Creates a new NETEMU_SOCKET. */
    NETEMU_SOCKET netemu_socket(int address_family, int socket_type);

    /* Binds a given NETEMU_SOCKET to an address. */
    int bind(NETEMU_SOCKET socket, const struct sockaddr *address, socklen_t address_len);

    /* Places a socket in a state where it listens for incoming connection attempts. */
    int listen(NETEMU_SOCKET socket, int backlog);

    /* Accepts a connection attempt made on the listening socket. */
    NETEMU_SOCKET accept(NETEMU_SOCKET socket, struct sockaddr *address, socklen_t *address_len);

    /* Sends data through a connected socket. */
    int send(NETEMU_SOCKET socket, const char *buffer, int len, int flags);

    /* Sends data to a specific destination. */
    int sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const struct sockaddr *dest_address, socklen_t *address_len);

    /* Receives data on a connected socket. */
    int recv(NETEMU_SOCKET socket, char *buffer, int len, int flags);

    /* Received a datagram and stores the sender address */
    int recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, struct sockaddr *address, socklen_t *address_len);

    /* Disables send or receive on a socket. */
    int shutdown(NETEMU_SOCKET socket, int how);

    int free(NETEMU_SOCKET socket);

    /* TODO: Look into error handling.
     * In *nix, whenever a socket function returns -1, the system variable 'errno' is set with the appropriate error code.
     * In windows, whenever a socket function returns -1, WSAGetLastError can be called to retrieve the error code for the last error that occured on the calling thread.
     * How to abstract between the two? */

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

