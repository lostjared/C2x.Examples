#include<stdio.h>
#include<time.h>
#include<stdint.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/time.h>

int64_t get_tick_count() {
    struct timeval val;
    gettimeofday(&val, 0);
    int64_t t = (val.tv_sec * 1000) + (val.tv_usec / 1000);
    return t;
}

int main(void) {
	srand((unsigned int)time(0));
	int i = 0;
	while(i < 3) {
		sleep((unsigned int)rand()%3);
		auto count = get_tick_count();
		printf("tick: -> %ld\n", count);
		++i;
	}
	return 0;
}


