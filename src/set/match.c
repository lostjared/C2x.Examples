#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>
#include<ctype.h>
#include"set.h"

bool insert_text(Set *set, char *restrict word,  size_t *index) {
	if(*index > 0) {
		word[*index] = 0;
		if(!set_insert(set, word, *index+1)) {
			fprintf(stderr, "Set insert failed.\n");
			return false;
		}
		*index = 0;
	}
	return true;
}

bool insert_words(FILE *fptr, Set *set) {
	static constexpr size_t buf_size = 4096;
	char buffer[buf_size];	
	size_t bytes = 0;
	size_t word_size = buf_size;
	size_t index = 0;
	char *word = malloc(word_size+1);
	if(word == nullptr) {
		fprintf(stderr, "Error on allocation.\n");
		return false;
	}
	while((bytes = fread(buffer, sizeof(char),  buf_size, fptr)) > 0) {
		for(size_t i = 0; i < bytes; ++i) {
			unsigned char c = (unsigned char)buffer[i];
			if(isalnum(c)) {
				if(index >= word_size) {
					word_size *= 2;
					char *temp = realloc(word, word_size+1);
					if(temp == nullptr) {
						free(word);
						return false;
					}
					word = temp;
				}
				word[index++] = (char) tolower(c);
			} else {

				if(!insert_text(set, word, &index)) {
					free(word);
					return false;
				}
			}
		}
	}

	if(index > 0) {
		word[index] = 0;
		if(!insert_text(set, word, &index)) {
			free(word);
			return false;
		}
	}
	free(word);
	return true;
}

void destroy(void *ptr) {
	free(ptr);
}

int compare(const void *a, const void *b) {
	if(a == nullptr || b == nullptr)
		return -1;
	return strcmp((const char *)a, (const char *)b);
}

void echo(const void *ptr) {
	printf("\t%s\n", (const char *)ptr);
}

void echo_notab(const void *ptr) {
	printf("%s\n", (const char *)ptr);
}

int main(int argc, char **argv) {

	if(argc > 1) {
		size_t files = 0;
		Set *total = nullptr;
		bool show_info = true;
		int start = 1;
		if(strcmp(argv[1], "-q") == 0) {
			show_info = false;
			start++;
		}
		if(!set_init(&total, destroy, compare)) {
			fprintf(stderr, "Error creating set..\n");
			return EXIT_FAILURE;
		}
		for(int i = start; i < argc; ++i) {
			FILE *fptr = fopen(argv[i], "rb");
			if(fptr == nullptr) {
				if(show_info)
				fprintf(stderr, "Error file: %s could not be opened.\n", argv[i]);
				continue;
			}
			files++;
			if(insert_words(fptr, total)) {
				if(show_info)
					printf("Processed %s.\n", argv[i]);				
			}
			fclose(fptr);
		}
		if(show_info)
			printf("Unique words across all files: {\n");
		set_print(total, (show_info == true) ? echo : echo_notab);
                if(show_info) {
			printf("}\n");
			printf("%zu total unique words across %zu files.\n", set_count(total), files);
		}
		set_free(total);
	}
	return EXIT_SUCCESS;
}

