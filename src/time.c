#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void) {
	time_t t = time(nullptr);
	char *value = ctime(&t);
	printf("%s", value);
	struct tm *t_val = localtime(&t);
	printf("Hours: %d Minutes: %d Seconds: %d\n", t_val->tm_hour, t_val->tm_min, t_val->tm_sec);
	return 0;
}

