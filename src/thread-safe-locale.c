#define _GNU_SOURCE
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

int print_thread(void *arg) {
    const char *locale_type = (const char *)arg;
    locale_t nl = newlocale(LC_TIME_MASK, locale_type, (locale_t)0);
    if (nl == (locale_t)0) {
        fprintf(stderr, "Failed to create locale\n");
        return thrd_error;
    }
    locale_t old_loc = uselocale(nl);
    for (int i = 0; i < 25; ++i) {
        time_t t = time(nullptr);
        struct tm time_i;
        localtime_r(&t, &time_i);
        char buffer[256];
        strftime(buffer, sizeof(buffer), "%c", &time_i);
        printf("Current Time: %s\n", buffer);
        sleep(1);
    }
    uselocale(old_loc);
    freelocale(nl);
    return thrd_success;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: Requires locale.\n");
        return EXIT_FAILURE;
    }

    thrd_t t;
    if (thrd_create(&t, print_thread, argv[1]) != thrd_success) {
        fprintf(stderr, "Error creating thread.\n");
        return EXIT_FAILURE;
    }
    time_t t_ = time(nullptr);
    struct tm i;
    localtime_r(&t_, &i);
    char buffer[256];
    strftime(buffer, sizeof(buffer), "%c", &i);
    printf("Current Time: %s\n", buffer);
    thrd_join(t, nullptr);
    return EXIT_SUCCESS;
}
