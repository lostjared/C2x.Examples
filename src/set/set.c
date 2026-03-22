#include "set.h"
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

SetNode *set_node_create(const void *data, size_t bytes) {
	SetNode *s = malloc(sizeof(*s));
	if(s == nullptr)
		return nullptr;
	s->data = malloc(bytes);
	if(s->data == nullptr) {
		free(s);
		return nullptr;
	}
	memcpy(s->data, data, bytes);
	s->bytes= bytes;
	s->next = nullptr;
	return s;
}

bool set_init(Set **set_value, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(set_value == nullptr || compare == nullptr)  
		return false;
	Set *s = malloc (sizeof(*s));
	if(s == nullptr) 
		return false;
	s->count = 0;
	s->top = s->tail = nullptr;
	s->destroy = destroy;
	s->compare = compare;
	*set_value = s;
	return true;
}

bool set_contains(const Set *set, const void *data) {
	if(set == nullptr || data == nullptr)
		return false;
	SetNode *n = set->top;
	while(n != nullptr) {
		if(set->compare(n->data, data) == 0) 
			return true;
		n = n->next;
	}
	return false;
}


bool _set_insert_no_check(Set *set, const void *data, size_t bytes) {
	if(set == nullptr || data == nullptr || bytes == 0)
		return false;
	SetNode *n = set_node_create(data, bytes);
	if(n == nullptr)
		return false;
	n->next = set->top;
	set->top = n;
	set->count++;
	return true;

}

bool set_insert(Set *set, const void *data, size_t bytes) {
	if(set == nullptr || data == nullptr || bytes == 0)
		return false;
	if(set_contains(set, data)) 
		return true;
	SetNode *n = set_node_create(data, bytes);
	if(n == nullptr)
		return false;
	n->next = set->top;
	set->top = n;
	set->count++;
	return true;
}

bool set_remove(Set *set, const void *data) {
	if(set == nullptr || data == nullptr || set->count == 0)
		return false;
	SetNode *n = set->top;
	SetNode *prev = nullptr;
	while(n != nullptr) {
		if(set->compare(n->data, data) == 0) {
			if(prev == nullptr) 
				set->top = n->next;
			 else 
 				prev->next = n->next;
			if(set->destroy != nullptr) {
				set->destroy(n->data);
			} else {
				free(n->data);
			}
			free(n);
			set->count--;
			return true;

		}
		n = n->next;
	}
	return false;
}

bool set_union(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(set1 == nullptr || set2 == nullptr || compare == nullptr)
		return false;
	if(!set_init(setu, destroy, compare)) 
		return false;
	SetNode *n = set1->top;
	while(n != nullptr) {
		if(!_set_insert_no_check(*setu, n->data, n->bytes)) {
			fprintf(stderr, "set insert failed.\n");
			set_free(*setu);
		}
		n = n->next;
	}
	n = set2->top;
	while(n != nullptr) {
		if(!set_insert(*setu, n->data, n->bytes)) {
			fprintf(stderr, "set insert failed.\n");
			set_free(*setu);
		}
		n = n->next;
	}
	return true;
}

void set_print(const Set *set, void (*echo)(const void *ptr)) {
	if(set == nullptr || echo == nullptr)
		return;

	SetNode *n = set->top;
	while(n != nullptr) {
		if(n->data != nullptr)
			echo(n->data);
		n = n->next;
	}
}

void set_free(Set *set) {
	if(set == nullptr)
		return;
	SetNode *sn = set->top;
	while(sn != nullptr) {
		SetNode *temp = sn->next;
		if(set->destroy != nullptr)
			set->destroy(sn->data);
		else
			free(sn->data);
		free(sn);
		sn = temp;
	}
	free(set);
}

