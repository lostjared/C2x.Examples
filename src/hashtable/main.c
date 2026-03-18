#include<stdio.h>
#include<stdlib.h>
#include"hashtable.h"

char *f_getline(char buffer[static 1], int bytes, FILE *fptr) {
	buffer[0] = 0;
	char *ret = fgets(buffer, bytes, fptr);
	if(ret) {
		char *position = strchr(buffer, '\n');
		if(position) 
			*position = 0;
		else 
			ret = nullptr;
	}
	return ret;
}
int main(void) {
	static constexpr size_t BUFFER_SIZE = 1024 * 4;
	struct HashTable table;
	hash_init(&table);
	char buffer[BUFFER_SIZE]= {};
	bool active = true;

	while(active) {

		static constexpr size_t CBUF = 256;
		char cbuf[CBUF] = {};
		size_t choice = 0;

		printf("Choice 0, or 1\n0 = Insert\n1 = Print\n>");
		if(f_getline(cbuf, CBUF-1, stdin)) {
			choice = strtoull(cbuf, nullptr, 0);
			if(choice != 0 && choice != 1)
				continue;
		}

		printf("Enter key: ");
		if(f_getline(buffer, BUFFER_SIZE-1, stdin)) {
			if(choice == 0) {
				char value[BUFFER_SIZE] = {};
				printf("Enter value: ");
				if(f_getline(value, BUFFER_SIZE-1, stdin)) {
					struct Node *n = hash_insert(&table, buffer);
					if(n != nullptr) {
						if(n->value != nullptr) {
							n->cleanup(n->value);
							n->value = strdup(value);
							if(n->value == nullptr) {
								fprintf(stderr, "Error on allocation.");
								hash_cleanup(&table);
								return EXIT_FAILURE;
							}
						} else {
							n->value = strdup(value);
							if(n->value == nullptr) {
								fprintf(stderr, "Error on allocation out of memory.\n");
								hash_cleanup(&table);
								return EXIT_FAILURE;
							}
							n->cleanup = cleanup_ptr;
						}
					}
				}
				hash_print(&table);
			} else {
				struct Node *n = hash_lookup(&table, buffer);
				if(n != nullptr) {
					printf("%s = %s\n", n->text, (const char *)n->value);
				} else {
					printf("Key not found!\n");
				}
			}
		}
	}
	hash_cleanup(&table);

	return 0;
}
