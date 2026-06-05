#include<stdio.h>
#include<stdlib.h>
#include "../mx_socket.h"

static constexpr size_t BUFFER_SIZE = 1024;

void proc_cmd(int clientfd) {
	ssize_t bytes = 0;
	char buffer[BUFFER_SIZE] = {};
	while((bytes = read(clientfd, buffer, BUFFER_SIZE)) > 0)
		if(write(STDOUT_FILENO, buffer, (size_t)bytes) != bytes) 
			perror("write");
	if(bytes == -1) 
		perror("read");
}


int main(void) {
	MXSocket sock;
	int clientfd = -1;
	if(mx_socket_unix_listen(&sock, "/tmp/un_socket1.sock", 5)) {
		while(1) {
			clientfd = accept(sock.sockfd, 0, 0);
			if(clientfd == -1) {
				perror("accept");
				continue;
			}
			proc_cmd(clientfd);
			if(close(clientfd) == -1) 
				perror("close");
		}
	}
	return EXIT_SUCCESS;
}

