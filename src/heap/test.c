#include "heap.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *f_getline(char *buffer, size_t bytes, FILE *fptr) {
    if (buffer == nullptr || bytes < 2 || fptr == nullptr)
        return nullptr;

    char *ret = fgets(buffer, (int)bytes, fptr);
    if (ret == nullptr)
        return nullptr;

    char *position = strchr(buffer, '\n');
    if (position != nullptr) {
        *position = 0;
    } else {
        int ch = 0;
        while ((ch = fgetc(fptr)) != '\n' && ch != EOF) {
        }
    }
    return ret;
}

int compare(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void destroy(void *ptr) {
    free(ptr);
}

void print(const void *ptr) {
    if (ptr != nullptr)
        printf("\theap value -> [%s]\n", (const char *)ptr);
}

int main(void) {
    Heap heap;
    if (!heap_init(&heap, compare, destroy)) {
        fprintf(stderr, "Error: could not init heap.\n");
        return EXIT_FAILURE;
    }
    printf("Enter 10 strings: \n");
    for (size_t i = 0; i < 10; ++i) {
        char *temp = malloc(sizeof(char) * 256);
        if (temp == nullptr) {
            fprintf(stderr, "Error on allocation.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        printf("String #%zu:", i+1);
        if (!f_getline(temp, 256, stdin)) {
            fprintf(stderr, "Error on getline no data.\n");
            free(temp);
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        if (!heap_insert(&heap, temp)) {
            fprintf(stderr, "Error on insert.\n");
            free(temp);
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
    }

    for (size_t i = 0; i < 10; ++i) {
        void *buffer = nullptr;
        if (!heap_extract(&heap, &buffer)) {
            fprintf(stderr, "Error could not extract from heap.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        printf("Extracted from heap #%zu: %s\n", i+1, (const char *)buffer);
        destroy(buffer);
        printf("Left in Heap: {\n");
        heap_print(&heap, print);
        printf("}\n");
    }
    heap_destroy(&heap);
    return EXIT_SUCCESS;
}
