#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void printOut(const char *src, int times) {
	size_t length = strlen(src);
	asm volatile("1: \n\t"
	"movq $1, %%rax \n\t"
	"movq $1, %%rdi \n\n"
	"movq %[str], %%rsi \n\t"
	"movq %[len], %%rdx \n\t"
	"syscall \n\t"
	"decl %[count] \n\t"
	"jnz 1b \n\t"
	: [count] "+r" (times)
	: [str] "r" (src), [len] "r" (length)
	: "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory"
	);
}

int main(void) {
	printOut("Hello Wrld\n", 10);
	return EXIT_SUCCESS;
}

