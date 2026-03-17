#include<stdio.h>
#include<stdlib.h>

void print_integer(int value) {
	printf("integer: %d\n", value);
}

void print_float(float value) {
	printf("float: %g\n", value);
}


void print_str(const char *s) {
	printf("str: %s\n", s);
}

#define print(VALUE)  \
_Generic((VALUE),     \
	float: print_float, \
	int: print_integer, \
	default: print_str)(VALUE)



int main(void) {
	print(10);
	print(10.5f);
	print("Hello World");
	return 0;
}

