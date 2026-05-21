#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(int argc, char **argv) {
	if(argc > 1) {
		int opt;
		while((opt = getopt(argc, argv, "vhi:")) != -1) {
			switch(opt) {
			case 'v':
				printf("Version 1.0\n");
				break;
			case 'h':
				printf("getopt example\n");
				break;
			case 'i':
				printf("arg value: %s\n", optarg);
				break;
			}
		}
	} else {
		printf("%s:\nv\nh\ni <value>\n", argv[0]);
	}
	return EXIT_SUCCESS;
}

