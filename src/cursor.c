#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void set_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

static void clear_screen() {
    printf("\033[2J\033[H");
}

void quit(int) {
    clear_screen();
    char message[] = "\bProgress interrupted.\n";
    write(STDOUT_FILENO, message, sizeof(message) -1);
    exit(EXIT_FAILURE);
}

int main(void) {
    if (signal(SIGINT, quit) == SIG_ERR) {
        fprintf(stderr, "Error installing handler.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i <= 100; ++i) {
        clear_screen();
        set_cursor(1, 25);
        sleep(1);
        printf("Progress (Ctrl+C to break): %d%%\n", i);
    }
    return EXIT_SUCCESS;
}
