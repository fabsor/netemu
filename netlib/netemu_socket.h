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


    /* Method definitions */
    
    NETEMU_SOCKET netemu_socket(int address_family, int socket_type);



#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

