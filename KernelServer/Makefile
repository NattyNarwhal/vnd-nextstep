LIBRARY=../Library
MIGOUTPUT=vnddrvUser.c vnddrvServer.c vnddrv.h
SERVER_OBJ= vnddrv_main.o vnddrvServer.o vnd.o disksort.o
#SERVER_OBJ= vnddrv_main.o vnddrvServer.o
CFLAGS= -DKERNEL -DMACH_USER_API -DMACH -Wall -g -DDEBUG -I../Library

all:	vnddrv_reloc ${LIBRARY}/vnddrvUser.o

vnddrv_reloc: ${SERVER_OBJ} LoadCommands UnloadCommands
	kl_ld -n vnddrv -l LoadCommands -u UnloadCommands -i instance \
		-d vnddrv_loadable -o $@ ${SERVER_OBJ}

${LIBRARY}/vnddrvUser.o:	vnddrvUser.c
	${CC} ${CFLAGS} -c -o $@ vnddrvUser.c

${MIGOUTPUT}:	vnddrv.defs
	mig vnddrv.defs
	cp vnddrv.h ${LIBRARY}/vnddrv.h

clean:
	rm -f *.o
	rm -f ${MIGOUTPUT}
	rm -f vnddrv_reloc vnddrv_loadable
	rm -f ${LIBRARY}/vnddrvUser.o ${LIBRARY}/vnddrv.h
	
.c.o:
	${CC} ${CFLAGS} -c $*.c
