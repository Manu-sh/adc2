#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

// word_32
typedef int32_t w32;
#include "my_collections/src/inlined/vector/vector_w32.h"

typedef enum {
	ADD = 1,
	MUL,
	DIE = 99
} OP;

// ensure that there is no padding
typedef struct {
	w32 op;
	w32 a;
	w32 b;
	w32 where;
} __attribute__((__packed__)) x32;

static inline bool x32_read(x32 *slice, FILE *stream) {
	return fscanf(stream, "%"SCNd32"," "%"SCNd32"," "%"SCNd32"," "%"SCNu32",", &slice->op, &slice->a, &slice->b, &slice->where) == 4;
}

static inline bool x32_write(const x32 *slice, FILE *stream) {
	return fprintf(stream, "%"PRId32"," "%"PRId32"," "%"PRId32"," "%"PRId32",", slice->op, slice->a, slice->b, slice->where), true;
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

static inline vector_w32 * x32_mmap(const char *fname) {

	FILE *fp = fopen(fname, "r");
	vector_w32 *vct = vector_w32_new();
	x32 slice;

	assert(fp);

	while (x32_read(&slice, fp)) {
		vector_w32_pushBack(vct, slice.op);
		vector_w32_pushBack(vct, slice.a);
		vector_w32_pushBack(vct, slice.b);
		vector_w32_pushBack(vct, slice.where);
	}

	return fclose(fp), vct;
}



int main() {

	assert(sizeof(x32) == 4 * sizeof(w32));
	vector_w32 *vct = x32_mmap("todo.txt");

	w32 *const raw_vct = vector_w32_data(vct);
	const int len = vector_w32_length(vct);

	// run
	for (int i = 0; i < len; i += sizeof(x32) / sizeof(w32)) {
		const x32 *slice = (const x32 *)(raw_vct + i);
		x32_run(raw_vct, len, slice);
	}

	for (int i = 0; i < len; i += sizeof(x32) / sizeof(w32)) {
		const x32 *slice = (const x32 *)(raw_vct + i);
		x32_write(slice, stdout);
	}

	return 0;
}
