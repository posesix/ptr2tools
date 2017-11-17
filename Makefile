CC ?= gcc
CXX ?= g++
CFLAGS ?= -Os
PROGS ?= ptr2tex ptr2spm ptr2int
SOURCES ?= sources
INSTALL_DIR ?= /usr/bin
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


clean:
	for PROG in $(PROGS) ; do \
		$(MAKEC) clean ; \
	done

strip:
	for PROG in $(PROGS) ; do \
		$(MAKEC) strip ; \
	done

install: 
	for PROG in $(PROGS) ; do \
		$(MAKEC) install ; \
	done

uninstall:
	for PROG in $(PROGS) ; do \
		$(MAKEC) uninstall ; \
	done
