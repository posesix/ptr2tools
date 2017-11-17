CXX ?= g++
INSTALLDIR ?= /usr/bin
IMPORT_PNG = -lpng

ptr2tex: *.*
	$(CXX) $(CFLAGS) $(IMPORT_PNG) *.cpp -o ptr2tex

clean:
	-rm -fv ptr2tex

strip:
	strip ptr2tex

install:
	-cp -v ptr2tex $(INSTALLDIR)/ptr2tex

uninstall:
	-rm -fv $(INSTALLDIR)/ptr2tex

