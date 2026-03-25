#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include "../mx_socket.h"
#include<pthread.h>


void *process_input(void *p) {
	int *sockfd = (int *)p;
	char buffer[4096];
	ssize_t bytes = 0;
	while((bytes = recv(*sockfd, buffer, 4096, 0)) > 0) {
		buffer[bytes-1] = 0;
		printf("%s\n", buffer);
	}
	close(*sockfd);
	printf("Connection to Socket: %d closed\n", *sockfd);
	return 0;
}

void socket_listen(const char *port) {

	MXSocket sock;
	bool active = true;

	if(mx_socket_listen(&sock, port, 5)) {
		printf("Listening on port %s\n", port);
		while(active) {
			MXSocket new_socket;
			if(mx_socket_accept(&sock, &new_socket)) {
				pthread_t id;
				if(pthread_create(&id, nullptr, process_input, &new_socket.sockfd) == 0) {
					pthread_detach(id);
				} else {
					fprintf(stderr, "Error spawning new thread...\n");
					active = false;
					break;
				}
			}
		}
	} else {
		fprintf(stderr, "Error on listen..\n");
		return;
	}
}


int main(int argc,  char **argv) {

	if(argc == 2) {
		socket_listen(argv[1]);
	}
	return EXIT_SUCCESS;
}


