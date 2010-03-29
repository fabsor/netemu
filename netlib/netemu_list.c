#include <sys/socket.h>
#include <stdlib.h>

#include "netemu_list.h"

void _netemu_enlarge_list(struct netemu_list* list, int size);

struct netemu_list* netemu_list_new(int element_size,int count) {
  struct netemu_list list;
  list.size = count;
  list.element_size = element_size;
  list.elements = malloc(sizeof(element_size)*list.size);
}

void netemu_list_add(struct netemu_list* list, void* element) {
    if(list->size < list->count){
        _netemu_enlarge_list(list,10);
    }
}

void _netemu_enlarge_list(struct netemu_list* list, int size) {
    int i;
    list->size += size;
    void** elements = malloc(sizeof(list->element_size)*list->size);
    for (i  = 0; i < list->count; i++) {
        elements[i] = list->elements[i];
    }
}

int netemu_list_remove(struct netemu_list* list, void* element){
    
}


int netemu_list_remove_at(struct netemu_list* list,int index);

/**
 * Get a pointer to an element in the list.
 */
void* netemu_list_get(int index);