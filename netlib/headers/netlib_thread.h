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
* This header defines an interface which clients can use to get cross platform multithreading.
*/
#pragma once
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

#define NETLIB_INFINITE			0xFFFFFFFF
#define NETLIB_WAIT_TIMEOUT		0xF0F0F0F0
#include "netlib_util.h"
	typedef struct netlib_mutex_internal* netlib_mutex;

	typedef struct netlib_event_internal* netlib_event;

	/**
	* Start a new thread.
	* @param netemu_thread* identifier an identifier that can be used to identify the thread.
	* @param callback a function callback that will be called when the thread has started.
	* @return 0 if the thread was created successfully, an error code otherwise.
	*/
	netemu_thread netlib_thread_new(void (*start_fn) (void *), void* arg);

	/**
	* Exit the current thread.
	* @return 0 if everything went ok.
	*/
	int netlib_thread_exit();

	/**
	* Create a new mutex lock.
	* @return an identifier for the mutex lock.
	*/
	netlib_mutex netlib_thread_mutex_create();

	/**
	* Lock a mutex lock.
	* @param NETEMU_MUTEX identifier the identifier of the mutex lock.
	*/
	int netlib_thread_mutex_lock(netlib_mutex mutex_identifier, NETEMU_DWORD timeout);

   /**
	* Release a lock.
	* @param NETEMU_MUTEX identifier the identifier for this lock.
	*/
	int netlib_thread_mutex_release(netlib_mutex mutex_identifier);

   /**
	* Destroy a mutex lock.
	* @param NETEMU_MUTEX the identifier of the lock.
	*/
	int netlib_thread_mutex_destroy(netlib_mutex mutex_identifier);

   /**
	* Creates an event.
	* @return an identifier for the event, or NULL if something went wrong.
	*/
	netlib_event netlib_thread_event_create();

   /**
	* Signals an event.
	* @param event_identifier the identifier of the event.
	*/
	int netlib_thread_event_signal(netlib_event event_identifier);

	/**
	* Waits for an event to be signaled.
	* @param event_identifier the identifier of the event.
	*/
	int netlib_thread_event_wait(netlib_event event_identifier, NETEMU_DWORD seconds);

	/**
	* Destroy an event.
	* @param event_identifier the identifier of the event.
	*/
	int netlib_thread_event_destroy(netlib_event event_identifier);

#ifdef	__cplusplus
}
#endif
#endif /* NETEMU_THREAD_H_ */
