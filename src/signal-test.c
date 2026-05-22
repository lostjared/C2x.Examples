#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void signal_handler(int sig [[maybe_unused]]) {

    char message[] = "\nSignal SIGINT caught.\n";
    write(STDOUT_FILENO, message, sizeof(message) - 1);

    exit(EXIT_SUCCESS);
}

int main(void) {

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Error initalizing signal.\n");
        return EXIT_FAILURE;
    }

    int index = 0;
    while (1) {
        ++index;
        sleep(3);
        printf("Count index: %d\n", index);
    }
    return 0;
}
