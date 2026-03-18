#include "hashtable.h"

struct Node *create_node(const char *text) {
	if(text == nullptr)
		return nullptr;

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
	n->cleanup = nullptr;
	return n;
}

struct Node *insert_node(struct Node **root, const char *text) {
	if(text == nullptr) 
		return nullptr;

	struct Node *n = create_node(text);
	if(n == nullptr) return nullptr;
	n->next = *root;
	if(*root != nullptr) {
		(*root)->prev = n;
	}
	*root = n;
	return n;
}

void print_node(struct Node *root) {
	if(root == nullptr) 
		return;
	struct Node *temp = root;
	while(temp != nullptr) {
		printf("\t[%s]\n", temp->text);
		temp = temp ->next;
	}
}

void release_node(struct Node *root) {
	while(root != nullptr) {
		struct Node *n = root->next;
		free(root->text);
		if(root->cleanup != nullptr && root->value != nullptr)
			root->cleanup(root->value);
		free(root);
		root = n;
	}
}

struct Node *find_node(struct Node *root,const char *text) {
	if(text == nullptr) 
		return nullptr;

	struct Node *n = root;
	while(n != nullptr) {
		if(strcmp(n->text, text) == 0) 
			return n;
		n = n->next;
	}
	return nullptr;
}

size_t hash(const char *key) {
	if(key == nullptr)
		return 0;

	size_t h = 1469598103934665603ull;
	while(*key != '\0') {
		h ^= (unsigned char)*key;
		h *= 1099511628211ull;
		++key;
	}
	return h % TABLE_SIZE;
}

struct Node *hash_insert(struct HashTable *table, const char *text) {
	if(text == nullptr)
		return nullptr;
	size_t key = hash(text);	
	struct Node *existing = find_node(table->buckets[key], text);
	if(existing != nullptr)
		return existing;
	return insert_node(&table->buckets[key], text);
}

struct Node *hash_lookup(struct HashTable *table, const char *text) {
	if(text == nullptr)
		return nullptr;

	size_t  key = hash(text);
	struct Node **n = &table->buckets[key];
	return find_node(*n, text);

}

void hash_init(struct HashTable *table) {
	memset(table, 0, sizeof(struct HashTable));
}

void hash_cleanup(struct HashTable *table) {
	for(size_t i = 0; i < TABLE_SIZE; ++i) {
		release_node(table->buckets[i]);
		table->buckets[i] = nullptr;
	}
}

void hash_print(struct HashTable *table) {
	printf("Keys: {\n");
	for(size_t i = 0; i < TABLE_SIZE; ++i) {
		print_node(table->buckets[i]);
	}
	printf("}\n");
}

void cleanup_ptr(void *ptr){
	if(ptr != nullptr) {
		free(ptr);
	}
}
