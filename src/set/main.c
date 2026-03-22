#include"set.h"
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>

void destroy(void *ptr) {
	free(ptr);
}

int compare(const void *a, const void *b) {
	const size_t *one = a;
	const size_t *two = b;
	if(*one < *two)
		return 1;
	else if(*one > *two)
		return -1;
	return 0;
}

void echo(const void *ptr) {
	if(ptr == nullptr)
		return;

	const size_t *s = ptr;
	printf("value is: %zu\n", *s);
}

int main(void) {
	Set *set_value1 = nullptr;
	if(!set_init(&set_value1, destroy, compare)) {
		fprintf(stderr, "Error allcoating set.\n");
		return EXIT_FAILURE;
	}
	size_t data = 25;
	if(!set_insert(set_value1, &data, sizeof(data))) { 
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	data = 50;
	
	if(!set_insert(set_value1, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	
	if(!set_insert(set_value1, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	data = 255;
	if(!set_insert(set_value1, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	printf("with value 255:\n");
	set_print(set_value1, echo);
	if(!set_remove(set_value1, &data)) {
		fprintf(stderr, "Error on remove.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}

	printf("After remove:\n");
	set_print(set_value1, echo);

	Set *set_value2 = nullptr;
 	if(!set_init(&set_value2, destroy, compare)) {
		fprintf(stderr, "Error allcoating set.\n");
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	data = 251;
	if(!set_insert(set_value2, &data, sizeof(data))) { 
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value2);
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	data = 50;
	if(!set_insert(set_value2, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value2);
		set_free(set_value1);
		return EXIT_FAILURE;
	}
	printf("Set #2\n");
	set_print(set_value2, echo);
	Set *set_u = nullptr;
	if(!set_union(&set_u, set_value1, set_value2, destroy, compare)) {
		set_free(set_value1);
		set_free(set_value2);
		fprintf(stderr, "Error on union of two sets.\n");
		return EXIT_FAILURE;
	}
	printf("union of two sets.\n");
	set_print(set_u, echo);
	set_free(set_value1);
	set_free(set_value2);
	set_free(set_u);
	return EXIT_SUCCESS;
}


