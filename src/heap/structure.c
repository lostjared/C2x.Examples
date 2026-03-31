#include "heap.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
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

    FILE *fptr = fopen(filename, "rb");
    if (fptr == nullptr) {
        fprintf(stderr, "Error could not open: %s\n", filename);
        return false;
    }
    char player[512];
    while (f_getline(player, 255, fptr)) {
        char *pos = strchr(player, ':');
        if (pos != nullptr) {
            char *p = pos + 1;
            size_t num = strtoull(p, nullptr, 0);
            *pos = 0;
            printf("%s : %zu\n", player, num);
            Score *s = malloc(sizeof(*s));
            if (s == nullptr) {
                fprintf(stderr, "Error on allocation %s.\n", strerror(errno));
                return false;
            }
            strncpy(s->player, player, 255);
            s->player[strlen(player)] = 0;
            s->score = num;
            if (!heap_insert(heap, s)) {
                fprintf(stderr, "Error on allocation: %s\n", strerror(errno));
                heap_destroy(heap);
                return false;
            }
        }
    }
    fclose(fptr);
    return true;
}

bool extract_from_heap(Heap *heap) {
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
                fprintf(stderr, "Error reading file..\n");
                heap_destroy(&heap);
                return EXIT_FAILURE;
            }
            if (!extract_from_heap(&heap)) {
                fprintf(stderr, "Error on extration.\n");
                heap_destroy(&heap);
                return EXIT_FAILURE;
            }
        }
        heap_destroy(&heap);
    }

    return EXIT_SUCCESS;
}
