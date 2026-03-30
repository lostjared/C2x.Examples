#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int64_t size;
    int (*compare)(const void *, const void *);
    void (*destroy)(void *);
    void **tree;
} Heap;

static inline int64_t heap_parent(int64_t npos) {
    return ((npos - 1) / 2);
}

static inline int64_t heap_left(int64_t npos) {
    return ((npos * 2) + 1);
}

static inline int64_t heap_right(int64_t npos) {
    return ((npos * 2) + 2);
}

extern void heap_init(Heap *heap, int (*compare)(const void *, const void *), void (*destroy)(void *));
extern void heap_destroy(Heap *heap);
extern bool heap_insert(Heap *heap, void *data);
extern bool heap_extract(Heap *heap, void **data);

#endif
