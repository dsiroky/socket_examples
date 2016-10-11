all: publisher subscriber

publisher: publisher.c common.c common.h
	gcc -Wall -x c publisher.c common.c -o publisher

subscriber: subscriber.c common.c common.h
	gcc -Wall -x c subscriber.c common.c -o subscriber

clean: 
	rm -f publisher subscriber
