bin/tl: bin/ lib/libtimelog.so tl.c
	cc -Iinclude -Llib -Wall -ansi -pedantic -O0 -g -o bin/tl tl.c -ltimelog

lib/libtimelog.so: lib/libtimelog.so.0
	ln -s libtimelog.so.0 lib/libtimelog.so

lib/libtimelog.so.0: lib/ timelog.c
	cc -Iinclude -Wall -ansi -pedantic -O0 -fPIC -g -o lib/libtimelog.so.0 -c timelog.c

bin/:
	test -d bin/ || mkdir bin/

lib/:
	test -d lib/ || mkdir lib/

clean:
	test -f lib/libtimelog.so && rm lib/libtimelog.so
	test -f lib/libtimelog.so.0 && rm lib/libtimelog.so.0
	test -d lib/ && rmdir lib/
	test -f bin/tl && rm bin/tl
	test -d bin/ && rmdir bin/
	test -f tests && rm tests

tests: tests.c
	cc -Wall -ansi -pedantic -O0 -g -o tests tests.c

.PHONY: test
test: tests bin/tl
	TZ=Europe/Oslo ./tests
