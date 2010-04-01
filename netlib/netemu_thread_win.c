#include "netemu_thread.h"

struct netemu_threadstart{
	void (*start_fn) (void *);
	void *arguments;
};

DWORD WINAPI _netemu_thread_callback(void *arg);

int netemu_thread_new(netemu_thread* identifier, void (*start_fn) (void *), void* arg) {
	struct netemu_threadstart data;
	data.arguments = arg;
	data.start_fn = start_fn;
	//return CreateThread(NULL, 0, start_fn, arg, 0, identifier);
}

int netemu_thread_stop(netemu_thread* identifier) {
	return (int)TerminateThread(identifier, 0);
}

DWORD WINAPI _netemu_thread_callback(void *arg) {
	struct netemu_threadstart *data = (struct netemu_threadstart *)arg;
	data->start_fn(data->arguments);
}