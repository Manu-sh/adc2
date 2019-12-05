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


static inline bool x32_read(x32 *const restrict slice, FILE *stream) {

	const int ret = fscanf(stream, "%"SCNu32"," "%"SCNu32"," "%"SCNu32"," "%"SCNu32",", &slice->op, &slice->a, &slice->b, &slice->where);

	if (ret == EOF) return false;
	for (int_fast8_t x = sizeof(x32) / sizeof(w32) -1; x >= ret; --x)
		((w32 *)slice)[x] = W32_INVALID;

	return true;
}

static inline const x32 * x32_run(w32 *const mem32, size_t len, const x32 *const slice) {

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

	for (int a = 0; a <= 99; a++) {
		for (int b = 0; b <= 99; b++) {

			vector_w32 *vct = mx32_load("programs.txt");
			assert(sizeof(x32) == 4 * sizeof(w32));
			assert(vct);

			const int len = vector_w32_length(vct);
			w32 *raw_vct  = vector_w32_data(vct);

			assert(len > 0);

			raw_vct[1] = a;
			raw_vct[2] = b;

			// run
			for (int_fast32_t i = 0; i < len; i += sizeof(x32) / sizeof(w32)) {
				const x32 *slice = (const x32 *)(raw_vct + i);
				if (!x32_run(raw_vct, len, slice))
					break;
			}

			{ // dump
				int_fast32_t i;
				for (i = 1; i < len && raw_vct[i] != W32_INVALID; i++)
					fprintf(stdout, "%"PRIu32",", raw_vct[i - 1]);

				fprintf(stdout, "%"PRIu32"\n", raw_vct[i-1]);
			}

			if (raw_vct[0] == 19690720) {
				printf("%d-> a=%d b=%d\n", raw_vct[0], a, b);
				printf("%d-> v[a]=%d v[b]=%d\n", raw_vct[0], raw_vct[a], raw_vct[b]);

				printf("%d-> 100 * a=%d + b=%d = %d\n", raw_vct[0], a, b, 100 * a + b);
				printf("%d-> 100 * v[a]=%d + v[b]=%d = %d\n", raw_vct[0], raw_vct[a], raw_vct[b], 100 * raw_vct[a] + raw_vct[b]);
				a = b = 100; // brk
			}

			mx32_unload(vct);
		}
	}

	return 0;
}
