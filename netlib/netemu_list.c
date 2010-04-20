#include <stdlib.h>
#include "headers/netemu_list.h"
#include <string.h>
#include "headers/netlib_util.h"

struct _netemu_list_internal {
	int (* comparator)(const void *, const void *);
	int size;
};

void _netemu_enlarge_list(struct netemu_list* list, int size);
int _netemu_list_search_linear(struct netemu_list* list, void* element);

struct netemu_list* netemu_list_new(int count) {
	struct netemu_list* list;
	struct _netemu_list_internal* intern;

	intern = malloc(sizeof(struct _netemu_list_internal));
	list = malloc(sizeof(struct netemu_list));
	intern->size = count;
	list->elements = malloc(sizeof(void*) * intern->size);
	list->count = 0;
	list->_intern = intern;
	return list;
}

void netemu_list_add(struct netemu_list* list, void* element) {
	/* Enlarge the array if necessary. */
	if (list->_intern->size <= list->count) {
		_netemu_enlarge_list(list, 10);
	}
	list->elements[list->count] = element;
	list->count++;
	list->sorted = 0;
}

void _netemu_enlarge_list(struct netemu_list* list, int size) {
	list->_intern->size += size;
	list->elements = realloc(list->elements,list->_intern->size * sizeof(void*));
}

int netemu_list_contains(struct netemu_list* list, void* element) {
	return _netemu_list_search_linear(list, element);
}

int _netemu_list_search_linear(struct netemu_list* list, void* element) {
	int i;
	int index = -1;
	for (i = 0; i < list->count; i++) {
		if (list->elements[i] == element) {
			return index;
		}
	}
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
	if (list->count < index) {
		return -1;
	}
	for (i = index; i < list->count - 1; i++) {
		list->elements[i] = list->elements[i + 1];
	}
	list->count--;
	return 1;
}

int netemu_list_copy(struct netemu_list* list, void ***buffer) {
	NETEMU_DWORD size;
	size = sizeof(void*) * list->count;

	if((*buffer = malloc(size)) == NULL)
		return -1;

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
 * @return 1 if the sorting was successful, 0 if you haven't registered a sorting function.
 */
int netemu_list_sort(struct netemu_list* list) {
	/* If this list is sorted, then there's nothing to be done. */
	if (list->sorted)
		return 1;

	/* We can't sort if we don't have a comparator. */
	if (list->_intern->comparator != 0) {
		qsort(list->elements[0], list->count, sizeof(void*),
				list->_intern->comparator);
		list->sorted = 1;
		return 1;
	}
	return 0;
}

/**
 * Trim the list to the actual size.
 */
void netemu_list_trim(struct netemu_list* list) {

}

/**
 * Free the resources taken up by this list. Note that this will not free the memory the items in the list
 * takes up, since the items might be used in other parts of the program.
 */
void netemu_list_free(struct netemu_list* list) {
	free(list->_intern);
	free(list->elements);
}

/**
 * Get a pointer to an element in the list.
 * @return the pointer to the element in the list or null.
 */
void* netemu_list_get(struct netemu_list* list, int index) {
	if (index > list->count - 1) {
		return NULL;
	}
	return list->elements[index];
}

void netemu_list_clear(struct netemu_list* list) {
	free(list->elements);
	list->elements = malloc(sizeof(void*)*20);
	list->count = 0;
}
