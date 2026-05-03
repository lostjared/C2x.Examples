#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool copy_file(const char *restrict src, const char *restrict dst) {
    int ifd = -1, ofd = -1;
    if ((ifd = open(src, O_RDONLY)) == -1) {
        perror("Could not open file");
        return false;
    }
    if ((ofd = open(dst, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
        close(ifd);
        perror("Could not open file");
        return false;
    }
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(ifd, buffer, 4096)) > 0) {
        if (write(ofd, buffer, (size_t)bytes) != bytes) {
            close(ifd);
            close(ofd);
            perror("Write failed.\n");
            return false;
        }
    }
    close(ifd);
    close(ofd);
    return true;
}

int main(int argc, char **argv) {
    if (argc == 3) {
        if (copy_file(argv[1], argv[2])) {
            printf("%s -> %s\n", argv[1], argv[2]);
            return EXIT_SUCCESS;
        }
    } else {
        fprintf(stderr, "%s <src> <dst>\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
