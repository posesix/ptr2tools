CXX ?= g++
CFLAGS ?= -Os
INSTALL_DIR ?= /mingw64/bin

ptr2spm: *.*
	$(CXX) $(CFLAGS) *.cpp -o ptr2spm

install:
	-cp -v ptr2spm $(INSTALL_DIR)/ptr2spm

