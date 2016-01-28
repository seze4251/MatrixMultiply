# Makefile for Matrix Multiplication

.PHONY: clean
CC = gcc
CFLAGS = -Wall -g  -c -O3
LFLAGS = -Wall -g  -O3 -pg
OBJS = main.o matrixDoublePointer.o

matrixmult: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $<

main.o: matrixDoublePointer.h

matrixDoublePoiner.o: matrixDoublePointer.h

clean:
	rm -rf *.o matrixmult
