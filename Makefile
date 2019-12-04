CFLAGS=-std=c11 -O3 -Wall -Wextra -funroll-loops -ffast-math -pipe #-pedantic
.PHONY: all clean

all:
	cd my_collections/src/inlined/vector &&	sh generics-gen-vector.sh 'w32'
	cd -
	$(CC) $(CFLAGS) -o adc2 adc2.c

clean:
	rm -f adc2
