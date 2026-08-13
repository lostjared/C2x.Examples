#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ex_list_node {
    void *data;
    size_t data_size;
    struct ex_list_node *next;
};

struct List {
    struct ex_list_node *head;
};

struct ex_list_node *ex_list_node_alloc(void *buf, size_t ds) {
    struct ex_list_node *n;
    n = (struct ex_list_node *)malloc(sizeof(struct ex_list_node));
    if (n == nullptr)
        return nullptr;
    n->data = malloc(ds);
    memcpy(n->data, buf, ds);
    n->data_size = ds;
    n->next = nullptr;
    return n;
}

struct ex_list_node *ex_list_node_insert(struct ex_list_node *n, void *buf, size_t ds) {
    struct ex_list_node *node, **empty = &n;
    node = ex_list_node_alloc(buf, ds);
    if (!node)
        return nullptr;
    while (*empty != nullptr) {
        empty = &((*empty)->next);
    }
    *empty = node;
    return node;
}

void echo_list(struct ex_list_node *n) {
    if (n && n->next) {
        echo_list(n->next);
    }
    printf("%d\n", *(int *)n->data);
}

void free_list(struct ex_list_node *n) {
    if (n && n->next) {
        free_list(n->next);
    }
    printf("Free index: %zu\n", (size_t)n);
    free(n->data);
    free(n);
}

struct ex_list_node *read_list(const char *src) {
    FILE *fptr = fopen(src, "r");
    if (!fptr)
        return nullptr;
    struct ex_list_node *n = nullptr, **node = &n;
    while (!feof(fptr)) {
        int value = 0;
        int result = 0;
        if ((result = fscanf(fptr, "%d", &value)) > 0) {
            struct ex_list_node *nn = ex_list_node_alloc(&value, sizeof(value));
            printf("value is: %d\n", value);
            *node = nn;
            node = &(*node)->next;
        } else if (result != EOF) {
            printf("Read error has occoured..\n");
            exit(EXIT_FAILURE);
        } else
            break;
    }
    fclose(fptr);
    return n;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error requires one argument.\n");
        exit(EXIT_FAILURE);
    }
    struct List lst;
    memset(&lst, 0, sizeof(lst));
    lst.head = read_list(argv[1]);
    if (lst.head == nullptr) {
        perror("read_list");
        exit(EXIT_FAILURE);
    }
    echo_list(lst.head);
    free_list(lst.head);
    return EXIT_SUCCESS;
}
