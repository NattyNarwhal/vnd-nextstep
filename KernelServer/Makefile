LIBRARY=../Library
SERVER_OBJ= vnddrv_main.o vnd.o disksort.o kern_physio.o
CFLAGS= -DKERNEL -DMACH_USER_API -DMACH -Wall -g -DDEBUG -I../Library
USERLAND_CFLAGS= -D_POSIX_SOURCE -Wall -d -DDEBUG -I../Library

all:	vnddrv_reloc vnconfig

vnconfig: vnconfig.o
	${CC} -o $@ vnconfig.o

vnconfig.o: vnconfig.c
	${CC} ${USERLAND_CFLAGS} -c -o vnconfig.o vnconfig.c

vnddrv_reloc: ${SERVER_OBJ} LoadCommands UnloadCommands
	kl_ld -n vnddrv -l LoadCommands -u UnloadCommands -i vnddrv_inst \
		-d vnddrv_loadable -o $@ ${SERVER_OBJ}

clean:
	rm -f *.o
	rm -f vnddrv_reloc vnddrv_loadable
	rm -f vnconfig
	
.c.o:
	${CC} ${CFLAGS} -c $*.c
