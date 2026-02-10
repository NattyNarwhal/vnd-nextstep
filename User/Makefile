CFLAGS=-Wall -g
LIBRARY=../Library
CLIENTOBJS=test.o ${LIBRARY}/mydriverUser.o
LIBS=-lkernload

test:	${CLIENTOBJS}
	cc ${CFLAGS} -o test ${CLIENTOBJS} ${LIBS}
	
test.o:	test.c
	cc ${CFLAGS} -c test.c

clean:
	rm -f *.o
	rm -f test
