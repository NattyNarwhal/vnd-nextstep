LIBRARY=../Library
MIGOUTPUT=mydriverUser.c mydriverServer.c mydriver.h
SERVER_OBJ= mydriver_main.o mydriverServer.o vnd.o disksort.o
#SERVER_OBJ= mydriver_main.o mydriverServer.o
CFLAGS= -DKERNEL -DMACH_USER_API -DMACH -Wall -g -DDEBUG -I../Library

all:	mydriver_reloc ${LIBRARY}/mydriverUser.o

mydriver_reloc: ${SERVER_OBJ} LoadCommands UnloadCommands
	kl_ld -n mydriver -l LoadCommands -u UnloadCommands -i instance \
		-d mydriver_loadable -o $@ ${SERVER_OBJ}

${LIBRARY}/mydriverUser.o:	mydriverUser.c
	${CC} ${CFLAGS} -c -o $@ mydriverUser.c

${MIGOUTPUT}:	mydriver.defs
	mig mydriver.defs
	cp mydriver.h ${LIBRARY}/mydriver.h

clean:
	rm -f *.o
	rm -f ${MIGOUTPUT}
	rm -f mydriver_reloc mydriver_loadable
	rm -f ${LIBRARY}/mydriverUser.o ${LIBRARY}/mydriver.h
	
.c.o:
	${CC} ${CFLAGS} -c $*.c
