#ifndef SET_H
#define SET_H
#include<stddef.h>

typedef struct _SetNode {
	void *data;
	size_t bytes;
	size_t count;
	struct _SetNode *next;

} SetNode;

typedef struct _Set {
	SetNode *top, *tail;
	size_t count;
	void (*destroy)(void *);
	int (*compare)(const void *a, const void *b);
} Set;

bool set_init(Set **set, void (*destroy)(void *), int (*compare)(const void *a, const void *b));
bool set_contains(const Set *set, const void *data);
bool set_insert(Set *set, const void *data, size_t bytes);
bool multiset_insert(Set *set, const void *data, size_t bytes);
size_t multiset_count(Set *set, const void *data);
bool set_concat(Set *set, const Set *set_from);
bool multiset_concat(Set **set, const Set *set_from);
void multiset_print(Set *set, void (*print)(const void *, size_t));
bool set_remove(Set *set, const void *data);
bool set_union(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *));
bool set_intersection(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *));
bool set_difference(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *));
bool set_is_subset(const Set *set1, const Set *set2);
bool set_is_equal(const Set *set1, const Set *set2);
void set_print(const Set *set, void (*echo)(const void *ptr));
size_t set_count(const Set *set);
void set_free(Set *set);
#endif
