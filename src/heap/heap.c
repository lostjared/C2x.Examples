#include "heap.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool heap_init(Heap *heap, int (*compare)(const void *, const void *), void (*destroy)(void *)) {
    if (heap == nullptr || compare == nullptr)
        return false;
    heap->size = 0;
    heap->compare = compare;
    heap->destroy = destroy;
    heap->tree = nullptr;
    return true;
}

void heap_destroy(Heap *heap) {
    if (heap == nullptr)
        return;
    if (heap->destroy != nullptr) {
        for (size_t i = 0; i < heap->size; i++) {
            heap->destroy(heap->tree[i]);
        }
    }
    free(heap->tree);
    heap->tree = nullptr;
    heap->compare = nullptr;
    heap->destroy = nullptr;
    heap->size = 0;
}

bool heap_insert(Heap *heap, void *data) {
    if (heap == nullptr || data == nullptr || heap->compare == nullptr)
        return false;
    void **temp = nullptr;
    if ((temp = realloc(heap->tree, (heap->size + 1) * sizeof(void *))) == nullptr) {
        return false;
    } else {
        heap->tree = temp;
    }
    heap->tree[heap->size] = data;
    size_t ipos = heap->size;
    void *t_temp = nullptr;
    while (ipos > 0) {
         size_t parent = heap_parent(ipos);
	 if(heap->compare(heap->tree[parent], heap->tree[ipos]) >= 0)
		break;
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
    if (heap == nullptr || data == nullptr || heap->compare == nullptr)
        return false;
    if (heap->size == 0)
        return false;
    void *ptemp = heap->tree[0];
    void *s = heap->tree[heap->size - 1];
    if (heap->size == 1) {
	*data = ptemp;
        free(heap->tree);
        heap->tree = nullptr;
        heap->size = 0;
        return true;
    }
    heap->tree[0] = s;
    --heap->size;

    size_t i = 0;

    while (1) {
        size_t l = heap_left(i);
        size_t r = heap_right(i);
	size_t m =  i;
        if (l < heap->size && heap->compare(heap->tree[l], heap->tree[i]) > 0) 
            m = l;
	if (r < heap->size && heap->compare(heap->tree[r], heap->tree[m]) > 0) 
	    m = r;
	if(m == i)
		break;
        void *temp = heap->tree[m];
        heap->tree[m] = heap->tree[i];
        heap->tree[i] = temp;
        i = m;
    }

    void **t = realloc(heap->tree, heap->size * sizeof(void *));
    if(t != nullptr)
    	heap->tree = t;

    *data = ptemp;

    return true;
}
