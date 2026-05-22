#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main(int argc, char **argv) {
    if(argc == 2) {
        struct stat sb;
        if(stat(argv[1], &sb) == -1) {
            printf("Error on stat: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        switch(sb.st_mode & S_IFMT) {
            case S_IFREG:
                printf("Regular file..\n");
                break;
            case S_IFDIR:
                printf("Directory..\n");
                break;
            case S_IFCHR:
                printf("Character device..\n");
                break;
            case S_IFBLK:
                printf("Block Device..\n");
                break;
            case S_IFLNK:
                printf("Soft link..\n");
                break;
            case S_IFIFO:
                printf("FIFO or pipe..\n");
                break;
            case S_IFSOCK:
                printf("Socket..\n");
                break;
            default:
                printf("Unknown file type..\n");
        }
    } else {
        printf("Requires one argument file\n");
        printf("%s: filename\n", argv[0]);
        return EXIT_FAILURE;
    }
    return 0;
}
