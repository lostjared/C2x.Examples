#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

[[nodiscard]] bool copy_file(const char *restrict src, const char *restrict dst) {
    int ifd = -1, ofd = -1;
    if ((ifd = open(src, O_RDONLY)) == -1) {
        fprintf(stderr, "Error Could not open input file: %s: %s\n", src, strerror(errno));
        return false;
    }

    bool passed = true;

    if ((ofd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
        fprintf(stderr, "Error could not open output file %s: %s\n", dst, strerror(errno));
        passed = false;
        goto input_cleanup;
    }
    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(ifd, buffer, 4096)) > 0) {
        if (write(ofd, buffer, (size_t)bytes) != bytes) {
            printf("Write failed: %s\n", strerror(errno));
            passed = false;
            goto all_cleanup;
        }
    }

    if (bytes == -1) {
        fprintf(stderr, "Read failed: %s\n", strerror(errno));
        passed = false;
    }

    if (fsync(ofd) == -1) {
        fprintf(stderr, "fsync failed: %s\n", strerror(errno));
        passed = false;
    }
all_cleanup:
    if (close(ofd) == -1) {
        fprintf(stderr, "Error closing output file %s: %s\n", dst, strerror(errno));
        passed = false;
    }
input_cleanup:
    if (close(ifd) == -1) {
        fprintf(stderr, "Closing input file %s: %s\n", src, strerror(errno));
    }

    return passed;
}

int main(int argc, char **argv) {
    if (argc == 3) {
        if (copy_file(argv[1], argv[2])) {
            printf("%s -> %s\n", argv[1], argv[2]);
            return EXIT_SUCCESS;
        } else {
            printf("Copy failed.\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "%s <src> <dst>\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
