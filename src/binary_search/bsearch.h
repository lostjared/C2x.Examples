#ifndef BSEARCH_H
#define BSEARCH_H
#include <stddef.h>

int bin_search(void *sorted, const void *target, size_t size, size_t esize, int (*cmp)(const void *, const void *));

#endif
