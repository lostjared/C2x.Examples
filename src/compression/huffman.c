#include "huffman.h"
#include "bintree.h"
#include "heap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static constexpr size_t BYTE_VALUES = 256;

static int bit_get(const unsigned char *bits, int pos) {
	unsigned char mask = 0x80;
	for(int i = 0; i < (pos % 8); ++i) {
		mask = mask >> 1;
	}
	return (((mask & bits[(size_t)(pos / 8)]) == mask) ? 1 : 0);

}

static void bit_set(unsigned char *bits, int pos, int state) {
	unsigned char mask = 0x80;
	for(int i = 0; i < (pos % 8); ++i) 
		mask = mask >> 1;

	if(state)
		bits[pos / 8] = bits[pos / 8] | mask;
	else
		bits[pos / 8] = bits[pos / 8] & (~mask);
}


static int huff_compare(const void *a, const void *b) {
	const Node *na = a;
	const Node *nb = b;
	const HuffNode *ha = na->data;
	const HuffNode *hb = nb->data;
	if (ha->freq < hb->freq)
		return 1;
	else if (ha->freq > hb->freq)
		return -1;
	return 0;
}

static void destroy_tree(void *tree) {
	Node *n = tree;
	if (n == nullptr)
		return;
	destroy_tree(n->left);
	destroy_tree(n->right);
	free(n->data);
	free(n);
}

static void build_codes(const Node *node, uint32_t code, unsigned char depth,
                        HuffCode table[static BYTE_VALUES]) {
	if (node == nullptr)
		return;
	if (node->left == nullptr && node->right == nullptr) {
		const HuffNode *hn = node->data;
		table[hn->symbol].code = code;
		table[hn->symbol].size = depth;
		table[hn->symbol].used = 1;
		return;
	}
	if (depth >= 32)
		return;
	build_codes(node->left, code << 1,
	            (unsigned char)(depth + 1), table);
	build_codes(node->right, (code << 1) | 1,
	            (unsigned char)(depth + 1), table);
}

static Node *build_huff_tree(const int freqs[static BYTE_VALUES]) {
	Heap heap;
	if (!heap_init(&heap, huff_compare, destroy_tree))
		return nullptr;

	for (size_t i = 0; i < BYTE_VALUES; ++i) {
		if (freqs[i] == 0)
			continue;
		HuffNode hn = { .symbol = (unsigned char)i, .freq = freqs[i] };
		Node *leaf = createNode(&hn, sizeof(HuffNode));
		if (leaf == nullptr) {
			heap_destroy(&heap);
			return nullptr;
		}
		if (!heap_insert(&heap, leaf)) {
			destroy_tree(leaf);
			heap_destroy(&heap);
			return nullptr;
		}
	}

	if (heap.size == 0) {
		heap_destroy(&heap);
		return nullptr;
	}

	while (heap.size > 1) {
		void *va = nullptr;
		void *vb = nullptr;
		if (!heap_extract(&heap, &va)) {
			heap_destroy(&heap);
			return nullptr;
		}
		if (!heap_extract(&heap, &vb)) {
			destroy_tree(va);
			heap_destroy(&heap);
			return nullptr;
		}
		Node *left = va;
		Node *right = vb;
		const HuffNode *ha = left->data;
		const HuffNode *hb = right->data;
		HuffNode merged = { .symbol = 0, .freq = ha->freq + hb->freq };
		Node *parent = createNode(&merged, sizeof(HuffNode));
		if (parent == nullptr) {
			destroy_tree(left);
			destroy_tree(right);
			heap_destroy(&heap);
			return nullptr;
		}
		parent->left = left;
		parent->right = right;
		if (!heap_insert(&heap, parent)) {
			destroy_tree(parent);
			heap_destroy(&heap);
			return nullptr;
		}
	}

	void *root = nullptr;
	if (!heap_extract(&heap, &root)) {
		heap_destroy(&heap);
		return nullptr;
	}
	heap_destroy(&heap);
	return root;
}

int huffman_compress(const unsigned char *original, unsigned char **compressed, int size) {
	if (original == nullptr || compressed == nullptr || size <= 0)
		return -1;

	int freqs[BYTE_VALUES] = {0};
	for (int i = 0; i < size; ++i)
		freqs[original[i]]++;

	int num_symbols = 0;
	for (size_t i = 0; i < BYTE_VALUES; ++i) {
		if (freqs[i] > 0)
			num_symbols++;
	}
	if (num_symbols == 0)
		return -1;

	Node *root = build_huff_tree(freqs);
	if (root == nullptr)
		return -1;

	HuffCode table[BYTE_VALUES];
	memset(table, 0, sizeof(table));
	if (num_symbols == 1) {
		const HuffNode *hn = root->data;
		table[hn->symbol].code = 0;
		table[hn->symbol].size = 1;
		table[hn->symbol].used = 1;
	} else {
		build_codes(root, 0, 0, table);
	}

	int64_t total_bits_64 = 0;
	for (size_t i = 0; i < BYTE_VALUES; ++i) {
		if (table[i].used)
			total_bits_64 += (int64_t)freqs[i] * table[i].size;
	}
	if (total_bits_64 > INT32_MAX) {
		destroy_tree(root);
		return -1;
	}
	auto total_bits = (int)total_bits_64;
	int encoded_bytes = (total_bits + 7) / 8;

	int header_size = 4 + 2 + num_symbols * 5 + 4;
	int comp_size = header_size + encoded_bytes;

	unsigned char *out = calloc(1, (size_t)comp_size);
	if (out == nullptr) {
		fprintf(stderr, "Failed to allocate compressed buffer\n");
		destroy_tree(root);
		return -1;
	}

	int pos = 0;
	auto orig_u = (uint32_t)size;
	out[pos++] = (unsigned char)((orig_u >> 24) & 0xFF);
	out[pos++] = (unsigned char)((orig_u >> 16) & 0xFF);
	out[pos++] = (unsigned char)((orig_u >> 8) & 0xFF);
	out[pos++] = (unsigned char)(orig_u & 0xFF);
	out[pos++] = (unsigned char)((num_symbols >> 8) & 0xFF);
	out[pos++] = (unsigned char)(num_symbols & 0xFF);
	for (size_t i = 0; i < BYTE_VALUES; ++i) {
		if (freqs[i] > 0) {
			auto freq_u = (uint32_t)freqs[i];
			out[pos++] = (unsigned char)i;
			out[pos++] = (unsigned char)((freq_u >> 24) & 0xFF);
			out[pos++] = (unsigned char)((freq_u >> 16) & 0xFF);
			out[pos++] = (unsigned char)((freq_u >> 8) & 0xFF);
			out[pos++] = (unsigned char)(freq_u & 0xFF);
		}
	}

	auto bits_u = (uint32_t)total_bits;
	out[pos++] = (unsigned char)((bits_u >> 24) & 0xFF);
	out[pos++] = (unsigned char)((bits_u >> 16) & 0xFF);
	out[pos++] = (unsigned char)((bits_u >> 8) & 0xFF);
	out[pos++] = (unsigned char)(bits_u & 0xFF);

	unsigned char *encoded = out + pos;
	int bit_pos = 0;
	for (int i = 0; i < size; ++i) {
		HuffCode *hc = &table[original[i]];
		for (int b = hc->size - 1; b >= 0; --b) {
			int bit_val = (hc->code >> b) & 1;
			bit_set(encoded, bit_pos, bit_val);
			bit_pos++;
		}
	}

	destroy_tree(root);
	*compressed = out;
	return comp_size;
}

int huffman_decompress(const unsigned char *compressed, unsigned char **original) {
	if (compressed == nullptr || original == nullptr)
		return -1;

	const unsigned char *in = compressed;
	int pos = 0;

	auto orig_size = (int)(((uint32_t)in[pos] << 24) | ((uint32_t)in[pos + 1] << 16) |
	                       ((uint32_t)in[pos + 2] << 8) | (uint32_t)in[pos + 3]);
	pos += 4;
	if (orig_size <= 0)
		return -1;

	int num_symbols = (in[pos] << 8) | in[pos + 1];
	pos += 2;
	if (num_symbols <= 0 || num_symbols > (int)BYTE_VALUES)
		return -1;

	int freqs[BYTE_VALUES] = {0};
	for (int i = 0; i < num_symbols; ++i) {
		unsigned char sym = in[pos++];
		auto freq = (int)(((uint32_t)in[pos] << 24) | ((uint32_t)in[pos + 1] << 16) |
		                   ((uint32_t)in[pos + 2] << 8) | (uint32_t)in[pos + 3]);
		pos += 4;
		freqs[sym] = freq;
	}

	auto total_bits = (int)(((uint32_t)in[pos] << 24) | ((uint32_t)in[pos + 1] << 16) |
	                        ((uint32_t)in[pos + 2] << 8) | (uint32_t)in[pos + 3]);
	pos += 4;

	Node *root = build_huff_tree(freqs);
	if (root == nullptr)
		return -1;

	unsigned char *out = malloc((size_t)orig_size);
	if (out == nullptr) {
		fprintf(stderr, "Failed to allocate decompressed buffer\n");
		destroy_tree(root);
		return -1;
	}

	const unsigned char *enc = in + pos;

	if (num_symbols == 1) {
		const HuffNode *hn = root->data;
		memset(out, hn->symbol, (size_t)orig_size);
	} else {
		const Node *current = root;
		int out_pos = 0;
		int bit_pos = 0;
		while (out_pos < orig_size && bit_pos < total_bits) {
			int bit = bit_get(enc, bit_pos);
			bit_pos++;
			if (bit == 0)
				current = current->left;
			else
				current = current->right;
			if (current == nullptr) {
				free(out);
				destroy_tree(root);
				return -1;
			}
			if (current->left == nullptr && current->right == nullptr) {
				const HuffNode *hn = current->data;
				out[out_pos++] = hn->symbol;
				current = root;
			}
		}
		if (out_pos != orig_size) {
			free(out);
			destroy_tree(root);
			return -1;
		}
	}

	destroy_tree(root);
	*original = out;
	return orig_size;
}
