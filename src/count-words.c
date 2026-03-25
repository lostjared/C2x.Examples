#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *text;
    size_t count;
    struct Node *left, *right;
};

[[nodiscard]] struct Node *create_node(const char *text) {
    struct Node *n = malloc(sizeof(struct Node));
    if (n == nullptr) {
        fprintf(stderr, "Error on allocation\n");
        return nullptr;
    }
    n->text = strdup(text);
    if (n->text == nullptr) {
        fprintf(stderr, "Error on allocation.\n");
        free(n);
        return nullptr;
    }
    n->count = 1;
    n->left = nullptr;
    n->right = nullptr;
    return n;
}

bool insert_node(struct Node **root, const char *text) {
    struct Node **current = root;
    while (*current != nullptr) {
        int compare = strcmp(text, (*current)->text);
        if (compare < 0) {
            current = &(*current)->left;
        } else if (compare > 0) {
            current = &(*current)->right;
        } else {
            (*current)->count++;
            return true;
        }
    }
    *current = create_node(text);
    if (*current == nullptr) {
        return false;
    }
    return true;
}

void release_node(struct Node *node) {
    struct Node *current = node;
    while (current != nullptr) {
        if (current->left != nullptr) {
            struct Node *left_ = current->left;
            current->left = left_->right;
            left_->right = current;
            current = left_;
        } else {
            struct Node *next = current->right;
            free(current->text);
            free(current);
            current = next;
        }
    }
}

size_t print_node(const struct Node *root) {
    if (root == nullptr)
        return 0;
    size_t cap_size = 128;
    size_t top = 0;
    size_t total = 0;
    const struct Node **stack = malloc(cap_size * sizeof(struct Node *));
    if (stack == nullptr) {
        fprintf(stderr, "Error on allocation\n");
        return 0;
    }
    const struct Node *curr = root;
    while (curr != nullptr || top > 0) {
        while (curr != nullptr) {
            if (top >= cap_size) {
                cap_size *= 2;
                const struct Node **temp = realloc(stack, cap_size * sizeof(struct Node *));
                if (temp == nullptr) {
                    fprintf(stderr, "Out of memory");
                    free(stack);
                    return 0;
                }
                stack = temp;
            }
            stack[top++] = curr;
            curr = curr->left;
        }
        curr = stack[--top];
        printf("%s: %zu\n", curr->text, curr->count);
        total++;
        curr = curr->right;
    }
    free(stack);
    return total;
}

size_t count_words(size_t *total, FILE *fptr) {
    struct Node *root = nullptr;
    static constexpr int CHUNK_SIZE = 16384;
    static constexpr int MAX_WORD = 1024;
    char buffer[CHUNK_SIZE];
    size_t index = 0;
    size_t bytes_read = 0;
    size_t counted = 0;
    size_t word_size = MAX_WORD;
    char *word = malloc(word_size + 1);
    if (word == nullptr) {
        fprintf(stderr, "Error on allocation\n");
        return 0;
    }
    while ((bytes_read = fread(buffer, sizeof(char), CHUNK_SIZE, fptr)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char c = (unsigned char)buffer[i];
            if (isalnum(c)) {
                if (index < word_size - 1) {
                    word[index++] = (char)tolower(c);
                } else {
                    word_size *= 2;
                    char *temp = realloc(word, word_size + 1);
                    if (temp == nullptr) {
                        fprintf(stderr, "Error on allocation\n");
                        free(word);
                        release_node(root);
                        return 0;
                    }
                    word = temp;
                    word[index++] = (char)tolower(c);
                }
            } else {
                if (index > 0) {
                    word[index] = 0;
                    if (!insert_node(&root, word)) {
                        free(word);
                        release_node(root);
                        return 0;
                    }
                    if (index > MAX_WORD) {
                        free(word);
                        word_size = MAX_WORD;
                        word = malloc(word_size + 1);
                        if (word == nullptr) {
                            fprintf(stderr, "Error on allocation.\n");
                            release_node(root);
                            return 0;
                        }
                    }
                    counted++;
                    index = 0;
                }
            }
        }
    }
    if (index > 0) {
        word[index] = 0;
        if (!insert_node(&root, word)) {
            free(word);
            release_node(root);
            return 0;
        }
        counted++;
    }
    free(word);
    *total = print_node(root);
    release_node(root);
    return counted;
}

void proc_count_words(const char *restrict filename) {
    FILE *fptr = fopen(filename, "rb");
    if (fptr == nullptr) {
        fprintf(stderr, "Error could not open: %s\n", filename);
        return;
    }
    printf("File: %s\n", filename);
    size_t total = 0;
    size_t count = count_words(&total, fptr);
    printf("contains: %zu tokens, unique: %zu\n", count, total);
    fclose(fptr);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "%s <files1> <file2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; ++i) {
        proc_count_words(argv[i]);
    }
    return EXIT_SUCCESS;
}
