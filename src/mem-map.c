#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "%s: Error incorrect arguments passed.\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file: %s: %s\n", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        fprintf(stderr, "File: %s error: %s\n", argv[1], strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    if (st.st_size <= 0) {
        fprintf(stderr, "Error empty / zero length: %s.\n", argv[1]);
        close(fd);
        return EXIT_FAILURE;
    }

    auto length = (size_t)st.st_size;
    void *mem = mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0);

    close(fd);

    if (mem == MAP_FAILED) {
        fprintf(stderr, "Error on mmap: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    [[maybe_unused]] const char *ptr = mem;
    size_t count = 0;
    for (size_t i = 0; i < length; ++i) {
        if (ptr[i] >= 'a' && ptr[i] <= 'z') {
            count++;
        }
    }
    munmap(mem, length);
    if (count > 0) {
        printf("found %zu lowercase charcter bytes..\n", count);
    } else {
        printf("No characters found.\n");
    }

    return EXIT_SUCCESS;
}
