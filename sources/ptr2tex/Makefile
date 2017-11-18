CXX ?= g++
INSTALL_DIR ?= /usr/bin
IMPORT_PNG ?= -lpng

ptr2tex: *.*
	$(CXX) $(CFLAGS) *.cpp -o ptr2tex $(IMPORT_PNG)

clean:
	-rm -fv ptr2tex

strip:
	strip ptr2tex

install:
	-cp -v ptr2tex $(INSTALL_DIR)/ptr2tex

uninstall:
	-rm -fv $(INSTALL_DIR)/ptr2tex

