#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void echo_ip(const char *device) {
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *temp_addr = nullptr;
    int pass = getifaddrs(&interfaces);
    if (pass == 0) {
        temp_addr = interfaces;
        while (temp_addr != nullptr) {
            if (temp_addr->ifa_addr != nullptr) {
                if (temp_addr->ifa_addr->sa_family == AF_INET) {
                    void *temp_ptr = &((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr;
                    char address[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, temp_ptr, address, INET_ADDRSTRLEN);
                    if (device == nullptr || strcmp(temp_addr->ifa_name, device) == 0)
                        printf("interface: %-8s IP Address: %s\n", temp_addr->ifa_name, address);
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    } else {
        fprintf(stderr, "address lookup failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    freeifaddrs(interfaces);
}

int main(int argc, char *argv[argc + 1]) {

    if (argc == 1)
        echo_ip(nullptr);
    else if (argc == 2)
        echo_ip(argv[1]);

    return EXIT_SUCCESS;
}
