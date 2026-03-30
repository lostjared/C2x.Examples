#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char *data;
    size_t bytes;
    struct node *left, *right;
} Node;

typedef int cmp(const char *, const char *);

int compare(const char *a, const char *b) {
    if (a == nullptr && b == nullptr)
        return 0;
    if (a == nullptr)
        return -1;
    if (b == nullptr)
        return 1;
    return strcmp(a, b);
}

[[nodiscard]] Node *createNode(const char *restrict data, const size_t bytes) {
    if (data == nullptr)
        return nullptr;
    if (bytes > SIZE_MAX - sizeof(Node))
        return nullptr;

    Node *n = malloc(sizeof(Node) + bytes);
    if (!n) {
        fprintf(stderr, "Error on allocation\n");
        return nullptr;
    }
    n->left = n->right = nullptr;
    n->data = (char *)n + sizeof(*n);
    memcpy(n->data, data, bytes);
    n->bytes = bytes;
    return n;
}

void freeNodes(Node *n) {
    Node *current = n;
    while (current != nullptr) {
        if (current->left != nullptr) {
            Node *left_ = current->left;
            current->left = left_->right;
            left_->right = current;
            current = left_;
        } else {
            Node *next = current->right;
            free(current);
            current = next;
        }
    }
}

void printNodes(const Node *root) {
    if (root == nullptr)
        return;
    size_t cap_size = 128;
    size_t top = 0;
    const Node **stack = malloc(cap_size * sizeof(const Node *));
    if (!stack) {
        fprintf(stderr, "Error on allocation.\n");
        return;
    }
    const Node *curr = root;
    while (curr != nullptr || top > 0) {
        while (curr != nullptr) {
            if (top >= cap_size) {
                cap_size *= 2;
                const Node **temp = realloc(stack, cap_size * sizeof(const Node *));
                if (!temp) {
                    fprintf(stderr, "Fatal out of memory\n");
                    free(stack);
                    return;
                }
                stack = temp;
            }
            stack[top++] = curr;
            curr = curr->left;
        }
        curr = stack[--top];
        printf("Data: %s\n", curr->data);
        curr = curr->right;
    }
    free(stack);
}

bool insertNode(Node **root, const char *data, size_t bytes, cmp c) {
    Node **current = root;
    while (*current != nullptr) {
        int res = c(data, (*current)->data);
        if (res < 0) {
            current = &(*current)->left;
        } else if (res > 0) {
            current = &(*current)->right;
        } else {
            return true;
        }
    }
    Node *temp = createNode(data, bytes);
    if (temp == nullptr) {
        return false;
    }
    *current = temp;
    return true;
}

[[nodiscard]] Node *findNode(Node *root, const char *data, cmp c) {
    Node *current = root;
    while (current != nullptr) {
        int res = c(data, current->data);
        if (res < 0) {
            current = current->left;
        } else if (res > 0) {
            current = current->right;
        } else {
            return current;
        }
    }
    return nullptr;
}

int main(void) {
    Node *root = nullptr;
    static constexpr unsigned int BUFFER_SIZE = 4096;
    while (true) {
        char buffer[BUFFER_SIZE];
        printf("Enter text (exit to quit): ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == nullptr)
            break;

        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "exit") == 0)
            break;
        if (!insertNode(&root, buffer, strlen(buffer) + 1, compare)) {
            fprintf(stderr, "Node insertion failed.\n");
            freeNodes(root);
            return EXIT_FAILURE;
        }
    }
    Node *f = findNode(root, "Hello", compare);
    if (f != nullptr) {
        printf("%s World!\n", f->data);
    }
    printNodes(root);
    freeNodes(root);
    return 0;
}
