/*
 * netemu_thread_nix.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabbe
 */
#include "pthread.h"

int netemu_thread_new(netemu_thread* identifier, NETEMU_API *(*start_fn) (void *)) {
	pthread_create(identifier,NULL,start_fn,identifier);
}

int netemu_thread_stop(netemu_thread* identifier) {
	pthread_exit(identifier);
}

int netemu_thread_mutex_lock();

int netemu_thread_mutex_release(void* memory);
