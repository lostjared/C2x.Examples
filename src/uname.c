#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

int main(void) {
    struct utsname uts;
    if (uname(&uts) == -1) {
        fprintf(stderr, "Error on uname: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Node: %s\nSystem: %s\nRelease: %s\nVersion: %s\nMachine: %s\n", uts.nodename, uts.sysname, uts.release, uts.version, uts.machine);
    return EXIT_SUCCESS;
}
