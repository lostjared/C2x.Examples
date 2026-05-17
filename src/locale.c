#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    if (setlocale(LC_ALL, "") == nullptr) {
        fprintf(stderr, "Error failed to set locale.\n");
    }
    time_t t = time(nullptr);
    struct tm *i = localtime(&t);
    if (i == nullptr) {
        fprintf(stderr, "Error: Failed to get local time.\n");
        return EXIT_FAILURE;
    }

    char buffer[256];
    if (strftime(buffer, sizeof(buffer), "%c", i) == 0) {
        fprintf(stderr, "Error: strftime failed.\n");
        return EXIT_FAILURE;
    }
    printf("Current time: %s\n", buffer);
    return 0;
}
