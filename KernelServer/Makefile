LIBRARY=../Library
SERVER_OBJ= vnddrv_main.o vnd.o disksort.o kern_physio.o
CFLAGS= -DKERNEL -DMACH_USER_API -DMACH -Wall -g -DDEBUG -I../Library

all:	vnddrv_reloc

vnddrv_reloc: ${SERVER_OBJ} LoadCommands UnloadCommands
	kl_ld -n vnddrv -l LoadCommands -u UnloadCommands -i vnddrv_inst \
		-d vnddrv_loadable -o $@ ${SERVER_OBJ}

clean:
	rm -f *.o
	rm -f vnddrv_reloc vnddrv_loadable
	rm -f ${LIBRARY}/vnddrv.h
	
.c.o:
	${CC} ${CFLAGS} -c $*.c
