/*
 *   This file is part of netlib.
 *
 *   Netlib is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netlib is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netlib.  If not, see <http://www.gnu.org/licenses/>
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
#include <time.h>
#include <errno.h>
#include "headers/netlib_error.h"

struct netemu_event_internal {
	pthread_mutex_t* mutex;
	pthread_cond_t* cond;
};

struct netemu_mutex_internal{
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
netemu_thread netemu_thread_new(void (*start_fn) (void *), void* arg) {
	struct netemu_thread_args* thread_args;
	netemu_thread identifier;
	int rc;
	identifier = malloc(sizeof(pthread_t));
	thread_args = malloc(sizeof(struct netemu_thread_args));
	thread_args->start_fn = start_fn;
	thread_args->arg = arg;
	rc = pthread_create(identifier, NULL, _netemu_thread_callback, thread_args);
	if(rc) {
        return NULL;
	}
	return identifier;
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
	struct netemu_mutex_internal* mutex_struct;
	/* We need a pointer to this mutex, so we can save it off in our list and send the index back to the user. Let's allocate it on the heap.*/
	mutex_struct = malloc(sizeof(struct netemu_mutex_internal));
	mutex_struct->mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_struct->mutex, NULL);
	return mutex_struct;
}

/**
 * Lock a mutex lock.
 * @param NETEMU_MUTEX identifier the identifier of the mutex lock.
 */
int netemu_thread_mutex_lock(netemu_mutex mutex_identifier, NETEMU_DWORD timeout) {
	int error;
	if(timeout == NETEMU_INFINITE) {
		error = pthread_mutex_lock(mutex_identifier->mutex);
	}
	else {
		error = pthread_mutex_timedlock(mutex_identifier,timeout);
	}
	if(error != 0) {
		return -1;
	}
	return 0;
}

/**
 * Release a lock.
 * @param NETEMU_MUTEX identifier the identifier for this lock.
 */
int netemu_thread_mutex_release(netemu_mutex mutex_identifier) {
	int error;
	error = pthread_mutex_unlock(mutex_identifier->mutex);
	if(error != 0) {
		return -1;
	}
	return 0;
}

/**
 * Destroy a mutex lock.
 * @param NETEMU_MUTEX the identifier of the lock.
 */
int netemu_thread_mutex_destroy(netemu_mutex mutex_identifier) {
	int error;
	error = pthread_mutex_destroy(mutex_identifier->mutex);
	free(mutex_identifier);
	if(error != 0) {
		return -1;
	}
	return 0;
}

/**
 * Creates an event.
 * @return an identifier for the event, or NULL if something went wrong.
 */
netemu_event netemu_thread_event_create() {
	struct netemu_event_internal *event;

	event = (struct netemu_event_internal*)malloc(sizeof(struct netemu_event_internal));
	event->mutex = malloc(sizeof(pthread_mutex_t));
	event->cond = malloc(sizeof(pthread_cond_t));

	pthread_cond_init(event->cond, NULL);
	pthread_mutex_init(event->mutex, NULL);

	return event;
}

/**
 * Signals an event.
 * @param event_identifier the identifier of the event.
 */
int netemu_thread_event_signal(netemu_event event_identifier) {
	pthread_mutex_lock(event_identifier->mutex);
	pthread_cond_signal(event_identifier->cond);
	pthread_mutex_unlock(event_identifier->mutex);

	return 0;
}

/**
 * Waits for an event to be signaled.
 * @param event_identifier the identifier of the event.
 */
int netemu_thread_event_wait(netemu_event event_identifier, NETEMU_DWORD seconds) {
	struct timespec spec;
	int retval;

	pthread_mutex_lock(event_identifier->mutex);
	if(seconds = NETEMU_INFINITE) {
		retval = pthread_cond_wait(event_identifier->cond, event_identifier->mutex);
	}
	else {
		spec.tv_sec = seconds;
		retval = pthread_cond_timedwait(event_identifier->cond, event_identifier->mutex, &spec);
	}
	pthread_mutex_unlock(event_identifier->mutex);

	if(retval == ETIMEDOUT) {
		retval = NETEMU_WAIT_TIMEOUT;
	}
	else if(retval != 0) {
		retval = -1;
		netlib_set_last_error(NETEMU_EUNKNOWNERROR);
	}
	else {
		retval = 0;
	}

	return retval;
}

int netemu_thread_event_destroy(netemu_event event_identifier) {
	pthread_mutex_destroy(event_identifier->mutex);
	pthread_cond_destroy(event_identifier->cond);
	return 0;
}
