#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *text;
    struct Node *next, *prev;
};

struct Node *create_node(const char *text) {
    struct Node *n = malloc(sizeof(struct Node));
    if (n == nullptr) {
        fprintf(stderr, "Error on allocation..\n");
        return nullptr;
    }
    n->text = strdup(text);
    if (n->text == nullptr) {
        fprintf(stderr, "Could not allocate text buffer\n");
        free(n);
        return nullptr;
    }
    n->next = n->prev = nullptr;
    return n;
}

void insert_node(struct Node **root, const char *text) {
    struct Node *n = create_node(text);
    if (n == nullptr)
        return;
    if (*root == nullptr) {
        *root = n;
        return;
    }
    struct Node *temp = *root;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = n;
    n->prev = temp;
}

void print_node(struct Node *root) {
    struct Node *temp = root;
    struct Node *last = nullptr;
    printf("Forward: {\n\n");
    while (temp != nullptr) {
        printf("text:  [%s]\n", temp->text);
        last = temp;
        temp = temp->next;
    }
    printf("\n}\n");
    printf("Backwards: {\n");
    while (last != nullptr) {
        printf("text last: [%s]\n", last->text);
        last = last->prev;
    }
    printf("\n}\n");
}

void release_node(struct Node *root) {
    while (root != nullptr) {
        struct Node *n = root->next;
        free(root->text);
        free(root);
        root = n;
    }
}

struct Node *find_node(struct Node *root, const char *text) {
    struct Node *n = root;
    while (n != nullptr) {
        if (strcmp(n->text, text) == 0)
            return n;
        n = n->next;
    }
    return nullptr;
}

int main(void) {

    struct Node *llist = nullptr;
    insert_node(&llist, "One ");
    insert_node(&llist, "itme two");
    insert_node(&llist, "three");

    print_node(llist);
    struct Node *f = find_node(llist, "One ");
    if (f) {
        printf("foudd it");
    }
    release_node(llist);

    return EXIT_SUCCESS;
}
