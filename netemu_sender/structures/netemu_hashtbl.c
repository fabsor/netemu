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

#include "netemu_hashtbl.h"

#include <string.h>
#include <stdio.h>

static void *keydup(const void *s, const size_t size) {
	void *b;
	if (!(b = malloc(size)))
		return NULL;
	memcpy(b, s, size);
	return b;
}

hash_size def_hashfunc_str(const void *key, size_t key_len) {
	char* char_key = (char*) key;
	hash_size hash = 0;

	while (*char_key)
		hash += (unsigned char) *char_key++;

	return hash;
}

hash_size def_hashfunc_int(const void *key, size_t key_len) {
	int* int_key = (int*) key;
	return *int_key;
}

hash_size def_hashfunc_char(const void *key, size_t key_len) {
	char* char_key = (char*) key;
	return *char_key;
}

int comparator_int(const void* value1, const void* value2) {
	return (*(int*)value1)-(*(int*)value2);
}

int comparator_char(const void* value1, const void* value2) {
	return (*(char*)value1)-(*(char*)value2);
}

NETEMU_HASHTBL *netemu_hashtbl_create(hash_size size, hash_size(*hashfunc)(
		const void *, size_t), int (*comparator)(const void* value1, const void* value2)) {
	NETEMU_HASHTBL *hashtbl;
	int i;
	if (!(hashtbl = malloc(sizeof(NETEMU_HASHTBL))))
		return NULL;

	if (!(hashtbl->nodes = calloc(size, sizeof(struct hashnode_s*)))) {
		free(hashtbl);
		return NULL;
	}
	hashtbl->keys = netemu_list_new(size,0);
	hashtbl->size = size;

	if (hashfunc)
		hashtbl->hashfunc = hashfunc;
	else
		hashtbl->hashfunc = def_hashfunc_str;

	if (comparator)
		hashtbl->comparator = comparator;
	else
		hashtbl->comparator = comparator_int;

	/* We explicitly set all nodes to NULL. */
	for(i = 0; i < hashtbl->size; i++) {
		hashtbl->nodes[i] = NULL;
	}
	return hashtbl;
}

void netemu_hashtbl_clear(NETEMU_HASHTBL *hashtbl) {
	free(hashtbl->nodes);
	hashtbl->nodes = calloc(hashtbl->size, sizeof(struct hashnode_s*));
}

struct netemu_hashtable_iter  *netemu_hashtbl_iterator_new(NETEMU_HASHTBL *hashtbl) {
	struct netemu_hashtable_iter *iterator;
	iterator = (struct netemu_hashtable_iter*)malloc(sizeof(struct netemu_hashtable_iter));
	iterator->table = hashtbl;
	iterator->hashindex = 0;
	return iterator;
}

void* netemu_hashtbl_iterator_next(struct netemu_hashtable_iter  *iterator) {
	if(iterator->hashindex < iterator->table->keys->count) {
		return NULL;
	}
	//return netemu_hashtbl_get(iterator->table,iterator->table->keys[iterator->hashindex],iterator->table->count);
}

void* netemu_hashtbl_iterator_reset(struct netemu_hashtable_iter *iterator) {
	iterator->hashindex = 0;
	//iterator->currentnode = iterator->table->nodes[0];
}

void netemu_hashtbl_iterator_free(struct netemu_hashtable_iter* iterator) {
	free(iterator);
}

void netemu_hashtbl_destroy(NETEMU_HASHTBL *hashtbl) {
	free(hashtbl->nodes);
	free(hashtbl);
}

int netemu_hashtbl_insert(NETEMU_HASHTBL *hashtbl, const void *key,
		size_t key_len, void *data) {
	struct hashnode_s *node;
	hash_size hash = hashtbl->hashfunc(key, key_len) % hashtbl->size;

	/*	fprintf(stderr, "hashtbl_insert() key=%s, hash=%d, data=%s\n", key, hash, (char*)data);*/

	node = hashtbl->nodes[hash];
	while (node) {
		if (hashtbl->comparator(node->key, key) == 0) {
			node->data = data;
			return 0;
		}
		node = node->next;
	}

	if (!(node = malloc(sizeof(struct hashnode_s))))
		return -1;
	if (!(node->key = keydup(key,key_len))) {
		free(node);
		return -1;
	}
	netemu_list_add(hashtbl->keys, node->key);
	node->data = data;
	node->next = hashtbl->nodes[hash];
	hashtbl->nodes[hash] = node;
	hashtbl->count++;
	return 0;
}

int netemu_hashtbl_remove(NETEMU_HASHTBL *hashtbl, const void *key,
		size_t key_len) {
	struct hashnode_s *node, *prevnode = NULL;
	hash_size hash = hashtbl->hashfunc(key, key_len) % hashtbl->size;
	int index;
	node = hashtbl->nodes[hash];
	while (node) {
		if (hashtbl->comparator(node->key, key) == 0) {
			if (prevnode)
				prevnode->next = node->next;
			else
				hashtbl->nodes[hash] = node->next;
			free(node);
			hashtbl->count--;
			return 0;
		}
		prevnode = node;
		node = node->next;
	}
	index = netemu_list_contains(hashtbl->keys,key);
	netemu_list_remove_at(hashtbl->keys, index);

	return -1;
}

void *netemu_hashtbl_get(NETEMU_HASHTBL *hashtbl, const void *key,
		size_t key_len) {
	struct hashnode_s *node;
	hash_size hash = hashtbl->hashfunc(key, key_len) % hashtbl->size;

	/*	fprintf(stderr, "hashtbl_get() key=%s, hash=%d\n", key, hash);*/

	node = hashtbl->nodes[hash];
	while (node) {
		if (!hashtbl->comparator(node->key, key))
			return node->data;
		node = node->next;
	}

	return NULL;
}

int netemu_hashtbl_resize(NETEMU_HASHTBL *hashtbl, hash_size size) {
	NETEMU_HASHTBL newtbl;
	hash_size n;
	struct hashnode_s *node, *next;

	newtbl.size = size;
	newtbl.hashfunc = hashtbl->hashfunc;

	if (!(newtbl.nodes = calloc(size, sizeof(struct hashnode_s*))))
		return -1;

	for (n = 0; n < hashtbl->size; ++n) {
		for (node = hashtbl->nodes[n]; node; node = next) {
			next = node->next;
			netemu_hashtbl_insert(&newtbl, node->key, node->key_len, node->data);
			netemu_hashtbl_remove(hashtbl, node->key, node->key_len);

		}
	}
	hashtbl->size = newtbl.size;
	hashtbl->nodes = newtbl.nodes;

	return 0;
}

