#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static constexpr size_t BUFFER_SIZE = 4096;

void print_info(pid_t id, const char *file_info) {
    char file_buffer[BUFFER_SIZE];
    memset(file_buffer, 0, sizeof(file_buffer));
    snprintf(file_buffer, BUFFER_SIZE, "/proc/%d/%s", id, file_info);
    FILE *fptr = fopen(file_buffer, "r");
    if(!fptr) {
	    fprintf(stderr, "Error could not find process file: %s.\n", file_buffer);
	    return;
    }
    char buffer[BUFFER_SIZE];
    size_t bytes = 0;
    memset(buffer, 0, sizeof(buffer));
    while((bytes = fread(buffer, 1, BUFFER_SIZE - 1, fptr)) > 0) {
	    for(size_t i = 0; i < bytes; ++i)
		    if(buffer[i] == '\0')
			    buffer[i] = ' ';		   
	    printf("%s", buffer);
	    memset(buffer, 0, sizeof(buffer));
    }
    printf("\n");
    fclose(fptr);
}

int main(void) {
    pid_t id = getpid();
    printf("process id: %d\n", id);
    print_info(id, "cmdline");
    print_info(id, "status");
    print_info(id, "environ");
    print_info(id, "maps");
   return EXIT_SUCCESS;
}
