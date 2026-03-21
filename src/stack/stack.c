#include"stack.h"

Node *create_node(void *data, size_t size) {
	if(data == nullptr || size == 0) return nullptr;
	Node *n = nullptr;
	if((n = malloc(sizeof(*n) )) != nullptr) {
		n->size = size;
		n->data = malloc(size);
		if(n->data == nullptr) {
			free(n);
			return nullptr;
		}
		memcpy(n->data, data, size);
		n->next = nullptr;
	        n->prev = nullptr;	
	} 
	return n;

}

bool stack_init(Stack **stack, void (*destroy)(void *)) {
	Stack *s = malloc(sizeof (*s));
	if(s == nullptr) {
		return false;
	}
	s->destroy = destroy;
	s->top = nullptr;
	*stack = s;
	return true;
}

void stack_free(Stack *stack) {
	if(stack == nullptr)
		return;
	Node *n = stack->top;
	while(n != nullptr) {
		Node *next = n->next;
		stack->destroy(n->data);
		free(n);
		n = next;
	}
	free(stack);
}

bool stack_push(Stack *stack, void *data, size_t size) {
	if(stack == nullptr || data == nullptr || size == 0) 
		return false;
	if(stack->top ==  nullptr) {
		stack->top = create_node(data, size);
		stack->top->prev = 0;
		stack->tail = stack->top;
		return true;
	}
	Node **n =  &stack->top;
	Node *prev = stack->top;
	while(*n != nullptr) {
		prev = *n;
		n = &(*n)->next;
	}
	*n = create_node(data, size);
        if(*n == nullptr) {
		fprintf(stderr, "Error on allocation.\n");
		return false;
	}
	(*n)->prev = prev;
	stack->tail = *n;
	return true;
}

bool stack_pop(Stack *stack, void **data) {
	if(stack == nullptr || data == nullptr)
		return false;
	Node *n = stack->tail;
	if(n != nullptr) {
		if(*data != nullptr) {
			stack->destroy(data);
		}
		*data = malloc(n->size);
		memcpy(*data, n->data, n->size);
		Node *prev = n->prev;
		prev->next = nullptr;
		free(n->data);
		free(n);	
	} else return false;
	return true;
}

void stack_print(Stack *stack, void (*print)(void *)) {
	if(stack == nullptr) 
		return;

	Node *n = stack->tail;
	while(n != nullptr) {
		if(n->data != nullptr)
			print(n->data);
		n = n->prev;
	}
}
