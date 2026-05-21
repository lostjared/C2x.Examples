#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

[[nodiscard]] int system_cmd(const char *restrict command) {
    if (command == nullptr) {
        return 1;
    }
    sigset_t bmask, omask;
    struct sigaction sa_ignore = {}, sa_oquit = {}, sa_origint = {}, sa_default = {};
    pid_t id;
    int status, serrno;

    if (strlen(command) == 0)
        return 0;

    sigemptyset(&bmask);
    sigaddset(&bmask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &bmask, &omask);
    sa_ignore.sa_handler = SIG_IGN;
    sa_ignore.sa_flags = 0;
    sigemptyset(&sa_ignore.sa_mask);
    sigaction(SIGINT, &sa_ignore, &sa_origint);
    sigaction(SIGQUIT, &sa_ignore, &sa_oquit);
    switch ((id = fork())) {
    case -1:
        status = -1;
        break;
    case 0:
        sa_default.sa_handler = SIG_DFL;
        sa_default.sa_flags = 0;
        sigemptyset(&sa_default.sa_mask);
        if (sa_origint.sa_handler != SIG_IGN)
            sigaction(SIGINT, &sa_default, nullptr);
        if (sa_oquit.sa_handler != SIG_IGN)
            sigaction(SIGQUIT, &sa_default, nullptr);
        sigprocmask(SIG_SETMASK, &omask, nullptr);
        execl("/bin/sh", "sh", "-c", command, (char *)nullptr);
        _exit(127);
        break;
    default:
        while (waitpid(id, &status, 0) == -1) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
        break;
    }
    serrno = errno;
    sigprocmask(SIG_SETMASK, &omask, nullptr);
    sigaction(SIGINT, &sa_origint, nullptr);
    sigaction(SIGQUIT, &sa_oquit, nullptr);
    errno = serrno;
    return status;
}

int main(void) {
    if (system_cmd("pwd") != 0) {
        fprintf(stderr, "Error: command failed.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
