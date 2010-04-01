#include "netemu_thread.h"

struct netemu_thread_args {
	void (*start_fn) (void *);
	void *arguments;
};

DWORD WINAPI _netemu_thread_callback(void *arg);

int netemu_thread_new(netemu_thread identifier, void (*start_fn) (void *), void* arg) {
	struct netemu_thread_args *thread_args;

	thread_args = malloc(sizeof(struct netemu_thread_args));
	thread_args->start_fn = start_fn;
	thread_args->arguments = arg;
	return CreateThread(NULL, 0, _netemu_thread_callback, thread_args, 0, identifier);
}

int netemu_thread_stop(netemu_thread* identifier) {
	return (int)TerminateThread(identifier, 0);
}

DWORD WINAPI _netemu_thread_callback(void *arg) {
	struct netemu_thread_args *data = (struct netemu_thread_args *)arg;
	data->start_fn(data->arguments);
}