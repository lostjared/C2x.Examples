#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

typedef struct List {
	void *data;
	size_t data_size;
	struct List *next;
} LinkedList;

LinkedList *init_node(void *data, size_t bytes) {
	LinkedList *item = malloc(sizeof(LinkedList));
	if(!item) {
		perror("Error allocate");
		exit(EXIT_FAILURE);
	}
	item->next = nullptr;
	item->data = malloc(bytes);
	if(!item->data) {
		free(item);
		perror("data allocation failed");
		exit(EXIT_FAILURE);
	}
	memcpy(item->data, data, bytes);
	item->data_size = bytes;
	return item;
}

void add_item(LinkedList **llist, void *data, size_t bytes) {
	 LinkedList **current = llist;
	 while(*current != nullptr) {
		 current = &(*current)->next;
	 }
	 *current = init_node(data, bytes);
}

void free_list(LinkedList *lst) {
	while(lst != nullptr) {
		LinkedList *next = lst->next;
		free(lst->data);
		free(lst);
		lst = next;
	}
}

void print_list(const LinkedList *lst) {
	while(lst != nullptr) {
		printf("%d\n", *(int *)lst->data);
		lst = lst->next;
	}
}

int main() {
	LinkedList *root = {};
	int arr[3] = { 10, 20, 30 };
	for(int i = 0; i < 3; ++i) {
		add_item(&root, &arr[i], sizeof(int));
	}
	print_list(root);
	free_list(root);
	return EXIT_SUCCESS;
}
