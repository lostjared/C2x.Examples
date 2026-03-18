#ifndef __HASH_TABLE__H_
#define __HASH_TABLE__H_

#include<stddef.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct Node {
	char *text;
	void *value;
	struct Node *next, *prev;
	void (*cleanup)(void *);
};

static constexpr size_t DEFAULT_TABLE_SIZE = 1024u * 4u;

struct HashTable {
    struct Node **buckets;
    size_t bucket_size;
};

extern struct Node *create_node(const char *text);
extern struct Node *insert_node(struct Node **root, const char *text);
extern void print_node(struct Node *root);
extern void release_node(struct Node *root);
extern struct Node *find_node(struct Node *root,const char *text);
extern size_t hash(const char *key, size_t bucket_size);
extern void hash_init(struct HashTable *table, size_t bucket_size);
extern struct Node *hash_insert(struct HashTable *table, const char *text);
extern struct Node *hash_lookup(struct HashTable *table, const char *text);
extern void hash_print(struct HashTable *table);
extern void hash_cleanup(struct HashTable *table);
extern void cleanup_ptr(void *ptr);
extern char *dup_string(const char *text);
#endif
