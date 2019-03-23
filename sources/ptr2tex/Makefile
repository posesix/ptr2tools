CXX ?= g++
INSTALL_DIR ?= /mingw64/bin
IMPORT_PNG ?= -lpng

ptr2tex: *.*
	$(CXX) $(CFLAGS) -g *.cpp -o ptr2tex $(IMPORT_PNG)
	-cp -v ptr2tex $(INSTALL_DIR)/ptr2tex

install:
	-cp -v ptr2tex $(INSTALL_DIR)/ptr2tex

