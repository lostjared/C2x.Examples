#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

typedef struct List {
	void *data;
	size_t data_size;
	struct List *next;
} LinkedList;

typedef int (*cmp_func)(const void *, const void *);

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

void sort_list(LinkedList *root, cmp_func cmp) {
	if(root == nullptr) return;
	bool swapped;
	LinkedList *ptr_1 = nullptr,  *l_ptr = nullptr;
	do {
		swapped = false;
		ptr_1 = root;
		while(ptr_1->next != l_ptr) {
			if(cmp(ptr_1->data, ptr_1->next->data) > 0) {
				void *temp_data = ptr_1->data;
				ptr_1->data = ptr_1->next->data;
				ptr_1->next->data = temp_data;

				size_t temp_sz = ptr_1->data_size;
				ptr_1->data_size = ptr_1->next->data_size;
				ptr_1->next->data_size = temp_sz;
				swapped = true;
			}
			ptr_1 = ptr_1->next;
		}
		l_ptr = ptr_1;
	} while(swapped);
}

bool remove_item(LinkedList **llist, int index) {
	LinkedList **current = llist;
	while(*current != nullptr) {
		if(*(int *)(*current)->data == index) {
			LinkedList *e = *current;
			*current = e->next;
			free(e->data);
			free(e);
			return true;
		}
		current = &(*current)->next;
	}
	return false;
}

bool insert_at_index(LinkedList **llist, void *data, size_t bytes, int target_index) {
	LinkedList **current = llist;
	int index = 0;
	while(*current != nullptr && index < target_index) {
		current = &(*current)->next;
		++index;
	}
	if(index == target_index) {
		LinkedList *new_node = init_node(data, bytes);
		new_node->next = *current;
		*current = new_node;
		return true;
	}
	return false;
}

bool remove_index(LinkedList **llist, int by_index) {
	LinkedList **current = llist;
	int index = 0;
	while(*current != nullptr) {
		if(index == by_index) {
			LinkedList *e = *current;
			*current = e->next;
			free(e->data);
			free(e);
			return true;
		}
		current = &(*current)->next;
		++index;
	}
	return false;
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

int compare_integer(const void *a, const void *b) {
	return *(int*)a > *(int*)b;
}

int main() {
	LinkedList *root = nullptr;
	int arr[3] = { 10, 20, 30 };
	for(int i = 0; i < 3; ++i) {
		add_item(&root, &arr[i], sizeof(int));
	}
	puts("before remove: ");
	print_list(root);

	if(insert_at_index(&root, &arr[0], sizeof(int), 1)) {
			puts("inserted 10 at index 1");
			print_list(root);
	}

	if(remove_item(&root, 20)) {
		puts("removed 20");
	}
	if(remove_index(&root, 1)) {
		puts("removed 10");
	}

	puts("after remove:");
	print_list(root);
	int arr_1[3] = { 10, -1, 5 };
	add_item(&root, &arr_1[0], sizeof(int));
	add_item(&root, &arr_1[1], sizeof(int));
	add_item(&root, &arr_1[2], sizeof(int));
	sort_list(root, compare_integer);
	puts("after sort: ");
	print_list(root);
	free_list(root);
	return EXIT_SUCCESS;
}
