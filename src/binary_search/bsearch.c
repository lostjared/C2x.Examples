#include "bsearch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bin_search(void *sorted, const void *target, size_t size, size_t esize, int (*cmp)(const void *, const void *)) {
    int l = 0, m = 0, r = 0;
    r = (int)size - 1;
    while (l <= r) {
        m = (l + r) / 2;
        switch (cmp((char *)sorted + (size_t)(esize * (size_t)m), target)) {
        case -1:
            l = m + 1;
            break;
        case 1:
            r = m - 1;
            break;
        case 0:
            return m;
        }
    }
    return -1;
}
