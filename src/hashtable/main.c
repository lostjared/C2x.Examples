#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"hashtable.h"

char *f_getline(char *buffer, size_t bytes, FILE *fptr) {
	if(buffer == nullptr || bytes < 2 || fptr == nullptr)
		return nullptr;

	char *ret = fgets(buffer, (int)bytes, fptr);
	if(ret == nullptr)
		return nullptr;

	char *position = strchr(buffer, '\n');
	if(position != nullptr) {
		*position = 0;
	} else {
		int ch = 0;
		while((ch = fgetc(fptr)) != '\n' && ch != EOF) {}
	}
	return ret;
}

int main(void) {
	static constexpr size_t BUFFER_SIZE = 1024 * 4;
	struct HashTable table;
	if(!hash_init(&table, DEFAULT_TABLE_SIZE)) {
		fprintf(stderr, "Error creating hash table.\n");
		return EXIT_FAILURE;
	}
	char buffer[BUFFER_SIZE]= {};
	bool active = true;

	while(active) {

		static constexpr size_t CBUF = 256;
		char cbuf[CBUF] = {};
		size_t choice = 0;

		printf("Choice:\n0 = Insert\n1 = Lookup\n2 = Remove\n3 = Exit\n>");
		if(f_getline(cbuf, CBUF, stdin)) {
			choice = strtoull(cbuf, nullptr, 0);
			if(choice > 3)
				continue;
			else if(choice == 3) {
				active = false;
				break;
			}
		}
		printf("Enter key: ");
		if(f_getline(buffer, BUFFER_SIZE, stdin)) {
			 if(choice == 0) {
				char value[BUFFER_SIZE] = {};
				printf("Enter value: ");
				if(f_getline(value, BUFFER_SIZE, stdin)) {
					struct Node *n = hash_insert(&table, buffer);
					if(n != nullptr) {
						char *new_value = dup_string(value);
						if(new_value == nullptr) {
							fprintf(stderr, "Error on allocation.\n");
							hash_cleanup(&table);
							return EXIT_FAILURE;
						}
						if(n->value != nullptr && n->cleanup != nullptr) {
							n->cleanup(n->value);
						}
						n->value = new_value;
						n->cleanup = cleanup_ptr;
					}
				}
				hash_print(&table);
			} else if(choice == 1) {
				struct Node *n = hash_lookup(&table, buffer);
				if(n != nullptr) {
					if(n->value != nullptr) 
						printf("%s = %s\n", n->text, (const char *)n->value);
					else 
						printf("%s [no value]\n", n->text);
				} else {
					printf("Key not found!\n");
				}
			} else if(choice == 2) {
				hash_remove(&table, buffer);
			}
		}
	}
	hash_cleanup(&table);

	return 0;
}
