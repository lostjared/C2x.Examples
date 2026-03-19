#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stddef.h>
#include"hashtable.h"

void print_word_table(struct HashTable *table) {
	printf("Tokens: {\n");
	for(size_t i = 0; i < table->bucket_size; ++i) {		
		struct Node *root = table->buckets[i];
		if(root == nullptr) 
			continue;
		struct Node *temp = root;
		while(temp != nullptr) {
			if(temp->value != nullptr)
				printf("\t[%s] = %d\n", temp->text, *(int*)temp->value);
			temp = temp ->next;
		}
	}
	printf("}\n");
}

size_t count_words(FILE *fptr, size_t *num) {
	static constexpr size_t BUFFER_SIZE = 1024u * 4u;
	char buffer[BUFFER_SIZE] = {};
	size_t count = 0;
	struct HashTable table;
	size_t bytes_read = 0;
	size_t word_size = BUFFER_SIZE;
	size_t index = 0;
	char *word = malloc(word_size + 1);
	if(!word) {
		fprintf(stderr, "Error out of memory\n");
		return 0;
	}

	if(!hash_init(&table, DEFAULT_TABLE_SIZE)) {
		fprintf(stderr, "Error loading table..\n");
		free(word);
		return 0;
	}

	while((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, fptr)) > 0) {
		for(size_t i  = 0; i < bytes_read; ++i) {
			unsigned char c = (unsigned char)buffer[i];
			if(isalnum(c)) {
				if(index < word_size) {
					word[index++] = (char)tolower(c);				
				} else {
					word_size *= 2;
					char *temp = realloc(word, word_size);
					if(temp == nullptr) {
						fprintf(stderr, "Error on allocation out of memory \n");
						free(word);
						hash_cleanup(&table);
						return 0;
					}
					word = temp;
					word[index++] = (char)tolower(c);
				}
			} else {
				if(index > 0) {
					word[index] = 0;	
					index = 0;	
					(*num) ++;
					struct Node *n = hash_insert(&table, word);
					if(n == nullptr) {
						fprintf(stderr, "Error on allocation.\n");
						hash_cleanup(&table);
						return 0;
					}
					else {	
						if(n->value == nullptr) {
							n->cleanup = cleanup_ptr;
							n->value = malloc(sizeof(int));
							*((int*)n->value) = 1;
							count++;
						} else {
							*((int*)n->value) += 1;
						}
					}
					if(word != nullptr && word_size > BUFFER_SIZE) {
						free(word);
						word_size = BUFFER_SIZE;
						word = malloc (word_size);
						if(word == nullptr) {
							fprintf(stderr, "Error on allocation.\n");
							hash_cleanup(&table);
							return 0;
						}
					}
				}
			}
		}
	}

	if(index > 0) {
		word[index] = 0;
		(*num) ++;
		struct Node *n = hash_insert(&table, word);
		if(n == nullptr) {
			fprintf(stderr, "Error on allocation..\n");
			hash_cleanup(&table);
			return 0;
		}
		if(n->value == nullptr) {
			n->value = malloc(sizeof(int));
			if(n->value == nullptr) {
				fprintf(stderr, "Error on allocation.\n");
				hash_cleanup(&table);
				return 0;
			}
			*((int*)n->value) = 1;
			count++;
		} else {
			*((int*)n->value) += 1;
		}
	}
	free(word);
	print_word_table(&table);
	hash_cleanup(&table);
	return count;	
}


int main(int argc, char **argv) {
	if(argc <= 1) {
		size_t num = 0, count = 0;
		count = count_words(stdin, &num);
		printf("Contains: %zu tokens, %zu unique\n", num, count);
		return EXIT_SUCCESS;
	}

	for(int i = 1; i < argc; ++i) {
		FILE *fptr = fopen(argv[i], "rb");
		if(fptr == nullptr) {
			fprintf(stderr, "Error could not open. file: %s\n", argv[i]);
			continue;
		}
		size_t num = 0;
		size_t count = count_words(fptr, &num);
		printf("Contains: %zu tokens, %zu unique\n", num, count);
		fclose(fptr);
	}
	return EXIT_SUCCESS;
}


