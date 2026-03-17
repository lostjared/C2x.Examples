#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stddef.h>
#include<stdint.h>

struct Node {
	char *text;
	size_t count;
	struct Node *left, *right;
};

[[nodiscard]] struct Node *create_node(const char *text) {
	struct Node *n = malloc(sizeof(struct Node));
	n->text = strdup(text);
	n->count = 1;
	n->left = nullptr;
	n->right = nullptr;
	return n;
}

void insert_node(struct Node **root, const char *text) {
	struct Node **current = root;
	while(*current != nullptr) {
		int compare = strcmp(text, (*current)->text);
		if(compare < 0) {
			current = &(*current)->left;
		} else if(compare > 0) {
			current = &(*current)->right;
		} else {
			(*current)->count++;
			return;
		}
	}
	*current = create_node(text);
}

void release_node(struct Node *node) {
	struct Node *current = node;
	while(current != nullptr) {
		if(current->left != nullptr) {
			struct Node *left_ = current->left;
			current->left = left_->right;
			left_->right = current;
			current = left_;
		} else {
			struct Node *next = current->right;
			free(current->text);
			free(current);
			current = next;
		}
	}
}

void print_node(const struct Node *root) {
	if(root == nullptr) return;
	size_t cap_size = 128;
	size_t top = 0;
	const struct Node **stack = malloc(cap_size * sizeof(struct Node *));
	if(!stack) {
		fprintf(stderr, "Error on allocation\n");
		return;
	}
	const struct Node *curr = root;
	while(curr !=  nullptr || top > 0) {
		while(curr != nullptr) {
			if(top >= cap_size) {
				cap_size *= 2;
				const struct Node **temp = realloc(stack, cap_size * sizeof(struct Node *));
				if(!temp) {
					fprintf(stderr, "Out of memory");
					free(stack);
					return;
				}
				stack = temp;
			}
			stack[top++] = curr;
			curr = curr->left;
		}
		curr = stack[--top];
		printf("%s: %ld\n", curr->text, curr->count);
		curr = curr->right;
	}
	free(stack);
}


void count_words(const char *restrict text) {
	char *temp_buffer = malloc (strlen(text) + 1);
	int count = 0;
	size_t len = strlen(text);
	struct Node *root = nullptr;

	for(size_t i = 0; i < len; ++i) {
		if(text[i] != ' ' && ((text[i] >= 'a' && text[i] <= 'z') ||  (text[i] >= 'A' && text[i] <= 'Z'))) {
			temp_buffer[count++] = text[i];
			temp_buffer[count] = 0;
		} else {
			if(count > 0) {	
				insert_node(&root, temp_buffer);
				count = 0;
			}
		}
	}
	free(temp_buffer);
	print_node(root);
	release_node(root);
}

void proc_count_words(const char *restrict filename) { 
	FILE *fptr = fopen(filename, "rb");
	if(fptr == nullptr) {
		fprintf(stderr, "Error could not open: %s\n", filename);
		return;
	}
	fseek(fptr, 0, SEEK_END);
	size_t bytes = (size_t)ftell(fptr);
	rewind(fptr);
	char *buffer = calloc((bytes+1), sizeof(char));
	if(fread(buffer, sizeof(char), bytes, fptr) != bytes) {
		fprintf(stderr, "Invalid read: %s\n", filename);
	}
	count_words(buffer);
	free(buffer);
	fclose(fptr);
}

int main(int argc, char **argv) {
	if(argc <= 1) {
		 fprintf(stderr, "%s <files1> <file2> ...\n", argv[0]);
		 return EXIT_FAILURE;
	}
	for(int i = 1; i < argc; ++i) {
		 proc_count_words(argv[i]);
	}
	return EXIT_SUCCESS;
}

