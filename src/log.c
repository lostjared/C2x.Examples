#include<stdio.h>
#include<string.h>
#include<stddef.h>

// typedef function pointer
typedef void logf(size_t bytes, const char src[static bytes+1]);

void echo(size_t n, const char src[static n+1]) {
	printf("forward: %s\n", src);
}

void rev(size_t bytes, const char src[static bytes+1]) {
	printf("backward: ");
	for(size_t i = strlen(src); i > 0; i--) {
		printf("%c", src[i]);
	}
	printf("\n");
}

int main() {
	logf *func[2] = {
		[0] = echo,
		[1] = rev
	};
	int e;
	printf("Echo or Rev: 0 or 1: ");
	scanf("%d", &e);
	const char *s = "Hello, World";
	if(e >= 0 && e < 2)
	 	func[e](strlen(s), s);
	else
		puts("Invalid choice.");

	return 0;
}
