/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#include <stdlib.h>
#include "netemu_list.h"
#include <string.h>
#include "netlib_error.h"
#include "netemu_thread.h"

struct _netemu_list_internal {
	int (* comparator)(const void *, const void *);
	int size;
	int FOO;
	netemu_mutex list_mutex;
};

int _netemu_enlarge_list(struct netemu_list* list, int size);
int _netemu_list_search_linear(struct netemu_list* list, void* element);

struct netemu_list* netemu_list_create(int count, NETEMU_BOOL thread_safe) {
	struct netemu_list* list;
	struct _netemu_list_internal* intern;
	if((intern = malloc(sizeof(struct _netemu_list_internal))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	if((list = malloc(sizeof(struct netemu_list))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(intern);
		return NULL;
	}

	list->thread_safe = thread_safe;
	if(list->thread_safe)
		intern->list_mutex = netemu_thread_mutex_create();

	intern->size = count;
	if((list->elements = malloc(sizeof(void*) * intern->size)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(intern);
		free(list);
		free(list->elements);
		return NULL;
	}
	intern->FOO = 0;
	list->count = 0;
	list->sorted = 0;
	list->_intern = intern;
	list->_intern->comparator = NULL;
	return list;
}

int netemu_list_add(struct netemu_list* list, void* element) {
	int error;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	/* Enlarge the array if necessary. */
	if (list->_intern->size <= list->count) {
		error = _netemu_enlarge_list(list, 10);
		if(error == -1)
			return -1;
	}
	list->elements[list->count] = element;
	list->count++;
	list->sorted = 0;

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return 0;
}

int _netemu_enlarge_list(struct netemu_list* list, int size) {
	void** elements;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	list->_intern->size += size;
	elements = list->elements;
	if((list->elements = malloc(sizeof(void*)*list->_intern->size)) == NULL) {
		netlib_set_last_mapped_error(NETEMU_ENOTENOUGHMEMORY);
		if(list->thread_safe)
			netemu_thread_mutex_release(list->_intern->list_mutex);
		return -1;
	}
	memcpy(list->elements,elements,sizeof(void*)*list->count);
	free(elements);

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return 0;
}

int netemu_list_contains(struct netemu_list* list, void* element) {
	return _netemu_list_search_linear(list, element);
}

int _netemu_list_search_linear(struct netemu_list* list, void* element) {
	int i;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	
	for (i = 0; i < list->count; i++) {
		if(list->_intern->comparator != NULL) {
			if(list->_intern->comparator(list->elements[i],element) == 0) {
				if(list->thread_safe)
					netemu_thread_mutex_release(list->_intern->list_mutex);
				return i;
			}
		}
		else {
			if (list->elements[i] == element) {
				if(list->thread_safe)
					netemu_thread_mutex_release(list->_intern->list_mutex);
				return i;
			}
		}
	}

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return -1;
}

int netemu_list_remove(struct netemu_list* list, void* element) {
	int index;

	/*TODO: Add binary search when sorting is available. */
	index = _netemu_list_search_linear(list, element);
	if (index == -1) {
		return index;
	}

	return netemu_list_remove_at(list, index);
}

int netemu_list_remove_at(struct netemu_list* list, int index) {
	int i;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	if (list->count < index) {
		if(list->thread_safe)
			netemu_thread_mutex_release(list->_intern->list_mutex);
		return -1;
	}
	for (i = index; i < list->count - 1; i++) {
		list->elements[i] = list->elements[i + 1];
	}
	list->count--;

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return 0;
}

int netemu_list_copy(struct netemu_list* list, void ***buffer) {
	NETEMU_DWORD size;
	size = sizeof(void*) * list->count;

	if((*buffer = malloc(size)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	memcpy(*buffer, list->elements, size);
	return 0;
}

/**
 * Register a sorting function for your elements. If you do this, the elements will be sorted with
 * qsort and searches can be done with binary search.
 * @param struct netemu_list* list the list to register the function to.
 * @param the comparator function.
 */
void netemu_list_register_sort_fn(struct netemu_list* list, int(* comparator)(
		const void *, const void *)) {
	list->_intern->comparator = comparator;
}

/**
 * Sort the list. To use this function, you need to register a sorting function.
 * @return 0 if the sorting was successful, 0 if you haven't registered a sorting function.
 */
int netemu_list_sort(struct netemu_list* list) {
	int return_value;

	return_value = -1;
	/* If this list is sorted, then there's nothing to be done. */
	if (list->sorted)
		return 0;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	/* We can't sort if we don't have a comparator. */
	if (list->_intern->comparator != 0) {
		qsort(list->elements[0], list->count, sizeof(void*),
				list->_intern->comparator);
		list->sorted = 1;
		return_value = 0;
	}

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return return_value;
}

/**
 * Trim the list to the actual size.
 * @todo: Actually implement this function.
 * @param list the list instance
 */
void netemu_list_trim(struct netemu_list* list) {

}

/**
 * Free the resources taken up by this list. Note that this will not free the memory the items in the list
 * takes up, since the items might be used in other parts of the program.
 */
void netemu_list_destroy(struct netemu_list* list) {
	free(list->elements);
	if(list->thread_safe) {
		netemu_thread_mutex_destroy(list->_intern->list_mutex);
	}
	free(list->_intern);
	free(list);
}

/**
 * Get a pointer to an element in the list.
 * @return the pointer to the element in the list, or NULL.
 */
void* netemu_list_get(struct netemu_list* list, int index) {
	if (index > list->count - 1 || index < 0) {
		return NULL;
	}
	return list->elements[index];
}

int netemu_list_clear(struct netemu_list* list) {
	int error = 0;
	void **new_buffer;

	if(list->thread_safe)
		netemu_thread_mutex_lock(list->_intern->list_mutex, NETEMU_INFINITE);

	if((new_buffer = malloc(sizeof(void*)*20)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		error = -1;
	}
	free(list->elements);
	list->elements = new_buffer;
	list->count = 0;
	list->_intern->size = 20;

	if(list->thread_safe)
		netemu_thread_mutex_release(list->_intern->list_mutex);

	return error;
}
