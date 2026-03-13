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

static int in_output_dir(const char path[static 1], const char output_dir[static 1]) {
	size_t n = strlen(output_dir);
	if(strncmp(path, output_dir, n) != 0)
		return 0;
	return path[n] == '\0' || path[n] == '/';
}

size_t cp_dir(size_t *counter, const char input[static 1], const char output[static 1], const char output_dir[static 1], const char input_dir[static 1], const char output_name[static 1]) {

	if(strcmp(input, output) == 0)
		return *counter;

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
		if(strcmp(input, input_dir) == 0 && strcmp(e->d_name, output_name) == 0) {
			continue;
		}
		char *full = nullptr;
		if(asprintf(&full, "%s/%s", input, e->d_name) == -1)
			continue;

		if(e->d_type == DT_DIR)  {
			if(!in_output_dir(full, output_dir)) {
				int same_dir = 0;
				struct stat st_in, st_out;
				if(stat(full, &st_in) == 0 && stat(output_dir, &st_out) == 0) {
					if(st_in.st_dev == st_out.st_dev && st_in.st_ino == st_out.st_ino)
						same_dir = 1;
				}

				if(!same_dir) {
					char *out = nullptr;
					if(asprintf(&out, "%s/%s", output, e->d_name) != -1) {
						if(strcmp(full,out) != 0)
							cp_dir(counter, full, out, output_dir, input_dir, output_name);
						free(out);
					}
				}
			}
		} else {
			char *fout = nullptr;
			if(asprintf(&fout, "%s/%s", output, e->d_name) != -1) {
			        if(access(fout, F_OK) != 0) {
					++(*counter);
					int source_fd = open(full, O_RDONLY);
					if(source_fd < 0) {
						fprintf(stderr, "could not open input file: %s\n", full);
					} else {
						int dest_fd = open(fout, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if(dest_fd < 0) {
							fprintf(stderr, "could not open output file: %s \n", fout);
							close(source_fd);
						} else {
							struct stat dst;
							fstat(source_fd, &dst);
	 						sendfile(dest_fd, source_fd, nullptr, dst.st_size);
							close(source_fd);
							close(dest_fd);
							printf("%s -> %s\n", full, fout);
						}
					}
				}
				free(fout);
			}
		}
		free(full);
	}
	closedir(dir);
	return *counter;
}

int main(int argc, char **argv)  {
	if(argc != 3) {
		fprintf(stderr, "Error invalid args use: %s <input> <output>\n", argv[0]);
		return EXIT_FAILURE;
	}

	size_t counter = 0;
	int created = 0;
	int err = mkdir(argv[2], 0755);
	if(err == -1 && errno != EEXIST)  {
		fprintf(stderr, "Error on creation of directory\n");
		return EXIT_FAILURE;
	}
	if(err == 0)
		created = 1;

	char *real_in = realpath(argv[1], nullptr);
	char *real_out = realpath(argv[2], nullptr);
	if(real_in == nullptr || real_out == nullptr) {
		fprintf(stderr, "Error resolving path\n");
		free(real_in);
		free(real_out);
		return EXIT_FAILURE;
	}

	if(in_output_dir(real_out, real_in)) {
		fprintf(stderr, "Error: Destination is inside Source!\n");
		if(created)
			rmdir(argv[2]);
		free(real_in);
		free(real_out);
		return EXIT_FAILURE;
	}

	const char *output_name = strrchr(real_out, '/');
	if(output_name == nullptr)
		output_name = real_out;
	else
		++output_name;

	counter = cp_dir(&counter, real_in, real_out, real_out, real_in, output_name);
	free(real_in);
	free(real_out);
	printf("%ld files\n", counter);
	return  EXIT_SUCCESS;
}

