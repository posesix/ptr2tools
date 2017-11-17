CXX ?= g++
CFLAGS ?= -Os
INSTALLDIR ?= /usr/bin

ptr2spm: *.*
	$(CXX) $(CFLAGS) *.cpp -o ptr2spm

strip:
	strip ptr2spm

install:
	-cp -v ptr2spm $(INSTALLDIR)/ptr2spm

uninstall:
	-rm -vf $(INSTALLDIR)/ptr2spm

clean:
	-rm -vf ptr2spm
