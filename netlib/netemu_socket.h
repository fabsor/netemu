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

    /* Error code constants */
    #define EINTR           4
    #define EWOULDBLOCK     11
    #define EACCES          13
    #define EFAULT          14
    #define EINVAL          22
    #define EMFILE          24
    #define ENOTSOCK        88
    #define EDESTADDRREQ    89
    #define EMSGSIZE        90
    #define EPROTOTYPE      91
    #define EPROTONOSUPPORT 93
    #define ESOCKNOSUPPORT  94
    #define EOPNOTSUPP      95
    #define EAFNOSUPP       97
    #define EADDRINUSE      98
    #define EADDRNOTAVAIL   99
    #define ENETDOWN        100
    #define ENETUNREACH     101
    #define ENETRESET       102
    #define ECONNABORTED    103
    #define ECONNRESET      104
    #define ENOBUFS         105
    #define EISCONN         106
    #define ENOTCONN        107
    #define ESHUTDOWN       108
    #define ETIMEOUT        110
    #define ECONNREFUSED    111
    #define EHOSTUNREACH    113
    #define EALREADY        114
    #define EINPROGRESS     115
    #define ENOTINITIALIZED 10003

    #else

    typedef unsigned int NETEMU_SOCKET;
    #define INVALID_SOCKET  (NETEMU_SOCKET)(~0)

    #define ENOTINITIALIZED 10003
    #define EINTR           10004
    #define EWOULDBLOCK     10035
    #define EACCES          10013
    #define EFAULT          10014
    #define EINVAL          10022
    #define EMFILE          10024
    #define ENOTSOCK        10038
    #define EDESTADDRREQ    10039
    #define EMSGSIZE        10040
    #define EPROTOTYPE      10041
    #define EPROTONOSUPPORT 10043
    #define ESOCKNOSUPPORT  10044
    #define EOPNOTSUPP      10045
    #define EAFNOSUPP       10047
    #define EADDRINUSE      10048
    #define EADDRNOTAVAIL   10049
    #define ENETDOWN        10050
    #define ENETUNREACH     10051
    #define ENETRESET       10052
    #define ECONNABORTED    10053
    #define ECONNRESET      10054
    #define ENOBUFS         10055
    #define EISCONN         10056
    #define ENOTCONN        10057
    #define ESHUTDOWN       10058
    #define ETIMEOUT        10060
    #define ECONNREFUSED    10061
    #define EHOSTUNREACH    10065
    #define EALREADY        10037
    #define EINPROGRESS     10036


    #endif

    /* Constants for the different address families that
     * can be specified when creating a new socket. */
    enum {
        NETEMU_AF_APPLETALK = 0,
        NETEMU_AF_BLUETOOTH,
        NETEMU_AF_INET,
        NETEMU_AF_INET6,
        NETEMU_AF_IPX,
        NETEMU_AF_IRDA,
        NETEMU_AF_UNSPEC
    };

    /* Constans for the different socket types that
     * can be specified when creating a new socket. */
    enum {
        NETEMU_SOCK_DGRAM,
        NETEMU_SOCK_RAW,
        NETEMU_SOCK_RDM,
        NETEMU_SOCK_STREAM
    };
    
    /* Constants defining flags for the send/recv calls. */
    enum {
        NETEMU_MSG_OOB,        // Process Out-of-band data (send/recv).
        NETEMU_MSG_DONTROUTE,  // Don't use local routing (send).
        NETEMU_MSG_PEEK,       // Peeks at incoming data (recv).
        NETEMU_MSG_WAITALL     // Incoming data will be received only when the buffer is full, or when the connection is closed or an error occured (recv).
    };

    /* Constants for the shutdown method. */
    enum {
        NETEMU_SHUT_RD,        // Disables further data receival on the socket.
        NETEMU_SHUT_WR,        // Disables further data sending from the socket.
        NETEMU_SHUT_RDWR       // Disables sending and receiving data on the socket.
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

    /* Frees the memory allocated for the socket */
    int free(NETEMU_SOCKET socket);

    int GetLastError();



    /* TODO: Look into error handling.
     * In *nix, whenever a socket function returns -1, the system variable 'errno' is set with the appropriate error code.
     * In windows, whenever a socket function returns -1, WSAGetLastError can be called to retrieve the error code for the last error that occured on the calling thread.
     * How to abstract between the two? */

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

