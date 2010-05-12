/**
 * netemu.h
 * This header file contains functions and structs that are used by both the p2p and and kaillera interfaces.
 */

#ifndef NETEMU_H_
#define NETEMU_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "netlib_util.h"
typedef void (* chatFn)(char *user, char *message, void *user_data);
typedef void (* leaveFn)(NETEMU_WORD id, char *user, char *exit_message, void *user_data);
typedef void (* cachedValuesReceivedFn)(struct intelligently_cached_buffered_play_values *result, void *user_data);


typedef struct _netemu_info_internal *server_connection_internal;


#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_H_ */
