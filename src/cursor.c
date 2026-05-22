#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

static void set_cursor(int x, int y) {
	printf("\033[%d;%dH", y, x);
}

static void clear_screen() {
	printf("\033[2J\033[H");
}

void quit(int) {
	clear_screen();
	printf("Progress interrupted.\n");
	exit(EXIT_FAILURE);
}

int main(void) {
	if(signal(SIGINT, quit) == SIG_ERR) {
		fprintf(stderr, "Error installing handler.\n");
		return EXIT_FAILURE;
	}
	for(int i = 0; i <= 100; ++i) {
		clear_screen();
		set_cursor(0, 25);
		sleep(1);
		printf("Progress (Ctrl+C to break): %d%%\n", i);
	}
	return EXIT_SUCCESS;
}

