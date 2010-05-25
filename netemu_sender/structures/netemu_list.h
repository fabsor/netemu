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
 * This file defines the interface for the netemu_list module.
 */

/**
 * @defgroup netemu_list Netemu list module
 * The netemu list module can be used to construct list without
 * having to worry about the size of it.
 */
#pragma once
#ifndef _NETEMU_LIST_H
#define	_NETEMU_LIST_H

#include "netlib_util.h"

#ifdef	__cplusplus
extern "C" {
#endif

	typedef struct _netemu_list_internal* netemu_list_internal;
	/**
	 * This struct is the base for the netemu_list module.
	 * @ingroup netemu_list
	 */
	struct netemu_list{
        void** elements; /**< The pointer to the block in memory containing all entries. It's totally safe to loop over this. */
        int count; /**< The number of elements in the list */
        NETEMU_BOOL sorted; /**< Indiciates if the list is sorted or not. */
		NETEMU_BOOL thread_safe; /**< Indicates if the list is thread safe or not. */
        netemu_list_internal _intern; /**< Internal data used by the module. Do not touch this! */
    };

	/**
	 * Create a new instance of netemu_list.
	 * @param size the starting size of the array. This size will be allocated.
	 * @param thread_safe Indicates if this list should be thread safe or not.
	 * @return the list instance of the module.
	 */
    struct netemu_list* netemu_list_create(int size, NETEMU_BOOL thread_safe);

    /**
     * Add an item to the list.
     * @ingroup netemu_list
     * @param list the list instance.
     * @param the element to add
     * @return -1 on failure, 0 if everything went OK.
     */
    int netemu_list_add(struct netemu_list* list, void* element);

    int netemu_list_remove(struct netemu_list* list, void* element);

    /**
     * Remove item at specified inde
     * @ingroup netemu_list
     * @param list the list instance
     * @param index the index of the element to be removed.
     * @return < 0 on failure, 0 if everything went OK.
     */
    int netemu_list_remove_at(struct netemu_list* list, int index);

    /**
     * Copy a list to a pointer buffer.
     * @ingroup netemu_list
     * @param list the list instance
     * @param buffer the buffer to copy to. The function will allocate enough memory to hold all elements itself.
     * @return -1 on failure, 0 if everything went OK.
     */
    int netemu_list_copy(struct netemu_list* list, void ***buffer);

    /**
     * Check if the element in the list contains the provided element.
     * @ingroup netemu_list
     * @param list the list instance.
     * @param element the element to look for.
     * @return >= 0 if the element exists, -1 otherwise.
     */
    int netemu_list_contains(struct netemu_list* list, void* element);

    /**
     * Free the resources taken up by this list. Note that this will not free the memory the items in the list
     * takes up, since the items might be used in other parts of the program.
     * @ingroup netemu_list
     * @param list the pointer to the list to be freed.
     */
    void netemu_list_destroy(struct netemu_list* list);

    /**
     * Trim the list to the actual size.
     * @ingroup netemu_list
     * @param list the list instance.
     */
    void netemu_list_trim(struct netemu_list* list);

    /**
     * Get a pointer to an element in the list.
     * @return the pointer to the element in the list, or NULL.
     */
    void* netemu_list_get(struct netemu_list* list, int index);

    /**
     * Sort the list. To use this function, you need to register a sorting function.
     * @return 1 if the sorting was successful, 0 if you haven't registered a sorting function.
     */
    int netemu_list_sort(struct netemu_list* list);

    /**
     * Register a sorting function for your elements. If you do this, the elements will be sorted with
     * qsort and searches can be done with binary search.
     * @ingroup netemu_list
     * @param list the list to register the function to.
     * @param comparator the comparator function.
     */
    void netemu_list_register_sort_fn(struct netemu_list* list, int ( * comparator ) ( const void *, const void * ));

    /**
     * Clear out the list.
     * @ingroup netemu_list
     * @param list a list instance.
     */
    int netemu_list_clear(struct netemu_list* list);
#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_LIST_H */

