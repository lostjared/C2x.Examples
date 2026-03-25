#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List {
    void *data;
    size_t data_size;
    struct List *next;
} LinkedList;

LinkedList *init_node(void *data, size_t bytes) {
    LinkedList *item = malloc(sizeof(LinkedList));
    if (!item) {
        perror("Error allocate");
        exit(EXIT_FAILURE);
    }
    item->next = nullptr;
    item->data = malloc(bytes);
    if (!item->data) {
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
    while (*current != nullptr) {
        current = &(*current)->next;
    }
    *current = init_node(data, bytes);
}

bool remove_item(LinkedList **llist, int index) {
    LinkedList **current = llist;
    while (*current != nullptr) {
        if (*(int *)(*current)->data == index) {
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
    while (*current != nullptr && index < target_index) {
        current = &(*current)->next;
        ++index;
    }
    if (index == target_index) {
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
    while (*current != nullptr) {
        if (index == by_index) {
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
    while (lst != nullptr) {
        LinkedList *next = lst->next;
        free(lst->data);
        free(lst);
        lst = next;
    }
}

void print_list(const LinkedList *lst) {
    while (lst != nullptr) {
        printf("%d\n", *(int *)lst->data);
        lst = lst->next;
    }
}

int main() {
    LinkedList *root = nullptr;
    int arr[3] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) {
        add_item(&root, &arr[i], sizeof(int));
    }
    puts("before remove: ");
    print_list(root);

    if (insert_at_index(&root, &arr[0], sizeof(int), 1)) {
        puts("inserted 10 at index 1");
        print_list(root);
    }

    if (remove_item(&root, 20)) {
        puts("removed 20");
    }
    if (remove_index(&root, 1)) {
        puts("removed 10");
    }
    puts("after remove: ");
    print_list(root);
    free_list(root);
    return EXIT_SUCCESS;
}
