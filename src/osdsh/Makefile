CC     = gcc
PREFIX = /usr
BINDIR = $(PREFIX)/bin
DOCDIR = $(PREFIX)/doc/osdsh
LIBDIR = -L/usr/X11R6/lib -L/usr/lib
LIBS   = $(LIBDIR) -lxosd -lpthread
CFLAGS = -O3

#APMOBJS = apmwatch.o
#APMLIBS = -lapm
#APM=1

OBJS   =  controlsh.o utils.o

all: osdsh libs

libs: libosdshclock.so libosdshmixer.so libosdshapm.so libosdshnet.so

%.o: %.c

	gcc -c $<

osdsh: $(OBJS)
	gcc -c $(CFLAGS) $@.c
	gcc $(CFLAGS) -o $@ osdsh.o $(OBJS) $(LIBS) &#8211;ldl

clean: libclean
	rm osdsh
	rm *.o

libclean:
	rm libosdsh*.so

libosdshclock.so: 
	gcc -fPIC -c clockdisplay.c
	gcc -shared -o $@ clockdisplay.o utils.o

libosdshmixer.so:
	gcc -fPIC -c mixerwatch.c
	gcc -shared -o $@ mixerwatch.o utils.o

libosdshapm.so:
	gcc -fPIC -c apmwatch.c
	gcc -shared -o $@ apmwatch.o utils.o -lapm

libosdshnet.so: 
	gcc -fPIC -c connectionwatch.c
	gcc -shared -o $@ connectionwatch.o utils.o
