#include "bsearch.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int spell(char **dict, size_t ssize, size_t arr_size, const char *word);

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

static int compare_str(const void *a, const void *b) {
    int result = strcmp(*(const char **)a, *(const char **)b);

    if (result > 0)
        return 1;
    else if (result < 0)
        return -1;
    return 0;
}

int spell(char **dict, size_t ssize, size_t arr_size, const char *word) {

    if (bin_search(dict, &word, arr_size, ssize, compare_str) >= 0)
        return 1;

    return 0;
}

void free_buffer(char **buffer, size_t arr_size) {
    for (size_t i = 0; i < arr_size; ++i) {
        free(buffer[i]);
    }
    free(buffer);
}

static constexpr size_t MAX_LINE = 4096;

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error requires 3 arguments.\n /usr/share/dict/words textfile.txt\n");
        return EXIT_FAILURE;
    }
    char **buffer = nullptr;
    FILE *fptr = fopen(argv[1], "r");
    if (fptr == nullptr) {
        fprintf(stderr, "Error could not open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    char buf[MAX_LINE];
    size_t arr_size = 1024;
    size_t index = 0;
    buffer = malloc(sizeof(char *) * arr_size);
    for (size_t i = 0; i < arr_size; ++i) {
        buffer[i] = nullptr;
    }
    while (f_getline(buf, MAX_LINE, fptr)) {
        if (index >= arr_size) {
            arr_size *= 2;
            char **temp = realloc(buffer, sizeof(char *) * arr_size);
            if (temp == nullptr) {
                free_buffer(buffer, arr_size / 2);
                fclose(fptr);
                fprintf(stderr, "Error: memory exhuasted.\n");
                return EXIT_FAILURE;
            }
            buffer = temp;
            for (size_t i = index; i < arr_size; ++i) {
                buffer[i] = nullptr;
            }
        }
        size_t len = strlen(buf);
        if (len > 0) {
            buffer[index] = malloc(sizeof(char) * (len + 1));
            if (buffer[index] == nullptr) {
                free_buffer(buffer, arr_size);
                fclose(fptr);
                return EXIT_FAILURE;
            }
            strncpy(buffer[index++], buf, len + 1);
        }
    }
    fclose(fptr);
    FILE *in_f = fopen(argv[2], "r");
    if (in_f == nullptr) {
        fprintf(stderr, "Error opening text file.\n");
        free_buffer(buffer, arr_size);
        return EXIT_FAILURE;
    }
    qsort(buffer, index, sizeof(char *), compare_str);

    char data[MAX_LINE] = {};
    size_t bytes = 0;
    size_t word_index = 0;
    size_t line = 1;
    char max_word[MAX_LINE] = {};

    while ((bytes = fread(data, 1, MAX_LINE - 1, in_f)) > 0) {
        data[bytes] = 0;
        size_t len = strlen(data);
        for (size_t i = 0; i < len; ++i) {
            unsigned char c = (unsigned char)tolower(data[i]);
            if (c == data[i])
                ++line;

            if (isalnum(c)) {
                if (word_index < MAX_LINE - 1) {
                    max_word[word_index++] = (char)c;
                    max_word[word_index] = 0;
                }
            } else {
                if (word_index > 0) {
                    if (word_index > 1) {
                        if (!spell(buffer, sizeof(char *), index, max_word)) {
                            printf("Incorrect English Word: %s on Line: %zu\n", max_word, line);
                        }
                    }
                    word_index = 0;
                }
            }
        }
    }
    if (word_index > 1) {
        if (!spell(buffer, sizeof(char *), index, max_word)) {
            printf("Incorrect English Word: %s on Line: %zu\n", max_word, line);
        }
    }
    free_buffer(buffer, arr_size);
    fclose(in_f);
    return EXIT_SUCCESS;
}
