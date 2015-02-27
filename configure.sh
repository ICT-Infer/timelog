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

bin/tl: src/include/timelog.h src/tl.c bin/
EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
	cc -Isrc/include -Llib -Wall -ansi -pedantic -O0 -g -o bin/tl src/tl.c -ltimelog

lib/libtimelog.0.dylib: src/timelog.c oobj/ lib/
	cc -Isrc/include -fPIC -Wall -ansi -pedantic -O0 -g -o oobj/timelog.o -c src/timelog.c
	cc -dynamiclib -o lib/libtimelog.0.dylib -Wl,-install_name,@loader_path/../lib/libtimelog.0.dylib oobj/timelog.o
	test -f lib/libtimelog.dylib || ln -s libtimelog.0.dylib lib/libtimelog.dylib

EOF
else
cat >>Makefile <<EOF
	cc -Isrc/include -Llib -Wl,-z,origin,-rpath='\$\$ORIGIN/../lib/' -Wall -ansi -pedantic -O0 -g -o bin/tl src/tl.c -ltimelog

lib/libtimelog.so.0: src/timelog.c oobj/ lib/
	cc -Isrc/include -fPIC -Wall -ansi -pedantic -O0 -g -o oobj/timelog.o -c src/timelog.c
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
	TZ=Europe/Oslo ./tests

tests: src/tests.c
	cc -Wall -ansi -pedantic -O0 -g -o tests src/tests.c

.PHONY: clean
clean:
	rm -rf bin/ lib/ oobj/
	rm -f tests
.PHONY: pretty
pretty:
	echo | clang-format >/dev/null # Instead of \`which' because on
	                               # Mac OS X 10.4.11, \`which'
	                               # exits with 0 regardless of
	                               # whether or not the program was found.
	mkdir fmt/
	mkdir fmt/include/
	clang-format src/timelog.c > fmt/timelog.c
	diff src/timelog.c fmt/timelog.c >/dev/null && \\
		mv fmt/timelog.c src/timelog.c || rm fmt/timelog.c
	clang-format src/tl.c > fmt/tl.c
	diff src/tl.c fmt/tl.c >/dev/null && mv fmt/tl.c src/tl.c \
		|| rm fmt/tl.c
	clang-format src/include/timelog.h > fmt/include/timelog.h
	diff include/timelog.h include/timelog.h >/dev/null && \\
		mv include/timelog.h include/timelog.h \\
		|| rm include/timelog.h
	clang-format src/tests.c > fmt/tests.c
	diff src/tests.c fmt/tests.c >/dev/null \
		&& mv fmt/tests.c src/tests.c || rm fmt/tests.c
	rmdir fmt/include
	rmdir fmt/
EOF
