#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[argc + 1]) {
    DIR *dir = (argc <= 1) ? opendir(".") : opendir(argv[1]);
    if (dir == nullptr) {
        fprintf(stderr, "Error reading from dir: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    struct dirent *d;
    while ((d = readdir(dir)) != nullptr) {
        if (strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".") == 0)
            continue;
        if (d->d_type == DT_DIR)
            printf("[%s]\n", d->d_name);
        else
            printf("%s\n", d->d_name);
    }
    closedir(dir);
    return EXIT_SUCCESS;
}
