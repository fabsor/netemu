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

	/* TODO: make sure this is a good type to use. */
	typedef struct netemu_mutex* netemu_mutex;

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
	int netemu_thread_mutex_lock(netemu_mutex mutex_identifier);

   /**
	* Release a lock.
	* @param NETEMU_MUTEX identifier the identifier for this lock.
	*/
	void netemu_thread_mutex_release(netemu_mutex mutex_identifier);

   /**
	* Destroy a mutex lock.
	* @param NETEMU_MUTEX the identifier of the lock.
	*/
	void netemu_thread_mutex_destroy(netemu_mutex mutex_identifier);

#ifdef	__cplusplus
}
#endif
#endif /* NETEMU_THREAD_H_ */
