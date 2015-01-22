bin/tl: bin/ tl.c
	cc -Wall -ansi -pedantic -O0 -g -o bin/tl tl.c

bin/:
	test -d bin/ || mkdir bin/

clean:
	rm bin/tl
	rmdir bin/
	rm tests

tests: tests.c
	cc -o tests tests.c

.PHONY: test
test: tests bin/tl
	./tests
