#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

typedef uint32_t w32; // word32
#include "my_collections/src/inlined/vector/vector_w32.h"

#define W32_INVALID UINT32_MAX

enum {
	ADD = 1,
	MUL,
	DIE = 99
};

// ensure that there is no padding here
typedef struct {
	w32 op;
	w32 a;
	w32 b;
	w32 where;
} __attribute__((__packed__)) x32;


static inline bool x32_read(x32 *slice, FILE *stream) {

	const int ret = fscanf(stream, "%"SCNu32"," "%"SCNu32"," "%"SCNu32"," "%"SCNu32",", &slice->op, &slice->a, &slice->b, &slice->where);

	if (ret == EOF) return false;
	for (int x = sizeof(x32) / sizeof(w32) -1; x >= ret; --x)
		((w32 *)slice)[x] = W32_INVALID;

	return true;
}

static inline const x32 * x32_run(w32 *mem32, size_t len, const x32 *slice) {

	switch (slice->op) {
		case ADD:
			assert((size_t)slice->where < len);
			assert((size_t)slice->a     < len);
			assert((size_t)slice->b     < len);
			mem32[slice->where] = mem32[slice->a] + mem32[slice->b];
			return slice;
		case MUL:
			assert((size_t)slice->where < len);
			assert((size_t)slice->a     < len);
			assert((size_t)slice->b     < len);
			mem32[slice->where] = mem32[slice->a] * mem32[slice->b];
			return slice;
		case DIE:
			return NULL;
	}

	assert(0);
}

static inline vector_w32 * mx32_load(const char *fname) {

	assert(sizeof(x32) == 4 * sizeof(w32));

	FILE *fp = fopen(fname, "r");
	vector_w32 *vct = vector_w32_new();
	assert(fp);

	for (x32 slice; x32_read(&slice, fp);) {
		vector_w32_pushBack(vct, slice.op);
		vector_w32_pushBack(vct, slice.a);
		vector_w32_pushBack(vct, slice.b);
		vector_w32_pushBack(vct, slice.where);
	}

	return fclose(fp), vct;
}

static inline void mx32_unload(vector_w32 *mem) {
	vector_w32_free(mem);
}


int main() {

	vector_w32 *vct = mx32_load("programs.txt");
	assert(sizeof(x32) == 4 * sizeof(w32));
	assert(vct);

	const int len = vector_w32_length(vct);
	w32 *raw_vct  = vector_w32_data(vct);

	assert(len > 0);

	// raw_vct[1] = 12;
	// raw_vct[2] = 2;

	// run
	for (int i = 0; i < len; i += sizeof(x32) / sizeof(w32)) {
		const x32 *slice = (const x32 *)(raw_vct + i);
		if (!x32_run(raw_vct, len, slice))
			break;
	}

	{ // dump
		int i;
		for (i = 1; i < len && raw_vct[i] != W32_INVALID; i++)
			fprintf(stdout, "%"PRIu32",", raw_vct[i - 1]);

		fprintf(stdout, "%"PRIu32"\n", raw_vct[i-1]);
	}

	mx32_unload(vct);
	return 0;
}
