#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<ifaddrs.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>

int main(void) {
	struct ifaddrs *interfaces = nullptr;
	struct ifaddrs *temp_addr = nullptr;
	int pass = getifaddrs(&interfaces);
	if(pass == 0) {
		temp_addr = interfaces;
		while(temp_addr != nullptr) {
			if(temp_addr->ifa_addr != nullptr) {
				if(temp_addr->ifa_addr->sa_family == AF_INET) {
					void *temp_ptr =  &((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr;
					char address[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, temp_ptr, address, INET_ADDRSTRLEN);
					printf("interface: %-8s IP Address: %s\n", temp_addr->ifa_name, address);
				}
			}
			temp_addr = temp_addr->ifa_next;
		}
	} else {
		fprintf(stderr, "address lookup failed: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	freeifaddrs(interfaces);
	return EXIT_SUCCESS;
}

