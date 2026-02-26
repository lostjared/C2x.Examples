
#include<stdio.h>
#include<stdlib.h>
#include"inline.h"

void init() {
	for_each(0, 10, "Hello, World");
	for_loop(0, 10, "Goodbye, World");
}

int main() {
	init();
	return 0;
}
