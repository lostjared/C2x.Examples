#ifndef SET_H
#define SET_H

#include<stddef.h>

typedef struct _SetNode {
	void *data;
	size_t bytes;
	struct _SetNode *next, *prev;

} SetNode;

typedef struct _Set {
	SetNode *top, *tail;
	size_t count;
	void (*destroy)(void *);
	int (*compare)(const void *a, const void *b);
} Set;

SetNode *set_node_create(const void *data, size_t bytes);
bool set_init(Set **set, void (*destroy)(void *), int (*compare)(const void *a, const void *b));
bool set_insert(Set *set, const void *data, size_t bytes);
void set_free(Set *set);



#endif
