#include<stdio.h>
#include<stdlib.h>
#include<locale.h>
#include<time.h>



int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "Error must provide locale\n");
		return EXIT_FAILURE;
	}
	setlocale(LC_ALL, argv[1]);
	time_t t = time(nullptr);
	struct tm *i = localtime(&t);
	char date[256];
	strftime(date, sizeof(date), "%c", i);
	printf("Current time: %s\n", date);
	return EXIT_SUCCESS;
}

