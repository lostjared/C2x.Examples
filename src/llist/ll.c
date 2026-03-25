#include "ll.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ListNode *list_create_node(void *data, size_t bytes) {
    if (data == nullptr || bytes == 0)
        return nullptr;
    ListNode *n = malloc(sizeof(*n));
    if (n == nullptr)
        return nullptr;
    n->data = malloc(bytes);
    if (n->data == nullptr) {
        free(n);
        return nullptr;
    }
    memcpy(n->data, data, bytes);
    n->bytes = bytes;
    return n;
}

bool list_init(LList **ll) {
    if (ll == nullptr)
        return false;
    LList *ll_n = malloc(sizeof(LList));
    if (ll_n == nullptr)
        return false;
    ll_n->count = 0;
    ll_n->root = nullptr;
    *ll = ll_n;
    return true;
}

void list_free(LList *ll) {
    if (ll == nullptr)
        return;
    ListNode *n = ll->root;
    while (n != nullptr) {
        ListNode *temp = n->next;
        if (n->data != nullptr)
            free(n->data);
        free(n);
        n = temp;
    }
    free(ll);
}

size_t list_count(const LList *ll) {
    if (ll == nullptr)
        return 0;
    return ll->count;
}

bool list_insert(LList *ll, void *data, size_t bytes) {
    if (ll == nullptr || data == nullptr || bytes == 0)
        return false;

    ListNode *n = list_create_node(data, bytes);
    if (n == nullptr)
        return false;

    n->next = ll->root;
    ll->root = n;
    ll->count++;
    return true;
}

void list_print(const LList *ll, void (*print)(const void *)) {
    if (ll == nullptr || print == nullptr)
        return;
    ListNode *n = ll->root;
    while (n != nullptr) {
        print(n->data);
        n = n->next;
    }
}
