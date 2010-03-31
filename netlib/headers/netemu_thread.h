/*
 * netemu_thread.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

/**
 * @file
 * This header defines an interface which clients can use to get cross platform multithreading.
 */
#ifndef NETEMU_THREAD_H_
#define NETEMU_THREAD_H_

/* TODO: Fix this for windows as well. */
typedef unsigned long int netemu_thread;

/* TODO: make sure this is a good type to use. */
typedef int NETEMU_MUTEX;

/**
 * Start a new thread.
 * @param netemu_thread* identifier an identifier that can be used to identify the thread.
 * @param callback a function callback that will be called when the thread has started.
 * @return 0 if the thread was created successfully, an error code otherwise.
 */
int netemu_thread_new(netemu_thread* identifier, void (*start_fn) (void *), void* arg);

/**
 * Exit the current thread.
 * @return 0 if everything went ok.
 */
int netemu_thread_exit();

/**
 * Create a new mutex lock.
 * @return an identifier for the mutex lock.
 */
NETEMU_MUTEX netemu_thread_mutex_create();

/**
 * Lock a mutex lock.
 * @param NETEMU_MUTEX identifier the identifier of the mutex lock.
 */
void netemu_thread_mutex_lock(NETEMU_MUTEX* mutex_identifier);

/**
 * Release a lock.
 * @param NETEMU_MUTEX identifier the identifier for this lock.
 */
void netemu_thread_mutex_release(NETEMU_MUTEX* mutex_identifier);

/**
 * Destroy a mutex lock.
 * @param NETEMU_MUTEX the identifier of the lock.
 */
void netemu_thread_mutex_destroy(NETEMU_MUTEX* mutex_identifier);



#endif /* NETEMU_THREAD_H_ */
