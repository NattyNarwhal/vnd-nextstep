#define NVND 1

#include <kernserv/prototypes.h>
#include <kernserv/machine/spl.h>
#include <dev/ldd.h>

/* BSD differences... */
#define v_mount v_vfsp
#define b_data b_un.b_addr

/* from disklabel.h */
#define	MAXPARTITIONS	8 /* XXX: maybe true on x86, is it on NS 68k? */
#define DISKUNIT(dev)   (minor(dev) / MAXPARTITIONS)

/* workarounds */
#ifndef __P
#define __P(x) x
#endif

/* malloc shim */
#define MALLOC(space, cast, size, type, flags) \
	space = (cast)malloc(size)

/* because i don't want to deal with porting to the nextstep disksort... */
void disksort(register struct buf*, register struct buf*);
