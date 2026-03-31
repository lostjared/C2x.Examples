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

    position = strchr(buffer, '\r');
    if (position != nullptr) {
        *position = '\0';
    }
    return ret;
}

static constexpr size_t BUFFER_SIZE = 4096;

int compare(const void *a, const void *b) {
    int result = strcmp((const char *)a, (const char *)b);
    if (result > 0)
        return -1;
    else if (result < 0)
        return 1;
    return 0;
}

void destroy(void *ptr) {
    free(ptr);
}

bool echo_empty_out(Heap *heap) {
    if (heap == nullptr)
        return false;
    while (heap->size > 0) {
        void *temp = nullptr;
        if (!heap_extract(heap, &temp)) {
            fprintf(stderr, "Error on extract.\n");
            return false;
        }
        printf("%s\n", (const char *)temp);
        free(temp);
    }
    return true;
}

bool sort_input(FILE *fptr, Heap *heap) {
    if (heap == nullptr || fptr == nullptr)
        return false;
    char buffer[BUFFER_SIZE];
    while (f_getline(buffer, BUFFER_SIZE, fptr)) {
        char *temp = nullptr;
        size_t buf_size = strlen(buffer);
        temp = malloc(buf_size + 1);
        if (temp == nullptr) {
            return false;
        }
        memcpy(temp, buffer, buf_size + 1);
        if (!heap_insert(heap, temp)) {
            fprintf(stderr, "Error on insertion.\n");
            free(temp);
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    Heap heap;
    if (!heap_init(&heap, compare, destroy)) {
        fprintf(stderr, "Error on init of heap.\n");
        return EXIT_FAILURE;
    }
    if (argc == 1) {
        if (!sort_input(stdin, &heap)) {
            fprintf(stderr, "Error on input: <stdin>.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE *fptr;
            fptr = fopen(argv[i], "r");
            if (fptr == nullptr) {
                fprintf(stderr, "Error could not open file: %s.\n", argv[i]);
                continue;
            }

            if (!sort_input(fptr, &heap)) {
                fprintf(stderr, "Error on input: %s\n", argv[i]);
                fclose(fptr);
                heap_destroy(&heap);
                return EXIT_FAILURE;
            }

            fclose(fptr);
        }
    }
    if(!echo_empty_out(&heap)) {
	    fprintf(stderr, "Error on listing heap.\n");
	    heap_destroy(&heap);
	    return EXIT_FAILURE;
    }
    heap_destroy(&heap);
    return EXIT_SUCCESS;
}
