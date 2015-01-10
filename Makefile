bin/tl: bin/ tl.c
	cc -o bin/tl tl.c

bin/:
	test -d bin/ || mkdir bin/

clean:
	rm bin/tl
	rmdir bin/

test: bin/tl
	./test.sh
