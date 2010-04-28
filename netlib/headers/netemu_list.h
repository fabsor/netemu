/* 
 * File:   netemu_list.h
 * Author: fabian
 *
 * Created on den 29 mars 2010, 09:20
 */

#ifndef _NETEMU_LIST_H
#define	_NETEMU_LIST_H

#include "netlib_util.h"

#ifdef	__cplusplus
extern "C" {
#endif

	typedef struct _netemu_list_internal* netemu_list_internal;

	struct netemu_list{
        void** elements;
        int count;
        int sorted;
		NETEMU_BOOL thread_safe;
        netemu_list_internal _intern;
    };

    struct netemu_list* netemu_list_new(int count, NETEMU_BOOL thread_safe);

    int netemu_list_add(struct netemu_list* list, void* element);

    int netemu_list_remove(struct netemu_list* list, void* element);

    int netemu_list_remove_at(struct netemu_list* list, int index);

    int netemu_list_copy(struct netemu_list* list, void ***buffer);

    /**
     * Check if the element in the list contains the provided element.
     * @return 1 if the element exists, 0 otherwise.
     */
    int netemu_list_contains(struct netemu_list*, void* element);

    /**
     * Free the resources taken up by this list. Note that this will not free the memory the items in the list
     * takes up, since the items might be used in other parts of the program.
     * @param struct netemu_list* list the pointer to the list to be freed.
     */
    void netemu_list_free(struct netemu_list* list);

    /**
     * Trim the list to the actual size.
     */
    void netemu_list_trim(struct netemu_list* list);

    /**
     * Get a pointer to an element in the list.
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
     * @param struct netemu_list* list the list to register the function to.
     * @param the comparator function.
     */
    void netemu_list_register_sort_fn(struct netemu_list* list, int ( * comparator ) ( const void *, const void * ));

    int netemu_list_clear(struct netemu_list* list);
#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_LIST_H */

