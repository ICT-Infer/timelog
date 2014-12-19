bin/tl: bin/ main.c
	gcc -o bin/tl main.c -ldb

bin/:
	test -d bin/ || mkdir bin/

clean:
	rm bin/tl
	rmdir bin/
