CC ?= gcc
CXX ?= g++
CFLAGS ?= -Os
PROGS ?= ptr2tex ptr2spm ptr2int
SOURCES ?= sources
INSTALL_DIR ?= /mingw64/bin
IMPORT_PNG?=-lpng

GENERIC = $(MAKE) -C $(SOURCES)/$@
MAKEC = $(MAKE) -C $(SOURCES)/$$PROG

export

all: $(PROGS)

ptr2tex:
	$(GENERIC)
ptr2spm:
	$(GENERIC)
ptr2int: 
	$(GENERIC)

install: 
	for PROG in $(PROGS) ; do \
		$(MAKEC) install ; \
	done
