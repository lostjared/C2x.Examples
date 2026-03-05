
#include<stdio.h>
#include<stdlib.h>
#include"inline.h"

inline void init() {
	for_each(0, 10, "Hello, World");
	for_loop(0, 10, "Goodbye, World");
}

inline void print(const char [static 1]);

int main() {
	init();
	print("Hello, World inline");
	return 0;
}


