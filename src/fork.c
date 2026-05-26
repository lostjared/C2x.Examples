#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    printf("Process started..\n");
    pid_t id = fork();
    switch (id) {
    case -1:
        printf("Error on fork.\n");
        exit(EXIT_FAILURE);
        break;
    case 0:
        printf("In child process..\n");
        _exit(EXIT_SUCCESS);
        break;
    default:
        printf("in parent..\n");
        break;
    }
    int status = 0;
    waitpid(id, &status, 0);
    printf("Program complete..\n");
    printf("Exit code: %d\n", status);
    return EXIT_SUCCESS;
}
