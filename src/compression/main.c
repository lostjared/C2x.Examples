#include "huffman.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static constexpr size_t CHUNK_SIZE = 65536;

static void usage(const char *prog) {
	fprintf(stderr, "Usage: %s -c|-d -i <input> -o <output>\n", prog);
}

static bool write_u32_be(FILE *fp, uint32_t val) {
	unsigned char buf[4] = {
		(unsigned char)((val >> 24) & 0xFF),
		(unsigned char)((val >> 16) & 0xFF),
		(unsigned char)((val >> 8) & 0xFF),
		(unsigned char)(val & 0xFF),
	};
	return fwrite(buf, 1, 4, fp) == 4;
}

static bool read_u32_be(FILE *fp, uint32_t *val) {
	unsigned char buf[4];
	if (fread(buf, 1, 4, fp) != 4)
		return false;
	*val = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
	       ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return true;
}

static bool compress_file(FILE *in, FILE *out, long *total_in, long *total_out) {
	unsigned char *chunk = malloc(CHUNK_SIZE);
	if (chunk == nullptr) {
		fprintf(stderr, "Failed to allocate read buffer\n");
		return false;
	}

	*total_in = 0;
	*total_out = 0;

	while (true) {
		auto nread = fread(chunk, 1, CHUNK_SIZE, in);
		if (nread == 0)
			break;

		unsigned char *compressed = nullptr;
		int comp_size = huffman_compress(chunk, &compressed, (int)nread);
		if (comp_size < 0) {
			fprintf(stderr, "Compression failed on chunk\n");
			free(chunk);
			return false;
		}

		if (!write_u32_be(out, (uint32_t)comp_size)) {
			fprintf(stderr, "Write error\n");
			free(compressed);
			free(chunk);
			return false;
		}
		if (fwrite(compressed, 1, (size_t)comp_size, out) != (size_t)comp_size) {
			fprintf(stderr, "Write error\n");
			free(compressed);
			free(chunk);
			return false;
		}

		*total_in += (long)nread;
		*total_out += 4 + (long)comp_size;
		free(compressed);
	}

	if (!write_u32_be(out, 0)) {
		fprintf(stderr, "Write error\n");
		free(chunk);
		return false;
	}
	*total_out += 4;

	free(chunk);
	return !ferror(in);
}

static bool decompress_file(FILE *in, FILE *out, long *total_in, long *total_out) {
	*total_in = 0;
	*total_out = 0;

	while (true) {
		uint32_t chunk_size = 0;
		if (!read_u32_be(in, &chunk_size)) {
			fprintf(stderr, "Read error on chunk header\n");
			return false;
		}
		*total_in += 4;
		if (chunk_size == 0)
			break;

		unsigned char *chunk = malloc(chunk_size);
		if (chunk == nullptr) {
			fprintf(stderr, "Failed to allocate %u bytes\n", chunk_size);
			return false;
		}
		if (fread(chunk, 1, chunk_size, in) != chunk_size) {
			fprintf(stderr, "Read error on chunk data\n");
			free(chunk);
			return false;
		}
		*total_in += (long)chunk_size;

		unsigned char *decompressed = nullptr;
		int decomp_size = huffman_decompress(chunk, &decompressed);
		free(chunk);
		if (decomp_size < 0) {
			fprintf(stderr, "Decompression failed on chunk\n");
			return false;
		}

		if (fwrite(decompressed, 1, (size_t)decomp_size, out) != (size_t)decomp_size) {
			fprintf(stderr, "Write error\n");
			free(decompressed);
			return false;
		}
		*total_out += (long)decomp_size;
		free(decompressed);
	}

	return true;
}

static void print_stats(long in_size, long out_size) {
	printf("%ld -> %ld bytes", in_size, out_size);
	if (in_size > 0) {
		double ratio = ((double)(in_size - out_size) / (double)in_size) * 100.0;
		if (out_size <= in_size)
			printf(" (deflated %.1f%%)\n", ratio);
		else
			printf(" (inflated %.1f%%)\n", -ratio);
	} else {
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	int mode = 0;
	const char *infile = nullptr;
	const char *outfile = nullptr;

	int opt;
	while ((opt = getopt(argc, argv, "cdi:o:")) != -1) {
		switch (opt) {
		case 'c':
			mode = 1;
			break;
		case 'd':
			mode = 2;
			break;
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (mode == 0 || infile == nullptr || outfile == nullptr) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	size_t outlen = strlen(outfile);
	if (outlen > 4090) {
		fprintf(stderr, "Output path too long\n");
		return EXIT_FAILURE;
	}
	char tmpfile[4096];
	snprintf(tmpfile, sizeof(tmpfile), "%s.tmp", outfile);

	FILE *fin = fopen(infile, "rb");
	if (fin == nullptr) {
		fprintf(stderr, "Cannot open %s\n", infile);
		return EXIT_FAILURE;
	}
	FILE *fout = fopen(tmpfile, "wb");
	if (fout == nullptr) {
		fprintf(stderr, "Cannot open %s for writing\n", tmpfile);
		fclose(fin);
		return EXIT_FAILURE;
	}

	long total_in = 0;
	long total_out = 0;
	bool ok;

	if (mode == 1)
		ok = compress_file(fin, fout, &total_in, &total_out);
	else
		ok = decompress_file(fin, fout, &total_in, &total_out);

	fclose(fin);
	fclose(fout);

	if (!ok) {
		remove(tmpfile);
		return EXIT_FAILURE;
	}

	if (rename(tmpfile, outfile) != 0) {
		fprintf(stderr, "Failed to rename %s to %s\n", tmpfile, outfile);
		remove(tmpfile);
		return EXIT_FAILURE;
	}

	print_stats(total_in, total_out);
	return EXIT_SUCCESS;
}
