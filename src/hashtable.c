#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *text;
    void *value;
    struct Node *next, *prev;
    void (*cleanup)(void *);
};

struct Node *create_node(const char *text) {
    if (text == nullptr)
        return nullptr;

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
    n->value = nullptr;
    n->cleanup = nullptr;
    return n;
}

struct Node *insert_node(struct Node **root, const char *text) {
    if (text == nullptr)
        return nullptr;

    struct Node *n = create_node(text);
    if (n == nullptr)
        return nullptr;
    n->next = *root;
    if (*root != nullptr) {
        (*root)->prev = n;
    }
    *root = n;
    return n;
}

void print_node(struct Node *root) {
    if (root == nullptr)
        return;

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
        if (root->cleanup != nullptr && root->value != nullptr)
            root->cleanup(root->value);
        free(root);
        root = n;
    }
}

struct Node *find_node(struct Node *root, const char *text) {
    if (text == nullptr)
        return nullptr;

    struct Node *n = root;
    while (n != nullptr) {
        if (strcmp(n->text, text) == 0)
            return n;
        n = n->next;
    }
    return nullptr;
}

static constexpr size_t TABLE_SIZE = 1024 * 4;
struct Node *buckets[TABLE_SIZE] = {};

size_t hash(const char *key) {
    if (key == nullptr)
        return 0;

    size_t h = 1469598103934665603ull;
    while (*key != '\0') {
        h ^= (unsigned char)*key;
        h *= 1099511628211ull;
        ++key;
    }
    return h % TABLE_SIZE;
}

struct Node *hash_insert(const char *text) {
    if (text == nullptr)
        return nullptr;

    size_t key = hash(text);
    struct Node *existing = find_node(buckets[key], text);
    if (existing != nullptr)
        return existing;
    return insert_node(&buckets[key], text);
}

struct Node *hash_lookup(const char *text) {
    if (text == nullptr)
        return nullptr;

    size_t key = hash(text);
    struct Node **n = &buckets[key];
    return find_node(*n, text);
}

void hash_cleanup(void) {
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
        release_node(buckets[i]);
        buckets[i] = nullptr;
    }
}

void cleanup(void *ptr) {
    if (ptr != nullptr)
        free(ptr);
}

void cleanup_stack(void *) {}

int main(void) {

    struct Node *value = hash_insert("key ");
    if (value == nullptr) {
        fprintf(stderr, "Error hash value not inserted.\n");
        hash_cleanup();
        return EXIT_FAILURE;
    }
    value->value = strdup("Value 1");
    value->cleanup = cleanup;

    if (value->value == nullptr) {
        fprintf(stderr, "Error on allocation.\n");
        hash_cleanup();
        return EXIT_FAILURE;
    }
    struct Node *look = hash_lookup("key ");
    if (look != nullptr) {
        printf("value is; %s\n", (const char *)look->value);
    } else {
        printf("could not be found.\n");
    }

    struct Node *test_value = hash_insert("key value");
    if (test_value == nullptr) {
        fprintf(stderr, "Could not find key");
        hash_cleanup();
        return EXIT_FAILURE;
    }
    test_value->value = malloc(sizeof(int));
    test_value->cleanup = cleanup;
    if (test_value->value == nullptr) {
        fprintf(stderr, "Error on allocatoin.\n");
        hash_cleanup();
        return EXIT_FAILURE;
    }
    *((int *)test_value->value) = 10;
    struct Node *i_value = hash_lookup("key value");
    if (i_value == nullptr) {
        fprintf(stderr, "Hash key not found.\n");
    } else {
        fprintf(stderr, "Value is: %d\n", *((int *)i_value->value));
    }

    struct Node *stack_value = hash_insert("integer");
    if (stack_value == nullptr) {
        fprintf(stderr, "Insertion error\n");
        hash_cleanup();
        return EXIT_FAILURE;
    }
    int x = 25;
    stack_value->value = &x;
    stack_value->cleanup = cleanup_stack;
    struct Node *value_lookup = hash_lookup("integer");
    if (value_lookup != nullptr) {
        printf("integer value: %d\n", *((int *)value_lookup->value));
    }
    hash_cleanup();
    return EXIT_SUCCESS;
}
