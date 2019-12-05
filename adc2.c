#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

typedef uint32_t w32; // word32
#include "my_collections/src/inlined/vector/vector_w32.h"

#define W32_INVALID UINT32_MAX
#define ADD 1
#define MUL 2
#define DIE 99
#define CHUNK_LEN (sizeof(x32) / sizeof(w32))

// ensure that there is no padding here
typedef struct {
	w32 op;
	w32 a;
	w32 b;
	w32 where;
} __attribute__((aligned( sizeof(w32) ))) __attribute__((__packed__)) x32;


static inline bool x32_read(x32 *const restrict slice, FILE *stream) {

	const int ret = fscanf(stream, "%"SCNu32"," "%"SCNu32"," "%"SCNu32"," "%"SCNu32",", &slice->op, &slice->a, &slice->b, &slice->where);

	if (ret == EOF) return false;
	for (int_fast8_t x = CHUNK_LEN-1; x >= ret; --x)
		((w32 *)slice)[x] = W32_INVALID;

	return true;
}

static inline const x32 * x32_run(w32 *const mem32, int_fast32_t len, const x32 *const slice) {

	switch (slice->op) {
		case ADD:
			assert(slice->where < len);
			assert(slice->a     < len);
			assert(slice->b     < len);
			mem32[slice->where] = mem32[slice->a] + mem32[slice->b];
			return slice;
		case MUL:
			assert(slice->where < len);
			assert(slice->a     < len);
			assert(slice->b     < len);
			mem32[slice->where] = mem32[slice->a] * mem32[slice->b];
			return slice;
		case DIE:
			return NULL;
	}

	assert(0);
}

static inline vector_w32 * mx32_load(const char *fname) {

	FILE *fp = fopen(fname, "r");
	vector_w32 *vct = vector_w32_new();
	assert(fp);

	for (x32 slice; x32_read(&slice, fp);)
		vector_w32_pushBackAll(vct, (w32 *)&slice, CHUNK_LEN);

	return fclose(fp), vct;
}

static inline void mx32_unload(vector_w32 *mem) {
	vector_w32_free(mem);
}

static inline void mx32_dump(const vector_w32 *mem) {

	const w32 *const raw_vct = vector_w32_data(mem);
	const int_fast32_t len = vector_w32_length(mem);
	int_fast32_t i;

	for (i = 1; i < len && raw_vct[i] != W32_INVALID; i++)
		fprintf(stdout, "%"PRIu32",", raw_vct[i - 1]);

	fprintf(stdout, "%"PRIu32"\n", raw_vct[i - 1]);
}

static inline void mx32_exec(vector_w32 *mem) {

	w32 *const raw_mem = vector_w32_data(mem);
	const int_fast32_t len = vector_w32_length(mem);

	for (int_fast32_t i = 0; i < len; i += CHUNK_LEN) {
		const x32 *slice = (const x32 *)(raw_mem + i);
		if (!x32_run(raw_mem, len, slice))
			break;
	}
}

int main() {

	vector_w32 *restrict const vct = mx32_load("program.txt");
	const int_fast32_t len = vector_w32_length(vct);
	const w32 *restrict const raw_vct  = vector_w32_data(vct);
	assert(len > 0);

	for (int_fast8_t a = 0; a <= 99; a++) {
		for (int_fast8_t b = 0; b <= 99; b++) {

			// mx32_copy
			vector_w32 *mem = vector_w32_new();
			vector_w32_pushBackAll(mem, raw_vct, len);
			w32 *raw_mem = vector_w32_data(mem);

			assert(mem);

			raw_mem[1] = a;
			raw_mem[2] = b;

			mx32_exec(mem);
			// mx32_dump(mem);

			if (raw_mem[0] == 19690720) {
				printf("%d\n", 100 * a + b);
				a = b = 100; // brk
			}

			vector_w32_free(mem);
		}
	}

	mx32_unload(vct);
	return 0;
}
