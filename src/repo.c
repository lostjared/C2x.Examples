#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

int by2(int x, int y) [[reproducible]];

int main(void) {
	int x = 2;
	int y = 2;
	int value1 = by2(x, y);
	int value2 = by2(x, y);
	assert(value1 == value2);
	printf("value %d == %d\n", value1, value2);
	return 0;
}

int by2(int x, int y) [[reproducible]] {
	return x * y;
}

