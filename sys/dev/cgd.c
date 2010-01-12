/* $NetBSD: cgd.c,v 1.65 2010/01/12 21:08:08 dyoung Exp $ */

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Roland C. Dowdeswell.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: cgd.c,v 1.65 2010/01/12 21:08:08 dyoung Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/buf.h>
#include <sys/bufq.h>
#include <sys/malloc.h>
#include <sys/pool.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/disk.h>
#include <sys/disklabel.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/conf.h>
#include <sys/syslog.h>

#include <dev/dkvar.h>
#include <dev/cgdvar.h>

/* Entry Point Functions */

void	cgdattach(int);

static dev_type_open(cgdopen);
static dev_type_close(cgdclose);
static dev_type_read(cgdread);
static dev_type_write(cgdwrite);
static dev_type_ioctl(cgdioctl);
static dev_type_strategy(cgdstrategy);
static dev_type_dump(cgddump);
static dev_type_size(cgdsize);

const struct bdevsw cgd_bdevsw = {
	cgdopen, cgdclose, cgdstrategy, cgdioctl,
	cgddump, cgdsize, D_DISK
};

const struct cdevsw cgd_cdevsw = {
	cgdopen, cgdclose, cgdread, cgdwrite, cgdioctl,
	nostop, notty, nopoll, nommap, nokqfilter, D_DISK
};

static int cgd_match(device_t, cfdata_t, void *);
static void cgd_attach(device_t, device_t, void *);
static int cgd_detach(device_t, int);
static struct cgd_softc	*cgd_spawn(int);
static int cgd_destroy(device_t);

/* Internal Functions */

static int	cgdstart(struct dk_softc *, struct buf *);
static void	cgdiodone(struct buf *);

static int	cgd_ioctl_set(struct cgd_softc *, void *, struct lwp *);
static int	cgd_ioctl_clr(struct cgd_softc *, struct lwp *);
static int	cgdinit(struct cgd_softc *, const char *, struct vnode *,
			struct lwp *);
static void	cgd_cipher(struct cgd_softc *, void *, void *,
			   size_t, daddr_t, size_t, int);

/* Pseudo-disk Interface */

static struct dk_intf the_dkintf = {
	DTYPE_CGD,
	"cgd",
	cgdopen,
	cgdclose,
	cgdstrategy,
	cgdstart,
};
static struct dk_intf *di = &the_dkintf;

static struct dkdriver cgddkdriver = {
	.d_strategy = cgdstrategy,
	.d_minphys = minphys,
};

CFATTACH_DECL3_NEW(cgd, sizeof(struct cgd_softc),
    cgd_match, cgd_attach, cgd_detach, NULL, NULL, NULL, DVF_DETACH_SHUTDOWN);
extern struct cfdriver cgd_cd;

/* DIAGNOSTIC and DEBUG definitions */

#if defined(CGDDEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#ifdef DEBUG
int cgddebug = 0;

#define CGDB_FOLLOW	0x1
#define CGDB_IO	0x2
#define CGDB_CRYPTO	0x4

#define IFDEBUG(x,y)		if (cgddebug & (x)) y
#define DPRINTF(x,y)		IFDEBUG(x, printf y)
#define DPRINTF_FOLLOW(y)	DPRINTF(CGDB_FOLLOW, y)

static void	hexprint(const char *, void *, int);

#else
#define IFDEBUG(x,y)
#define DPRINTF(x,y)
#define DPRINTF_FOLLOW(y)
#endif

#ifdef DIAGNOSTIC
#define DIAGPANIC(x)		panic x
#define DIAGCONDPANIC(x,y)	if (x) panic y
#else
#define DIAGPANIC(x)
#define DIAGCONDPANIC(x,y)
#endif

/* Global variables */

/* Utility Functions */

#define CGDUNIT(x)		DISKUNIT(x)
#define GETCGD_SOFTC(_cs, x)	if (!((_cs) = getcgd_softc(x))) return ENXIO

/* The code */

static struct cgd_softc *
getcgd_softc(dev_t dev)
{
	int	unit = CGDUNIT(dev);
	struct cgd_softc *sc;

	DPRINTF_FOLLOW(("getcgd_softc(0x%"PRIx64"): unit = %d\n", dev, unit));

	sc = device_lookup_private(&cgd_cd, unit);
	if (sc == NULL)
		sc = cgd_spawn(unit);
	return sc;
}

static int
cgd_match(device_t self, cfdata_t cfdata, void *aux)
{

	return 1;
}

static void
cgd_attach(device_t parent, device_t self, void *aux)
{
	struct cgd_softc *sc = device_private(self);

	sc->sc_dev = self;
	simple_lock_init(&sc->sc_slock);
	dk_sc_init(&sc->sc_dksc, sc, device_xname(sc->sc_dev));
	disk_init(&sc->sc_dksc.sc_dkdev, sc->sc_dksc.sc_xname, &cgddkdriver);
}


static int
cgd_detach(device_t self, int flags)
{
	int	ret = 0;
	int	pmask = 1 << RAW_PART;
	struct cgd_softc *sc = device_private(self);
	struct dk_softc *dksc;

	dksc = &sc->sc_dksc;
	if ((dksc->sc_flags & DKF_INITED) != 0)
	{
		if (DK_BUSY(&sc->sc_dksc, pmask))
			ret = EBUSY;
		else
			ret = cgd_ioctl_clr(sc, curlwp);
	}

	disk_destroy(&sc->sc_dksc.sc_dkdev);

	return ret;
}

void
cgdattach(int num)
{
	int error;

	error = config_cfattach_attach(cgd_cd.cd_name, &cgd_ca);
	if (error != 0)
		aprint_error("%s: unable to register cfattach\n",
		    cgd_cd.cd_name);
}

static struct cgd_softc *
cgd_spawn(int unit)
{
	cfdata_t cf;

	cf = malloc(sizeof(*cf), M_DEVBUF, M_WAITOK);
	cf->cf_name = cgd_cd.cd_name;
	cf->cf_atname = cgd_cd.cd_name;
	cf->cf_unit = unit;
	cf->cf_fstate = FSTATE_STAR;

	return device_private(config_attach_pseudo(cf));
}

static int
cgd_destroy(device_t dev)
{
	int error;
	cfdata_t cf;

	cf = device_cfdata(dev);
	error = config_detach(dev, DETACH_QUIET);
	if (error)
		return error;
	free(cf, M_DEVBUF);
	return 0;
}

static int
cgdopen(dev_t dev, int flags, int fmt, struct lwp *l)
{
	struct	cgd_softc *cs;

	DPRINTF_FOLLOW(("cgdopen(0x%"PRIx64", %d)\n", dev, flags));
	GETCGD_SOFTC(cs, dev);
	return dk_open(di, &cs->sc_dksc, dev, flags, fmt, l);
}

static int
cgdclose(dev_t dev, int flags, int fmt, struct lwp *l)
{
	int error;
	struct	cgd_softc *cs;
	struct	dk_softc *dksc;

	DPRINTF_FOLLOW(("cgdclose(0x%"PRIx64", %d)\n", dev, flags));
	GETCGD_SOFTC(cs, dev);
	dksc = &cs->sc_dksc;
	if ((error =  dk_close(di, dksc, dev, flags, fmt, l)) != 0)
		return error;

	if ((dksc->sc_flags & DKF_INITED) == 0) {
		if ((error = cgd_destroy(cs->sc_dev)) != 0) {
			aprint_error_dev(cs->sc_dev,
			    "unable to detach instance\n");
			return error;
		}
	}
	return 0;
}

static void
cgdstrategy(struct buf *bp)
{
	struct	cgd_softc *cs = getcgd_softc(bp->b_dev);

	DPRINTF_FOLLOW(("cgdstrategy(%p): b_bcount = %ld\n", bp,
	    (long)bp->b_bcount));
	/* XXXrcd: Should we test for (cs != NULL)? */
	dk_strategy(di, &cs->sc_dksc, bp);
	return;
}

static int
cgdsize(dev_t dev)
{
	struct cgd_softc *cs = getcgd_softc(dev);

	DPRINTF_FOLLOW(("cgdsize(0x%"PRIx64")\n", dev));
	if (!cs)
		return -1;
	return dk_size(di, &cs->sc_dksc, dev);
}

/*
 * cgd_{get,put}data are functions that deal with getting a buffer
 * for the new encrypted data.  We have a buffer per device so that
 * we can ensure that we can always have a transaction in flight.
 * We use this buffer first so that we have one less piece of
 * malloc'ed data at any given point.
 */

static void *
cgd_getdata(struct dk_softc *dksc, unsigned long size)
{
	struct	cgd_softc *cs =dksc->sc_osc;
	void *	data = NULL;

	simple_lock(&cs->sc_slock);
	if (cs->sc_data_used == 0) {
		cs->sc_data_used = 1;
		data = cs->sc_data;
	}
	simple_unlock(&cs->sc_slock);

	if (data)
		return data;

	return malloc(size, M_DEVBUF, M_NOWAIT);
}

static void
cgd_putdata(struct dk_softc *dksc, void *data)
{
	struct	cgd_softc *cs =dksc->sc_osc;

	if (data == cs->sc_data) {
		simple_lock(&cs->sc_slock);
		cs->sc_data_used = 0;
		simple_unlock(&cs->sc_slock);
	} else {
		free(data, M_DEVBUF);
	}
}

static int
cgdstart(struct dk_softc *dksc, struct buf *bp)
{
	struct	cgd_softc *cs = dksc->sc_osc;
	struct	buf *nbp;
	void *	addr;
	void *	newaddr;
	daddr_t	bn;
	struct	vnode *vp;

	DPRINTF_FOLLOW(("cgdstart(%p, %p)\n", dksc, bp));
	disk_busy(&dksc->sc_dkdev); /* XXX: put in dksubr.c */

	bn = bp->b_rawblkno;

	/*
	 * We attempt to allocate all of our resources up front, so that
	 * we can fail quickly if they are unavailable.
	 */

	nbp = getiobuf(cs->sc_tvn, false);
	if (nbp == NULL) {
		disk_unbusy(&dksc->sc_dkdev, 0, (bp->b_flags & B_READ));
		return -1;
	}

	/*
	 * If we are writing, then we need to encrypt the outgoing
	 * block into a new block of memory.  If we fail, then we
	 * return an error and let the dksubr framework deal with it.
	 */
	newaddr = addr = bp->b_data;
	if ((bp->b_flags & B_READ) == 0) {
		newaddr = cgd_getdata(dksc, bp->b_bcount);
		if (!newaddr) {
			putiobuf(nbp);
			disk_unbusy(&dksc->sc_dkdev, 0, (bp->b_flags & B_READ));
			return -1;
		}
		cgd_cipher(cs, newaddr, addr, bp->b_bcount, bn,
		    DEV_BSIZE, CGD_CIPHER_ENCRYPT);
	}

	nbp->b_data = newaddr;
	nbp->b_flags = bp->b_flags;
	nbp->b_oflags = bp->b_oflags;
	nbp->b_cflags = bp->b_cflags;
	nbp->b_iodone = cgdiodone;
	nbp->b_proc = bp->b_proc;
	nbp->b_blkno = bn;
	nbp->b_bcount = bp->b_bcount;
	nbp->b_private = bp;

	BIO_COPYPRIO(nbp, bp);

	if ((nbp->b_flags & B_READ) == 0) {
		vp = nbp->b_vp;
		mutex_enter(&vp->v_interlock);
		vp->v_numoutput++;
		mutex_exit(&vp->v_interlock);
	}
	VOP_STRATEGY(cs->sc_tvn, nbp);
	return 0;
}

/* expected to be called at splbio() */
static void
cgdiodone(struct buf *nbp)
{
	struct	buf *obp = nbp->b_private;
	struct	cgd_softc *cs = getcgd_softc(obp->b_dev);
	struct	dk_softc *dksc = &cs->sc_dksc;

	KDASSERT(cs);

	DPRINTF_FOLLOW(("cgdiodone(%p)\n", nbp));
	DPRINTF(CGDB_IO, ("cgdiodone: bp %p bcount %d resid %d\n",
	    obp, obp->b_bcount, obp->b_resid));
	DPRINTF(CGDB_IO, (" dev 0x%"PRIx64", nbp %p bn %" PRId64 " addr %p bcnt %d\n",
	    nbp->b_dev, nbp, nbp->b_blkno, nbp->b_data,
	    nbp->b_bcount));
	if (nbp->b_error != 0) {
		obp->b_error = nbp->b_error;
		DPRINTF(CGDB_IO, ("%s: error %d\n", dksc->sc_xname,
		    obp->b_error));
	}

	/* Perform the decryption if we are reading.
	 *
	 * Note: use the blocknumber from nbp, since it is what
	 *       we used to encrypt the blocks.
	 */

	if (nbp->b_flags & B_READ)
		cgd_cipher(cs, obp->b_data, obp->b_data, obp->b_bcount,
		    nbp->b_blkno, DEV_BSIZE, CGD_CIPHER_DECRYPT);

	/* If we allocated memory, free it now... */
	if (nbp->b_data != obp->b_data)
		cgd_putdata(dksc, nbp->b_data);

	putiobuf(nbp);

	/* Request is complete for whatever reason */
	obp->b_resid = 0;
	if (obp->b_error != 0)
		obp->b_resid = obp->b_bcount;
	disk_unbusy(&dksc->sc_dkdev, obp->b_bcount - obp->b_resid,
	    (obp->b_flags & B_READ));
	biodone(obp);
	dk_iodone(di, dksc);
}

/* XXX: we should probably put these into dksubr.c, mostly */
static int
cgdread(dev_t dev, struct uio *uio, int flags)
{
	struct	cgd_softc *cs;
	struct	dk_softc *dksc;

	DPRINTF_FOLLOW(("cgdread(0x%llx, %p, %d)\n",
	    (unsigned long long)dev, uio, flags));
	GETCGD_SOFTC(cs, dev);
	dksc = &cs->sc_dksc;
	if ((dksc->sc_flags & DKF_INITED) == 0)
		return ENXIO;
	return physio(cgdstrategy, NULL, dev, B_READ, minphys, uio);
}

/* XXX: we should probably put these into dksubr.c, mostly */
static int
cgdwrite(dev_t dev, struct uio *uio, int flags)
{
	struct	cgd_softc *cs;
	struct	dk_softc *dksc;

	DPRINTF_FOLLOW(("cgdwrite(0x%"PRIx64", %p, %d)\n", dev, uio, flags));
	GETCGD_SOFTC(cs, dev);
	dksc = &cs->sc_dksc;
	if ((dksc->sc_flags & DKF_INITED) == 0)
		return ENXIO;
	return physio(cgdstrategy, NULL, dev, B_WRITE, minphys, uio);
}

static int
cgdioctl(dev_t dev, u_long cmd, void *data, int flag, struct lwp *l)
{
	struct	cgd_softc *cs;
	struct	dk_softc *dksc;
	struct	disk *dk;
	int	ret;
	int	part = DISKPART(dev);
	int	pmask = 1 << part;

	DPRINTF_FOLLOW(("cgdioctl(0x%"PRIx64", %ld, %p, %d, %p)\n",
	    dev, cmd, data, flag, l));
	GETCGD_SOFTC(cs, dev);
	dksc = &cs->sc_dksc;
	dk = &dksc->sc_dkdev;
	switch (cmd) {
	case CGDIOCSET:
	case CGDIOCCLR:
		if ((flag & FWRITE) == 0)
			return EBADF;
	}

	switch (cmd) {
	case CGDIOCSET:
		if (dksc->sc_flags & DKF_INITED)
			ret = EBUSY;
		else
			ret = cgd_ioctl_set(cs, data, l);
		break;
	case CGDIOCCLR:

		if (DK_BUSY(&cs->sc_dksc, pmask))
			ret = EBUSY;
		else
			ret = cgd_ioctl_clr(cs, l);
		break;

	case DIOCCACHESYNC:
		/*
		 * XXX Do we really need to care about having a writable
		 * file descriptor here?
		 */
		if ((flag & FWRITE) == 0)
			return (EBADF);

		/*
		 * We pass this call down to the underlying disk.
		 */
		ret = VOP_IOCTL(cs->sc_tvn, cmd, data, flag, l->l_cred);
		break;

	default:
		ret = dk_ioctl(di, dksc, dev, cmd, data, flag, l);
		break;
	}

	return ret;
}

static int
cgddump(dev_t dev, daddr_t blkno, void *va, size_t size)
{
	struct	cgd_softc *cs;

	DPRINTF_FOLLOW(("cgddump(0x%"PRIx64", %" PRId64 ", %p, %lu)\n",
	    dev, blkno, va, (unsigned long)size));
	GETCGD_SOFTC(cs, dev);
	return dk_dump(di, &cs->sc_dksc, dev, blkno, va, size);
}

/*
 * XXXrcd:
 *  for now we hardcode the maximum key length.
 */
#define MAX_KEYSIZE	1024

static const struct {
	const char *n;
	int v;
	int d;
} encblkno[] = {
	{ "encblkno",  CGD_CIPHER_CBC_ENCBLKNO8, 1 },
	{ "encblkno8", CGD_CIPHER_CBC_ENCBLKNO8, 1 },
	{ "encblkno1", CGD_CIPHER_CBC_ENCBLKNO1, 8 },
};

/* ARGSUSED */
static int
cgd_ioctl_set(struct cgd_softc *cs, void *data, struct lwp *l)
{
	struct	 cgd_ioctl *ci = data;
	struct	 vnode *vp;
	int	 ret;
	size_t	 i;
	size_t	 keybytes;			/* key length in bytes */
	const char *cp;
	char	 *inbuf;

	cp = ci->ci_disk;
	if ((ret = dk_lookup(cp, l, &vp, UIO_USERSPACE)) != 0)
		return ret;

	inbuf = malloc(MAX_KEYSIZE, M_TEMP, M_WAITOK);

	if ((ret = cgdinit(cs, cp, vp, l)) != 0)
		goto bail;

	(void)memset(inbuf, 0, MAX_KEYSIZE);
	ret = copyinstr(ci->ci_alg, inbuf, 256, NULL);
	if (ret)
		goto bail;
	cs->sc_cfuncs = cryptfuncs_find(inbuf);
	if (!cs->sc_cfuncs) {
		ret = EINVAL;
		goto bail;
	}

	(void)memset(inbuf, 0, MAX_KEYSIZE);
	ret = copyinstr(ci->ci_ivmethod, inbuf, MAX_KEYSIZE, NULL);
	if (ret)
		goto bail;

	for (i = 0; i < __arraycount(encblkno); i++)
		if (strcmp(encblkno[i].n, inbuf) == 0)
			break;

	if (i == __arraycount(encblkno)) {
		ret = EINVAL;
		goto bail;
	}

	keybytes = ci->ci_keylen / 8 + 1;
	if (keybytes > MAX_KEYSIZE) {
		ret = EINVAL;
		goto bail;
	}

	(void)memset(inbuf, 0, MAX_KEYSIZE);
	ret = copyin(ci->ci_key, inbuf, keybytes);
	if (ret)
		goto bail;

	cs->sc_cdata.cf_blocksize = ci->ci_blocksize;
	cs->sc_cdata.cf_mode = encblkno[i].v;
	cs->sc_cdata.cf_priv = cs->sc_cfuncs->cf_init(ci->ci_keylen, inbuf,
	    &cs->sc_cdata.cf_blocksize);
	if (cs->sc_cdata.cf_blocksize > CGD_MAXBLOCKSIZE) {
	    log(LOG_WARNING, "cgd: Disallowed cipher with blocksize %zu > %u\n",
		cs->sc_cdata.cf_blocksize, CGD_MAXBLOCKSIZE);
	    cs->sc_cdata.cf_priv = NULL;
	}
		
	/*
	 * The blocksize is supposed to be in bytes. Unfortunately originally
	 * it was expressed in bits. For compatibility we maintain encblkno
	 * and encblkno8.
	 */
	cs->sc_cdata.cf_blocksize /= encblkno[i].d;
	(void)memset(inbuf, 0, MAX_KEYSIZE);
	if (!cs->sc_cdata.cf_priv) {
		ret = EINVAL;		/* XXX is this the right error? */
		goto bail;
	}
	free(inbuf, M_TEMP);

	bufq_alloc(&cs->sc_dksc.sc_bufq, "fcfs", 0);

	cs->sc_data = malloc(MAXPHYS, M_DEVBUF, M_WAITOK);
	cs->sc_data_used = 0;

	cs->sc_dksc.sc_flags |= DKF_INITED;

	/* Attach the disk. */
	disk_attach(&cs->sc_dksc.sc_dkdev);

	/* Try and read the disklabel. */
	dk_getdisklabel(di, &cs->sc_dksc, 0 /* XXX ? (cause of PR 41704) */);

	/* Discover wedges on this disk. */
	dkwedge_discover(&cs->sc_dksc.sc_dkdev);

	return 0;

bail:
	free(inbuf, M_TEMP);
	(void)vn_close(vp, FREAD|FWRITE, l->l_cred);
	return ret;
}

/* ARGSUSED */
static int
cgd_ioctl_clr(struct cgd_softc *cs, struct lwp *l)
{
	int	s;
	struct	dk_softc *dksc;

	dksc = &cs->sc_dksc;

	if ((dksc->sc_flags & DKF_INITED) == 0)
		return ENXIO;

	/* Delete all of our wedges. */
	dkwedge_delall(&cs->sc_dksc.sc_dkdev);

	/* Kill off any queued buffers. */
	s = splbio();
	bufq_drain(cs->sc_dksc.sc_bufq);
	splx(s);
	bufq_free(cs->sc_dksc.sc_bufq);

	(void)vn_close(cs->sc_tvn, FREAD|FWRITE, l->l_cred);
	cs->sc_cfuncs->cf_destroy(cs->sc_cdata.cf_priv);
	free(cs->sc_tpath, M_DEVBUF);
	free(cs->sc_data, M_DEVBUF);
	cs->sc_data_used = 0;
	cs->sc_dksc.sc_flags &= ~DKF_INITED;
	disk_detach(&cs->sc_dksc.sc_dkdev);

	return 0;
}

static int
getsize(struct lwp *l, struct vnode *vp, size_t *size)
{
	struct partinfo dpart;
	struct dkwedge_info dkw;
	int ret;

	if ((ret = VOP_IOCTL(vp, DIOCGWEDGEINFO, &dkw, FREAD,
	    l->l_cred)) == 0) {
		*size = dkw.dkw_size;
		return 0;
	}

	if ((ret = VOP_IOCTL(vp, DIOCGPART, &dpart, FREAD, l->l_cred)) == 0) {
		*size = dpart.part->p_size;
		return 0;
	}

	return ret;
}


static int
cgdinit(struct cgd_softc *cs, const char *cpath, struct vnode *vp,
	struct lwp *l)
{
	struct	dk_geom *pdg;
	struct	vattr va;
	size_t	size;
	int	ret;
	char	*tmppath;

	cs->sc_dksc.sc_size = 0;
	cs->sc_tvn = vp;
	cs->sc_tpath = NULL;

	tmppath = malloc(MAXPATHLEN, M_TEMP, M_WAITOK);
	ret = copyinstr(cpath, tmppath, MAXPATHLEN, &cs->sc_tpathlen);
	if (ret)
		goto bail;
	cs->sc_tpath = malloc(cs->sc_tpathlen, M_DEVBUF, M_WAITOK);
	memcpy(cs->sc_tpath, tmppath, cs->sc_tpathlen);

	if ((ret = VOP_GETATTR(vp, &va, l->l_cred)) != 0)
		goto bail;

	cs->sc_tdev = va.va_rdev;

	if ((ret = getsize(l, vp, &size)) != 0)
		goto bail;

	if (!size) {
		ret = ENODEV;
		goto bail;
	}

	cs->sc_dksc.sc_size = size;

	/*
	 * XXX here we should probe the underlying device.  If we
	 *     are accessing a partition of type RAW_PART, then
	 *     we should populate our initial geometry with the
	 *     geometry that we discover from the device.
	 */
	pdg = &cs->sc_dksc.sc_geom;
	pdg->pdg_secsize = DEV_BSIZE;
	pdg->pdg_ntracks = 1;
	pdg->pdg_nsectors = 1024 * (1024 / pdg->pdg_secsize);
	pdg->pdg_ncylinders = cs->sc_dksc.sc_size / pdg->pdg_nsectors;

bail:
	free(tmppath, M_TEMP);
	if (ret && cs->sc_tpath)
		free(cs->sc_tpath, M_DEVBUF);
	return ret;
}

/*
 * Our generic cipher entry point.  This takes care of the
 * IV mode and passes off the work to the specific cipher.
 * We implement here the IV method ``encrypted block
 * number''.
 *
 * For the encryption case, we accomplish this by setting
 * up a struct uio where the first iovec of the source is
 * the blocknumber and the first iovec of the dest is a
 * sink.  We then call the cipher with an IV of zero, and
 * the right thing happens.
 *
 * For the decryption case, we use the same basic mechanism
 * for symmetry, but we encrypt the block number in the
 * first iovec.
 *
 * We mainly do this to avoid requiring the definition of
 * an ECB mode.
 *
 * XXXrcd: for now we rely on our own crypto framework defined
 *         in dev/cgd_crypto.c.  This will change when we
 *         get a generic kernel crypto framework.
 */

static void
blkno2blkno_buf(char *sbuf, daddr_t blkno)
{
	int	i;

	/* Set up the blkno in blkno_buf, here we do not care much
	 * about the final layout of the information as long as we
	 * can guarantee that each sector will have a different IV
	 * and that the endianness of the machine will not affect
	 * the representation that we have chosen.
	 *
	 * We choose this representation, because it does not rely
	 * on the size of buf (which is the blocksize of the cipher),
	 * but allows daddr_t to grow without breaking existing
	 * disks.
	 *
	 * Note that blkno2blkno_buf does not take a size as input,
	 * and hence must be called on a pre-zeroed buffer of length
	 * greater than or equal to sizeof(daddr_t).
	 */
	for (i=0; i < sizeof(daddr_t); i++) {
		*sbuf++ = blkno & 0xff;
		blkno >>= 8;
	}
}

static void
cgd_cipher(struct cgd_softc *cs, void *dstv, void *srcv,
    size_t len, daddr_t blkno, size_t secsize, int dir)
{
	char		*dst = dstv;
	char 		*src = srcv;
	cfunc_cipher	*cipher = cs->sc_cfuncs->cf_cipher;
	struct uio	dstuio;
	struct uio	srcuio;
	struct iovec	dstiov[2];
	struct iovec	srciov[2];
	size_t		blocksize = cs->sc_cdata.cf_blocksize;
	char		sink[CGD_MAXBLOCKSIZE];
	char		zero_iv[CGD_MAXBLOCKSIZE];
	char		blkno_buf[CGD_MAXBLOCKSIZE];

	DPRINTF_FOLLOW(("cgd_cipher() dir=%d\n", dir));

	DIAGCONDPANIC(len % blocksize != 0,
	    ("cgd_cipher: len %% blocksize != 0"));

	/* ensure that sizeof(daddr_t) <= blocksize (for encblkno IVing) */
	DIAGCONDPANIC(sizeof(daddr_t) > blocksize,
	    ("cgd_cipher: sizeof(daddr_t) > blocksize"));

	memset(zero_iv, 0x0, blocksize);

	dstuio.uio_iov = dstiov;
	dstuio.uio_iovcnt = 2;

	srcuio.uio_iov = srciov;
	srcuio.uio_iovcnt = 2;

	dstiov[0].iov_base = sink;
	dstiov[0].iov_len  = blocksize;
	srciov[0].iov_base = blkno_buf;
	srciov[0].iov_len  = blocksize;
	dstiov[1].iov_len  = secsize;
	srciov[1].iov_len  = secsize;

	for (; len > 0; len -= secsize) {
		dstiov[1].iov_base = dst;
		srciov[1].iov_base = src;

		memset(blkno_buf, 0x0, blocksize);
		blkno2blkno_buf(blkno_buf, blkno);
		if (dir == CGD_CIPHER_DECRYPT) {
			dstuio.uio_iovcnt = 1;
			srcuio.uio_iovcnt = 1;
			IFDEBUG(CGDB_CRYPTO, hexprint("step 0: blkno_buf",
			    blkno_buf, blocksize));
			cipher(cs->sc_cdata.cf_priv, &dstuio, &srcuio,
			    zero_iv, CGD_CIPHER_ENCRYPT);
			memcpy(blkno_buf, sink, blocksize);
			dstuio.uio_iovcnt = 2;
			srcuio.uio_iovcnt = 2;
		}

		IFDEBUG(CGDB_CRYPTO, hexprint("step 1: blkno_buf",
		    blkno_buf, blocksize));
		cipher(cs->sc_cdata.cf_priv, &dstuio, &srcuio, zero_iv, dir);
		IFDEBUG(CGDB_CRYPTO, hexprint("step 2: sink",
		    sink, blocksize));

		dst += secsize;
		src += secsize;
		blkno++;
	}
}

#ifdef DEBUG
static void
hexprint(const char *start, void *buf, int len)
{
	char	*c = buf;

	DIAGCONDPANIC(len < 0, ("hexprint: called with len < 0"));
	printf("%s: len=%06d 0x", start, len);
	while (len--)
		printf("%02x", (unsigned char) *c++);
}
#endif

#ifdef _MODULE

#include <sys/module.h>

MODULE(MODULE_CLASS_DRIVER, cgd, NULL);

static int
cgd_modcmd(modcmd_t cmd, void *arg)
{
	int bmajor = -1, cmajor = -1,  error = 0;
	
	switch (cmd) {
	case MODULE_CMD_INIT:
		cgdattach(4);
		
		return devsw_attach("cgd", &cgd_bdevsw, &bmajor,
		    &cgd_cdevsw, &cmajor);
		break;

	case MODULE_CMD_FINI:
		return devsw_detach(&cgd_bdevsw, &cgd_cdevsw);
		break;

	case MODULE_CMD_STAT:
		return ENOTTY;

	default:
		return ENOTTY;
	}

	return error;
}

#endif
