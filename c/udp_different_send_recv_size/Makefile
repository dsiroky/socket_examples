all: cli srv

cli: cli.c common.h
	gcc -Wall -x c cli.c -o cli

srv: srv.c common.h
	gcc -Wall -x c srv.c -o srv

clean: 
	rm -f cli srv
