#include "heap.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static constexpr size_t HEAP_DEFAULT_SIZE = 32;

bool heap_init(Heap *heap, int (*compare)(const void *, const void *), void (*destroy)(void *)) {
    if (heap == nullptr || compare == nullptr)
        return false;
    heap->size = 0;
    heap->compare = compare;
    heap->destroy = destroy;
    heap->capacity = 0;
    heap->tree = nullptr;
    void **tree = malloc(sizeof(void *) * HEAP_DEFAULT_SIZE);
    if (tree == nullptr) {
        heap->compare = nullptr;
        heap->destroy = nullptr;
        return false;
    }
    heap->tree = tree;
    heap->capacity = HEAP_DEFAULT_SIZE;
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
    heap->capacity = 0;
}

void heap_print(const Heap *heap, void (*print)(const void *)) {
    if (heap == nullptr || print == nullptr)
        return;
    for (size_t i = 0; i < heap->size; ++i) {
        print(heap->tree[i]);
    }
}

bool heap_insert(Heap *heap, void *data) {
    if (heap == nullptr || data == nullptr || heap->compare == nullptr)
        return false;

    if (heap->size == heap->capacity) {
        size_t new_cap = 0;
        if (heap->capacity == 0) {
            new_cap = 1;
        } else {
            if (heap->capacity > SIZE_MAX / 2)
                return false;
            new_cap = heap->capacity * 2;
        }

        if (new_cap > SIZE_MAX / sizeof(*heap->tree))
            return false;

        void **temp = realloc(heap->tree, new_cap * sizeof(*heap->tree));
        if (temp == nullptr)
            return false;
        heap->tree = temp;
        heap->capacity = new_cap;
    }
    heap->tree[heap->size] = data;
    size_t ipos = heap->size;
    void *t_temp = nullptr;
    while (ipos > 0) {
        size_t parent = heap_parent(ipos);
        if (heap->compare(heap->tree[parent], heap->tree[ipos]) >= 0)
            break;
        t_temp = heap->tree[parent];
        heap->tree[parent] = heap->tree[ipos];
        heap->tree[ipos] = t_temp;
        ipos = parent;
    }
    ++heap->size;
    return true;
}

bool heap_extract(Heap *heap, void **data) {
    if (heap == nullptr || data == nullptr || heap->compare == nullptr || heap->size == 0)
        return false;
    void *ptemp = heap->tree[0];
    void *s = heap->tree[heap->size - 1];
    if (heap->size == 1) {
        *data = ptemp;
        heap->size = 0;
        return true;
    }
    heap->tree[0] = s;
    --heap->size;
    size_t i = 0;
    while (1) {
        size_t l = heap_left(i);
        size_t r = heap_right(i);
        size_t m = i;
        if (l < heap->size && heap->compare(heap->tree[l], heap->tree[m]) > 0)
            m = l;
        if (r < heap->size && heap->compare(heap->tree[r], heap->tree[m]) > 0)
            m = r;
        if (m == i)
            break;
        void *temp = heap->tree[m];
        heap->tree[m] = heap->tree[i];
        heap->tree[i] = temp;
        i = m;
    }
    *data = ptemp;
    return true;
}

bool heap_peek(const Heap *heap, void **data) {
    if (heap == nullptr || data == nullptr || heap->size == 0 || heap->tree == nullptr)
        return false;
    *data = heap->tree[0];
    return true;
}

bool heap_sort_ex(void *ptr, size_t count, size_t esize, int (*compare)(const void *, const void *)) {
    if (ptr == nullptr || esize == 0 || compare == nullptr)
        return false;
    if (count <= 1)
        return true;

    if (count > SIZE_MAX / esize)
        return false;

    Heap heap;
    if (!heap_init(&heap, compare, nullptr)) {
        return false;
    }
    unsigned char *values = ptr;
    unsigned char *output = malloc(count * esize);
    if (output == nullptr) {
        heap_destroy(&heap);
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        void *e = (values + (i * esize));
        if (!heap_insert(&heap, e)) {
            heap_destroy(&heap);
            free(output);
            return false;
        }
    }
    for (size_t i = 0; i < count; ++i) {
        void *tmp = nullptr;
        if (!heap_extract(&heap, &tmp)) {
            heap_destroy(&heap);
            free(output);
            return false;
        }
        memcpy(output + (i * esize), tmp, esize);
    }
    memcpy(ptr, output, esize * count);
    free(output);
    heap_destroy(&heap);
    return true;
}

static void swap_e(unsigned char *a, unsigned char *b, size_t esize) {
    unsigned char temp[256];
    while (esize > 0) {
        size_t c = (esize < sizeof(temp)) ? esize : sizeof(temp);
        memcpy(temp, a, c);
        memcpy(a, b, c);
        memcpy(b, temp, c);
        a += c;
        b += c;
        esize -= c;
    }
}

static void shift_down(unsigned char *base, size_t start, size_t end, size_t esize, int (*compare)(const void *, const void *)) {
    size_t root = start;
    while ((root * 2) + 1 <= end) {
        size_t c = (root * 2) + 1;
        size_t i = root;
        if (compare(base + (i * esize), base + (c * esize)) > 0) {
            i = c;
        }
        if (c + 1 <= end && compare(base + (i * esize), base + ((c + 1) * esize)) > 0) {
            i = c + 1;
        }
        if (i == root)
            return;
        else {
            swap_e(base + (root * esize), base + (i * esize), esize);
            root = i;
        }
    }
}

bool heap_sort_t(void *ptr, size_t count, size_t esize, int (*compare)(const void *, const void *)) {
    if (ptr == nullptr || esize == 0 || compare == nullptr)
        return false;
    if (count <= 1)
        return true;

    unsigned char *ptr_b = ptr;
    size_t start = (count - 2) / 2;
    while (1) {
        shift_down(ptr_b, start, count - 1, esize, compare);
        if (start == 0)
            break;
        --start;
    }

    size_t end = count - 1;
    while (end > 0) {
        swap_e(ptr_b, ptr_b + (end * esize), esize);
        --end;
        shift_down(ptr_b, 0, end, esize, compare);
    }
    return true;
}