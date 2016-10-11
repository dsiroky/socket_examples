all: cli srv

cli: cli.c common.c common.h
	gcc -Wall -x c cli.c common.c -o cli

srv: srv.c common.c common.h
	gcc -Wall -x c srv.c common.c -o srv

clean: 
	rm -f cli srv
