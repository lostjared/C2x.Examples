#include "hashtable.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_sorted_word_table(const struct HashTable *table) {
    if (table == nullptr || table->buckets == nullptr || table->bucket_size == 0)
        return;
    struct Node *flat = hash_flat_list(table);
    if (flat == nullptr) {
        fprintf(stderr, "Error could not create flat list.\n");
        return;
    }
    sort_node(flat);
    printf("Sorted Tokens: {\n");
    const struct Node *temp = flat;
    while (temp != nullptr) {
        struct Node *value = hash_lookup(table, temp->text);
        if (value->value != nullptr)
            printf("\t[%s] = %zu\n", value->text, *(size_t *)value->value);
        temp = temp->next;
    }
    printf("}\n");
    release_node(flat);
}

void print_word_table(const struct HashTable *table) {
    if (table == nullptr || table->buckets == nullptr || table->bucket_size == 0)
        return;
    printf("Tokens: {\n");
    for (size_t i = 0; i < table->bucket_size; ++i) {
        const struct Node *root = table->buckets[i];
        if (root == nullptr)
            continue;
        const struct Node *temp = root;
        while (temp != nullptr) {
            if (temp->value != nullptr)
                printf("\t[%s] = %zu\n", temp->text, *(size_t *)temp->value);
            temp = temp->next;
        }
    }
    printf("}\n");
}

bool count_words(FILE *fptr, size_t *unique_count, size_t *token_count) {
    if (fptr == nullptr || token_count == nullptr || unique_count == nullptr)
        return false;

    *unique_count = 0;
    *token_count = 0;

    static constexpr size_t BUFFER_SIZE = 1024u * 4u;
    char buffer[BUFFER_SIZE];
    size_t count = 0;
    struct HashTable table;
    size_t bytes_read = 0;
    size_t word_size = BUFFER_SIZE;
    size_t index = 0;
    char *word = malloc(word_size + 1);
    if (!word) {
        fprintf(stderr, "Error out of memory\n");
        return false;
    }

    if (!hash_init(&table, DEFAULT_TABLE_SIZE)) {
        fprintf(stderr, "Failed to initialize hash table.\n");
        free(word);
        return false;
    }

    while ((bytes_read = fread(buffer, sizeof(buffer[0]), BUFFER_SIZE, fptr)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char c = (unsigned char)buffer[i];
            if (isalnum(c)) {
                if (index < word_size) {
                    word[index++] = (char)tolower(c);
                } else {
                    word_size *= 2;
                    char *temp = realloc(word, word_size + 1);
                    if (temp == nullptr) {
                        fprintf(stderr, "Error on allocation out of memory \n");
                        free(word);
                        hash_cleanup(&table);
                        return false;
                    }
                    word = temp;
                    word[index++] = (char)tolower(c);
                }
            } else {
                if (index > 0) {
                    word[index] = 0;
                    index = 0;
                    (*token_count)++;
                    struct Node *n = hash_insert(&table, word);
                    if (n == nullptr) {
                        fprintf(stderr, "Allocation failed.\n");
                        free(word);
                        hash_cleanup(&table);
                        return false;
                    }

                    if (n->value == nullptr) {
                        n->cleanup = cleanup_ptr;
                        n->value = malloc(sizeof(size_t));
                        if (n->value == nullptr) {
                            fprintf(stderr, "Allocation failed.\n");
                            free(word);
                            hash_cleanup(&table);
                            return false;
                        }
                        *((size_t *)n->value) = 1;
                        count++;
                    } else {
                        *((size_t *)n->value) += 1;
                    }
                    if (word_size > BUFFER_SIZE) {
                        free(word);
                        word_size = BUFFER_SIZE;
                        word = malloc(word_size + 1);
                        if (word == nullptr) {
                            fprintf(stderr, "Allocation failed.\n");

                            hash_cleanup(&table);
                            return false;
                        }
                    }
                }
            }
        }
    }

    if (ferror(fptr)) {
        fprintf(stderr, "Read error.\n");
        free(word);
        hash_cleanup(&table);
        return false;
    }

    if (index > 0) {
        word[index] = 0;
        (*token_count)++;
        struct Node *n = hash_insert(&table, word);
        if (n == nullptr) {
            fprintf(stderr, "Allocation failed.\n");
            free(word);
            hash_cleanup(&table);
            return false;
        }
        if (n->value == nullptr) {
            n->value = malloc(sizeof(size_t));
            if (n->value == nullptr) {
                fprintf(stderr, "Allocation failed.\n");
                free(word);
                hash_cleanup(&table);
                return false;
            }
            n->cleanup = cleanup_ptr;
            *((size_t *)n->value) = 1;
            count++;
        } else {
            *((size_t *)n->value) += 1;
        }
    }
    free(word);
    print_word_table(&table);
    hash_cleanup(&table);
    *unique_count = count;
    return true;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        size_t num = 0, count = 0;
        if (count_words(stdin, &count, &num)) {
            printf("Contains: %zu tokens, %zu unique\n", num, count);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Error has occurred.\n");
            return EXIT_FAILURE;
        }
    }
    size_t total = 0, unique_total = 0;
    int status = EXIT_SUCCESS;
    size_t errors = 0;

    for (int i = 1; i < argc; ++i) {
        FILE *fptr = fopen(argv[i], "rb");
        if (fptr == nullptr) {
            fprintf(stderr, "Could not open file: %s\n", argv[i]);
            status = EXIT_FAILURE;
            errors++;
            continue;
        }
        size_t num = 0;
        size_t count = 0;
        if (count_words(fptr, &count, &num)) {
            printf("%s Contains: %zu tokens, %zu unique\n", argv[i], num, count);
            total += num;
            unique_total += count;

        } else {
            fprintf(stderr, "Error has occurred.\n");
            status = EXIT_FAILURE;
            errors++;
        }
        fclose(fptr);
    }
    if (argc > 2)
        printf("%zu total tokens, %zu total unique\n", total, unique_total);
    if (errors > 0)
        fprintf(stderr, "Encountered %zu file errors.\n", errors);
    return status;
}
