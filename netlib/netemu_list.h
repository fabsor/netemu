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
        void** elements;
        int element_size;
        int count;
    };

    struct netemu_list* netemu_list_new(int element_size, int count);

    void netemu_list_add(struct netemu_list* list, void* element);

    int netemu_remove(struct netemu_list* list, void* element);

    int netemu_list_remove_at(struct netemu_list* list, int index);

    /**
     * Get a pointer to an element in the list.
     */
    void* netemu_list_get(struct netemu_list* list, int index);

    void netemu_list_clear(struct netemu_list* list);
#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_LIST_H */

