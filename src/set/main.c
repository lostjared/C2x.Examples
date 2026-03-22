#include"set.h"
#include<stdio.h>
#include<stdlib.h>

void destroy(void *ptr) {
	free(ptr);
}

int compare(const void *a, const void *b) {


	return 0;
}


int main(void) {
	Set *set_value = nullptr;
	if(!set_init(&set_value, destroy, compare)) {
		fprintf(stderr, "Error allcoating set.\n");
		return EXIT_FAILURE;
	}
	set_free(set_value);
	return EXIT_SUCCESS;
}

