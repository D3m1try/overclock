CC = gcc
CFLAGS = -O2 -s -std=c99 -Wall -pedantic -finline -DDEBUG
INSTALL=/usr/bin/install -c
OBJS=i2c.o
PLL=pll/*.o
LIBS=-lm

prefix = /usr/local
sbindir = $(prefix)/sbin

.PHONY: pll clean

all: pll lfsb

pll:
	$(MAKE) -C pll all

lfsb: pll $(OBJS) lfsb.c pll.h
	$(CC) $(CFLAGS) $(OBJS) $(PLL) $(LIBS) lfsb.c -o lfsb

install: lfsb
	su -c "$(INSTALL) lfsb $(sbindir)"

uninstall:
	su -c "rm -f $(sbindir)/lfsb"

clean:
	rm -f *.o *~ lfsb tmp
	$(MAKE) -C pll clean
