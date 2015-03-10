.include "config.mk"

TIMELOGVER=0

WORKDIR=work
CFLAGS=-Wall -ansi -pedantic

.ifndef RELEASE
OUTDIR=debug
CFLAGS+=-O0 -g
.else
OUTDIR=dist
.endif

.ifdef DARWIN
LIBTIMELOGEXT=dylib
LIBTIMELOGVEREXT=$(TIMELOGVER).dylib
LIBTIMELOGSHLIB=-dynamiclib -Wl,-install_name,@loader_path/../lib/libtimelog.$(LIBTIMELOGVEREXT)
TIMELOGRELOC=
.else
LIBTIMELOGEXT=so
LIBTIMELOGVEREXT=so.$(TIMELOGVER)
LIBTIMELOGSHLIB=-shared -Wl,-soname,libtimelog.$(LIBTIMELOGVEREXT)
TIMELOGRELOC=-Wl,-z,origin,-rpath='$$ORIGIN/../lib/'
.endif

.PHONY: all
all: directories $(OUTDIR)/bin/tl

.PHONY: directories
directories: $(WORKDIR)/obj $(OUTDIR)/include $(OUTDIR)/lib $(OUTDIR)/bin

$(WORKDIR)/obj $(OUTDIR)/include $(OUTDIR)/lib $(OUTDIR)/bin:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(WORKDIR)
	rm -rf $(OUTDIR)

.PHONY: confclean
confclean:
	rm -f GNUmakefile
	rm -f conf.mk

#
# libtimelog
#

$(OUTDIR)/lib/libtimelog.$(LIBTIMELOGEXT): $(OUTDIR)/lib/libtimelog.$(LIBTIMELOGVEREXT)
	ln -s libtimelog.$(LIBTIMELOGVEREXT) $@

$(OUTDIR)/lib/libtimelog.$(LIBTIMELOGVEREXT): $(WORKDIR)/obj/timelog.o
	$(CC) $(LIBTIMELOGSHLIB) -o $@ $(WORKDIR)/obj/timelog.o -lcrypto

$(WORKDIR)/obj/timelog.o: $(OUTDIR)/include/timelog.h
	$(CC) -I$(OUTDIR)/include -fPIC $(CFLAGS) -o $@ -c src/timelog.c

$(OUTDIR)/include/timelog.h: src/include/timelog.h
	cp src/include/timelog.h $@

#
# tl
#

$(OUTDIR)/bin/tl: $(OUTDIR)/include/timelog.h $(OUTDIR)/lib/libtimelog.$(LIBTIMELOGEXT) src/tl.c
	$(CC) -I${OUTDIR}/include -L${OUTDIR}/lib $(TIMELOGRELOC) $(CFLAGS) \
	  -o $@ src/tl.c -ltimelog
