#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<limits.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/sendfile.h>

static constexpr size_t M_PATH = PATH_MAX * 2;

size_t cp_dir(size_t *counter, const char input[static 1], const char output[static 1]) {
	if(mkdir(output, 0755) == -1 && errno != EEXIST) {
		fprintf(stderr, "Error creating directory: %s\n", output);
		exit(EXIT_FAILURE);
	}
	DIR *dir = opendir(input);
	if(dir == nullptr) {
		fprintf(stderr, "Error opening directory: %s\n", input);
		return *counter;
	}
	struct dirent *e;
	while((e = readdir(dir)) != nullptr) {
		if(strcmp(e->d_name, ".." ) == 0 || strcmp(e->d_name, ".") == 0) {
			continue;
		}
		char full[4096];
		char out[4096];
		snprintf(full, M_PATH, "%s/%s", input, e->d_name);
		if(e->d_type == DT_DIR)  {
			snprintf(out, M_PATH, "%s/%s", output, e->d_name);
			cp_dir(counter, full, out);
		} else {
			char fout[4096+1];
			snprintf(fout,M_PATH, "%s/%s", output, e->d_name);
		        if(access(fout, F_OK) != 0) {
				++(*counter);
				int source_fd = open(full, O_RDONLY);
				int dest_fd = open(fout, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if(source_fd < 0) {
					fprintf(stderr, "could not open input file: %s\n", full);
					continue;
				}

				if(dest_fd < 0) {
					fprintf(stderr, "could not open output file: %s \n", fout);
					continue;
				}

				struct stat dst;
				fstat(source_fd, &dst);
	 			sendfile(dest_fd, source_fd, nullptr, dst.st_size);
				close(source_fd);
				close(dest_fd);
				printf("%s -> %s\n", full, fout);
			}
		}
	}
	closedir(dir);
	return *counter;
}

int main(int argc, char **argv)  {
	if(argc != 3) {
		fprintf(stderr, "Error invalid args use: skiip-cpy <input> <output>\n");
		return EXIT_FAILURE;
	}
	size_t counter = 0;
	int err = mkdir(argv[2], 0755);
	if(err == -1 && errno != EEXIST)  {
		fprintf(stderr, "Error on creation of directory\n");
		return EXIT_FAILURE;
	}
	counter = cp_dir(&counter, argv[1], argv[2]);
	printf("%ld files\n", counter);
	return  EXIT_SUCCESS;
}

