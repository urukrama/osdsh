CC     = gcc
PREFIX = /usr
BINDIR = $(PREFIX)/bin
DOCDIR = $(PREFIX)/doc/osdsh
LIBDIR = -L/usr/X11R6/lib -L/usr/lib
LIBS   = $(LIBDIR)
CFLAGS = -O3


all: osdctl

osdctl:
	gcc $(LIBDIR) $(LIBS) $(CFLAGS) -o $@ $@.c

clean: 
	rm osdctl

