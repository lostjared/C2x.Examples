#include<stdio.h>

void safe(const char * restrict buffer, FILE * restrict unique_ptr) {
	fputs(buffer, unique_ptr);
}

void echo(const char buffer[restrict static 1]) {
	safe(buffer, stdout);
}

int main(void) {
	echo ("Hey\n");
	return 0;
}

