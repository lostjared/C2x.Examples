#include "heap.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char player[256];
    size_t score;
} Score;

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

int compare(const void *a, const void *b) {
    const Score *ia = (const Score *)a;
    const Score *ib = (const Score *)b;
    if (ia->score < ib->score)
        return -1;
    else if (ia->score > ib->score)
        return 1;
    return 0;
}

void destroy(void *ptr) {
    free(ptr);
}

bool read_file(const char *restrict filename, Heap *heap) {
    if (filename == nullptr || heap == nullptr || heap->compare == nullptr)
        return false;

    FILE *fptr = fopen(filename, "r");
    if (fptr == nullptr) {
        fprintf(stderr, "Error could not open: %s\n", filename);
        return false;
    }
    char line_input[512];
    while (f_getline(line_input, sizeof(line_input), fptr)) {
        char *pos = strchr(line_input, ':');

        if (pos == nullptr) {
            fprintf(stderr, "Malformed input, use format PlayerName:Score\n");
            fclose(fptr);
            return false;
        }

        char *p = pos + 1;
        char *e = nullptr;
        errno = 0;
        auto value = strtoull(p, &e, 10);
        *pos = 0;
        if (e == p || *e != '\0' || errno == ERANGE || value > SIZE_MAX) {
            fprintf(stderr, "Error on conversion in source file: %s.\n", filename);
            fclose(fptr);
            return false;
        }
        size_t num = (size_t)value;
        Score *s = malloc(sizeof(*s));
        if (s == nullptr) {
            fprintf(stderr, "Error on allocation.\n");
            fclose(fptr);
            return false;
        }
        strncpy(s->player, line_input, sizeof(s->player) - 1);
        s->player[sizeof(s->player) - 1] = 0;
        s->score = num;
        if (!heap_insert(heap, s)) {
            fprintf(stderr, "Error on allocation.\n");
            free(s);
            fclose(fptr);
            return false;
        }
    }
    fclose(fptr);
    return true;
}

bool extract_from_heap(Heap *heap) {

    if (heap == nullptr)
        return false;

    while (heap->size > 0) {
        void *temp = nullptr;
        if (!heap_extract(heap, &temp)) {
            return false;
        }
        Score *s = temp;
        printf("User name: %s, score: %zu\n", s->player, s->score);
        destroy(s);
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        Heap heap;
        if (!heap_init(&heap, compare, destroy)) {
            fprintf(stderr, "Error could not create heap.\n");
            return EXIT_FAILURE;
        }
        for (int i = 1; i < argc; ++i) {
            if (!read_file(argv[i], &heap)) {
                fprintf(stderr, "Error reading file.\n");
                heap_destroy(&heap);
                return EXIT_FAILURE;
            }
        }
        if (!extract_from_heap(&heap)) {
            fprintf(stderr, "Error on extraction.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }

        heap_destroy(&heap);
    } else {
        fprintf(stderr, "No file names provided. Nothing to do.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
