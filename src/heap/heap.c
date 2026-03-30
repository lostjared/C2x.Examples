#include "heap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void heap_init(Heap *heap, int (*compare)(const void *, const void *), void (*destroy)(void *)) {
    if (heap == nullptr || compare == nullptr)
        return;
    heap->size = 0;
    heap->compare = compare;
    heap->destroy = destroy;
    heap->tree = nullptr;
}

void heap_destroy(Heap *heap) {
    if (heap == nullptr)
        return;
    if (heap->destroy != nullptr) {
        for (int64_t i = 0; i < heap->size; i++) {
            heap->destroy(heap->tree[i]);
        }
    }
    free(heap->tree);
    heap->tree = nullptr;
}

bool heap_insert(Heap *heap, void *data) {
    if (heap == nullptr || data == nullptr)
        return false;
    void **temp = nullptr;
    if ((temp = (void **)realloc(heap->tree, ((size_t)heap->size + 1) * sizeof(void *))) == nullptr) {
        return false;
    } else {
        heap->tree = temp;
    }
    heap->tree[heap->size] = data;
    int64_t ipos = heap->size;
    int64_t parent = heap_parent(ipos);
    void *t_temp = nullptr;
    while (ipos > 0 && heap->compare(heap->tree[parent], heap->tree[ipos]) < 0) {
        t_temp = heap->tree[parent];
        heap->tree[parent] = heap->tree[ipos];
        heap->tree[ipos] = t_temp;
        ipos = parent;
        parent = heap_parent(ipos);
    }
    ++heap->size;
    return true;
}

bool heap_extract(Heap *heap, void **data) {

    if (heap == nullptr || data == nullptr)
        return false;
    if (heap->size == 0)
        return false;

    *data = heap->tree[0];

    void **t = nullptr;
    void *s = heap->tree[heap->size - 1];

    if (heap->size - 1 > 0) {
        if ((t = (void **)realloc(heap->tree, ((size_t)heap->size - 1) * sizeof(void *))) == nullptr) {
            return false;
        } else {
            heap->tree = t;
        }
        --heap->size;
    } else {
        free(heap->tree);
        heap->tree = nullptr;
        heap->size = 0;
        return true;
    }

    heap->tree[0] = s;

    int64_t i = 0, l = heap_left(i), r = heap_right(i), m = 0;
    while (1) {
        l = heap_left(i);
        r = heap_right(i);
        if (l < heap->size && heap->compare(heap->tree[l], heap->tree[i]) > 0) {
            m = l;
        } else {
            m = i;
        }
        if (r < heap->size && heap->compare(heap->tree[r], heap->tree[m]) > 0) {
            m = r;
        }
        if (m == i) {
            break;
        } else {
            void *temp = heap->tree[m];
            heap->tree[m] = heap->tree[i];
            heap->tree[i] = temp;
            i = m;
        }
    }

    return true;
}
