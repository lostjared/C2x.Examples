#include"dequeue.h"
#include<stdio.h>
#include<stdlib.h>

void release(void *ptr) {
	free(ptr);
}

void echo(void *p) {
	size_t *v = (size_t *)p;
	printf("Value is: %zu\n", *v);
}

int main(void) {
	Dequeue *dequeue = nullptr;
	if(!dequeue_init(&dequeue, release)) {
		fprintf(stderr, "Error setting up dequeue.\n");
		return EXIT_FAILURE;
	}
	size_t value = 255;
	if(!dequeue_push_front(dequeue, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into dequeue.\n");
		dequeue_free(dequeue);
		return EXIT_FAILURE;
	}
 	value = 100;
	if(!dequeue_push_front(dequeue, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into dequeue\n");
		dequeue_free(dequeue);
		return EXIT_FAILURE;
	}
	dequeue_print(dequeue, echo);
	size_t *rt_value = nullptr;
	size_t size_val;
	if(dequeue_pop(dequeue, (void **)&rt_value, &size_val)) {
		printf("%zu off top of dequeue\n", *rt_value);
		free(rt_value);
	} else {
		fprintf(stderr, "Error getting value from back of dequeue.\n");
		dequeue_free(dequeue);
		return EXIT_FAILURE;
	}
	for(size_t i = 0; i < 10; ++i) {
		if(!dequeue_push_back(dequeue, &i, sizeof(i))) {
			dequeue_free(dequeue);
			return EXIT_FAILURE;
		}
	}
	dequeue_print(dequeue, echo);
	for(size_t i = 0; i < 10; ++i) {
		if(dequeue_pop_front(dequeue, (void **)&rt_value, &size_val)) {
			printf("Front: %zu\n", *rt_value);
			free(rt_value);
		} else {
			fprintf(stderr, "Error getting front of dequeue.\n");
			dequeue_free(dequeue);
			return EXIT_FAILURE;
		}

		printf("%zu items left in dequeue\n", dequeue->count);
		
	}

	size_t peek_value;

	if(dequeue_peek_front(dequeue, &peek_value, sizeof(peek_value))) {
		printf("peeked value: %zu\n", peek_value);
	}  else {
		fprintf(stderr, "Error on pop of value: \n");
		dequeue_free(dequeue);
	}

	if(dequeue_peek_back(dequeue, &peek_value, sizeof(peek_value))) {
		printf("peek back: %zu\n", peek_value);
	} else {
		fprintf(stderr, "Error on peek.\n");
		dequeue_free(dequeue);
		return EXIT_FAILURE;
	}
	printf("%zu items left in dequeue\n", dequeue->count);
	dequeue_free(dequeue);
	return EXIT_SUCCESS;
}

