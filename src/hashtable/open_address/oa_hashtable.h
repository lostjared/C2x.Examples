#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum SlotState {
    SLOT_EMPTY = 0,
    SLOT_OCCUPIED,
    SLOT_DELETED
};

struct Node {
    char *text;
    void *value;
    size_t bytes;
    void (*cleanup)(void *);
};

struct HashSlot {
    enum SlotState state;
    struct Node node;
};

static constexpr size_t DEFAULT_TABLE_SIZE = 1024u * 4u;

struct HashTable {
    struct HashSlot *slots;
    size_t bucket_size;
    size_t count;
    size_t deleted;
};

enum HASH_VALUE_RETURN {
    VALUE_ERROR = 0,
    VALUE_NEW,
    VALUE_UPDATE
};

extern char *dup_string(const char *text);
extern void cleanup_ptr(void *ptr);

extern size_t hash(const char *key, size_t bucket_size);
extern bool hash_init(struct HashTable *table, size_t bucket_size);
extern void hash_cleanup(struct HashTable *table);

extern struct Node *hash_lookup(const struct HashTable *table, const char *text);
extern struct Node *hash_insert(struct HashTable *table, const char *text);
extern struct Node *hash_set(struct HashTable *table, const char *text, void *value, size_t bytes, void (*cleanup)(void *));
extern enum HASH_VALUE_RETURN hash_set_value(struct HashTable *table, const char *key, bool (*hash_setvalue)(struct Node *), void (*cleanup)(void *));
extern void hash_remove(struct HashTable *table, const char *text);

extern void hash_print(const struct HashTable *table);
extern size_t hash_count(struct HashTable *table);

#endif
