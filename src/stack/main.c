#include"stack.h"
#include<stdio.h>
#include<stdlib.h>

void release(void *ptr) {
	free(ptr);
}

void echo(void *p) {
	size_t *v = (size_t *)p;
	printf("%zu : value\n", *v);
}

int main(void) {
	Stack *stack = nullptr;
	if(!stack_init(&stack, release)) {
		fprintf(stderr, "Error setting up stack.\n");
		return EXIT_FAILURE;
	}
	size_t value = 255;
	if(!stack_push(stack, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into stack.\n");
		stack_free(stack);
		return EXIT_FAILURE;
	}
 	value = 100;
	if(!stack_push(stack, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into stack\n");
		stack_free(stack);
		return EXIT_FAILURE;
	}
	stack_print(stack, echo);
	size_t *rt_value = nullptr;
	if(stack_pop(stack, (void **)&rt_value)) {
		printf("%zu off top of stack\n", *rt_value);
		free(rt_value);
	} else {
		fprintf(stderr, "Error getting value from back of stack.\n");
		stack_free(stack);
		return EXIT_FAILURE;
	}
	for(size_t i = 0; i < 10; ++i) {
		if(!stack_push(stack, &i, sizeof(i))) {
			stack_free(stack);
			return EXIT_FAILURE;
		}
	}
	stack_print(stack, echo);
	stack_free(stack);
	return EXIT_SUCCESS;
}

