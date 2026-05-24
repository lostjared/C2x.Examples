#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    static constexpr size_t BUFFER_SIZE = 4096;
    pid_t id = getpid();
    printf("process id: %d\n", id);
    char file_buffer[BUFFER_SIZE];
    memset(file_buffer, 0, sizeof(file_buffer));
    snprintf(file_buffer, BUFFER_SIZE, "/proc/%d/cmdline", id);
    FILE *fptr = fopen(file_buffer, "r");
    if(!fptr) {
	    fprintf(stderr, "Error could not find process..\n");
	    return EXIT_FAILURE;
    }
    char buffer[BUFFER_SIZE];
    size_t bytes = 0;
    memset(buffer, 0, sizeof(buffer));
    printf("Information: \n");
    while((bytes = fread(buffer, 1, BUFFER_SIZE, fptr)) > 0) {
	    printf("%s\n", buffer);
	    memset(buffer, 0, sizeof(buffer));
    }
    fclose(fptr);
    return EXIT_SUCCESS;
}
