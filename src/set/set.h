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
} Set;

SetNode *set_node_create(const void *data, size_t bytes);
bool set_init(Set **set);
void set_free(Set *set);


#endif
