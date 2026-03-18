#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Node {
	char *text;
	void *value;
	struct Node *next, *prev;
};

struct Node *create_node(const char *text) {
	struct Node *n = malloc(sizeof(struct Node));
	if(n == nullptr) {
		fprintf(stderr, "Error on allocation..\n");
		return nullptr;
	}
	n->text =  strdup(text);
	if(n->text == nullptr) {
		fprintf(stderr, "Could not allocate text buffer\n");
		free(n);
		return nullptr;
	}
	n->next = n->prev = nullptr;
	n->value = nullptr;
	return n;
}

struct Node *insert_node(struct Node **root, const char *text) {
	struct Node *n = create_node(text);
	if(n == nullptr) return nullptr;
	if(*root == nullptr) {
		*root = n;
		return n;
	}
	struct Node *temp = *root;
	while(temp ->next != nullptr) {
		temp = temp->next;
	}
	temp->next = n;
	n->prev = temp;
	return n;
}

void print_node(struct Node *root) {
	struct Node *temp = root;
	struct Node *last = nullptr;
	printf("Forward: {\n\n");
	while(temp != nullptr) {
		printf("text:  [%s]\n", temp->text);
		last = temp;
		temp = temp ->next;
	}
	printf("\n}\n");
	printf("Backwards: {\n");
	while(last != nullptr) {
		printf("text last: [%s]\n", last->text);
		last = last->prev;
	}
	printf("\n}\n");
}

void release_node(struct Node *root) {
	while(root != nullptr) {
		struct Node *n = root->next;
		free(root->text);
		if(root->value != nullptr)
			free(root->value);
		free(root);
		root = n;
	}

}

struct Node *find_node(struct Node *root,const char *text) {
	struct Node *n = root;
	while(n != nullptr) {
		if(strcmp(n->text, text) == 0) 
			return n;
		n = n->next;
	}
	return nullptr;
}

static constexpr int TABLE_SIZE = 1024 * 4;
struct Node *buckets[TABLE_SIZE] = {};

int hash(const char *key) {
	size_t len = strlen(key);
	size_t total = 0;
	for(size_t i = 0; i < len; ++i) {
		total += (size_t)key[i];
	}
	return total%TABLE_SIZE;
}

struct Node *hash_insert(const char *text) {
	int key = hash(text);
	struct Node **n = &buckets[key];
	insert_node(n, text);
	return *n;
}

struct Node *hash_lookup(const char *text) {
	int key = hash(text);
	struct Node **n = &buckets[key];
	return find_node(*n, text);

}

void hash_cleanup() {
	for(int i = 0; i < TABLE_SIZE; ++i) {
		release_node(buckets[i]);
	}
}


int main(void) {

	struct Node *value = hash_insert("key ");
	if(value == nullptr) {
		fprintf(stderr, "Error hash value not inserted.\n");
		hash_cleanup();
		return EXIT_FAILURE;
	}
	value->value = strdup("Value 1");
	if(value->value == nullptr) {
		fprintf(stderr, "Error on allocation.\n");
		hash_cleanup();
		return EXIT_FAILURE;
	}
	struct Node *look = hash_lookup("key ");
	if(look != nullptr) {
		printf("value is; %s\n", (const char *)look->value);
	} else {
		printf("could not be found.\n");
	}

	struct Node *test_value = hash_insert("key value");
	if(test_value == nullptr) {
		fprintf(stderr, "Could not find key");
		hash_cleanup();
		return EXIT_FAILURE;
	}
	test_value->value = malloc (sizeof(int));
	if(test_value == nullptr) {
		fprintf(stderr, "Error on allocatoin.\n");
		hash_cleanup();
		return EXIT_FAILURE;
	}
	*((int*)test_value->value) = 10;
	struct Node *i_value = hash_lookup("key value");
	if(i_value == nullptr) {
		fprintf(stderr, "Hash key not found.\n");
	} else {
		fprintf(stderr, "Value is: %d\n", *((int*)i_value->value));
	}
	hash_cleanup();
	return  EXIT_SUCCESS;
}


