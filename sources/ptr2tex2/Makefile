CXX ?= g++
INSTALL_DIR ?= /mingw64/bin
IMPORT_PNG ?= -lpng

ptr2tex2: *.*
	$(CXX) $(CFLAGS) *.cpp -o ptr2tex2 $(IMPORT_PNG)
	-cp -v ptr2tex2 $(INSTALL_DIR)/ptr2tex2

install:
	-cp -v ptr2tex2 $(INSTALL_DIR)/ptr2tex2

