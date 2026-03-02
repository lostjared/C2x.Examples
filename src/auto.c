#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char **argv) {
	auto len = strlen("Test 1 2 3");
	printf("len is: %d bytes\n", sizeof(len));
	const auto x = argv[0];
	printf("argv[0] = %s\n", x);
	auto value = strtoull("0x1234",nullptr, 16);
	printf("%ld : %d bytes\n", value, sizeof(value));
	return EXIT_SUCCESS;
}

