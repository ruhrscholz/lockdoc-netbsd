#include "sys/param.h"
#include "sys/proc.h"
#include "sys/systm.h"
#include "sys/buf.h"
#include "sys/ioctl.h"
#include "sys/tty.h"
#include "sys/file.h"
#include "sys/conf.h"
#include "device.h"

#include "machine/autoconf.h"
#include "machine/mon.h"
#include "../sun3/cons.h"

#include "prom.h"

void promattach __P((struct device *, struct device *, void *));

struct prom_softc {
    struct device prom_dev;
    int prom_flags;
    int prom_nopen;
    struct tty *prom_t;
};

struct cfdriver promcd = 
{ NULL, "prom", always_match, promattach,
      DV_TTY, sizeof(struct prom_softc), 0};

#define PROM_CHECK(unit) \
      if (unit >= promcd.cd_ndevs || (promcd.cd_devs[unit] == NULL)) \
	  return ENXIO
#define UNIT_TO_PROMP(unit) promcd.cd_devs[unit]

void promattach(parent, self, args)
     struct device *parent;
     struct device *self;
     void *args;
{
    struct prom_softc *promp = (struct prom_softc *) self;

    printf("\n");		
}

void promstart __P((struct tty *));

int promopen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
    struct tty *tp;
    struct prom_softc *promp;
    int unit;
    int s,error=0;

    unit = minor(dev);
    PROM_CHECK(unit);
    promp = UNIT_TO_PROMP(unit);
    if (!promp->prom_t)
	tp = promp->prom_t = ttymalloc();
    else
	tp = promp->prom_t;
    tp->t_oproc = promstart;
    tp->t_dev = dev;
    if ((tp->t_state & TS_ISOPEN) == 0) {
	tp->t_state |= TS_WOPEN;
	ttychars(tp);
	if (tp->t_ispeed == 0) {
	    tp->t_iflag = TTYDEF_IFLAG;
	    tp->t_oflag = TTYDEF_OFLAG;
	    tp->t_cflag = TTYDEF_CFLAG;
	    tp->t_lflag = TTYDEF_LFLAG;
	    tp->t_ispeed = tp->t_ospeed = TTYDEF_SPEED;
	}
	ttsetwater(tp);
    }
    else if (tp->t_state & TS_XCLUDE && p->p_ucred->cr_uid != 0)
	return (EBUSY);

    s = spltty();
    while ((flag & O_NONBLOCK) == 0 && (tp->t_cflag &CLOCAL) == 0 &&
	   (tp->t_state & TS_CARR_ON) == 0) {
	tp->t_state |= TS_WOPEN;
	if (error = ttysleep(tp, (caddr_t)&tp->t_rawq, TTIPRI | PCATCH,
			     ttopen, 0))
	    break;
    }
    splx(s);
    if (error == 0)
	error = (*linesw[tp->t_line].l_open)(dev, tp);
    return (error);
}

promclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
    struct tty *tp;
    int unit;
    struct prom_softc *promp;    

    unit = minor(dev);
    PROM_CHECK(unit);
    promp = UNIT_TO_PROMP(unit);
    tp = promp->prom_t;
    (*linesw[tp->t_line].l_close)(tp, flag);
    ttyclose(tp);
    return 0;
}

promread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
{
    int unit;
    register struct tty *tp;
    struct prom_softc *promp;    

    unit = minor(dev);
    PROM_CHECK(unit);
    promp = UNIT_TO_PROMP(unit);
    tp = promp->prom_t;
    return ((*linesw[tp->t_line].l_read)(tp, uio, flag));
}
promwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
{
    int unit;
    register struct tty *tp;
    struct prom_softc *promp;    

    unit = minor(dev);
    PROM_CHECK(unit);
    promp = UNIT_TO_PROMP(unit);
    tp = promp->prom_t;
 
    return ((*linesw[tp->t_line].l_write)(tp, uio, flag));
}

int promioctl(dev, cmd, data, flag)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
{
    int unit;
    register struct tty *tp;
    struct prom_softc *promp;
    int error;

    unit = minor(dev);
    PROM_CHECK(unit);
    promp = UNIT_TO_PROMP(unit);
    tp = promp->prom_t;

    error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, data, flag);
    if (error >= 0)
	return error;
    error = ttioctl(tp, cmd, data, flag);
    if (error >= 0)
	return error;

    switch (cmd) {
    default:
	return ENOTTY;
    }

    return 0;
}
void promstart(tp)
     struct tty *tp;
{
    int s;
    int c;

    s = spltty();
    if (tp->t_state & (TS_BUSY | TS_TTSTOP)) goto out;
    if (tp->t_outq.c_cc <= tp->t_lowat) {
	if (tp->t_state & TS_ASLEEP) {
	    tp->t_state &=~ TS_ASLEEP;
	    wakeup((caddr_t)&tp->t_outq);
	}
	selwakeup(&tp->t_wsel);
    }
    if (tp->t_outq.c_cc == 0)
	goto out;
    tp->t_state |= TS_BUSY;
    c = getc(&tp->t_outq);
    mon_putchar(c);
 out:
    splx(s);
}

/*
 * Stop output on a line.
 */
promstop(tp, flag)
	register struct tty *tp;
{
	register int s;

	s = spltty();
	if (tp->t_state & TS_BUSY) {
		if ((tp->t_state&TS_TTSTOP)==0)
			tp->t_state |= TS_FLUSH;
	}
	splx(s);
}

/* prom console support */

promcnprobe(cp)
     struct consdev *cp;
{
    int prommajor;

    /* locate the major number */
    for (prommajor = 0; prommajor < nchrdev; prommajor++)
	if (cdevsw[prommajor].d_open == promopen)
	    break;

    cp->cn_dev = makedev(prommajor, 0);
    cp->cn_pri = CN_NORMAL;	/* will always exist but you don't
				 * want to use it unless you have to
				 */
}

promcninit(cp)
     struct consdev *cp;
{
    cp->cn_tp = NULL;
    mon_printf("prom console initialized\n");
}

promcngetc(dev)
     dev_t dev;
{
    mon_printf("not sure how to do promcngetc() yet\n");
}

/*
 * Console kernel output character routine.
 */
promcnputc(dev, c)
     dev_t dev;
     int c;
{
    if (minor(dev) != 0)
	mon_printf("non unit 0 prom console???\n");
    mon_putchar(c);
}
