/* 
 * File:   netemu_list.h
 * Author: fabian
 *
 * Created on den 29 mars 2010, 09:20
 */

#ifndef _NETEMU_LIST_H
#define	_NETEMU_LIST_H

#ifdef	__cplusplus
extern "C" {
#endif
    struct netemu_list{
        int size;
        void* elements;
        int element_size;
        int count;
    };

    struct netemu_list* netemu_list_new(int element_size);

    struct netemu_list* netemu_list_new(int element_size, int count);

    void netemu_list_add(void* element);

    int netemu_remove(void* element);

    int netemu_list_remove_at(int index);

    /**
     * Get a pointer to an element in the list.
     */
    void* netemu_list_get(int index);
#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_LIST_H */

