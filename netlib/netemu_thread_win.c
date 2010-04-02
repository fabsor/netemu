#include "netemu_thread.h"

struct netemu_mutex{
	HANDLE mutex;
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
	mutex_struct = malloc(sizeof(struct netemu_mutex));
	mutex_struct->mutex = CreateMutex(0, FALSE, "MUTEX");
	return mutex_struct;
}

void netemu_thread_mutex_lock(netemu_mutex mutex_identifier) {
	/* TODO: WaitForSingleObject returnerar värden som kan vara av värde! Kanske ska kolla lite på det? */
	WaitForSingleObject(mutex_identifier->mutex, INFINITE);
}

void netemu_thread_mutex_release(netemu_mutex mutex_identifier) {
	ReleaseMutex(mutex_identifier->mutex);
}

void netemu_thread_mutex_destroy(netemu_mutex mutex_identifier) {
	/* TODO: Mutex-objektet blir förstört först när ALLA handles till objektet
	 * har stängts, hur försäkrar man sig om att det inte finns mer handles till mutex-objektet?*/
	CloseHandle(mutex_identifier->mutex);
}
