VERSION=0.6.0

PREFIX=/usr/local
EXEC_PREFIX=$(PREFIX)
BINDIR=$(EXEC_PREFIX)/bin
LIBDIR=$(EXEC_PREFIX)/lib
DOCDIR=$(PREFIX)/doc/osdsh
INCLUDEDIR=$(PREFIX)/include

INSTALL=/usr/bin/install -c

all: osdsh osdctl

osdsh: 
	cd src/$@; make

osdctl:
	cd src/$@; make

install: all
	$(INSTALL) src/osdsh/osdsh $(BINDIR)
	$(INSTALL) src/osdctl/osdctl $(BINDIR)
	mkdir -p $(LIBDIR)/osdsh
	$(INSTALL) src/osdsh/libosdsh*.so $(LIBDIR)/osdsh
	mkdir -p $(DOCDIR)
	$(INSTALL) README $(DOCDIR)

clean:
	cd src/osdsh; make clean
	cd src/osdctl; make clean
