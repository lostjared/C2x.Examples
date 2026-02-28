#include<stdio.h>

int getResult(int value, int add [[maybe_unused]]) {
	if(value > 0) {
		return value + add;
	}
	return value;
}


int main() {
	int rt_value = getResult(10, 1);
	printf("value 1: %d\n", rt_value);
	rt_value = getResult(-10, 10);
	printf("value 2: %d\n", rt_value);
	return 0;
}
