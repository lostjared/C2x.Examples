#include "set.h"
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

SetNode *set_node_create(const void *data, size_t bytes) {
	SetNode *s = malloc(sizeof(SetNode));
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
	s->count = 1;
	return s;

}

bool set_init(Set **set_value, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(set_value == nullptr || compare == nullptr)  
		return false;
	Set *s = malloc (sizeof(*s));
	if(s == nullptr) 
		return false;
	s->count = 0;
	s->top = nullptr;
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

bool _multiset_insert_no_check(Set *set, const void *data, size_t bytes, size_t count) {
	if(set == nullptr || data == nullptr || bytes == 0)
		return false;
	SetNode *n = set_node_create(data, bytes);
	if(n == nullptr)
		return false;
	n->count = count;
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


bool multiset_insert(Set *set, const void *data, size_t bytes) {
	if(set == nullptr || data == nullptr || bytes == 0) 
		return false;
	SetNode *n = set->top;
	while(n != nullptr) {
		if(set->compare(n->data, data) == 0) {
			++n->count;
			return true;
		}
		n = n->next;
	}
	SetNode *node = set_node_create(data, bytes);
	if(node == nullptr) 
		return false;
	
	node->next = set->top;
	set->top = node;
	set->count++;
	return true;
}

bool _multiset_insert(Set *set, const void *data, size_t bytes, size_t count) {
	if(set == nullptr || data == nullptr || bytes == 0) 
		return false;
	SetNode *n = set->top;
	while(n != nullptr) {
		if(set->compare(n->data, data) == 0) {
			n->count += count;
			return true;
		}
		n = n->next;
	}
	SetNode *node = set_node_create(data, bytes);
	if(node == nullptr)
		return false;

	node->count = count;
	node->next = set->top;
	set->top = node;
	set->count++;
	return true; 
}


size_t multiset_count(const Set *set, const void *data) {
   	if(set == nullptr || data == nullptr) 
		return 0;
	SetNode *n = set->top;
	while(n != nullptr) {
		if(set->compare(n->data, data) == 0) {
			return n->count;
		}
		n = n->next;
	}
	return 0;
}


void multiset_print(const Set *set, void (*echo)(const void *, size_t)) {
	if(set == nullptr || echo == nullptr)
		return;
	SetNode *n = set->top;
	while(n != nullptr) {
		echo(n->data, n->count);
		n = n->next;
	}
}

bool set_concat(Set *set, const Set *set_from) {
	if(set == nullptr || set_from == nullptr)
		return false;


	if(set_from->count == 0)
		return true;
	SetNode *n = set_from->top;
	while(n != nullptr) {
		if(!set_insert(set, n->data, n->bytes)) {
			return false;
		}
		n = n->next;
	}
	return true;
}

bool multiset_concat(Set *set, const Set *set_from) {
	if(set == nullptr || set_from == nullptr)
		return false;
	if(set_from->count == 0)
		return true;
	SetNode *n = set_from->top;
	while(n != nullptr) {
		if(!_multiset_insert(set, n->data, n->bytes, n->count)) {
			return false;
		}
		n = n->next;
	}
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
		prev = n;
		n = n->next;
	}
	return false;
}

bool set_union(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(setu == nullptr || set1 == nullptr || set2 == nullptr || compare == nullptr)
		return false;

	if(!set_init(setu, destroy, compare)) 
		return false;
	SetNode *n = set1->top;
	while(n != nullptr) {
		if(!_set_insert_no_check(*setu, n->data, n->bytes)) {
			fprintf(stderr, "set insert failed.\n");
			set_free(*setu);
			return false;
		}
		n = n->next;
	}
	n = set2->top;
	while(n != nullptr) {
		if(!set_insert(*setu, n->data, n->bytes)) {
			fprintf(stderr, "set insert failed.\n");
			set_free(*setu);
			return false;
		}
		n = n->next;
	}
	return true;
}

bool set_intersection(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(setu == nullptr || set1 == nullptr || set2 == nullptr || compare == nullptr)  
		return false;

	if(!set_init(setu, destroy, compare)) {
		return false;
	}

	if(set2->count == 0)
		return true;

	SetNode *n = set1->top;
	while(n != nullptr) {
		if(set_contains(set2, n->data)) { 
			if(!_set_insert_no_check(*setu, n->data, n->bytes)) {
				fprintf(stderr, "Error on insertion..\n");
				set_free(*setu);
				return false;
			}
		}
		n = n->next;
	}
	return true;
}

bool set_difference(Set **setu, const Set *set1, const Set *set2, void (*destroy)(void *), int (*compare)(const void *, const void *)) {
	if(setu == nullptr || set1 == nullptr || set2 == nullptr || compare == nullptr)
		return false;

	if(!set_init(setu, destroy, compare)) {
		fprintf(stderr, "Error on init of set\n");
		return false;
	}
	SetNode *n = set1->top;
	while(n != nullptr) {
		if(!set_contains(set2, n->data)) {
			if(!_set_insert_no_check(*setu, n->data, n->bytes)) {
				fprintf(stderr, "Error on insertion.\n");
				set_free(*setu);
				return false;
			}
		}
		n = n->next;
	}
	return true;
}

bool set_is_subset(const Set *set1, const Set *set2) {

	if(set1 == nullptr || set2 == nullptr)
		return false;

	if(set1->count == 0)
		return true;
	
	if(set1->count > set2->count)
		return false;
	
	SetNode *n = set1->top;

	while(n != nullptr) {
		if(!set_contains(set2, n->data)) 
			return false;
		
		n = n->next;
	}
	return true;
}

bool set_is_equal(const Set *set1, const Set *set2) {

	 if(set1 == nullptr || set2 == nullptr)
		 return false;

	 if(set1->count == 0 && set2->count == 0)
		 return true;

	if(set1->top == nullptr || set2->top == nullptr)
		return false;

	 if(set1->count != set2->count)
		 return false;

	 if(set_is_subset(set1, set2)) 
		 return true;

	return false;
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

size_t set_count(const Set *set) {
	if(set == nullptr)
		return 0;

	return set->count;
}

