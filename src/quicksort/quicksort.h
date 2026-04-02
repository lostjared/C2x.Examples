#ifndef SORT_H
#define SORT_H

#include <stddef.h>

extern void insertion_sort(void *base, size_t count, size_t size, int (*cmp)(const void *, const void *));
extern bool quick_sort(void *data, size_t size, size_t esize, int i, int k, int (*compare)(const void *, const void *));

#endif
