#include"dequeue.h"
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<time.h>

#define DEQUEUE_CHECK(val) \
do { \
    if(!(val)) { \
        fprintf(stderr, "Error dequeue check failed %s:%d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

struct Event {
	size_t event_type;
	size_t index;
};


void echo(void *ptr) {
	struct Event *e = (struct Event *)ptr;
	printf("The value is: %zu:%zu\n", e->event_type, e->index);
}

int main(void)  {
	srand((unsigned int)time(nullptr));
	Dequeue *dequeue = nullptr;
	DEQUEUE_CHECK(dequeue_init(&dequeue, nullptr));
	for(size_t i = 0; i < 10; ++i) {
		struct Event e = { (size_t)rand()%255, i };
		if((i%2) == 0) {
			DEQUEUE_CHECK(dequeue_push_front(dequeue, &e, sizeof(e)));
		} else {
			DEQUEUE_CHECK(dequeue_push_back(dequeue, &e, sizeof(e)));
		}
	}
	dequeue_print_forward(dequeue, echo);
	for(size_t i = 0; i < 10; ++i) {
		struct Event value;
		size_t rt_size = 0;
		DEQUEUE_CHECK(dequeue_pop_front(dequeue, &value, sizeof(value), &rt_size));
		printf("Value popped from front: %zu : %zu\n", value.event_type, value.index);
	}
	dequeue_free(dequeue);
	return EXIT_SUCCESS;
}

