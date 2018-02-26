CC ?= gcc
CXX ?= g++
CFLAGS ?= -Os
INSTALL_DIR ?= /mingw64/bin

all: ptr2int

lzss.o: lzss.c lzss.h
	$(CC) $(CFLAGS) -c lzss.c -o lzss.o

ptr2int: lzss.o *.*
	$(CXX) $(CFLAGS) *.cpp lzss.o -o ptr2int

install:
	-cp -v ptr2int $(INSTALL_DIR)/ptr2int
