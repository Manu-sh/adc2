#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {
	ADD = 1,
	MUL,
	DIE = 99
} OP;

typedef struct {
	uint8_t op;
	int32_t a;
	int32_t b;
	int32_t x;
} x32;

static inline bool x32_read(x32 *slice, FILE *stream) {
	return fscanf(stream, "%hhu, %d, %d, %d", &slice->op, &slice->a, &slice->b, &slice->x) == 4;
}

static inline bool x32_write(const x32 *slice, FILE *stream) {
	fprintf(stream, "%hhu, %d, %d, %d", slice->op, slice->a, slice->b, slice->x);
	return 1;
}

static inline x32 * x32_run(x32 *slice) {

	switch (slice->op) {
		case ADD:
			slice->x = slice->a + slice->b;
			return slice;
		case MUL:
			slice->x = slice->a * slice->b;
			return slice;
		case DIE:
			return NULL;
	}

	assert(0);
}


int main() {

	x32 slice;
	while (x32_read(&slice, stdin) && x32_run(&slice))
		x32_write(&slice, stdout);

	return 0;
}
