CXX ?= g++
CFLAGS ?= -Os
PROGS ?= ptr2tex ptr2spm ptr2int
SOURCES ?= sources
GENERIC = $(MAKE) -C sources/$@
INSTALL_DIR ?= /usr/bin
MAKEC = $(MAKE) -C sources/$$PROG
IMPORT_PNG?=-lpng

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
		$(MAKE) -C sources/$$PROG clean ; \
	done

strip:
	for PROG in $(PROGS) ; do \
		$(MAKEC) strip ; \
	done

install: 
	for PROG in $(PROGS) ; do \
		$(MAKEC) install INSTALLDIR=$(INSTALLDIR) ; \
	done

uninstall:
	for PROG in $(PROGS) ; do \
		$(MAKEC) uninstall ; \
	done
