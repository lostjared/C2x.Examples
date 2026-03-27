#include "oa_hashtable.h"

static void node_reset(struct Node *n) {
    if (n == nullptr)
        return;
    n->text = nullptr;
    n->value = nullptr;
    n->bytes = 0;
    n->cleanup = nullptr;
}

static void node_release(struct Node *n) {
    if (n == nullptr)
        return;
    free(n->text);
    if (n->cleanup != nullptr && n->value != nullptr)
        n->cleanup(n->value);
    node_reset(n);
}

char *dup_string(const char *text) {
    if (text == nullptr)
        return nullptr;

    size_t len = strlen(text) + 1;
    char *out = malloc(len);
    if (out == nullptr)
        return nullptr;

    memcpy(out, text, len);
    return out;
}

void cleanup_ptr(void *ptr) {
    if (ptr != nullptr)
        free(ptr);
}

size_t hash(const char *key, size_t bucket_size) {
    if (key == nullptr || bucket_size == 0)
        return 0;

    size_t h = 1469598103934665603ull;
    while (*key != '\0') {
        h ^= (unsigned char)*key;
        h *= 1099511628211ull;
        ++key;
    }
    return h % bucket_size;
}

static bool hash_should_grow(const struct HashTable *table) {
    if (table == nullptr || table->bucket_size == 0)
        return false;

    size_t used = table->count + table->deleted;
    return used * 100 >= table->bucket_size * 70;
}

static bool hash_reinsert_owned(struct HashTable *table, struct Node *src) {
    size_t start = hash(src->text, table->bucket_size);

    for (size_t probe = 0; probe < table->bucket_size; ++probe) {
        size_t idx = (start + probe) % table->bucket_size;
        struct HashSlot *slot = &table->slots[idx];

        if (slot->state == SLOT_EMPTY) {
            slot->state = SLOT_OCCUPIED;
            slot->node = *src;
            table->count++;
            node_reset(src);
            return true;
        }
    }
    return false;
}

static bool hash_resize(struct HashTable *table, size_t new_size) {
    if (table == nullptr || new_size == 0)
        return false;

    struct HashSlot *old_slots = table->slots;
    size_t old_size = table->bucket_size;

    struct HashSlot *new_slots = calloc(new_size, sizeof(*new_slots));
    if (new_slots == nullptr)
        return false;

    table->slots = new_slots;
    table->bucket_size = new_size;
    table->count = 0;
    table->deleted = 0;

    for (size_t i = 0; i < old_size; ++i) {
        if (old_slots[i].state == SLOT_OCCUPIED) {
            if (!hash_reinsert_owned(table, &old_slots[i].node)) {
                for (size_t j = 0; j < new_size; ++j) {
                    if (table->slots[j].state == SLOT_OCCUPIED)
                        node_release(&table->slots[j].node);
                }
                free(table->slots);
                table->slots = old_slots;
                table->bucket_size = old_size;

                table->count = 0;
                table->deleted = 0;
                for (size_t j = 0; j < old_size; ++j) {
                    if (old_slots[j].state == SLOT_OCCUPIED)
                        table->count++;
                    else if (old_slots[j].state == SLOT_DELETED)
                        table->deleted++;
                }
                return false;
            }
        }
    }

    free(old_slots);
    return true;
}

bool hash_init(struct HashTable *table, size_t bucket_size) {
    if (table == nullptr || bucket_size == 0)
        return false;

    memset(table, 0, sizeof(*table));
    table->slots = calloc(bucket_size, sizeof(*table->slots));
    if (table->slots == nullptr)
        return false;

    table->bucket_size = bucket_size;
    table->count = 0;
    table->deleted = 0;
    return true;
}

void hash_cleanup(struct HashTable *table) {
    if (table == nullptr || table->slots == nullptr || table->bucket_size == 0)
        return;

    for (size_t i = 0; i < table->bucket_size; ++i) {
        if (table->slots[i].state == SLOT_OCCUPIED)
            node_release(&table->slots[i].node);
    }

    free(table->slots);
    table->slots = nullptr;
    table->bucket_size = 0;
    table->count = 0;
    table->deleted = 0;
}

static struct HashSlot *hash_find_slot(const struct HashTable *table, const char *text) {
    if (table == nullptr || table->slots == nullptr || table->bucket_size == 0 || text == nullptr)
        return nullptr;

    size_t start = hash(text, table->bucket_size);

    for (size_t probe = 0; probe < table->bucket_size; ++probe) {
        size_t idx = (start + probe) % table->bucket_size;
        struct HashSlot *slot = (struct HashSlot *)&table->slots[idx];

        if (slot->state == SLOT_EMPTY)
            return nullptr;

        if (slot->state == SLOT_OCCUPIED && strcmp(slot->node.text, text) == 0)
            return slot;
    }

    return nullptr;
}

struct Node *hash_lookup(const struct HashTable *table, const char *text) {
    struct HashSlot *slot = hash_find_slot(table, text);
    if (slot == nullptr)
        return nullptr;
    return &slot->node;
}

struct Node *hash_insert(struct HashTable *table, const char *text) {
    if (table == nullptr || table->slots == nullptr || table->bucket_size == 0 || text == nullptr)
        return nullptr;

    struct HashSlot *existing = hash_find_slot(table, text);
    if (existing != nullptr)
        return &existing->node;

    if (hash_should_grow(table)) {
        size_t new_size = table->bucket_size * 2;
        if (new_size < 8)
            new_size = 8;
        if (!hash_resize(table, new_size))
            return nullptr;
    }

    size_t start = hash(text, table->bucket_size);
    struct HashSlot *first_deleted = nullptr;

    for (size_t probe = 0; probe < table->bucket_size; ++probe) {
        size_t idx = (start + probe) % table->bucket_size;
        struct HashSlot *slot = &table->slots[idx];

        if (slot->state == SLOT_DELETED) {
            if (first_deleted == nullptr)
                first_deleted = slot;
            continue;
        }

        if (slot->state == SLOT_EMPTY) {
            if (first_deleted != nullptr)
                slot = first_deleted;

            slot->node.text = dup_string(text);
            if (slot->node.text == nullptr)
                return nullptr;

            slot->node.value = nullptr;
            slot->node.bytes = 0;
            slot->node.cleanup = nullptr;

            if (slot->state == SLOT_DELETED)
                table->deleted--;

            slot->state = SLOT_OCCUPIED;
            table->count++;
            return &slot->node;
        }
    }

    if (first_deleted != nullptr) {
        first_deleted->node.text = dup_string(text);
        if (first_deleted->node.text == nullptr)
            return nullptr;

        first_deleted->node.value = nullptr;
        first_deleted->node.bytes = 0;
        first_deleted->node.cleanup = nullptr;
        first_deleted->state = SLOT_OCCUPIED;
        table->deleted--;
        table->count++;
        return &first_deleted->node;
    }

    return nullptr;
}

struct Node *hash_set(struct HashTable *table, const char *text, void *value, size_t bytes, void (*cleanup)(void *)) {
    if (table == nullptr || text == nullptr)
        return nullptr;

    struct Node *n = hash_insert(table, text);
    if (n == nullptr)
        return nullptr;

    if (n->cleanup != nullptr && n->value != nullptr)
        n->cleanup(n->value);

    n->value = value;
    n->bytes = bytes;
    n->cleanup = cleanup;
    return n;
}

enum HASH_VALUE_RETURN hash_set_value(struct HashTable *table, const char *key, bool (*hash_setvalue)(struct Node *n), void (*cleanup)(void *)) {
    if (table == nullptr || key == nullptr || hash_setvalue == nullptr)
        return VALUE_ERROR;

    struct Node *n = hash_lookup(table, key);
    if (n == nullptr) {
        n = hash_insert(table, key);
        if (n == nullptr)
            return VALUE_ERROR;
        n->cleanup = cleanup;
        if (hash_setvalue(n))
            return VALUE_NEW;
        return VALUE_ERROR;
    }

    n->cleanup = cleanup;
    if (hash_setvalue(n))
        return VALUE_UPDATE;

    return VALUE_ERROR;
}

void hash_remove(struct HashTable *table, const char *text) {
    if (table == nullptr || table->slots == nullptr || table->bucket_size == 0 || text == nullptr)
        return;

    struct HashSlot *slot = hash_find_slot(table, text);
    if (slot == nullptr)
        return;

    node_release(&slot->node);
    slot->state = SLOT_DELETED;
    table->count--;
    table->deleted++;
}

void hash_print(const struct HashTable *table) {
    if (table == nullptr || table->slots == nullptr || table->bucket_size == 0)
        return;

    printf("Keys: {\n");
    for (size_t i = 0; i < table->bucket_size; ++i) {
        if (table->slots[i].state == SLOT_OCCUPIED)
            printf("\t[%s]\n", table->slots[i].node.text);
    }
    printf("}\n");
}

size_t hash_count(struct HashTable *table) {
    if (table == nullptr)
        return 0;
    return table->count;
}
