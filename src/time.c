#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void) {
	time_t t = time(nullptr);
	char *value = ctime(&t);
	printf("%s", value);
	return 0;
}

