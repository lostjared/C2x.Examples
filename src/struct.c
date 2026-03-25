#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct score {
    char user_name[256];
    int score;
} Score;

void score_init(Score *s, const char *user_name, int score);
Score *score_add(Score *s, const char *user_name, int score, size_t *size);
void score_free(Score *s);
int score_compare(const void *a, const void *b);
void score_print(const Score *s, size_t size) [[unsequenced]];

int main() {
    size_t size = 0;
    Score *s = nullptr;
    s = score_add(s, "Jared", 100, &size);
    s = score_add(s, "JPB", 1, &size);
    s = score_add(s, "value", 255, &size);
    s = score_add(s, "temp", 250, &size);

    char buffer[255];
    char int_buffer[255];
    bool active = true;
    while (active) {
        printf("Enter username: ");
        if (fgets(buffer, 254, stdin)) {
            printf("Enter score: ");
            if (strlen(buffer) > 0)
                buffer[strcspn(buffer, "\n")] = 0;

            if (strcmp(buffer, "exit") == 0) {
                active = false;
                break;
            }
            if (fgets(int_buffer, 254, stdin)) {
                int value = atoi(int_buffer);
                if (value > 0)
                    s = score_add(s, buffer, value, &size);
                else {
                    active = false;
                    break;
                }
            }
        }
    }

    qsort(&s[0], size, sizeof(Score), score_compare);
    score_print(s, size);
    score_free(s);
    return 0;
}

void score_init(Score *s, const char *user_name, int score) {
    if (s == nullptr) {
        fprintf(stderr, "error score is nullptr\n");
        exit(EXIT_FAILURE);
    }
    s->score = score;
    memset(s->user_name, 0, sizeof(s->user_name));
    strncpy(s->user_name, user_name, sizeof(s->user_name) - 1);
}

int score_compare(const void *a, const void *b) {
    return ((Score *)a)->score > ((Score *)b)->score;
}

Score *score_add(Score *s, const char *user_name, int score, size_t *size) {
    s = (Score *)realloc(s, sizeof(Score) * (*size + 1));
    score_init(s + *size, user_name, score);
    *size = *size + 1;
    return s;
}

void score_free(Score *s) {
    if (s == nullptr) {
        fprintf(stderr, "Error score is nullptr\n");
        exit(EXIT_FAILURE);
    }
    free(s);
}

void score_print(const Score *s, size_t size) {
    if (s == nullptr) {
        fprintf(stderr, "Error score is nullptr\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < size; ++i) {
        printf("%s: %d\n", s[i].user_name, s[i].score);
    }
}
