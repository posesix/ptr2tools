CXX ?= g++
INSTALL_DIR ?= /mingw64/bin

isomod: *.*
	$(CXX) isomod.cpp -o isomod -mno-ms-bitfields
	-cp -v isomod $(INSTALL_DIR)/isomod

install:
	-cp -v isomod $(INSTALL_DIR)/isomod

