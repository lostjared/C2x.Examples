#include "heap.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static constexpr int ADDRESS_MAX = 4096;

typedef struct {
    char address[ADDRESS_MAX];
    size_t p;
} Parcel;

int compare(const void *a, const void *b) {
    const Parcel *ia = a;
    const Parcel *ib = b;
    if (ia->p > ib->p)
        return 1;
    else if (ia->p < ib->p)
        return -1;
    return 0;
}

void destroy(void *ptr) {
    free(ptr);
}

char *f_getline(bool *truncated, char *buffer, size_t bytes, FILE *fptr) {
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
        *truncated = true;
        if (feof(fptr))
            return ret;
    }
    position = strchr(buffer, '\r');
    if (position != nullptr) {
        *position = '\0';
    }
    *truncated = false;
    return ret;
}

int main(int argc, char **argv) {

    if (argc <= 1) {
        fprintf(stderr, "No input address files.\n");
        return EXIT_FAILURE;
    }

    Heap heap;
    if (!heap_init(&heap, compare, destroy)) {
        fprintf(stderr, "Error on init of heap.\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        FILE *fptr = fopen(argv[i], "r");
        if (fptr == nullptr) {
            fprintf(stderr, "Error could not open file: %s\n", argv[i]);
            continue;
        }
        char buffer[ADDRESS_MAX];
        bool truncated = false;

        while (f_getline(&truncated, buffer, ADDRESS_MAX, fptr)) {
            if (truncated == true) {
                fprintf(stderr, "Error parcel address to long.\n");
                continue;
            }
            Parcel *p = malloc(sizeof(*p));
            if (p == nullptr) {
                fprintf(stderr, "Error on allocation.\n");
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }
            char *pos = strchr(buffer, ':');
            if (pos == nullptr) {
                fprintf(stderr, "Malformed input expecting ':'.\n");
                destroy(p);
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }
            if (*(pos + 1) == 0) {
                fprintf(stderr, "Error no integer for priority.\n");
                destroy(p);
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }
            char *num = pos + 1;
            *pos = 0;
            char *err = nullptr;
            errno = 0;
            auto pval = strtoul(num, &err, 10);
            if (*err != '\0' || errno == ERANGE) {
                fprintf(stderr, "Error malformed integer.\n");
                destroy(p);
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }
            if (pval == 0 || pval > 1000) {
                fprintf(stderr, "Error invalid range for priority.\n");
                destroy(p);
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }

            p->p = pval;
            strncpy(p->address, buffer, sizeof(p->address) - 1);
            if (!heap_insert(&heap, p)) {
                fprintf(stderr, "Error heap insert.\n");
                destroy(p);
                heap_destroy(&heap);
                fclose(fptr);
                return EXIT_FAILURE;
            }
        }
        fclose(fptr);
    }
    while (heap.size > 0) {
        void *e = nullptr;
        if (!heap_extract(&heap, &e)) {
            fprintf(stderr, "Error on extraction.\n");
            heap_destroy(&heap);
            return EXIT_FAILURE;
        }
        Parcel *p = e;
        printf("Delivery to: %s priority: %zu\n", p->address, p->p);
        destroy(p);
    }
    heap_destroy(&heap);
    return EXIT_SUCCESS;
}
