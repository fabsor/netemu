/*
 * netemu_thread.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef NETEMU_THREAD_H_
#define NETEMU_THREAD_H_

typedef int netemu_thread;

/* Since the windows threads are using the very microsofty WINAPI, we need to typedef a little =) */

typedef int NETEMU_MUTEX;

int netemu_thread_new(netemu_thread* identifier, void (*start_fn) (void *), void* arg);

int netemu_thread_exit(netemu_thread* identifier);

NETEMU_MUTEX netemu_thread_mutex_create();

void netemu_thread_mutex_lock(NETEMU_MUTEX* mutex);

void netemu_thread_mutex_release(NETEMU_MUTEX* mutex);

void netemu_thread_mutex_destroy(NETEMU_MUTEX* mutex);



#endif /* NETEMU_THREAD_H_ */
