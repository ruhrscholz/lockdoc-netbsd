/*	$NetBSD: sd.c,v 1.41 1994/10/20 16:54:44 mycroft Exp $	*/

/*
 * Copyright (c) 1994 Charles Hannum.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Charles Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * Originally written by Julian Elischer (julian@dialix.oz.au)
 * for TRW Financial Systems for use under the MACH(2.5) operating system.
 *
 * TRW Financial Systems, in accordance with their agreement with Carnegie
 * Mellon University, makes this software available to CMU to distribute
 * or use in any manner that they see fit as long as this message is kept with
 * the software. For this reason TFS also grants any other persons or
 * organisations permission to use or modify this software.
 *
 * TFS supplies this software to be publicly redistributed
 * on the understanding that TFS is not responsible for the correct
 * functioning of this software in any circumstances.
 *
 * Ported to run under 386BSD by Julian Elischer (julian@dialix.oz.au) Sept 1992
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/dkbad.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/buf.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/errno.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/disk.h>

#include <scsi/scsi_all.h>
#include <scsi/scsi_disk.h>
#include <scsi/scsiconf.h>

#ifdef	DDB
int     Debugger();
#else	/* DDB */
#define Debugger()
#endif	/* DDB */

#define	SDOUTSTANDING	2
#define	SDRETRIES	4

#define	SDUNIT(dev)			DISKUNIT(dev)
#define	SDPART(dev)			DISKPART(dev)
#define	MAKESDDEV(maj, unit, part)	MAKEDISKDEV(maj, unit, part)

#define	SDLABELDEV(dev)	(MAKESDDEV(major(dev), SDUNIT(dev), RAW_PART))

struct sd_data {
	struct device sc_dev;
	struct dkdevice sc_dk;

	int flags;
#define SDF_BSDLABEL	0x01	/* has a BSD disk label */
#define SDF_WRITEPROT	0x04	/* manual unit write protect */
#define	SDF_WLABEL	0x08	/* label is writable */
	struct scsi_link *sc_link;	/* contains our targ, lun etc. */
	u_int32 ad_info;	/* info about the adapter */
	u_int32 cmdscount;	/* cmds allowed outstanding by board */
	struct disk_parms {
		u_char heads;		/* Number of heads */
		u_int16 cyls;		/* Number of cylinders */
		u_char sectors;		/* Number of sectors/track */
		u_int32 blksize;	/* Number of bytes/sector */
		u_long disksize;	/* total number sectors */
	} params;
	u_int32 xfer_block_wait;
	struct buf buf_queue;
};

void sdattach __P((struct device *, struct device *, void *));

struct cfdriver sdcd = {
	NULL, "sd", scsi_targmatch, sdattach, DV_DISK, sizeof(struct sd_data)
};

int sdgetdisklabel __P((struct sd_data *));
int sd_get_parms __P((struct sd_data *, int));
void sdstrategy __P((struct buf *));
void sdstart __P((int));

struct dkdriver sddkdriver = { sdstrategy };

struct scsi_device sd_switch = {
	NULL,			/* Use default error handler */
	sdstart,		/* have a queue, served by this */
	NULL,			/* have no async handler */
	NULL,			/* Use default 'done' routine */
	"sd",
	0
};

/*
 * The routine called by the low level scsi routine when it discovers
 * a device suitable for this driver.
 */
void
sdattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct sd_data *sd = (void *)self;
	struct disk_parms *dp = &sd->params;
	struct scsi_link *sc_link = aux;

	SC_DEBUG(sc_link, SDEV_DB2, ("sdattach: "));

	/*
	 * Store information needed to contact our base driver
	 */
	sd->sc_link = sc_link;
	sc_link->device = &sd_switch;
	sc_link->dev_unit = self->dv_unit;

	sd->sc_dk.dk_driver = &sddkdriver;
#if !defined(i386) || defined(NEWCONFIG)
	dk_establish(&sd->sc_dk, &sd->sc_dev);
#endif

	if (sd->sc_link->adapter->adapter_info) {
		sd->ad_info = ((*(sd->sc_link->adapter->adapter_info)) (sc_link->adapter_softc));
		sd->cmdscount = sd->ad_info & AD_INF_MAX_CMDS;
		if (sd->cmdscount > SDOUTSTANDING)
			sd->cmdscount = SDOUTSTANDING;
	} else {
		sd->ad_info = 1;
		sd->cmdscount = 1;
	} 
	sc_link->opennings = sd->cmdscount;

	/*
	 * Use the subdriver to request information regarding
	 * the drive. We cannot use interrupts yet, so the
	 * request must specify this.
	 */
	sd_get_parms(sd, SCSI_NOSLEEP | SCSI_NOMASK);
        printf(": %dMB, %d cyl, %d head, %d sec, %d bytes/sec\n",
            dp->disksize / (1048576 / dp->blksize), dp->cyls,
            dp->heads, dp->sectors, dp->blksize);
}

/*
 * open the device. Make sure the partition info is a up-to-date as can be.
 */
int
sdopen(dev, flag, fmt, p)
	dev_t dev;
	int flag, fmt;
	struct proc *p;
{
	int error = 0;
	int unit, part;
	struct sd_data *sd;
	struct scsi_link *sc_link;

	unit = SDUNIT(dev);
	part = SDPART(dev);

	if (unit >= sdcd.cd_ndevs)
		return ENXIO;
	sd = sdcd.cd_devs[unit];
	if (!sd)
		return ENXIO;

	sc_link = sd->sc_link;

	SC_DEBUG(sc_link, SDEV_DB1,
	    ("sdopen: dev=0x%x (unit %d (of %d), partition %d)\n", dev, unit,
	    sdcd.cd_ndevs, part));

	/*
	 * If it's been invalidated, then forget the label
	 */
	if ((sc_link->flags & SDEV_MEDIA_LOADED) == 0) {
		sd->flags &= ~SDF_BSDLABEL;

		/*
		 * If somebody still has it open, then forbid re-entry.
		 */
		if (sd->sc_dk.dk_openmask != 0)
			return ENXIO;
	}

	/*
	 * "unit attention" errors should occur here if the 
	 * drive has been restarted or the pack changed.
	 * just ingnore the result, it's a decoy instruction
	 * The error code will act on the error though
	 * and invalidate any media information we had.
	 */
	scsi_test_unit_ready(sc_link, SCSI_SILENT);

	/*
	 * In case it is a funny one, tell it to start
	 * not needed for most hard drives (ignore failure)
	 */
	scsi_start(sc_link, SSS_START, SCSI_ERR_OK | SCSI_SILENT);

	/*
	 * Check that it is still responding and ok.
	 */
	sc_link->flags |= SDEV_OPEN;	/* unit attn becomes an err now */
	if (scsi_test_unit_ready(sc_link, 0)) {
		SC_DEBUG(sc_link, SDEV_DB3, ("device not reponding\n"));
		error = ENXIO;
		goto bad;
	}
	SC_DEBUG(sc_link, SDEV_DB3, ("device ok\n"));

	/* Lock the pack in. */
	scsi_prevent(sc_link, PR_PREVENT, SCSI_ERR_OK | SCSI_SILENT);

	/*
	 * Load the physical device parameters 
	 */
	if (sd_get_parms(sd, 0)) {
		error = ENXIO;
		goto bad;
	}
	SC_DEBUG(sc_link, SDEV_DB3, ("Params loaded "));

	/*
	 * Load the partition info if not already loaded.
	 */
	if ((error = sdgetdisklabel(sd)) && (part != RAW_PART))
		goto bad;
	SC_DEBUG(sc_link, SDEV_DB3, ("Disklabel loaded "));

	/*
	 *  Check that the partition exists
	 */
	if (part != RAW_PART &&
	    (part >= sd->sc_dk.dk_label.d_npartitions ||
	     sd->sc_dk.dk_label.d_partitions[part].p_fstype == FS_UNUSED)) {
		error = ENXIO;
		goto bad;
	}

	/* Insure only one open at a time. */
	switch (fmt) {
	case S_IFCHR:
		sd->sc_dk.dk_copenmask |= (1 << part);
		break;
	case S_IFBLK:
		sd->sc_dk.dk_bopenmask |= (1 << part);
		break;
	}
	sd->sc_dk.dk_openmask = sd->sc_dk.dk_copenmask | sd->sc_dk.dk_bopenmask;

	SC_DEBUG(sc_link, SDEV_DB3, ("open complete\n"));
	sc_link->flags |= SDEV_MEDIA_LOADED;
	return 0;

bad:
	if (sd->sc_dk.dk_openmask == 0) {
		scsi_prevent(sc_link, PR_ALLOW, SCSI_ERR_OK | SCSI_SILENT);
		sc_link->flags &= ~SDEV_OPEN;
	}
	return error;
}

/*
 * close the device.. only called if we are the LAST occurence of an open
 * device.  Convenient now but usually a pain.
 */
int 
sdclose(dev, flag, fmt)
	dev_t dev;
	int flag, fmt;
{
	struct sd_data *sd = sdcd.cd_devs[SDUNIT(dev)];
	int part = SDPART(dev);

	switch (fmt) {
	case S_IFCHR:
		sd->sc_dk.dk_copenmask &= ~(1 << part);
		break;
	case S_IFBLK:
		sd->sc_dk.dk_bopenmask &= ~(1 << part);
		break;
	}
	sd->sc_dk.dk_openmask = sd->sc_dk.dk_copenmask | sd->sc_dk.dk_bopenmask;

	if (sd->sc_dk.dk_openmask == 0) {
		scsi_prevent(sd->sc_link, PR_ALLOW, SCSI_ERR_OK | SCSI_SILENT);
		sd->sc_link->flags &= ~SDEV_OPEN;
	}
	return 0;
}

/*
 * trim the size of the transfer if needed, called by physio
 * basically the smaller of our max and the scsi driver's
 * minphys (note we have no max)
 *
 * Trim buffer length if buffer-size is bigger than page size
 */
void 
sdminphys(bp)
	struct buf *bp;
{
	register struct sd_data *sd = sdcd.cd_devs[SDUNIT(bp->b_dev)];

	(sd->sc_link->adapter->scsi_minphys) (bp);
}

/*
 * Actually translate the requested transfer into one the physical driver
 * can understand.  The transfer is described by a buf and will include
 * only one physical transfer.
 */
void 
sdstrategy(bp)
	struct buf *bp;
{
	int opri;
	struct sd_data *sd;
	int unit;

	unit = SDUNIT(bp->b_dev);
	sd = sdcd.cd_devs[unit];
	SC_DEBUG(sd->sc_link, SDEV_DB2, ("sdstrategy "));
	SC_DEBUG(sd->sc_link, SDEV_DB1,
	    ("%d bytes @ blk %d\n", bp->b_bcount, bp->b_blkno));
	sdminphys(bp);
	/*
	 * If the device has been made invalid, error out
	 */
	if (!(sd->sc_link->flags & SDEV_MEDIA_LOADED)) {
		sd->flags &= ~SDF_BSDLABEL;
		bp->b_error = EIO;
		goto bad;
	}
	/*
	 * "soft" write protect check
	 */
	if ((sd->flags & SDF_WRITEPROT) && (bp->b_flags & B_READ) == 0) {
		bp->b_error = EROFS;
		goto bad;
	}
	/*
	 * If it's a null transfer, return immediatly
	 */
	if (bp->b_bcount == 0)
		goto done;
	/*
	 * Decide which unit and partition we are talking about
	 * only raw is ok if no label
	 */
	if (SDPART(bp->b_dev) != RAW_PART) {
		if (!(sd->flags & SDF_BSDLABEL)) {
			bp->b_error = EIO;
			goto bad;
		}
		/*
		 * do bounds checking, adjust transfer. if error, process.
		 * if end of partition, just return
		 */
		if (bounds_check_with_label(bp, &sd->sc_dk.dk_label,
		    (sd->flags & SDF_WLABEL) != 0) <= 0)
			goto done;
		/* otherwise, process transfer request */
	}

	opri = splbio();

	/*
	 * Place it in the queue of disk activities for this disk
	 */
	disksort(&sd->buf_queue, bp);

	/*
	 * Tell the device to get going on the transfer if it's
	 * not doing anything, otherwise just wait for completion
	 */
	sdstart(unit);

	splx(opri);
	return;

bad:
	bp->b_flags |= B_ERROR;
done:
	/*
	 * Correctly set the buf to indicate a completed xfer
	 */
	bp->b_resid = bp->b_bcount;
	biodone(bp);
}

/*
 * sdstart looks to see if there is a buf waiting for the device
 * and that the device is not already busy. If both are true,
 * It dequeues the buf and creates a scsi command to perform the
 * transfer in the buf. The transfer request will call scsi_done
 * on completion, which will in turn call this routine again
 * so that the next queued transfer is performed.
 * The bufs are queued by the strategy routine (sdstrategy)
 *
 * This routine is also called after other non-queued requests
 * have been made of the scsi driver, to ensure that the queue
 * continues to be drained.
 *
 * must be called at the correct (highish) spl level
 * sdstart() is called at splbio from sdstrategy and scsi_done
 */
void 
sdstart(unit)
	int unit;
{
	register struct sd_data *sd = sdcd.cd_devs[unit];
	register struct	scsi_link *sc_link = sd->sc_link;
	struct buf *bp = 0;
	struct buf *dp;
	struct scsi_rw_big cmd;
	int blkno, nblks;
	struct partition *p;

	SC_DEBUG(sc_link, SDEV_DB2, ("sdstart "));
	/*
	 * Check if the device has room for another command
	 */
	while (sc_link->opennings) {
		/*
		 * there is excess capacity, but a special waits 
		 * It'll need the adapter as soon as we clear out of the
		 * way and let it run (user level wait).
		 */
		if (sc_link->flags & SDEV_WAITING) {
			sc_link->flags &= ~SDEV_WAITING;
			wakeup((caddr_t)sc_link);
			return;
		}

		/*
		 * See if there is a buf with work for us to do..
		 */
		dp = &sd->buf_queue;
		if ((bp = dp->b_actf) == NULL)	/* yes, an assign */
			return;
		dp->b_actf = bp->b_actf;

		/*
		 * If the device has become invalid, abort all the
		 * reads and writes until all files have been closed and
		 * re-openned
		 */
		if (!(sc_link->flags & SDEV_MEDIA_LOADED)) {
			sd->flags &= ~SDF_BSDLABEL;
			bp->b_error = EIO;
			bp->b_flags |= B_ERROR;
			biodone(bp);
			continue;
		}

		/*
		 * We have a buf, now we know we are going to go through
		 * With this thing..
		 *
		 *  First, translate the block to absolute
		 */
		blkno =
		    bp->b_blkno / (sd->sc_dk.dk_label.d_secsize / DEV_BSIZE);
		if (SDPART(bp->b_dev) != RAW_PART) {
			p = &sd->sc_dk.dk_label.d_partitions[SDPART(bp->b_dev)];
			blkno += p->p_offset;
		}
		nblks = howmany(bp->b_bcount, sd->sc_dk.dk_label.d_secsize);

		/*
		 *  Fill out the scsi command
		 */
		bzero(&cmd, sizeof(cmd));
		cmd.op_code = (bp->b_flags & B_READ) ? READ_BIG : WRITE_BIG;
		cmd.addr_3 = (blkno & 0xff000000) >> 24;
		cmd.addr_2 = (blkno & 0xff0000) >> 16;
		cmd.addr_1 = (blkno & 0xff00) >> 8;
		cmd.addr_0 = blkno & 0xff;
		cmd.length2 = (nblks & 0xff00) >> 8;
		cmd.length1 = (nblks & 0xff);

		/*
		 * Call the routine that chats with the adapter.
		 * Note: we cannot sleep as we may be an interrupt
		 */
		if (scsi_scsi_cmd(sc_link, (struct scsi_generic *)&cmd,
		    sizeof(cmd), (u_char *)bp->b_data, bp->b_bcount,
		    SDRETRIES, 10000, bp, SCSI_NOSLEEP |
		    ((bp->b_flags & B_READ) ? SCSI_DATA_IN : SCSI_DATA_OUT))
		    != SUCCESSFULLY_QUEUED)
			printf("%s: not queued", sd->sc_dev.dv_xname);
	}
}

/*
 * Perform special action on behalf of the user
 * Knows about the internals of this device
 */
int 
sdioctl(dev, cmd, addr, flag)
	dev_t dev;
	int cmd;
	caddr_t addr;
	int flag;
{
	int error = 0;
	int unit, part;
	register struct sd_data *sd;

	/*
	 * Find the device that the user is talking about
	 */
	unit = SDUNIT(dev);
	part = SDPART(dev);
	sd = sdcd.cd_devs[unit];
	SC_DEBUG(sd->sc_link, SDEV_DB1, ("sdioctl (0x%x)", cmd));

	/*
	 * If the device is not valid.. abandon ship
	 */
	if (!(sd->sc_link->flags & SDEV_MEDIA_LOADED))
		return EIO;
	switch (cmd) {

	case DIOCSBAD:
		return EINVAL;

	case DIOCGDINFO:
		*(struct disklabel *)addr = sd->sc_dk.dk_label;
		return 0;

	case DIOCGPART:
		((struct partinfo *)addr)->disklab = &sd->sc_dk.dk_label;
		((struct partinfo *)addr)->part =
		    &sd->sc_dk.dk_label.d_partitions[SDPART(dev)];
		return 0;

	case DIOCSDINFO:
		if ((flag & FWRITE) == 0)
			return EBADF;
		error = setdisklabel(&sd->sc_dk.dk_label,
		    (struct disklabel *)addr,
		    /*(sd->flags & SDF_BSDLABEL) ? sd->sc_dk.dk_openmask : */0,
		    &sd->sc_dk.dk_cpulabel);
		if (error == 0)
			sd->flags |= SDF_BSDLABEL;
		return error;

	case DIOCWLABEL:
		sd->flags &= ~SDF_WRITEPROT;
		if ((flag & FWRITE) == 0)
			return EBADF;
		if (*(int *)addr)
			sd->flags |= SDF_WLABEL;
		else
			sd->flags &= ~SDF_WLABEL;
		return 0;

	case DIOCWDINFO:
		sd->flags &= ~SDF_WRITEPROT;
		if ((flag & FWRITE) == 0)
			return EBADF;
		error = setdisklabel(&sd->sc_dk.dk_label,
		    (struct disklabel *)addr,
		    /*(sd->flags & SDF_BSDLABEL) ? sd->sc_dk.dk_openmask : */0,
		    &sd->sc_dk.dk_cpulabel);
		if (error == 0) {
			sd->flags |= SDF_BSDLABEL;

			/* simulate opening partition 0 so write succeeds */
			sd->sc_dk.dk_openmask |= (1 << 0);	/* XXX */
			error = writedisklabel(SDLABELDEV(dev), sdstrategy,
			    &sd->sc_dk.dk_label, &sd->sc_dk.dk_cpulabel);
			sd->sc_dk.dk_openmask =
			    sd->sc_dk.dk_copenmask | sd->sc_dk.dk_bopenmask;
		}
		return error;

	default:
		if (part != RAW_PART)
			return ENOTTY;
		return scsi_do_ioctl(sd->sc_link, cmd, addr, flag);
	}
#ifdef DIAGNOSTIC
	panic("sdioctl: impossible");
#endif
}

/*
 * Load the label information on the named device
 */
int 
sdgetdisklabel(sd)
	struct sd_data *sd;
{
	char *errstring;

	/*
	 * If the inflo is already loaded, use it
	 */
	if (sd->flags & SDF_BSDLABEL)
		return 0;

	bzero(&sd->sc_dk.dk_label, sizeof(struct disklabel));
	bzero(&sd->sc_dk.dk_cpulabel, sizeof(struct cpu_disklabel));
	/*
	 * make partition 3 the whole disk in case of failure then get pdinfo
	 * for historical reasons, make part a same as raw part
	 */
	sd->sc_dk.dk_label.d_partitions[0].p_offset = 0;
	sd->sc_dk.dk_label.d_partitions[0].p_size =
	    sd->params.disksize * (sd->params.blksize / DEV_BSIZE);
	sd->sc_dk.dk_label.d_partitions[0].p_fstype = 9;	/* XXXX */
	sd->sc_dk.dk_label.d_partitions[RAW_PART].p_offset = 0;
	sd->sc_dk.dk_label.d_partitions[RAW_PART].p_size =
	    sd->params.disksize * (sd->params.blksize / DEV_BSIZE);
	sd->sc_dk.dk_label.d_npartitions = MAXPARTITIONS;

	sd->sc_dk.dk_label.d_secsize = sd->params.blksize;
	sd->sc_dk.dk_label.d_ntracks = sd->params.heads;
	sd->sc_dk.dk_label.d_nsectors = sd->params.sectors;
	sd->sc_dk.dk_label.d_ncylinders = sd->params.cyls;
	sd->sc_dk.dk_label.d_secpercyl = sd->params.heads * sd->params.sectors;
	if (sd->sc_dk.dk_label.d_secpercyl == 0) {
		sd->sc_dk.dk_label.d_secpercyl = 100;
		/* as long as it's not 0 - readdisklabel divides by it (?) */
	}

	/*
	 * Call the generic disklabel extraction routine
	 */
	if (errstring = readdisklabel(MAKESDDEV(0, sd->sc_dev.dv_unit,
	    RAW_PART), sdstrategy, &sd->sc_dk.dk_label,
	    &sd->sc_dk.dk_cpulabel)) {
		printf("%s: %s\n", sd->sc_dev.dv_xname, errstring);
		return ENXIO;
	}
	sd->flags |= SDF_BSDLABEL;	/* WE HAVE IT ALL NOW */
	return 0;
}

/*
 * Find out from the device what it's capacity is
 */
u_int32 
sd_size(sd, flags)
	struct sd_data *sd;
	int flags;
{
	struct scsi_read_cap_data rdcap;
	struct scsi_read_capacity scsi_cmd;
	u_int32 size;

	/*
	 * make up a scsi command and ask the scsi driver to do
	 * it for you.
	 */
	bzero(&scsi_cmd, sizeof(scsi_cmd));
	scsi_cmd.op_code = READ_CAPACITY;

	/*
	 * If the command works, interpret the result as a 4 byte
	 * number of blocks
	 */
	if (scsi_scsi_cmd(sd->sc_link, (struct scsi_generic *)&scsi_cmd,
	    sizeof(scsi_cmd), (u_char *)&rdcap, sizeof(rdcap), SDRETRIES,
	    2000, NULL, flags | SCSI_DATA_IN) != 0) {
		printf("%s: could not get size\n", sd->sc_dev.dv_xname);
		return 0;
	} else {
		size = (rdcap.addr_3 << 24) + (rdcap.addr_2 << 16) +
		    (rdcap.addr_1 << 8) + rdcap.addr_0 + 1;
	}
	return size;
}

/*
 * Tell the device to map out a defective block
 */
int 
sd_reassign_blocks(sd, block)
	struct sd_data *sd;
	int block;
{
	struct scsi_reassign_blocks scsi_cmd;
	struct scsi_reassign_blocks_data rbdata;

	bzero(&scsi_cmd, sizeof(scsi_cmd));
	bzero(&rbdata, sizeof(rbdata));
	scsi_cmd.op_code = REASSIGN_BLOCKS;

	rbdata.length_msb = 0;
	rbdata.length_lsb = sizeof(rbdata.defect_descriptor[0]);
	rbdata.defect_descriptor[0].dlbaddr_3 = ((block >> 24) & 0xff);
	rbdata.defect_descriptor[0].dlbaddr_2 = ((block >> 16) & 0xff);
	rbdata.defect_descriptor[0].dlbaddr_1 = ((block >> 8) & 0xff);
	rbdata.defect_descriptor[0].dlbaddr_0 = ((block) & 0xff);

	return scsi_scsi_cmd(sd->sc_link, (struct scsi_generic *)&scsi_cmd,
	    sizeof(scsi_cmd), (u_char *)&rbdata, sizeof(rbdata), SDRETRIES,
	    5000, NULL, SCSI_DATA_OUT);
}

#define b2tol(a)	(((unsigned)(a##_1) << 8) + (unsigned)a##_0 )

/*
 * Get the scsi driver to send a full inquiry to the * device and use the
 * results to fill out the disk parameter structure.
 */
int 
sd_get_parms(sd, flags)
	struct sd_data *sd;
	int flags;
{
	struct disk_parms *disk_parms = &sd->params;
	struct scsi_mode_sense scsi_cmd;
	struct scsi_mode_sense_data {
		struct scsi_mode_header header;
		struct blk_desc blk_desc;
		union disk_pages pages;
	} scsi_sense;
	u_int32 sectors;

	/*
	 * First check if we have it all loaded
	 */
	if (sd->sc_link->flags & SDEV_MEDIA_LOADED)
		return 0;

	/*
	 * do a "mode sense page 4"
	 */
	bzero(&scsi_cmd, sizeof(scsi_cmd));
	scsi_cmd.op_code = MODE_SENSE;
	scsi_cmd.page = 4;
	scsi_cmd.length = 0x20;
	/*
	 * If the command worked, use the results to fill out
	 * the parameter structure
	 */
	if (scsi_scsi_cmd(sd->sc_link, (struct scsi_generic *)&scsi_cmd,
	    sizeof(scsi_cmd), (u_char *)&scsi_sense, sizeof(scsi_sense),
	    SDRETRIES, 6000, NULL, flags | SCSI_DATA_IN) != 0) {
		printf("%s: could not mode sense (4)", sd->sc_dev.dv_xname);
	fake_it:
		printf("; using ficticious geometry\n");
		/*
		 * use adaptec standard ficticious geometry
		 * this depends on which controller (e.g. 1542C is
		 * different. but we have to put SOMETHING here..)
		 */
		sectors = sd_size(sd, flags);
		disk_parms->heads = 64;
		disk_parms->sectors = 32;
		disk_parms->cyls = sectors / (64 * 32);
		disk_parms->blksize = 512;
		disk_parms->disksize = sectors;
	} else {
		SC_DEBUG(sd->sc_link, SDEV_DB3,
		    ("%d cyls, %d heads, %d precomp, %d red_write, %d land_zone\n",
		    _3btol(&scsi_sense.pages.rigid_geometry.ncyl_2),
		    scsi_sense.pages.rigid_geometry.nheads,
		    b2tol(scsi_sense.pages.rigid_geometry.st_cyl_wp),
		    b2tol(scsi_sense.pages.rigid_geometry.st_cyl_rwc),
		    b2tol(scsi_sense.pages.rigid_geometry.land_zone)));

		/*
		 * KLUDGE!! (for zone recorded disks)
		 * give a number of sectors so that sec * trks * cyls
		 * is <= disk_size 
		 * can lead to wasted space! THINK ABOUT THIS !
		 */
		disk_parms->heads = scsi_sense.pages.rigid_geometry.nheads;
		disk_parms->cyls =
		    _3btol(&scsi_sense.pages.rigid_geometry.ncyl_2);
		disk_parms->blksize = _3btol(scsi_sense.blk_desc.blklen);

		if (!disk_parms->heads || !disk_parms->cyls ||
		    !disk_parms->blksize) {
			printf("%s: mode sense (4) returned nonsense",
			    sd->sc_dev.dv_xname);
			goto fake_it;
		}

		sectors = sd_size(sd, flags);
		disk_parms->disksize = sectors;
		sectors /= (disk_parms->heads * disk_parms->cyls);
		disk_parms->sectors = sectors;	/* XXX dubious on SCSI */
	}
	sd->sc_link->flags |= SDEV_MEDIA_LOADED;
	return 0;
}

int
sdsize(dev_t dev)
{
	int unit = SDUNIT(dev), part = SDPART(dev), val;
	struct sd_data *sd;

	if (unit >= sdcd.cd_ndevs)
		return -1;
	sd = sdcd.cd_devs[unit];
	if (!sd)
		return -1;

	if ((sd->flags & SDF_BSDLABEL) == 0) {
		val = sdopen(MAKESDDEV(major(dev), unit, RAW_PART), FREAD,
		    S_IFBLK, 0);
		if (val != 0)
			return -1;
	}
	if (sd->flags & SDF_WRITEPROT)
		return -1;
	return sd->sc_dk.dk_label.d_partitions[part].p_size;
}


#define SCSIDUMP 1
#undef	SCSIDUMP
#define NOT_TRUSTED 1

#ifdef SCSIDUMP
#include <vm/vm.h>

static struct scsi_xfer sx;
#define	MAXTRANSFER 8		/* 1 page at a time */

/*
 * dump all of physical memory into the partition specified, starting
 * at offset 'dumplo' into the partition.
 */
int
sddump(dev_t dev)
{				/* dump core after a system crash */
	register struct sd_data *sd;	/* disk unit to do the IO */
	int32	num;		/* number of sectors to write */
	u_int32	unit, part;
	int32	blkoff, blknum, blkcnt = MAXTRANSFER;
	int32	nblocks;
	char	*addr;
	struct	scsi_rw_big cmd;
	extern	int Maxmem;
	static	int sddoingadump = 0;
#define MAPTO CADDR1
	extern	caddr_t MAPTO;	/* map the page we are about to write, here */
	struct	scsi_xfer *xs = &sx;
	int	retval;
	int	c;

	addr = (char *) 0;	/* starting address */

	/* toss any characters present prior to dump */
	while ((c = sgetc(1)) && (c != 0x100)); /*syscons and pccons differ */

	/* size of memory to dump */
	num = Maxmem;
	unit = SDUNIT(dev);	/* eventually support floppies? */
	part = SDPART(dev);	/* file system */
	/* check for acceptable drive number */
	if (unit >= sdcd.cd_ndevs)
		return ENXIO;

	sd = sd_data[unit];
	if (!sd)
		return ENXIO;
	if (sd->sc_link->flags & SDEV_MEDIA_LOADED != SDEV_MEDIA_LOADED)
		return ENXIO;
	if (sd->flags & SDF_WRITEPROT)
		return ENXIO;

	/* Convert to disk sectors */
	num = (u_int32)num * NBPG / sd->sc_dk.dk_label.d_secsize;

	/* check if controller active */
	if (sddoingadump)
		return EFAULT;

	nblocks = sd->sc_dk.dk_label.d_partitions[part].p_size;
	blkoff = sd->sc_dk.dk_label.d_partitions[part].p_offset;

	/* check transfer bounds against partition size */
	if ((dumplo < 0) || ((dumplo + num) > nblocks))
		return EINVAL;

	sddoingadump = 1;

	blknum = dumplo + blkoff;
	while (num > 0) {
		pmap_enter(kernel_pmap,
		    MAPTO,
		    trunc_page(addr),
		    VM_PROT_READ,
		    TRUE);
#ifndef	NOT_TRUSTED
		/*
		 *  Fill out the scsi command
		 */
		bzero(&cmd, sizeof(cmd));
		cmd.op_code = WRITE_BIG;
		cmd.addr_3 = (blknum & 0xff000000) >> 24;
		cmd.addr_2 = (blknum & 0xff0000) >> 16;
		cmd.addr_1 = (blknum & 0xff00) >> 8;
		cmd.addr_0 = blknum & 0xff;
		cmd.length2 = (blkcnt & 0xff00) >> 8;
		cmd.length1 = (blkcnt & 0xff);
		/*
		 * Fill out the scsi_xfer structure
		 *    Note: we cannot sleep as we may be an interrupt
		 * don't use scsi_scsi_cmd() as it may want 
		 * to wait for an xs.
		 */
		bzero(xs, sizeof(sx));
		xs->flags |= SCSI_NOMASK | SCSI_NOSLEEP | INUSE;
		xs->sc_link = sd->sc_link;
		xs->retries = SDRETRIES;
		xs->timeout = 10000;	/* 10000 millisecs for a disk ! */
		xs->cmd = (struct scsi_generic *)&cmd;
		xs->cmdlen = sizeof(cmd);
		xs->resid = blkcnt * 512;
		xs->error = XS_NOERROR;
		xs->bp = 0;
		xs->data = (u_char *) MAPTO;
		xs->datalen = blkcnt * 512;

		/*
		 * Pass all this info to the scsi driver.
		 */
		retval = (*(sd->sc_link->adapter->scsi_cmd)) (xs);
		switch (retval) {
		case SUCCESSFULLY_QUEUED:
		case HAD_ERROR:
			return ENXIO;		/* we said not to sleep! */
		case COMPLETE:
			break;
		default:
			return ENXIO;		/* we said not to sleep! */
		}
#else	/* NOT_TRUSTED */
		/* lets just talk about this first... */
		printf("sd%d: dump addr 0x%x, blk %d\n", unit, addr, blknum);
#endif	/* NOT_TRUSTED */

		if ((unsigned)addr % (1024 * 1024) == 0)
			printf("%d ", num / 2048);
		/* update block count */
		num -= blkcnt;
		blknum += blkcnt;
		(int)addr += 512 * blkcnt;

		/* operator aborting dump? */
		if ((c = sgetc(1)) && (c != 0x100))
			return EINTR;
	}
	return 0;
}
#else	/* SCSIDUMP */
int
sddump()
{
	printf("\nsddump()        -- not implemented\n");
	delay(60000000);	/* 60 seconds */
	return -1;
}
#endif	/* SCSIDUMP */
