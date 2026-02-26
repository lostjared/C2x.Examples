#include<stdio.h>
#include<stdlib.h>


int value(int x, int y) [[unsequenced]] {
	return x * y;
}


int main() {
	int v = value(10, 25);
	printf("value is: %d\n", v);
	return 0;
}
