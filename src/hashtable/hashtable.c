#include "hashtable.h"

char *dup_string(const char *text) {
	if(text == nullptr)
		return nullptr;

	size_t len = strlen(text) + 1;
	char *out = malloc(len);
	if(out == nullptr)
		return nullptr;

	memcpy(out, text, len);
	return out;
}

struct Node *create_node(const char *text) {
	if(text == nullptr)
		return nullptr;

	struct Node *n = malloc(sizeof(struct Node));
	if(n == nullptr) {
		fprintf(stderr, "Error on allocation..\n");
		return nullptr;
	}
	n->text = dup_string(text);
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
	if(root == nullptr || text == nullptr) 
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

void print_node(const struct Node *root) {
	if(root == nullptr) 
		return;
	const struct Node *temp = root;
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
	if(root == nullptr || text == nullptr) 
		return nullptr;

	struct Node *n = root;
	while(n != nullptr) {
		if(strcmp(n->text, text) == 0) 
			return n;
		n = n->next;
	}
	return nullptr;
}

size_t hash(const char *key, size_t bucket_size) {
	if(key == nullptr || bucket_size == 0)
		return 0;

	size_t h = 1469598103934665603ull;
	while(*key != '\0') {
		h ^= (unsigned char)*key;
		h *= 1099511628211ull;
		++key;
	}
	return h % bucket_size;
}

struct Node *hash_insert(struct HashTable *table, const char *text) {
	if(table == nullptr || text == nullptr || table->buckets == nullptr || table->bucket_size == 0)
		return nullptr;
	size_t key = hash(text, table->bucket_size);	
	struct Node *existing = find_node(table->buckets[key], text);
	if(existing != nullptr)
		return existing;
	return insert_node(&table->buckets[key], text);
}

struct Node *hash_lookup(const struct HashTable *table, const char *text) {
	if(table == nullptr || text == nullptr || table->buckets == nullptr || table->bucket_size == 0)
		return nullptr;

	size_t key = hash(text, table->bucket_size);
	return find_node(table->buckets[key], text);

}
bool hash_init(struct HashTable *table, size_t bucket_size) {
	if(table == nullptr || bucket_size == 0) return false;
	memset(table, 0, sizeof(*table));
	table->buckets = calloc(bucket_size, sizeof(*table->buckets));
	if(table->buckets == nullptr) return false;
	table->bucket_size = bucket_size;
	return true;
}

void hash_cleanup(struct HashTable *table) {
	if(table == nullptr || table->buckets == nullptr  || table->bucket_size == 0) return;
	for(size_t i = 0; i < table->bucket_size; ++i) {
		release_node(table->buckets[i]);
		table->buckets[i] = nullptr;
	}
	free(table->buckets);
	table->buckets = nullptr;
	table->bucket_size = 0;
}

void hash_print(const struct HashTable *table) {
	if(table == nullptr || table->buckets == nullptr || table->bucket_size == 0) return;	
	printf("Keys: {\n");
	for(size_t i = 0; i < table->bucket_size; ++i) {
		print_node(table->buckets[i]);
	}
	printf("}\n");
}

void cleanup_ptr(void *ptr){
	if(ptr != nullptr) {
		free(ptr);
	}
}

void hash_set(struct HashTable *table, const char *text, void *value, void (*cleanup)(void *)) {
	if(table == nullptr || text == nullptr || table->buckets == nullptr || table->bucket_size == 0)
		return;
	struct Node *n = hash_insert(table, text);
	if(n == nullptr) return;
	if(n->cleanup != nullptr && n->value != nullptr) {
		n->cleanup(n->value);
	}
	n->value = value;
	n->cleanup = cleanup;
}

void hash_remove(struct HashTable *table, const char *text) {
	if (table == nullptr || text == nullptr || table->buckets == nullptr || table->bucket_size == 0) return;
	size_t key = hash(text, table->bucket_size);
	struct Node *n = table->buckets[key];
	while (n != nullptr) {
		if (strcmp(n->text, text) == 0) {
			if (n->prev) n->prev->next = n->next;
			else table->buckets[key] = n->next;
			if (n->next) n->next->prev = n->prev;
			free(n->text);
			if (n->cleanup && n->value) n->cleanup(n->value);
			free(n);
			return;
		}
		n = n->next;
	}
}

