#include<stdio.h>

void c_print(const char str[static 1]) {
	printf("%s\n", str);
}

int main() {
	puts("Hello, World!\n");
	char buffer[256];
	if(fgets(buffer, 256, stdin)) {
		printf("value: %s\n", buffer);
	}
	return 0;
}
