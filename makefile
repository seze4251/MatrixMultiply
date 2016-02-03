# Makefile for Matrix Multiplication

.PHONY: clean
CC = gcc
CFLAGS = -Wall -g  -c 
LFLAGS = -Wall -g  -pg
OBJS = mainFix.o matrixFix.o

matrixmult: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $<

mainFix.o: matrixFix.h

matrixFix.o: matrixFix.h

clean:
	rm -rf *.o matrixmult
