bin/tl: lib/libtimelog.so tl.c bin/
	cc -Iinclude -Llib -Wall -ansi -pedantic -O0 -g -o bin/tl tl.c -ltimelog

lib/libtimelog.so: lib/libtimelog.so.0 lib/
	ln -s libtimelog.so.0 lib/libtimelog.so

lib/libtimelog.so.0: timelog.c oobj/ lib/
	cc -Iinclude -fPIC -Wall -ansi -pedantic -O0 -g -o oobj/timelog.o -c timelog.c
	cc -shared -Wl,-soname,libtimelog.so.0 -o lib/libtimelog.so.0 oobj/timelog.o

bin/:
	mkdir bin/

lib/:
	mkdir lib/

oobj/:
	mkdir oobj/

.PHONY: test
test: tests bin/tl
	LD_LIBRARY_PATH=${.CURDIR}/lib:${LD_LIBRARY_PATH} TZ=Europe/Oslo ./tests

tests: tests.c
	cc -Wall -ansi -pedantic -O0 -g -o tests tests.c

.PHONY: clean
clean:
	rm -rf bin/ lib/ oobj/
	rm -f tests
