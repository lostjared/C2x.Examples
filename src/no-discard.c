#include<stdio.h>
#include<stdlib.h>
#include<string.h>

[[nodiscard]] char *f_getline(char buffer[static 1], int  bytes, FILE *fptr) {
	char *r = fgets(buffer, bytes, fptr);
	if(r) {
		auto pos = strchr(r, '\n');
		if(pos) {
			*pos = '\0';
			return r;
		} 		
	}
	return nullptr;
}

int main(void) {
	static constexpr int bytes = 1024 * 4;
	char buffer[bytes];
	printf("Enter text: ");
	if(f_getline(buffer,  bytes-1, stdin)) {
		printf("%s\n", buffer);
	}
	// must capture return value
	// f_getline(buffer, bytes -1 , stdin);
	return EXIT_SUCCESS;
}


