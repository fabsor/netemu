/* Copyright (c) 2010 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Hash_table_(C)?action=history&offset=20100122102906

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Hash_table_(C)?oldid=16632
*/

#ifndef HASHTBL_H_INCLUDE_GUARD
#define HASHTBL_H_INCLUDE_GUARD

#include<stdlib.h>
#include "netemu_list.h"

typedef size_t hash_size;

struct hashnode_s {
	void *key;
	size_t key_len;
	void *data;
	struct hashnode_s *next;
};


typedef struct netemu_hashtbl {
	hash_size size;
	int count;
	struct netemu_list *keys;
	struct hashnode_s **nodes;
	int (*comparator)(const void* value1, const void* value2);
	hash_size (*hashfunc)(const void *, size_t);
} NETEMU_HASHTBL;

struct netemu_hashtable_iter {
	NETEMU_HASHTBL *table;
	int hashindex;
};

NETEMU_HASHTBL *netemu_hashtbl_create(hash_size size, hash_size (*hashfunc)(const void *, size_t), int (*comparator)(const void* value1, const void* value2));
void netemu_hashtbl_clear(NETEMU_HASHTBL *hashtbl);
void netemu_hashtbl_destroy(NETEMU_HASHTBL *hashtbl);
int netemu_hashtbl_insert(NETEMU_HASHTBL *hashtbl, const void *key, size_t key_len, void *data);
int netemu_hashtbl_remove(NETEMU_HASHTBL *hashtbl, const void *key, size_t key_len);
void *netemu_hashtbl_get(NETEMU_HASHTBL *hashtbl, const void *key, size_t key_len);
int netemu_hashtbl_resize(NETEMU_HASHTBL *hashtbl, hash_size size);

hash_size def_hashfunc_str(const void *key, size_t key_len);
hash_size def_hashfunc_int(const void *key, size_t key_len);
hash_size def_hashfunc_char(const void *key, size_t key_len);
int comparator_int(const void *value1, const void *value2);
int comparator_char(const void *value1, const void *value2);
#endif

