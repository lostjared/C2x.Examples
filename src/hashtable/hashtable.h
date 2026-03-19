#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include<stddef.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct Node {
	char *text;
	void *value;
	size_t bytes;
	struct Node *next, *prev;
	void (*cleanup)(void *);
};

static constexpr size_t DEFAULT_TABLE_SIZE = 1024u * 4u;

struct HashTable {
    struct Node **buckets;
    size_t bucket_size;
};

enum HASH_VALUE_RETURN { VALUE_ERROR=0, VALUE_NEW, VALUE_UPDATE };

extern struct Node *create_node(const char *text);
extern struct Node *insert_node(struct Node **root, const char *text);
extern void print_node(const struct Node *root);
extern void release_node(struct Node *root);
extern struct Node *find_node(struct Node *root,const char *text);
extern void sort_node(struct Node *node);
extern size_t hash(const char *key, size_t bucket_size);
extern bool hash_init(struct HashTable *table, size_t bucket_size);
extern struct Node *hash_insert(struct HashTable *table, const char *text);
extern struct Node *hash_lookup(const struct HashTable *table, const char *text);
extern struct Node *hash_set(struct HashTable *table, const char *text, void *value, size_t bytes, void (*cleanup)(void *));
extern enum HASH_VALUE_RETURN hash_set_value(struct HashTable *table, const char *key,  bool (*hash_setvalue)(struct Node *), void (*cleanup)(void *));
extern void hash_remove(struct HashTable *table, const char *text);
extern void hash_print(const struct HashTable *table);
extern void hash_cleanup(struct HashTable *table);
extern size_t hash_count(struct HashTable *table);
extern bool hash_merge(struct HashTable *hash_to, const struct HashTable *hash_from);
extern bool hash_clone_merge(struct HashTable *to, const struct HashTable *from);
extern void cleanup_ptr(void *ptr);
extern char *dup_string(const char *text);
extern struct Node *hash_flat_list(const struct HashTable *table);
#endif
