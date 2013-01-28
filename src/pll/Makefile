CC=gcc
CFLAGS=-O2 -s -std=c99 -Wall -pedantic -finline -DDEBUG

all: pll

pll: *.c
	$(CC) $(CFLAGS) -c *.c

clean:
	rm -f *~ *.o
