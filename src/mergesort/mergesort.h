#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool merge_sort_ex(void *data, size_t esize, ssize_t i, ssize_t k, int (*cmp)(const void *, const void *));

#endif
