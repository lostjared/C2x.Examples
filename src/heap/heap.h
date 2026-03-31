#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t size;
    int (*compare)(const void *, const void *);
    void (*destroy)(void *);
    void **tree;
} Heap;

static inline size_t heap_parent(size_t npos) {
    return ((npos - 1) / 2);
}

static inline size_t heap_left(size_t npos) {
    return ((npos * 2) + 1);
}

static inline size_t heap_right(size_t npos) {
    return ((npos * 2) + 2);
}

extern bool heap_init(Heap *heap, int (*compare)(const void *, const void *), void (*destroy)(void *));
extern void heap_destroy(Heap *heap);
extern bool heap_insert(Heap *heap, void *data);
extern bool heap_extract(Heap *heap, void **data);

#endif
