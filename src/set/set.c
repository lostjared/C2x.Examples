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
	s->next = s->prev = nullptr;
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

bool set_insert(Set *set, const void *data, size_t bytes) {


	return true;
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

