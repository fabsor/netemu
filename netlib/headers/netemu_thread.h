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
#ifdef _NIX

#ifndef STDCALL 
#define STDCALL __attribute__((stdcall)) 
#endif 

#define NETEMU_API		STDCALL

#else
#include <Windows.h>
#define NETEMU_API		WINAPI
#endif

int netemu_thread_new(netemu_thread* identifier, unsigned long (NETEMU_API *start_fn) (void *));

int netemu_thread_stop(netemu_thread* identifier);

int netemu_thread_mutex_lock(void* memory);

int netemu_thread_mutex_release(void* memory);

#endif /* NETEMU_THREAD_H_ */