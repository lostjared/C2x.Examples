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
	Set *set_value = nullptr;
	if(!set_init(&set_value, destroy, compare)) {
		fprintf(stderr, "Error allcoating set.\n");
		return EXIT_FAILURE;
	}
	size_t data = 25;
	if(!set_insert(set_value, &data, sizeof(data))) { 
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value);
		return EXIT_FAILURE;
	}
	data = 50;
	
	if(!set_insert(set_value, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value);
		return EXIT_FAILURE;
	}
	
	if(!set_insert(set_value, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value);
		return EXIT_FAILURE;
	}
	data = 255;
	if(!set_insert(set_value, &data, sizeof(data))) {
		fprintf(stderr, "Error on insert.\n");
		set_free(set_value);
		return EXIT_FAILURE;
	}
	printf("with value 255:\n");
	set_print(set_value, echo);
	if(!set_remove(set_value, &data)) {
		fprintf(stderr, "Error on remove.\n");
		set_free(set_value);
		return EXIT_FAILURE;
	}
	printf("After remove:\n");
	set_print(set_value, echo);
	set_free(set_value);
	return EXIT_SUCCESS;
}


