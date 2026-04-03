#include"bintree.h"

[[nodiscard]] Node *createNode(const void *restrict data, const size_t bytes) {
    if (data == nullptr)
        return nullptr;
    if (bytes > SIZE_MAX - sizeof(Node))
        return nullptr;

    Node *n = malloc(sizeof(Node));
    if (n == nullptr) {
        fprintf(stderr, "Error on allocation\n");
        return nullptr;
    }
    n->left = n->right = nullptr;
    n->data = malloc(bytes);
    if (n->data == nullptr) {
        fprintf(stderr, "Error on allocation\n");
        free(n);
        return nullptr;
    }
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
            free(current->data);
            free(current);
            current = next;
        }
    }
}

/*void printNodes(const Node *root) {
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
}*/

bool insertNode(Node **root, const void *data, size_t bytes, cmp c) {
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

[[nodiscard]] Node *findNode(Node *root, const void *data, cmp c) {
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


