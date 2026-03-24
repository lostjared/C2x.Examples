#ifndef LL_H
#define LL_H

#include<stddef.h>

typedef struct _ListNode {
	void *data;
	size_t bytes;
	struct _ListNode *next;
} ListNode;

typedef struct _LList {
	ListNode *root;
	size_t count;
} LList;


bool list_init(LList **ll);
size_t list_count(const LList *ll);
void list_print(const LList *ll, void (print)(const void *));
bool list_insert(LList *ll, void *data, size_t size);
void list_free(LList *ll);

#endif
