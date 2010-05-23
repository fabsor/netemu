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

#include "netemu_thread.h"
#include "netlib_error.h"

struct netemu_mutex_internal {
	HANDLE mutex;
};

struct netemu_event_internal {
	HANDLE eventhandle;
};

struct netemu_thread_args {
	void (*start_fn) (void *);
	void *arguments;
};

DWORD WINAPI _netemu_thread_callback(void *arg);

netemu_thread netemu_thread_new(void (*start_fn) (void *), void* arg) {
	netemu_thread return_thread;
	int errcode;
	struct netemu_thread_args *thread_args;
	if((thread_args = malloc(sizeof(struct netemu_thread_args))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	thread_args->start_fn = start_fn;
	thread_args->arguments = arg;
	return_thread = CreateThread(NULL, 0, _netemu_thread_callback, thread_args, 0, NULL);

	if(return_thread == NULL) {
		netlib_set_last_mapped_error(GetLastError());
		free(thread_args);
	}

	return return_thread;
}

int netemu_thread_exit() {
	ExitThread(0);
	return 0;
}

DWORD WINAPI _netemu_thread_callback(void *arg) {
	struct netemu_thread_args *data = (struct netemu_thread_args *)arg;
	data->start_fn(data->arguments);

	return 0;
}

netemu_mutex netemu_thread_mutex_create() {
	int errcode;
	netemu_mutex mutex_struct;

	if((mutex_struct = malloc(sizeof(struct netemu_mutex_internal))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	if((mutex_struct->mutex = CreateMutex(0, FALSE, NULL)) == NULL) {
		netlib_set_last_error(GetLastError());
		free(mutex_struct);

		return NULL;
	}
	return mutex_struct;
}

int netemu_thread_mutex_lock(netemu_mutex mutex_identifier, DWORD timeout) {
	int errcode;

	errcode = WaitForSingleObject(mutex_identifier->mutex, timeout);
	if(errcode == WAIT_OBJECT_0)
		return 0;
	else if(errcode == WAIT_FAILED)
		netlib_set_last_mapped_error(GetLastError());

	/* TODO: Det �r inte s� bra att bara returnera -1 h�r, eftersom det antyder att n�got gick fel i WaitForSingleObject,
	 * vilket det n�dv�ndigtvis inte gjorde.*/
	return -1;
}

int netemu_thread_mutex_release(netemu_mutex mutex_identifier) {
	int errcode;
	
	errcode = ReleaseMutex(mutex_identifier->mutex);
	if(errcode == 0) {
		netlib_set_last_mapped_error(GetLastError());
		return -1;
	}
	return 0;
}

int netemu_thread_mutex_destroy(netemu_mutex mutex_identifier) {
	int errcode;

	/* TODO: Mutex-objektet blir f�rst�rt f�rst n�r ALLA handles till objektet
	 * har st�ngts, hur f�rs�krar man sig om att det inte finns mer handles till mutex-objektet?*/
	errcode = CloseHandle(mutex_identifier->mutex);
	if(errcode == 0) {
		netlib_set_last_mapped_error(GetLastError());
		return -1;
	}
	return 0;
}

netemu_event netemu_thread_event_create() {
	netemu_event event_struct;
	HANDLE handle;

	if((event_struct = malloc(sizeof(struct netemu_event_internal))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	/* Manual reset s�tts till false, vilket betyder att s�fort den signaleras och n�t objekt v�ntat p� eventet,
	 * s� nollst�lls det till non-signaled automatiskt.*/
	if((handle = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		netlib_set_last_mapped_error(GetLastError());
		free(event_struct);
		return NULL;
	}

	event_struct->eventhandle = handle;

	return event_struct;
}

int netemu_thread_event_signal(netemu_event event_identifier) {
	int errcode;
	
	errcode = SetEvent(event_identifier->eventhandle);
	if(errcode == 0) {
		netlib_set_last_mapped_error(GetLastError());
		return -1;
	}
	return 0;
}

int netemu_thread_event_wait(netemu_event event_identifier, NETEMU_DWORD seconds) {
	int errcode;
	
	errcode = WaitForSingleObject(event_identifier->eventhandle, seconds);
	if(errcode == WAIT_OBJECT_0)
		return 0;
	else if(errcode == WAIT_FAILED)
		netlib_set_last_mapped_error(GetLastError());

	/* TODO: Se kommentaren i mutex_lock! */
	return -1;
}

int netemu_thread_event_destroy(netemu_event event_identifier) {
	int errcode;

	errcode = CloseHandle(event_identifier->eventhandle);
	if(errcode == 0) {
		netlib_set_last_mapped_error(GetLastError());
		return -1;
	}
	return 0;
}
