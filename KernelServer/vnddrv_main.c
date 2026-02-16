/* vnddrv_main.c:  major functions of vnddrv */

#import <kernserv/kern_server_types.h>
#import <kernserv/prototypes.h>
#import <ansi/string.h>
#import <sys/conf.h> 

#import "../Library/mydefs.h"

extern int nulldev();
extern int nodev();
extern int seltrue();
#define nullstr 0

/* definitions in vnd.c */
void vndattach(int);
void vndshutdown(void);
/* bdevsw */
int vndopen(dev_t, int, int/* , struct proc* */);
int vndclose(dev_t, int, int/* , struct proc* */);
void vndstrategy(register struct buf*);
int vnddump(dev_t);
int vndsize(dev_t);
/* cdevsw */
int vndioctl(dev_t, u_long, caddr_t, int/* , struct proc* */);
int vndread(dev_t, struct uio*);
int vndwrite(dev_t, struct uio*);

struct bdevsw my_bdevsw =  { 
                 vndopen,
                 vndclose,
                 vndstrategy,
                 vnddump,
                 vndsize,
                 0 };
struct cdevsw my_cdevsw =  { 
                 vndopen,
                 vndclose,
                 vndread,
                 vndwrite,
                 vndioctl,
                 nodev, /* stop */
                 nulldev, /* reset */
                 seltrue, /* select */
                 nodev, /* mmap */
                 nodev, /* getc (nextstep extension) */
                 nodev }; /* putc */

static struct bdevsw my_saved_bdevsw;
static struct cdevsw my_saved_cdevsw;

void vnddrv_init(void)
{
	/* XXX: With DriverKit on non-68k, we can dynamically allocate
	 * with IOAddToCdevsw; not present on 68k mach kernel (but if
	 * it was, we could write this in DriverKit...)
	 */
	my_saved_bdevsw = bdevsw[VND_BLOCK_MAJOR];
	my_saved_cdevsw = cdevsw[VND_RAW_MAJOR];
	bdevsw[VND_BLOCK_MAJOR]= my_bdevsw;
	cdevsw[VND_RAW_MAJOR]  = my_cdevsw;
	vndattach(1); // XXX: ???
	printf("vnd driver loaded\n");
}

void vnddrv_signoff(void)
{
	vndshutdown();
	bdevsw[VND_BLOCK_MAJOR]= my_saved_bdevsw;
	cdevsw[VND_RAW_MAJOR]  = my_saved_cdevsw;
	printf("vnd driver unloaded\n\n");
}
