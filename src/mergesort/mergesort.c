#include "mergesort.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool merge(void *data, size_t esize, size_t i, size_t z, size_t k, int (*cmp)(const void *, const void *)) {
    if (data == nullptr || esize == 0 || cmp == nullptr || i > z)
        return false;
    char *arr = data, *m_ptr;
    size_t i_, z_, m_;
    i_ = i;
    z_ = z + 1;
    m_ = 0;
    m_ptr = malloc(esize * (size_t)((k - i) + 1));
    if (m_ptr == nullptr)
        return false;
    while (i_ <= z && z_ <= k) {
        if (cmp(&arr[i_ * esize], &arr[z_ * esize]) <= 0) {
            memcpy(&m_ptr[m_ * esize], &arr[i_ * esize], esize);
            i_++;
        } else {
            memcpy(&m_ptr[m_ * esize], &arr[z_ * esize], esize);
            z_++;
        }
        m_++;
    }
    if (i_ <= z) {
        memcpy(&m_ptr[m_ * esize], &arr[i_ * esize], esize * ((z - i_) + 1));
    } else if (z_ <= k) {
        memcpy(&m_ptr[m_ * esize], &arr[z_ * esize], esize * ((k - z_) + 1));
    }
    memcpy(&arr[i * esize], m_ptr, esize * ((k - i) + 1));
    free(m_ptr);
    return true;
}

bool merge_sort_ex(void *data, size_t esize, ssize_t i, ssize_t k, int (*cmp)(const void *, const void *)) {
 	if(data == nullptr || esize == 0  || cmp == nullptr)
		return false;
	if (i < k) {
        ssize_t z = i + (k - i) / 2;
        if (merge_sort_ex(data, esize, i, z, cmp) == false)
            return false;
        if (merge_sort_ex(data, esize, z + 1, k, cmp) == false)
            return false;
        if (merge(data, esize, (size_t)i, (size_t)z, (size_t)k, cmp) == false)
            return false;
    }
    return true;
}
