/*
 * netemu_thread_nix.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabbe
 */

/**
 * @file
 * This is the nix implementation of netemu_thread.h, implemented with phtread.
 */
#include "headers/netemu_thread.h"
#include "pthread.h"

/*
struct threadargs {
	unsigned long (NETEMU_API *start_fn) (void *);
};

void* _netemu_thread_callback();

int netemu_thread_new(netemu_thread* identifier, ) {
	return pthread_create(identifier,0,_netemeu_thread_callback,identifier);
}

int netemu_thread_stop(netemu_thread *identifier) {
	pthread_exit(identifier);
	return 1;
}

void* _netemeu_thread_callback() {
	int hej = 1;
	start_fn(&hej);
}

int netemu_thread_mutex_lock();

int netemu_thread_mutex_release(void* memory);
*/
