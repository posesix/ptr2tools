CXX ?= g++
CFLAGS ?= -Os
INSTALL_DIR ?= /usr/bin

ptr2spm: *.*
	$(CXX) $(CFLAGS) *.cpp -o ptr2spm

strip:
	strip ptr2spm

install:
	-cp -v ptr2spm $(INSTALL_DIR)/ptr2spm

uninstall:
	-rm -vf $(INSTALL_DIR)/ptr2spm

clean:
	-rm -vf ptr2spm
