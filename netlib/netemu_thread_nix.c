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
#include "headers/netemu_thread.h"
#include "pthread.h"
#include "headers/netemu_list.h"

/* TODO: Tweak this value. We try a big one initially. */
#define NETEMU_THREAD_MUTEX_LIST_INITIAL_SIZE 100;

void *_netemu_thread_callback(void* arg);

/*! This struct is used for passing arguments to the thread. */
struct netemu_thread_struct {
	netemu_thread* identifier;
	void (*start_fn) (void *);
	void* arg;
};

/*! We save all mutex locks created by a program here. That way, we can make sure there are no duplicate mutex locks. */
struct netemu_list* mutexes;

/*! We need to protect the mutexes list from being accessed simultaneusly. We will need a lock for controlling this. */
pthread_mutex_t mutex_lock;

/**
 * Start a new thread.
 * @param netemu_thread* identifier an identifier that can be used to identify the thread.
 * @param callback a function callback that will be called when the thread has started.
 * @return 0 if the thread was created successfully, an error code otherwise.
 */
int netemu_thread_new(netemu_thread* identifier, void (*start_fn) (void *), void* arg) {
	return pthread_create(identifier, NULL, _netemu_thread_callback, arg);
}

/**
 * Callback for the pthread_create() call.
 * @param arg this will always be a netemu_thread_struct.
 */
void *_netemu_thread_callback(void* arg) {
	struct netemu_thread_struct *arg_struct;
	arg_struct = (struct netemu_thread_struct*)arg;
	/*Start the thread by calling the function passed by the user. */
	arg_struct->start_fn(arg);
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
NETEMU_MUTEX netemu_thread_mutex_create() {
	pthread_mutex_t* mutex;
	/* Make sure that the list is instantiated. */
	if (mutexes == NULL) {
		mutexes = netemu_list_new(sizeof(pthread_mutex_t), 100);
		pthread_mutex_init(&mutex_lock, NULL);
	}
	/* We need a pointer to this mutex, so we can save it off in our list and send the index back to the user. Let's allocate it on the heap.*/
	mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	/* Now, let's save it in our list of mutex locks. We also need to use our lock. */
	pthread_mutex_lock(&mutex_lock);
	netemu_list_add(mutexes,mutex);
	pthread_mutex_unlock(&mutex_lock);

	/* We just return the index of the added mutex. */
	return mutexes->count-1;
}

/**
 * Lock a mutex lock.
 * @param NETEMU_MUTEX identifier the identifier of the mutex lock.
 */
void netemu_thread_mutex_lock(NETEMU_MUTEX *mutex_identifier) {
	pthread_mutex_t* mutex;
	pthread_mutex_lock(&mutex_lock);
	mutex = mutexes->elements[*mutex_identifier];
	pthread_mutex_unlock(&mutex_lock);
	pthread_mutex_lock(mutex);
}

/**
 * Release a lock.
 * @param NETEMU_MUTEX identifier the identifier for this lock.
 */
void netemu_thread_mutex_release(NETEMU_MUTEX *mutex_identifier) {
	pthread_mutex_t* mutex;
	/* We need to acquire a lock in order to be able to modify the list. */
	pthread_mutex_lock(&mutex_lock);
	mutex = mutexes->elements[*mutex_identifier];
	pthread_mutex_unlock(&mutex_lock);
	pthread_mutex_unlock(mutex);
}

/**
 * Destroy a mutex lock.
 * @param NETEMU_MUTEX the identifier of the lock.
 */
void netemu_thread_mutex_destroy(NETEMU_MUTEX *mutex_identifier) {
	pthread_mutex_t* mutex;
	pthread_mutex_lock(&mutex_lock);
	mutex = mutexes->elements[*mutex_identifier];
	netemu_list_remove_at(mutexes,*mutex_identifier);
	pthread_mutex_unlock(&mutex_lock);
	pthread_mutex_destroy(mutex);
	free(mutex);
}
