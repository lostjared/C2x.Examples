#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stddef.h>

static uint64_t fnv1a64_bytes(const void *data, size_t n, uint64_t h) [[unsequenced]] {
	const uint8_t *p = (const uint8_t *)data;
	for (size_t i = 0; i < n; ++i) {
	    h ^= p[i];
	    h *= 1099511628211ull;
	}
	return h;
}

static uint64_t fnv1a64_file(const char filepath[static 1]) {
	FILE *f = fopen(filepath, "rb");
        if(!f) return 0;
        uint64_t h = 1469598103934665603ull;
	static constexpr size_t BUF_SIZE = 16384;
        char buf[BUF_SIZE];
	size_t bytes = 0;

        while ((bytes = fread(buf, sizeof(char), BUF_SIZE, f)) != 0) {
            h = fnv1a64_bytes(buf, bytes, h);
        }
        fclose(f);
	return h;
}


int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Incorrect arguments.\n");
		return EXIT_FAILURE;
	}
	uint64_t hash_value = fnv1a64_file(argv[1]);
	printf("%s: 0x%lx\n", argv[1], hash_value);
	return EXIT_SUCCESS;
}
