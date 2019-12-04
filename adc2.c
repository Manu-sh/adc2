#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {
	ADD = 1,
	MUL,
	DIE = 99
} OP;

typedef struct {
	int32_t op;
	int32_t a;
	int32_t b;
	uint32_t where;
} x32; // TODO: no padding
#include "my_collections/src/inlined/vector/vector_x32.h"


// TODO: me li sto leggendo 4 alla volta, il res è una mtx a me serve un array 1d
static inline bool x32_read(x32 *slice, FILE *stream) {
	return fscanf(stream, "%"SCNd32"," "%"SCNd32"," "%"SCNd32"," "%"SCNu32",", &slice->op, &slice->a, &slice->b, &slice->where) == 4;
}

static inline bool x32_write(const x32 *slice, FILE *stream) {
	return fprintf(stream, "%"PRId32"," "%"PRId32"," "%"PRId32"," "%"PRIu32",", slice->op, slice->a, slice->b, slice->where), true;
}

static inline const x32 * x32_run(x32 *vct, size_t len, const x32 *slice) {

	switch (slice->op) {
		case ADD:
			{
				assert(slice->where < len * sizeof(int32_t));
				assert(slice->a < len * sizeof(int32_t));
				assert(slice->b < len * sizeof(int32_t));
				int32_t *mem32 = (int32_t *)vct;
				mem32[slice->where] = mem32[slice->a] + mem32[slice->b];
				exit(1);
				return slice;
			}
		case MUL:
			{
				assert(slice->where < len * sizeof(int32_t));
				assert(slice->a < len * sizeof(int32_t));
				assert(slice->b < len * sizeof(int32_t));
				int32_t *mem32 = (int32_t *)vct;
				mem32[slice->where] = mem32[slice->a] + mem32[slice->b];
				((int32_t *)vct)[slice->where] = slice->a + slice->b;
				((int32_t *)vct)[slice->where] = slice->a * slice->b;
				return slice;
			}
		case DIE:
			// TODO: here
			for (int i = 0; i < len; i++)
				x32_write(vct + i, stdout);
			exit(0);
			// return NULL;
	}

	assert(0);
}

static inline vector_x32 * x32_mmap(const char *fname) {

	FILE *fp = fopen(fname, "r");
	assert(fp);

	vector_x32 *vct = vector_x32_new();

	for (x32 slice; x32_read(&slice, fp); vector_x32_pushBack(vct, slice));
	return vct;
}



int main() {

	// vector_x32 *vct = x32_mmap("/dev/stdin");
	vector_x32 *vct = x32_mmap("todo.txt");

	x32 *const raw_vct = vector_x32_data(vct);
	const int len = vector_x32_length(vct);

	// run
	// for (int i = 0; i < len && x32_run(raw_vct, len, raw_vct + i); i++)
	//	;


	// printf("->%d<-", raw_vct[3]);
	printf("->%d<-", sizeof(*raw_vct));
	printf("->%d<-", raw_vct[2]);


	for (int i = 0; i < len; i++)
		x32_write(raw_vct + i, stdout);

	return 0;
}
