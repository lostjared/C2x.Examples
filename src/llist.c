#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

typedef struct List {
	void *data;
	struct List *next;
} LinkedList;

LinkedList *init_node(void *data) {
	LinkedList *item = malloc(sizeof(LinkedList));
	if(!item) {
		fprintf(stderr, "Could not allcoate list item\n");
		perror("Error allocate");
		exit(EXIT_FAILURE);
	}
	item->next = nullptr;
	item->data = data;
	return item;
}

void add_item(LinkedList **llist, void *data) {
	 LinkedList **current = llist;
	 while(*current != nullptr) {
		 current = &(*current)->next;
	 }
	 *current = init_node(data);
}

void free_list(LinkedList *lst) {
	if(lst == nullptr) return;
	free_list(lst->next);
	free(lst);
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
		add_item(&root, &arr[i]);
	}
	print_list(root);
	free_list(root);
	return EXIT_SUCCESS;
}
