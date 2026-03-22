#include"set.h"
#include<stdio.h>
#include<stdlib.h>


int main(void) {
	Set *set_value = nullptr;
	if(!set_init(&set_value)) {
		fprintf(stderr, "Error allcoating set.\n");
		return EXIT_FAILURE;
	}
	set_free(set_value);
	return EXIT_SUCCESS;
}

