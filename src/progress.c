#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void quit(int) {
    char msg[] = "\nProgress interrupted.\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    exit(EXIT_FAILURE);
}

static int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return 80;
    }
    return w.ws_col;
}

int main(void) {
    if (signal(SIGINT, quit) == SIG_ERR) {
        fprintf(stderr, "Error installing handler.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i <= 100; ++i) {
        int term_width = get_terminal_width();
        int bar_width = term_width - 12;
        if (bar_width < 10)
            bar_width = 10;
        int num_equals = (i * bar_width) / 100;
        printf("\r\033[2K[");
        for (int j = 0; j < bar_width; ++j) {
            if (j < num_equals) {
                putchar('=');
            } else {
                putchar(' ');
            }
        }
        printf("] - %d%%", i);
        fflush(stdout);
        usleep(1000 * 100);
    }
    printf("\n");
    return EXIT_SUCCESS;
}
