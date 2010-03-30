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
     * while windows uses an unsigned integer.
     * TODO: Since there are no namespaces, we need to prefix all these with NETEMU_
     * also, wouldn't it be better to define them to their constants instead?
     */
    #ifdef _NIX
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <netdb.h>

    typedef int NETEMU_SOCKET;
    #define INVALID_SOCKET  (NETEMU_SOCKET)(-1)

    /* Error code constants */
    #define NETEMU_EINTR    EINTR /* Like this? */
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

	#include <winsock2.h>
	#include <ws2tcpip.h>

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

    #define NETEMU_AF_APPLETALK AF_APPLETALK
    #define NETEMU_AF_BLUETOOTH AF_BLUETOOTH
    #define NETEMU_AF_INET  AF_INET
    #define NETEMU_AF_INET6 AF_INET6
    #define NETEMU_AF_IPX   AF_IPX
    #define NETEMU_AF_IRDA  AF_IRDA
    #define NETEMU_AF_UNSPEC    AF_UNSPEC


    /* Constans for the different socket types that
     * can be specified when creating a new socket. */
    #define NETEMU_SOCK_DGRAM   SOCK_DGRAM
    #define NETEMU_SOCK_RAW     SOCK_RAW
    #define NETEMU_SOCK_RDM     SOCK_RDM
    #define NETEMU_SOCK_STREAM  SOCK_STREAM
    
    
    /* Constants defining flags for the send/recv calls. */

    #define NETEMU_MSG_OOB          MSG_OOB        /* Process Out-of-band data (send/recv). */
    #define NETEMU_MSG_DONTROUTE    MSG_DONTROUTE  /* Don't use local routing (send). */
    #define NETEMU_MSG_PEEK         MSG_PEEK       /* Peeks at incoming data (recv). */
    #define  NETEMU_MSG_WAITALL     MSG_WAITALL     /* Incoming data will be received only when the buffer is full, or when the connection is closed or an error occured (recv). */


    /* Constants for the shutdown method. */
    
    #define NETEMU_SHUT_RD      SHUT_RD     /* Disables further data receival on the socket. */
    #define NETEMU_SHUT_WR      SHUT_WR     /* Disables further data sending from the socket. */
    #define NETEMU_SHUT_RDWR    SHUT_RDWR   /* Disables sending and receiving data on the socket. */
    

    /**
     * Structure describing a generic socket address.
     * Hopefully this is identical on both the unix and win32 platform, not yet verified though.
     */
    /*
    struct netemu_sockaddr {
        unsigned short sa_family;   // THe address family of the socket.
        char sa_data[14];           // Address info.
    };
    */
    typedef struct sockaddr netemu_sockaddr;

    struct netemu_sockaddr_in{
        unsigned short family;
        char sa_data[14];
        unsigned int addr;
        short port;
    };

    typedef struct sockaddr_in6 netemu_sockaddr_in6;


    /*! Winsock has a handy struct which contains address information of a specific host. This struct
     * aims to replicate this.*/
    struct netemu_addrinfo {
        int port;
        long address;
        char* hostname;
        struct netemu_sockaddr* addr;
        struct netemu_addrinfo** next;
    };



    /* Method definitions */

    /*! Initializes the socket usage for the application on the platform. */
    int netemu_init_network();

    /*! Creates a new NETEMU_SOCKET. */
    NETEMU_SOCKET netemu_socket(int address_family, int socket_type);

    /*! Binds a given NETEMU_SOCKET to an address. */
    int netemu_bind(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len);

    /*! Places a socket in a state where it listens for incoming connection attempts. */
    int netemu_listen(NETEMU_SOCKET socket, int backlog);

    /* Accepts a connection attempt made on the listening socket. */
    NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, netemu_sockaddr *address, socklen_t *address_len);

    /* Sends data through a connected socket. */
    int netemu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags);

    /* Sends data to a specific destination. */
    int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const netemu_sockaddr *dest_address, socklen_t address_len);

    /* Receives data on a connected socket. */
    int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags);

    /* Received a datagram and stores the sender address */
    int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, netemu_sockaddr *address, socklen_t *address_len);

    /* Disables send or receive on a socket. */
    int netemu_shutdown(NETEMU_SOCKET socket, int how);

    /* Frees the memory allocated for the socket */
    int netemu_free(NETEMU_SOCKET socket);

    /* Returns the error code for the last error that occured */
    int netemu_get_last_error();

    int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result);

    netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr);

    /* Converts a string containing a dotted IPv4 address to a network order unsigned long. */
   unsigned long netemu_inet_addr(char* addr);
#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

