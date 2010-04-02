/*
 * netemu_thread_nix.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabbe
 */

/**
 * @file
 * This is the nix implementation of netemu_thread.h, implemented with pthread.
 * @author Fabian Sörqvist <fabian.sorqvist@gmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include "headers/netemu_thread.h"
#include "headers/netemu_list.h"
#include <pthread.h>
struct netemu_mutex{
	pthread_mutex_t* mutex;
};

void *_netemu_thread_callback(void* arg);

/*! This struct is used for passing arguments to the thread. */
struct netemu_thread_args {
	void (*start_fn) (void *);
	void* arg;
};

/**
 * Start a new thread.
 * @param netemu_thread* identifier an identifier that can be used to identify the thread.
 * @param callback a function callback that will be called when the thread has started.
 * @return 0 if the thread was created successfully, an error code otherwise.
 */
int netemu_thread_new(netemu_thread* identifier, void (*start_fn) (void *), void* arg) {
	struct netemu_thread_args* thread_args;
	int rc;
	thread_args = malloc(sizeof(struct netemu_thread_args));
	thread_args->start_fn = start_fn;
	thread_args->arg = arg;
	rc = pthread_create(identifier, NULL, _netemu_thread_callback, thread_args);
	if(rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
	}
	return 0;
}

/**
 * Callback for the pthread_create() call.
 * @param arg this will always be a netemu_thread_struct.
 */
void *_netemu_thread_callback(void *arg) {
	struct netemu_thread_args *arg_struct;
	arg_struct = (struct netemu_thread_args*)arg;
	/*Start the thread by calling the function passed by the user. */
	arg_struct->start_fn(arg_struct->arg);
	return NULL;
}
/**
 * Exit the current thread.
 * @return 0 if everything went ok.
 */
int netemu_thread_exit() {
	pthread_exit(NULL);
	return 0;
}

/**
 * Create a new mutex lock.
 * @return an identifier for the mutex lock.
 */
netemu_mutex netemu_thread_mutex_create() {
	netemu_mutex mutex_struct;
	/* We need a pointer to this mutex, so we can save it off in our list and send the index back to the user. Let's allocate it on the heap.*/
	mutex_struct = malloc(sizeof(struct netemu_mutex));
	mutex_struct->mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_struct->mutex, NULL);
	return mutex_struct;
}

/**
 * Lock a mutex lock.
 * @param NETEMU_MUTEX identifier the identifier of the mutex lock.
 */
void netemu_thread_mutex_lock(netemu_mutex mutex_identifier) {
	pthread_mutex_lock(mutex_identifier->mutex);
}

/**
 * Release a lock.
 * @param NETEMU_MUTEX identifier the identifier for this lock.
 */
void netemu_thread_mutex_release(netemu_mutex mutex_identifier) {
	pthread_mutex_unlock(mutex_identifier->mutex);
}

/**
 * Destroy a mutex lock.
 * @param NETEMU_MUTEX the identifier of the lock.
 */
void netemu_thread_mutex_destroy(netemu_mutex mutex_identifier) {
	pthread_mutex_destroy(mutex_identifier->mutex);
	free(mutex_identifier);
}
