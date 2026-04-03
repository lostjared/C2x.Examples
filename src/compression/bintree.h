#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    void *data;
    size_t bytes;
    struct node *left, *right;
} Node;

typedef int (*cmp)(const void *, const void *);
[[nodiscard]] Node *createNode(const void *restrict data, const size_t bytes);
void freeNodes(Node *n);
bool insertNode(Node **root, const void *data, size_t bytes, cmp c) ;
[[nodiscard]] Node *findNode(Node *root, const void *data, cmp c);

#endif

