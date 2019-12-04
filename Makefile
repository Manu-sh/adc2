CFLAGS=-std=c11 -O3 -Wall -Wextra -funroll-loops -ffast-math -pipe #-pedantic

.PHONY: all clean

all:
	$(CC) $(CFLAGS) -o adc2 adc2.c

clean:
	rm -f adc2
