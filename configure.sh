#!/usr/bin/env sh

host_os=$( uname -s )

cat >Makefile <<EOF
.PHONY: all
EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
all: lib/libtimelog.0.dylib bin/tl

EOF
else
cat >>Makefile <<EOF
all: lib/libtimelog.so.0 bin/tl

EOF
fi

cat >>Makefile <<EOF
Makefile: configure.sh
	./configure.sh

bin/tl: include/timelog.h tl.c bin/
	cc -Iinclude -Llib -Wall -ansi -pedantic -O0 -g -o bin/tl tl.c -ltimelog

EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
lib/libtimelog.0.dylib: timelog.c oobj/ lib/
	cc -Iinclude -fPIC -Wall -ansi -pedantic -O0 -g -o oobj/timelog.o -c timelog.c
	cc -dynamiclib -o lib/libtimelog.0.dylib oobj/timelog.o
	test -f lib/libtimelog.dylib || ln -s libtimelog.0.dylib lib/libtimelog.dylib

EOF
else
cat >>Makefile <<EOF
lib/libtimelog.so.0: timelog.c oobj/ lib/
	cc -Iinclude -fPIC -Wall -ansi -pedantic -O0 -g -o oobj/timelog.o -c timelog.c
	cc -shared -Wl,-soname,libtimelog.so.0 -o lib/libtimelog.so.0 oobj/timelog.o
	test -f lib/libtimelog.so || ln -s libtimelog.so.0 lib/libtimelog.so

EOF
fi

cat >>Makefile <<EOF
bin/:
	mkdir bin/

lib/:
	mkdir lib/

oobj/:
	mkdir oobj/

.PHONY: test
test: all tests
EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
	DYLD_LIBRARY_PATH=\${CURDIR}/lib:\${DYLD_LIBRARY_PATH} TZ=Europe/Oslo ./tests

EOF
else
cat >>Makefile <<EOF
	LD_LIBRARY_PATH=\${.CURDIR}/lib:\${LD_LIBRARY_PATH} TZ=Europe/Oslo ./tests

EOF
fi

cat >>Makefile <<EOF
tests: tests.c
	cc -Wall -ansi -pedantic -O0 -g -o tests tests.c

.PHONY: clean
clean:
	rm -rf bin/ lib/ oobj/
	rm -f tests
EOF
