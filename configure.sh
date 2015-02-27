#!/usr/bin/env sh

OUTDIR=$( pwd )/devel
host_os=$( uname -s )

cat >Makefile <<EOF
.PHONY: all
EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
all: ${OUTDIR}/lib/libtimelog.0.dylib ${OUTDIR}/bin/tl

EOF
else
cat >>Makefile <<EOF
all: ${OUTDIR}/lib/libtimelog.so.0 ${OUTDIR}/bin/tl

EOF
fi

cat >>Makefile <<EOF
Makefile: configure.sh
	./configure.sh

${OUTDIR}/bin/tl: src/include/timelog.h src/tl.c ${OUTDIR}/bin/
EOF

if [ "$host_os" = "Darwin" ] ; then
cat >>Makefile <<EOF
	cc -Isrc/include -L${OUTDIR}/lib -Wall -ansi -pedantic -O0 -g -o ${OUTDIR}/bin/tl src/tl.c -ltimelog

${OUTDIR}/lib/libtimelog.0.dylib: src/timelog.c build/oobj/ ${OUTDIR}/lib/
	cc -Isrc/include -fPIC -Wall -ansi -pedantic -O0 -g -o build/oobj/timelog.o -c src/timelog.c
	cc -dynamiclib -o ${OUTDIR}/lib/libtimelog.0.dylib -Wl,-install_name,@loader_path/../lib/libtimelog.0.dylib build/oobj/timelog.o
	test -f ${OUTDIR}/lib/libtimelog.dylib || ln -s libtimelog.0.dylib ${OUTDIR}/lib/libtimelog.dylib

EOF
else
cat >>Makefile <<EOF
	cc -Isrc/include -L${OUTDIR}/lib -Wl,-z,origin,-rpath='\$\$ORIGIN/../lib/' -Wall -ansi -pedantic -O0 -g -o ${OUTDIR}/bin/tl src/tl.c -ltimelog

${OUTDIR}/lib/libtimelog.so.0: src/timelog.c build/oobj/ ${OUTDIR}/lib/
	cc -Isrc/include -fPIC -Wall -ansi -pedantic -O0 -g -o build/oobj/timelog.o -c src/timelog.c
	cc -shared -Wl,-soname,libtimelog.so.0 -o ${OUTDIR}/lib/libtimelog.so.0 build/oobj/timelog.o
	test -f ${OUTDIR}/lib/libtimelog.so || ln -s libtimelog.so.0 ${OUTDIR}/lib/libtimelog.so

EOF
fi

cat >>Makefile <<EOF
${OUTDIR}/:
	test -d ${OUTDIR}/ || mkdir ${OUTDIR}/

${OUTDIR}/bin/: ${OUTDIR}/
	test -d ${OUTDIR}/bin/ || mkdir ${OUTDIR}/bin/

${OUTDIR}/lib/: ${OUTDIR}/
	test -d ${OUTDIR}/lib/ || mkdir ${OUTDIR}/lib/

build/:
	test -d build/ || mkdir build/

build/oobj/: build/
	test -d build/oobj/ || mkdir build/oobj/

.PHONY: test
test: all build/tests
	TZ=Europe/Oslo ./build/tests ${OUTDIR}/bin/tl

build/tests: src/tests.c build/
	cc -Wall -ansi -pedantic -O0 -g -o build/tests src/tests.c

.PHONY: clean
clean:
	rm -rf build/

.PHONY: clean-all
clean-all: clean
	rm -rf ${OUTDIR}/

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
