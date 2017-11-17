CC ?= gcc
CXX ?= g++
CFLAGS ?= -Os
INSTALLDIR ?= /usr/bin

all: ptr2int

lzss.o: lzss.c lzss.h
	$(CC) $(CFLAGS) -c lzss.c -o lzss.o

ptr2int: lzss.o *.*
	$(CXX) $(CFLAGS) *.cpp lzss.o -o ptr2int

strip:
	strip ptr2int

install:
	-cp -v ptr2int $(INSTALLDIR)/ptr2int

uninstall:
	-rm -vf $(INSTALLDIR)/ptr2int

clean:
	-rm -fv lzss.o
	-rm -fv ptr2int

