#include "netemu_thread.h"

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
	struct netemu_thread_args *thread_args;
	thread_args = malloc(sizeof(struct netemu_thread_args));
	thread_args->start_fn = start_fn;
	thread_args->arguments = arg;
	return CreateThread(NULL, 0, _netemu_thread_callback, thread_args, 0, NULL);
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
	netemu_mutex mutex_struct;

	/* TODO: ska mutex_struct verkligen vara en typdef för mutex_struct*? Namnkonflikt. */
	mutex_struct = malloc(sizeof(struct netemu_mutex_internal));
	mutex_struct->mutex = CreateMutex(0, FALSE, NULL);
	return mutex_struct;
}

int netemu_thread_mutex_lock(netemu_mutex mutex_identifier, DWORD timeout) {
	if(WaitForSingleObject(mutex_identifier->mutex, timeout) == WAIT_OBJECT_0)
		return 0;
	else
		return -1;
}

int netemu_thread_mutex_release(netemu_mutex mutex_identifier) {
	return ReleaseMutex(mutex_identifier->mutex);
}

int netemu_thread_mutex_destroy(netemu_mutex mutex_identifier) {
	/* TODO: Mutex-objektet blir förstört först när ALLA handles till objektet
	 * har stängts, hur försäkrar man sig om att det inte finns mer handles till mutex-objektet?*/
	return CloseHandle(mutex_identifier->mutex);
}

netemu_event netemu_thread_event_create() {
	netemu_event event_struct;
	HANDLE handle;

	/* Manual reset sätts till false, vilket betyder att såfort den signaleras och nåt objekt väntat på eventet,
	 * så nollställs det till non-signaled automatiskt.*/
	if((handle = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return NULL;

	if((event_struct = malloc(sizeof(struct netemu_event_internal))) == NULL)
		return NULL;

	event_struct->eventhandle = handle;

	return event_struct;
}

int netemu_thread_event_signal(netemu_event event_identifier) {
	return SetEvent(event_identifier->eventhandle);
}

int netemu_thread_event_wait(netemu_event event_identifier) {
	if(WaitForSingleObject(event_identifier->eventhandle, INFINITE) == WAIT_OBJECT_0)
		return 0;
	else
		return -1;
}

int netemu_thread_event_destroy(netemu_event event_identifier) {
	return CloseHandle(event_identifier->eventhandle);
}