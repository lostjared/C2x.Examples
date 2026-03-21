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
	Queue *queue = nullptr;
	if(!queue_init(&queue, release)) {
		fprintf(stderr, "Error setting up stack.\n");
		return EXIT_FAILURE;
	}
	size_t value = 255;
	if(!queue_push_front(queue, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into stack.\n");
		queue_free(queue);
		return EXIT_FAILURE;
	}
 	value = 100;
	if(!queue_push_front(queue, &value, sizeof(value))) {
		fprintf(stderr, "Error pushing into stack\n");
		queue_free(queue);
		return EXIT_FAILURE;
	}
	queue_print(queue, echo);
	size_t *rt_value = nullptr;
	size_t size_val;
	if(queue_pop(queue, (void **)&rt_value, &size_val)) {
		printf("%zu off top of stack\n", *rt_value);
		free(rt_value);
	} else {
		fprintf(stderr, "Error getting value from back of stack.\n");
		queue_free(queue);
		return EXIT_FAILURE;
	}
	for(size_t i = 0; i < 10; ++i) {
		if(!queue_push_back(queue, &i, sizeof(i))) {
			queue_free(queue);
			return EXIT_FAILURE;
		}
	}
	queue_print(queue, echo);
	for(size_t i = 0; i < 10; ++i) {
		if(queue_pop_front(queue, (void **)&rt_value, &size_val)) {
			printf("Front: %zu\n", *rt_value);
			free(rt_value);
		} else {
			fprintf(stderr, "Error getting front of stack.\n");
			queue_free(queue);
			return EXIT_FAILURE;
		}

		printf("%zu items left in stack\n", queue->count);
		
	}

	size_t peek_value;

	if(queue_peek_front(queue, &peek_value, sizeof(peek_value))) {
		printf("peeked value: %zu\n", peek_value);
	}  else {
		fprintf(stderr, "Error on pop of value: \n");
		queue_free(queue);
	}

	if(queue_peek_back(queue, &peek_value, sizeof(peek_value))) {
		printf("peek back: %zu\n", peek_value);
	} else {
		fprintf(stderr, "Error on peek.\n");
		queue_free(queue);
		return EXIT_FAILURE;
	}
	printf("%zu items left in stack\n", queue->count);
	queue_free(queue);
	return EXIT_SUCCESS;
}

