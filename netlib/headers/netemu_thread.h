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
#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _NIX
	#include <pthread.h>
	typedef pthread_t* netemu_thread;
#else
#include <Windows.h>
	typedef HANDLE netemu_thread;
#endif

#define NETEMU_INFINITE			0xFFFFFFFF
#define NETEMU_WAIT_TIMEOUT		0xF0F0F0F0
#include "netlib_util.h"

	/* TODO: make sure this is a good type to use. */
	typedef struct netemu_mutex_internal* netemu_mutex;

	typedef struct netemu_event_internal* netemu_event;

	/**
	* Start a new thread.
	* @param netemu_thread* identifier an identifier that can be used to identify the thread.
	* @param callback a function callback that will be called when the thread has started.
	* @return 0 if the thread was created successfully, an error code otherwise.
	*/
	netemu_thread netemu_thread_new(void (*start_fn) (void *), void* arg);

	/**
	* Exit the current thread.
	* @return 0 if everything went ok.
	*/
	int netemu_thread_exit();

	/**
	* Create a new mutex lock.
	* @return an identifier for the mutex lock.
	*/
	netemu_mutex netemu_thread_mutex_create();

	/**
	* Lock a mutex lock.
	* @param NETEMU_MUTEX identifier the identifier of the mutex lock.
	*/
	int netemu_thread_mutex_lock(netemu_mutex mutex_identifier, NETEMU_DWORD timeout);

   /**
	* Release a lock.
	* @param NETEMU_MUTEX identifier the identifier for this lock.
	*/
	int netemu_thread_mutex_release(netemu_mutex mutex_identifier);

   /**
	* Destroy a mutex lock.
	* @param NETEMU_MUTEX the identifier of the lock.
	*/
	int netemu_thread_mutex_destroy(netemu_mutex mutex_identifier);

   /**
	* Creates an event.
	* @return an identifier for the event, or NULL if something went wrong.
	*/
	netemu_event netemu_thread_event_create();

   /**
	* Signals an event.
	* @param event_identifier the identifier of the event.
	*/
	int netemu_thread_event_signal(netemu_event event_identifier);

	/**
	* Waits for an event to be signaled.
	* @param event_identifier the identifier of the event.
	*/
	int netemu_thread_event_wait(netemu_event event_identifier, NETEMU_DWORD seconds);

	/**
	* Destroy an event.
	* @param event_identifier the identifier of the event.
	*/
	int netemu_thread_event_destroy(netemu_event event_identifier);

#ifdef	__cplusplus
}
#endif
#endif /* NETEMU_THREAD_H_ */
