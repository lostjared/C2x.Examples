#include<stdio.h>
#include<string.h>

// typedef function pointer
typedef void logf(const char src[static 1]);

void echo(const char src[static 1]) {
	printf("forward: %s\n", src);
}

void rev(const char src[static 1]) {
	printf("backward: ");
	for(int i = (int)strlen(src)-1; i > 0; i--) {
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
	if(e >= 0 && e < 2)
	 	func[e]("Hello, World");
	else
		puts("Invalid choice.");

	return 0;
}
