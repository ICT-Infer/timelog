bin/tl: bin/
	gcc -o bin/tl main.c -lgdbm -lgdbm_compat

bin/:
	test -d bin/ || mkdir bin/

clean:
	rm bin/tl
	rmdir bin/
