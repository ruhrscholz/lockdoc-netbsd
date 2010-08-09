/*	$NetBSD: ffs_vfsops.c,v 1.262 2010/08/09 17:12:18 pooka Exp $	*/

/*-
 * Copyright (c) 2008, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Wasabi Systems, Inc, and by Andrew Doran.
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

/*
 * Copyright (c) 1989, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ffs_vfsops.c	8.31 (Berkeley) 5/20/95
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ffs_vfsops.c,v 1.262 2010/08/09 17:12:18 pooka Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ffs.h"
#include "opt_quota.h"
#include "opt_wapbl.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/socket.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/device.h>
#include <sys/mbuf.h>
#include <sys/file.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/malloc.h>
#include <sys/pool.h>
#include <sys/lock.h>
#include <sys/sysctl.h>
#include <sys/conf.h>
#include <sys/kauth.h>
#include <sys/wapbl.h>
#include <sys/fstrans.h>
#include <sys/module.h>

#include <miscfs/genfs/genfs.h>
#include <miscfs/specfs/specdev.h>

#include <ufs/ufs/quota.h>
#include <ufs/ufs/ufsmount.h>
#include <ufs/ufs/inode.h>
#include <ufs/ufs/dir.h>
#include <ufs/ufs/ufs_extern.h>
#include <ufs/ufs/ufs_bswap.h>
#include <ufs/ufs/ufs_wapbl.h>

#include <ufs/ffs/fs.h>
#include <ufs/ffs/ffs_extern.h>

MODULE(MODULE_CLASS_VFS, ffs, NULL);

static int	ffs_vfs_fsync(vnode_t *, int);

static struct sysctllog *ffs_sysctl_log;

/* how many times ffs_init() was called */
int ffs_initcount = 0;

extern const struct vnodeopv_desc ffs_vnodeop_opv_desc;
extern const struct vnodeopv_desc ffs_specop_opv_desc;
extern const struct vnodeopv_desc ffs_fifoop_opv_desc;

const struct vnodeopv_desc * const ffs_vnodeopv_descs[] = {
	&ffs_vnodeop_opv_desc,
	&ffs_specop_opv_desc,
	&ffs_fifoop_opv_desc,
	NULL,
};

struct vfsops ffs_vfsops = {
	MOUNT_FFS,
	sizeof (struct ufs_args),
	ffs_mount,
	ufs_start,
	ffs_unmount,
	ufs_root,
	ufs_quotactl,
	ffs_statvfs,
	ffs_sync,
	ffs_vget,
	ffs_fhtovp,
	ffs_vptofh,
	ffs_init,
	ffs_reinit,
	ffs_done,
	ffs_mountroot,
	ffs_snapshot,
	ffs_extattrctl,
	ffs_suspendctl,
	genfs_renamelock_enter,
	genfs_renamelock_exit,
	ffs_vfs_fsync,
	ffs_vnodeopv_descs,
	0,
	{ NULL, NULL },
};

static const struct genfs_ops ffs_genfsops = {
	.gop_size = ffs_gop_size,
	.gop_alloc = ufs_gop_alloc,
	.gop_write = genfs_gop_write,
	.gop_markupdate = ufs_gop_markupdate,
};

static const struct ufs_ops ffs_ufsops = {
	.uo_itimes = ffs_itimes,
	.uo_update = ffs_update,
	.uo_truncate = ffs_truncate,
	.uo_valloc = ffs_valloc,
	.uo_vfree = ffs_vfree,
	.uo_balloc = ffs_balloc,
	.uo_unmark_vnode = (void (*)(vnode_t *))nullop,
};

static int
ffs_modcmd(modcmd_t cmd, void *arg)
{
	int error;

#if 0
	extern int doasyncfree;
#endif
	extern int ffs_log_changeopt;

	switch (cmd) {
	case MODULE_CMD_INIT:
		error = vfs_attach(&ffs_vfsops);
		if (error != 0)
			break;

		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT,
			       CTLTYPE_NODE, "vfs", NULL,
			       NULL, 0, NULL, 0,
			       CTL_VFS, CTL_EOL);
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT,
			       CTLTYPE_NODE, "ffs",
			       SYSCTL_DESCR("Berkeley Fast File System"),
			       NULL, 0, NULL, 0,
			       CTL_VFS, 1, CTL_EOL);

		/*
		 * @@@ should we even bother with these first three?
		 */
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "doclusterread", NULL,
			       sysctl_notavail, 0, NULL, 0,
			       CTL_VFS, 1, FFS_CLUSTERREAD, CTL_EOL);
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "doclusterwrite", NULL,
			       sysctl_notavail, 0, NULL, 0,
			       CTL_VFS, 1, FFS_CLUSTERWRITE, CTL_EOL);
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "doreallocblks", NULL,
			       sysctl_notavail, 0, NULL, 0,
			       CTL_VFS, 1, FFS_REALLOCBLKS, CTL_EOL);
#if 0
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "doasyncfree",
			       SYSCTL_DESCR("Release dirty blocks asynchronously"),
			       NULL, 0, &doasyncfree, 0,
			       CTL_VFS, 1, FFS_ASYNCFREE, CTL_EOL);
#endif
		sysctl_createv(&ffs_sysctl_log, 0, NULL, NULL,
			       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
			       CTLTYPE_INT, "log_changeopt",
			       SYSCTL_DESCR("Log changes in optimization strategy"),
			       NULL, 0, &ffs_log_changeopt, 0,
			       CTL_VFS, 1, FFS_LOG_CHANGEOPT, CTL_EOL);
		break;
	case MODULE_CMD_FINI:
		error = vfs_detach(&ffs_vfsops);
		if (error != 0)
			break;
		sysctl_teardown(&ffs_sysctl_log);
		break;
	default:
		error = ENOTTY;
		break;
	}

	return (error);
}

pool_cache_t ffs_inode_cache;
pool_cache_t ffs_dinode1_cache;
pool_cache_t ffs_dinode2_cache;

static void ffs_oldfscompat_read(struct fs *, struct ufsmount *, daddr_t);
static void ffs_oldfscompat_write(struct fs *, struct ufsmount *);

/*
 * Called by main() when ffs is going to be mounted as root.
 */

int
ffs_mountroot(void)
{
	struct fs *fs;
	struct mount *mp;
	struct lwp *l = curlwp;			/* XXX */
	struct ufsmount *ump;
	int error;

	if (device_class(root_device) != DV_DISK)
		return (ENODEV);

	if ((error = vfs_rootmountalloc(MOUNT_FFS, "root_device", &mp))) {
		vrele(rootvp);
		return (error);
	}

	/*
	 * We always need to be able to mount the root file system.
	 */
	mp->mnt_flag |= MNT_FORCE;
	if ((error = ffs_mountfs(rootvp, mp, l)) != 0) {
		vfs_unbusy(mp, false, NULL);
		vfs_destroy(mp);
		return (error);
	}
	mp->mnt_flag &= ~MNT_FORCE;
	mutex_enter(&mountlist_lock);
	CIRCLEQ_INSERT_TAIL(&mountlist, mp, mnt_list);
	mutex_exit(&mountlist_lock);
	ump = VFSTOUFS(mp);
	fs = ump->um_fs;
	memset(fs->fs_fsmnt, 0, sizeof(fs->fs_fsmnt));
	(void)copystr(mp->mnt_stat.f_mntonname, fs->fs_fsmnt, MNAMELEN - 1, 0);
	(void)ffs_statvfs(mp, &mp->mnt_stat);
	vfs_unbusy(mp, false, NULL);
	setrootfstime((time_t)fs->fs_time);
	return (0);
}

/*
 * VFS Operations.
 *
 * mount system call
 */
int
ffs_mount(struct mount *mp, const char *path, void *data, size_t *data_len)
{
	struct lwp *l = curlwp;
	struct vnode *devvp = NULL;
	struct ufs_args *args = data;
	struct ufsmount *ump = NULL;
	struct fs *fs;
	int error = 0, flags, update;
	mode_t accessmode;

	if (*data_len < sizeof *args)
		return EINVAL;

	if (mp->mnt_flag & MNT_GETARGS) {
		ump = VFSTOUFS(mp);
		if (ump == NULL)
			return EIO;
		args->fspec = NULL;
		*data_len = sizeof *args;
		return 0;
	}

	update = mp->mnt_flag & MNT_UPDATE;

	/* Check arguments */
	if (args->fspec != NULL) {
		/*
		 * Look up the name and verify that it's sane.
		 */
		error = namei_simple_user(args->fspec,
					NSM_FOLLOW_NOEMULROOT, &devvp);
		if (error != 0)
			return (error);

		if (!update) {
			/*
			 * Be sure this is a valid block device
			 */
			if (devvp->v_type != VBLK)
				error = ENOTBLK;
			else if (bdevsw_lookup(devvp->v_rdev) == NULL)
				error = ENXIO;
		} else {
			/*
			 * Be sure we're still naming the same device
			 * used for our initial mount
			 */
			ump = VFSTOUFS(mp);
			if (devvp != ump->um_devvp) {
				if (devvp->v_rdev != ump->um_devvp->v_rdev)
					error = EINVAL;
				else {
					vrele(devvp);
					devvp = ump->um_devvp;
					vref(devvp);
				}
			}
		}
	} else {
		if (!update) {
			/* New mounts must have a filename for the device */
			return (EINVAL);
		} else {
			/* Use the extant mount */
			ump = VFSTOUFS(mp);
			devvp = ump->um_devvp;
			vref(devvp);
		}
	}

	/*
	 * If mount by non-root, then verify that user has necessary
	 * permissions on the device.
	 *
	 * Permission to update a mount is checked higher, so here we presume
	 * updating the mount is okay (for example, as far as securelevel goes)
	 * which leaves us with the normal check.
	 */
	if (error == 0) {
		accessmode = VREAD;
		if (update ?
		    (mp->mnt_iflag & IMNT_WANTRDWR) != 0 :
		    (mp->mnt_flag & MNT_RDONLY) == 0)
			accessmode |= VWRITE;
		vn_lock(devvp, LK_EXCLUSIVE | LK_RETRY);
		error = genfs_can_mount(devvp, accessmode, l->l_cred);
		VOP_UNLOCK(devvp);
	}

	if (error) {
		vrele(devvp);
		return (error);
	}

#ifdef WAPBL
	/* WAPBL can only be enabled on a r/w mount. */
	if ((mp->mnt_flag & MNT_RDONLY) && !(mp->mnt_iflag & IMNT_WANTRDWR)) {
		mp->mnt_flag &= ~MNT_LOG;
	}
#else /* !WAPBL */
	mp->mnt_flag &= ~MNT_LOG;
#endif /* !WAPBL */

	if (!update) {
		int xflags;

		if (mp->mnt_flag & MNT_RDONLY)
			xflags = FREAD;
		else
			xflags = FREAD | FWRITE;
		error = VOP_OPEN(devvp, xflags, FSCRED);
		if (error)
			goto fail;
		error = ffs_mountfs(devvp, mp, l);
		if (error) {
			vn_lock(devvp, LK_EXCLUSIVE | LK_RETRY);
			(void)VOP_CLOSE(devvp, xflags, NOCRED);
			VOP_UNLOCK(devvp);
			goto fail;
		}

		ump = VFSTOUFS(mp);
		fs = ump->um_fs;
	} else {
		/*
		 * Update the mount.
		 */

		/*
		 * The initial mount got a reference on this
		 * device, so drop the one obtained via
		 * namei(), above.
		 */
		vrele(devvp);

		ump = VFSTOUFS(mp);
		fs = ump->um_fs;
		if (fs->fs_ronly == 0 && (mp->mnt_flag & MNT_RDONLY)) {
			/*
			 * Changing from r/w to r/o
			 */
			flags = WRITECLOSE;
			if (mp->mnt_flag & MNT_FORCE)
				flags |= FORCECLOSE;
			error = ffs_flushfiles(mp, flags, l);
			if (error == 0)
				error = UFS_WAPBL_BEGIN(mp);
			if (error == 0 &&
			    ffs_cgupdate(ump, MNT_WAIT) == 0 &&
			    fs->fs_clean & FS_WASCLEAN) {
				if (mp->mnt_flag & MNT_SOFTDEP)
					fs->fs_flags &= ~FS_DOSOFTDEP;
				fs->fs_clean = FS_ISCLEAN;
				(void) ffs_sbupdate(ump, MNT_WAIT);
			}
			if (error == 0)
				UFS_WAPBL_END(mp);
			if (error)
				return (error);
		}

#ifdef WAPBL
		if ((mp->mnt_flag & MNT_LOG) == 0) {
			error = ffs_wapbl_stop(mp, mp->mnt_flag & MNT_FORCE);
			if (error)
				return error;
		}
#endif /* WAPBL */

		if (fs->fs_ronly == 0 && (mp->mnt_flag & MNT_RDONLY)) {
			/*
			 * Finish change from r/w to r/o
			 */
			fs->fs_ronly = 1;
			fs->fs_fmod = 0;
		}

		if (mp->mnt_flag & MNT_RELOAD) {
			error = ffs_reload(mp, l->l_cred, l);
			if (error)
				return (error);
		}

		if (fs->fs_ronly && (mp->mnt_iflag & IMNT_WANTRDWR)) {
			/*
			 * Changing from read-only to read/write
			 */
			fs->fs_ronly = 0;
			fs->fs_clean <<= 1;
			fs->fs_fmod = 1;
#ifdef WAPBL
			if (fs->fs_flags & FS_DOWAPBL) {
				printf("%s: replaying log to disk\n",
				    fs->fs_fsmnt);
				KDASSERT(mp->mnt_wapbl_replay);
				error = wapbl_replay_write(mp->mnt_wapbl_replay,
							   devvp);
				if (error) {
					return error;
				}
				wapbl_replay_stop(mp->mnt_wapbl_replay);
				fs->fs_clean = FS_WASCLEAN;
			}
#endif /* WAPBL */
			if (fs->fs_snapinum[0] != 0)
				ffs_snapshot_mount(mp);
		}

#ifdef WAPBL
		error = ffs_wapbl_start(mp);
		if (error)
			return error;
#endif /* WAPBL */

		if (args->fspec == NULL)
			return 0;
	}

	error = set_statvfs_info(path, UIO_USERSPACE, args->fspec,
	    UIO_USERSPACE, mp->mnt_op->vfs_name, mp, l);
	if (error == 0)
		(void)strncpy(fs->fs_fsmnt, mp->mnt_stat.f_mntonname,
		    sizeof(fs->fs_fsmnt));
	fs->fs_flags &= ~FS_DOSOFTDEP;
	if (fs->fs_fmod != 0) {	/* XXX */
		int err;

		fs->fs_fmod = 0;
		if (fs->fs_clean & FS_WASCLEAN)
			fs->fs_time = time_second;
		else {
			printf("%s: file system not clean (fs_clean=%#x); "
			    "please fsck(8)\n", mp->mnt_stat.f_mntfromname,
			    fs->fs_clean);
			printf("%s: lost blocks %" PRId64 " files %d\n",
			    mp->mnt_stat.f_mntfromname, fs->fs_pendingblocks,
			    fs->fs_pendinginodes);
		}
		err = UFS_WAPBL_BEGIN(mp);
		if (err == 0) {
			(void) ffs_cgupdate(ump, MNT_WAIT);
			UFS_WAPBL_END(mp);
		}
	}
	if ((mp->mnt_flag & MNT_SOFTDEP) != 0) {
		printf("%s: `-o softdep' is no longer supported, "
		    "consider `-o log'\n", mp->mnt_stat.f_mntfromname);
		mp->mnt_flag &= ~MNT_SOFTDEP;
	}

	return (error);

fail:
	vrele(devvp);
	return (error);
}

/*
 * Reload all incore data for a filesystem (used after running fsck on
 * the root filesystem and finding things to fix). The filesystem must
 * be mounted read-only.
 *
 * Things to do to update the mount:
 *	1) invalidate all cached meta-data.
 *	2) re-read superblock from disk.
 *	3) re-read summary information from disk.
 *	4) invalidate all inactive vnodes.
 *	5) invalidate all cached file data.
 *	6) re-read inode data for all active vnodes.
 */
int
ffs_reload(struct mount *mp, kauth_cred_t cred, struct lwp *l)
{
	struct vnode *vp, *mvp, *devvp;
	struct inode *ip;
	void *space;
	struct buf *bp;
	struct fs *fs, *newfs;
	struct partinfo dpart;
	int i, bsize, blks, error;
	int32_t *lp;
	struct ufsmount *ump;
	daddr_t sblockloc;

	if ((mp->mnt_flag & MNT_RDONLY) == 0)
		return (EINVAL);

	ump = VFSTOUFS(mp);
	/*
	 * Step 1: invalidate all cached meta-data.
	 */
	devvp = ump->um_devvp;
	vn_lock(devvp, LK_EXCLUSIVE | LK_RETRY);
	error = vinvalbuf(devvp, 0, cred, l, 0, 0);
	VOP_UNLOCK(devvp);
	if (error)
		panic("ffs_reload: dirty1");
	/*
	 * Step 2: re-read superblock from disk.
	 */
	fs = ump->um_fs;

	/* XXX we don't handle possibility that superblock moved. */
	error = bread(devvp, fs->fs_sblockloc / DEV_BSIZE, fs->fs_sbsize,
		      NOCRED, 0, &bp);
	if (error) {
		brelse(bp, 0);
		return (error);
	}
	newfs = malloc(fs->fs_sbsize, M_UFSMNT, M_WAITOK);
	memcpy(newfs, bp->b_data, fs->fs_sbsize);
#ifdef FFS_EI
	if (ump->um_flags & UFS_NEEDSWAP) {
		ffs_sb_swap((struct fs*)bp->b_data, newfs);
		fs->fs_flags |= FS_SWAPPED;
	} else
#endif
		fs->fs_flags &= ~FS_SWAPPED;
	if ((newfs->fs_magic != FS_UFS1_MAGIC &&
	     newfs->fs_magic != FS_UFS2_MAGIC)||
	     newfs->fs_bsize > MAXBSIZE ||
	     newfs->fs_bsize < sizeof(struct fs)) {
		brelse(bp, 0);
		free(newfs, M_UFSMNT);
		return (EIO);		/* XXX needs translation */
	}
	/* Store off old fs_sblockloc for fs_oldfscompat_read. */
	sblockloc = fs->fs_sblockloc;
	/*
	 * Copy pointer fields back into superblock before copying in	XXX
	 * new superblock. These should really be in the ufsmount.	XXX
	 * Note that important parameters (eg fs_ncg) are unchanged.
	 */
	newfs->fs_csp = fs->fs_csp;
	newfs->fs_maxcluster = fs->fs_maxcluster;
	newfs->fs_contigdirs = fs->fs_contigdirs;
	newfs->fs_ronly = fs->fs_ronly;
	newfs->fs_active = fs->fs_active;
	memcpy(fs, newfs, (u_int)fs->fs_sbsize);
	brelse(bp, 0);
	free(newfs, M_UFSMNT);

	/* Recheck for apple UFS filesystem */
	ump->um_flags &= ~UFS_ISAPPLEUFS;
	/* First check to see if this is tagged as an Apple UFS filesystem
	 * in the disklabel
	 */
	if ((VOP_IOCTL(devvp, DIOCGPART, &dpart, FREAD, cred) == 0) &&
		(dpart.part->p_fstype == FS_APPLEUFS)) {
		ump->um_flags |= UFS_ISAPPLEUFS;
	}
#ifdef APPLE_UFS
	else {
		/* Manually look for an apple ufs label, and if a valid one
		 * is found, then treat it like an Apple UFS filesystem anyway
		 */
		error = bread(devvp, (daddr_t)(APPLEUFS_LABEL_OFFSET / DEV_BSIZE),
			APPLEUFS_LABEL_SIZE, cred, 0, &bp);
		if (error) {
			brelse(bp, 0);
			return (error);
		}
		error = ffs_appleufs_validate(fs->fs_fsmnt,
			(struct appleufslabel *)bp->b_data, NULL);
		if (error == 0)
			ump->um_flags |= UFS_ISAPPLEUFS;
		brelse(bp, 0);
		bp = NULL;
	}
#else
	if (ump->um_flags & UFS_ISAPPLEUFS)
		return (EIO);
#endif

	if (UFS_MPISAPPLEUFS(ump)) {
		/* see comment about NeXT below */
		ump->um_maxsymlinklen = APPLEUFS_MAXSYMLINKLEN;
		ump->um_dirblksiz = APPLEUFS_DIRBLKSIZ;
		mp->mnt_iflag |= IMNT_DTYPE;
	} else {
		ump->um_maxsymlinklen = fs->fs_maxsymlinklen;
		ump->um_dirblksiz = DIRBLKSIZ;
		if (ump->um_maxsymlinklen > 0)
			mp->mnt_iflag |= IMNT_DTYPE;
		else
			mp->mnt_iflag &= ~IMNT_DTYPE;
	}
	ffs_oldfscompat_read(fs, ump, sblockloc);

	mutex_enter(&ump->um_lock);
	ump->um_maxfilesize = fs->fs_maxfilesize;
	if (fs->fs_flags & ~(FS_KNOWN_FLAGS | FS_INTERNAL)) {
		uprintf("%s: unknown ufs flags: 0x%08"PRIx32"%s\n",
		    mp->mnt_stat.f_mntonname, fs->fs_flags,
		    (mp->mnt_flag & MNT_FORCE) ? "" : ", not mounting");
		if ((mp->mnt_flag & MNT_FORCE) == 0) {
			mutex_exit(&ump->um_lock);
			return (EINVAL);
		}
	}
	if (fs->fs_pendingblocks != 0 || fs->fs_pendinginodes != 0) {
		fs->fs_pendingblocks = 0;
		fs->fs_pendinginodes = 0;
	}
	mutex_exit(&ump->um_lock);

	ffs_statvfs(mp, &mp->mnt_stat);
	/*
	 * Step 3: re-read summary information from disk.
	 */
	blks = howmany(fs->fs_cssize, fs->fs_fsize);
	space = fs->fs_csp;
	for (i = 0; i < blks; i += fs->fs_frag) {
		bsize = fs->fs_bsize;
		if (i + fs->fs_frag > blks)
			bsize = (blks - i) * fs->fs_fsize;
		error = bread(devvp, fsbtodb(fs, fs->fs_csaddr + i), bsize,
			      NOCRED, 0, &bp);
		if (error) {
			brelse(bp, 0);
			return (error);
		}
#ifdef FFS_EI
		if (UFS_FSNEEDSWAP(fs))
			ffs_csum_swap((struct csum *)bp->b_data,
			    (struct csum *)space, bsize);
		else
#endif
			memcpy(space, bp->b_data, (size_t)bsize);
		space = (char *)space + bsize;
		brelse(bp, 0);
	}
	if (fs->fs_snapinum[0] != 0)
		ffs_snapshot_mount(mp);
	/*
	 * We no longer know anything about clusters per cylinder group.
	 */
	if (fs->fs_contigsumsize > 0) {
		lp = fs->fs_maxcluster;
		for (i = 0; i < fs->fs_ncg; i++)
			*lp++ = fs->fs_contigsumsize;
	}

	/* Allocate a marker vnode. */
	if ((mvp = vnalloc(mp)) == NULL)
		return ENOMEM;
	/*
	 * NOTE: not using the TAILQ_FOREACH here since in this loop vgone()
	 * and vclean() can be called indirectly
	 */
	mutex_enter(&mntvnode_lock);
 loop:
	for (vp = TAILQ_FIRST(&mp->mnt_vnodelist); vp; vp = vunmark(mvp)) {
		vmark(mvp, vp);
		if (vp->v_mount != mp || vismarker(vp))
			continue;
		/*
		 * Step 4: invalidate all inactive vnodes.
		 */
		if (vrecycle(vp, &mntvnode_lock, l)) {
			mutex_enter(&mntvnode_lock);
			(void)vunmark(mvp);
			goto loop;
		}
		/*
		 * Step 5: invalidate all cached file data.
		 */
		mutex_enter(&vp->v_interlock);
		mutex_exit(&mntvnode_lock);
		if (vget(vp, LK_EXCLUSIVE)) {
			(void)vunmark(mvp);
			goto loop;
		}
		if (vinvalbuf(vp, 0, cred, l, 0, 0))
			panic("ffs_reload: dirty2");
		/*
		 * Step 6: re-read inode data for all active vnodes.
		 */
		ip = VTOI(vp);
		error = bread(devvp, fsbtodb(fs, ino_to_fsba(fs, ip->i_number)),
			      (int)fs->fs_bsize, NOCRED, 0, &bp);
		if (error) {
			brelse(bp, 0);
			vput(vp);
			(void)vunmark(mvp);
			break;
		}
		ffs_load_inode(bp, ip, fs, ip->i_number);
		brelse(bp, 0);
		vput(vp);
		mutex_enter(&mntvnode_lock);
	}
	mutex_exit(&mntvnode_lock);
	vnfree(mvp);
	return (error);
}

/*
 * Possible superblock locations ordered from most to least likely.
 */
static const int sblock_try[] = SBLOCKSEARCH;

/*
 * Common code for mount and mountroot
 */
int
ffs_mountfs(struct vnode *devvp, struct mount *mp, struct lwp *l)
{
	struct ufsmount *ump;
	struct buf *bp;
	struct fs *fs;
	dev_t dev;
	struct partinfo dpart;
	void *space;
	daddr_t sblockloc, fsblockloc;
	int blks, fstype;
	int error, i, bsize, ronly, bset = 0;
#ifdef FFS_EI
	int needswap = 0;		/* keep gcc happy */
#endif
	int32_t *lp;
	kauth_cred_t cred;
	u_int32_t sbsize = 8192;	/* keep gcc happy*/
	int32_t fsbsize;

	dev = devvp->v_rdev;
	cred = l ? l->l_cred : NOCRED;

	/* Flush out any old buffers remaining from a previous use. */
	vn_lock(devvp, LK_EXCLUSIVE | LK_RETRY);
	error = vinvalbuf(devvp, V_SAVE, cred, l, 0, 0);
	VOP_UNLOCK(devvp);
	if (error)
		return (error);

	ronly = (mp->mnt_flag & MNT_RDONLY) != 0;

	bp = NULL;
	ump = NULL;
	fs = NULL;
	sblockloc = 0;
	fstype = 0;

	error = fstrans_mount(mp);
	if (error)
		return error;

	ump = malloc(sizeof *ump, M_UFSMNT, M_WAITOK);
	memset(ump, 0, sizeof *ump);
	mutex_init(&ump->um_lock, MUTEX_DEFAULT, IPL_NONE);
	error = ffs_snapshot_init(ump);
	if (error)
		goto out;
	ump->um_ops = &ffs_ufsops;

#ifdef WAPBL
 sbagain:
#endif
	/*
	 * Try reading the superblock in each of its possible locations.
	 */
	for (i = 0; ; i++) {
		if (bp != NULL) {
			brelse(bp, BC_NOCACHE);
			bp = NULL;
		}
		if (sblock_try[i] == -1) {
			error = EINVAL;
			fs = NULL;
			goto out;
		}
		error = bread(devvp, sblock_try[i] / DEV_BSIZE, SBLOCKSIZE, cred,
			      0, &bp);
		if (error) {
			fs = NULL;
			goto out;
		}
		fs = (struct fs*)bp->b_data;
		fsblockloc = sblockloc = sblock_try[i];
		if (fs->fs_magic == FS_UFS1_MAGIC) {
			sbsize = fs->fs_sbsize;
			fstype = UFS1;
			fsbsize = fs->fs_bsize;
#ifdef FFS_EI
			needswap = 0;
		} else if (fs->fs_magic == bswap32(FS_UFS1_MAGIC)) {
			sbsize = bswap32(fs->fs_sbsize);
			fstype = UFS1;
			fsbsize = bswap32(fs->fs_bsize);
			needswap = 1;
#endif
		} else if (fs->fs_magic == FS_UFS2_MAGIC) {
			sbsize = fs->fs_sbsize;
			fstype = UFS2;
			fsbsize = fs->fs_bsize;
#ifdef FFS_EI
			needswap = 0;
		} else if (fs->fs_magic == bswap32(FS_UFS2_MAGIC)) {
			sbsize = bswap32(fs->fs_sbsize);
			fstype = UFS2;
			fsbsize = bswap32(fs->fs_bsize);
			needswap = 1;
#endif
		} else
			continue;


		/* fs->fs_sblockloc isn't defined for old filesystems */
		if (fstype == UFS1 && !(fs->fs_old_flags & FS_FLAGS_UPDATED)) {
			if (sblockloc == SBLOCK_UFS2)
				/*
				 * This is likely to be the first alternate
				 * in a filesystem with 64k blocks.
				 * Don't use it.
				 */
				continue;
			fsblockloc = sblockloc;
		} else {
			fsblockloc = fs->fs_sblockloc;
#ifdef FFS_EI
			if (needswap)
				fsblockloc = bswap64(fsblockloc);
#endif
		}

		/* Check we haven't found an alternate superblock */
		if (fsblockloc != sblockloc)
			continue;

		/* Validate size of superblock */
		if (sbsize > MAXBSIZE || sbsize < sizeof(struct fs))
			continue;

		/* Check that we can handle the file system blocksize */
		if (fsbsize > MAXBSIZE) {
			printf("ffs_mountfs: block size (%d) > MAXBSIZE (%d)\n",
			    fsbsize, MAXBSIZE);
			continue;
		}

		/* Ok seems to be a good superblock */
		break;
	}

	fs = malloc((u_long)sbsize, M_UFSMNT, M_WAITOK);
	memcpy(fs, bp->b_data, sbsize);
	ump->um_fs = fs;

#ifdef FFS_EI
	if (needswap) {
		ffs_sb_swap((struct fs*)bp->b_data, fs);
		fs->fs_flags |= FS_SWAPPED;
	} else
#endif
		fs->fs_flags &= ~FS_SWAPPED;

#ifdef WAPBL
	if ((mp->mnt_wapbl_replay == 0) && (fs->fs_flags & FS_DOWAPBL)) {
		error = ffs_wapbl_replay_start(mp, fs, devvp);
		if (error && (mp->mnt_flag & MNT_FORCE) == 0)
			goto out;
		if (!error) {
			if (!ronly) {
				/* XXX fsmnt may be stale. */
				printf("%s: replaying log to disk\n",
				    fs->fs_fsmnt);
				error = wapbl_replay_write(mp->mnt_wapbl_replay,
				    devvp);
				if (error)
					goto out;
				wapbl_replay_stop(mp->mnt_wapbl_replay);
				fs->fs_clean = FS_WASCLEAN;
			} else {
				/* XXX fsmnt may be stale */
				printf("%s: replaying log to memory\n",
				    fs->fs_fsmnt);
			}

			/* Force a re-read of the superblock */
			brelse(bp, BC_INVAL);
			bp = NULL;
			free(fs, M_UFSMNT);
			fs = NULL;
			goto sbagain;
		}
	}
#else /* !WAPBL */
	if ((fs->fs_flags & FS_DOWAPBL) && (mp->mnt_flag & MNT_FORCE) == 0) {
		error = EPERM;
		goto out;
	}
#endif /* !WAPBL */

	ffs_oldfscompat_read(fs, ump, sblockloc);
	ump->um_maxfilesize = fs->fs_maxfilesize;

	if (fs->fs_flags & ~(FS_KNOWN_FLAGS | FS_INTERNAL)) {
		uprintf("%s: unknown ufs flags: 0x%08"PRIx32"%s\n",
		    mp->mnt_stat.f_mntonname, fs->fs_flags,
		    (mp->mnt_flag & MNT_FORCE) ? "" : ", not mounting");
		if ((mp->mnt_flag & MNT_FORCE) == 0) {
			error = EINVAL;
			goto out;
		}
	}

	if (fs->fs_pendingblocks != 0 || fs->fs_pendinginodes != 0) {
		fs->fs_pendingblocks = 0;
		fs->fs_pendinginodes = 0;
	}

	ump->um_fstype = fstype;
	if (fs->fs_sbsize < SBLOCKSIZE)
		brelse(bp, BC_INVAL);
	else
		brelse(bp, 0);
	bp = NULL;

	/* First check to see if this is tagged as an Apple UFS filesystem
	 * in the disklabel
	 */
	if ((VOP_IOCTL(devvp, DIOCGPART, &dpart, FREAD, cred) == 0) &&
		(dpart.part->p_fstype == FS_APPLEUFS)) {
		ump->um_flags |= UFS_ISAPPLEUFS;
	}
#ifdef APPLE_UFS
	else {
		/* Manually look for an apple ufs label, and if a valid one
		 * is found, then treat it like an Apple UFS filesystem anyway
		 */
		error = bread(devvp, (daddr_t)(APPLEUFS_LABEL_OFFSET / DEV_BSIZE),
			APPLEUFS_LABEL_SIZE, cred, 0, &bp);
		if (error)
			goto out;
		error = ffs_appleufs_validate(fs->fs_fsmnt,
			(struct appleufslabel *)bp->b_data, NULL);
		if (error == 0) {
			ump->um_flags |= UFS_ISAPPLEUFS;
		}
		brelse(bp, 0);
		bp = NULL;
	}
#else
	if (ump->um_flags & UFS_ISAPPLEUFS) {
		error = EINVAL;
		goto out;
	}
#endif

#if 0
/*
 * XXX This code changes the behaviour of mounting dirty filesystems, to
 * XXX require "mount -f ..." to mount them.  This doesn't match what
 * XXX mount(8) describes and is disabled for now.
 */
	/*
	 * If the file system is not clean, don't allow it to be mounted
	 * unless MNT_FORCE is specified.  (Note: MNT_FORCE is always set
	 * for the root file system.)
	 */
	if (fs->fs_flags & FS_DOWAPBL) {
		/*
		 * wapbl normally expects to be FS_WASCLEAN when the FS_DOWAPBL
		 * bit is set, although there's a window in unmount where it
		 * could be FS_ISCLEAN
		 */
		if ((mp->mnt_flag & MNT_FORCE) == 0 &&
		    (fs->fs_clean & (FS_WASCLEAN | FS_ISCLEAN)) == 0) {
			error = EPERM;
			goto out;
		}
	} else
		if ((fs->fs_clean & FS_ISCLEAN) == 0 &&
		    (mp->mnt_flag & MNT_FORCE) == 0) {
			error = EPERM;
			goto out;
		}
#endif

	/*
	 * verify that we can access the last block in the fs
	 * if we're mounting read/write.
	 */

	if (!ronly) {
		error = bread(devvp, fsbtodb(fs, fs->fs_size - 1), fs->fs_fsize,
		    cred, 0, &bp);
		if (bp->b_bcount != fs->fs_fsize)
			error = EINVAL;
		if (error) {
			bset = BC_INVAL;
			goto out;
		}
		brelse(bp, BC_INVAL);
		bp = NULL;
	}

	fs->fs_ronly = ronly;
	/* Don't bump fs_clean if we're replaying journal */
	if (!((fs->fs_flags & FS_DOWAPBL) && (fs->fs_clean & FS_WASCLEAN)))
		if (ronly == 0) {
			fs->fs_clean <<= 1;
			fs->fs_fmod = 1;
		}
	bsize = fs->fs_cssize;
	blks = howmany(bsize, fs->fs_fsize);
	if (fs->fs_contigsumsize > 0)
		bsize += fs->fs_ncg * sizeof(int32_t);
	bsize += fs->fs_ncg * sizeof(*fs->fs_contigdirs);
	space = malloc((u_long)bsize, M_UFSMNT, M_WAITOK);
	fs->fs_csp = space;
	for (i = 0; i < blks; i += fs->fs_frag) {
		bsize = fs->fs_bsize;
		if (i + fs->fs_frag > blks)
			bsize = (blks - i) * fs->fs_fsize;
		error = bread(devvp, fsbtodb(fs, fs->fs_csaddr + i), bsize,
			      cred, 0, &bp);
		if (error) {
			free(fs->fs_csp, M_UFSMNT);
			goto out;
		}
#ifdef FFS_EI
		if (needswap)
			ffs_csum_swap((struct csum *)bp->b_data,
				(struct csum *)space, bsize);
		else
#endif
			memcpy(space, bp->b_data, (u_int)bsize);

		space = (char *)space + bsize;
		brelse(bp, 0);
		bp = NULL;
	}
	if (fs->fs_contigsumsize > 0) {
		fs->fs_maxcluster = lp = space;
		for (i = 0; i < fs->fs_ncg; i++)
			*lp++ = fs->fs_contigsumsize;
		space = lp;
	}
	bsize = fs->fs_ncg * sizeof(*fs->fs_contigdirs);
	fs->fs_contigdirs = space;
	space = (char *)space + bsize;
	memset(fs->fs_contigdirs, 0, bsize);
		/* Compatibility for old filesystems - XXX */
	if (fs->fs_avgfilesize <= 0)
		fs->fs_avgfilesize = AVFILESIZ;
	if (fs->fs_avgfpdir <= 0)
		fs->fs_avgfpdir = AFPDIR;
	fs->fs_active = NULL;
	mp->mnt_data = ump;
	mp->mnt_stat.f_fsidx.__fsid_val[0] = (long)dev;
	mp->mnt_stat.f_fsidx.__fsid_val[1] = makefstype(MOUNT_FFS);
	mp->mnt_stat.f_fsid = mp->mnt_stat.f_fsidx.__fsid_val[0];
	mp->mnt_stat.f_namemax = FFS_MAXNAMLEN;
	if (UFS_MPISAPPLEUFS(ump)) {
		/* NeXT used to keep short symlinks in the inode even
		 * when using FS_42INODEFMT.  In that case fs->fs_maxsymlinklen
		 * is probably -1, but we still need to be able to identify
		 * short symlinks.
		 */
		ump->um_maxsymlinklen = APPLEUFS_MAXSYMLINKLEN;
		ump->um_dirblksiz = APPLEUFS_DIRBLKSIZ;
		mp->mnt_iflag |= IMNT_DTYPE;
	} else {
		ump->um_maxsymlinklen = fs->fs_maxsymlinklen;
		ump->um_dirblksiz = DIRBLKSIZ;
		if (ump->um_maxsymlinklen > 0)
			mp->mnt_iflag |= IMNT_DTYPE;
		else
			mp->mnt_iflag &= ~IMNT_DTYPE;
	}
	mp->mnt_fs_bshift = fs->fs_bshift;
	mp->mnt_dev_bshift = DEV_BSHIFT;	/* XXX */
	mp->mnt_flag |= MNT_LOCAL;
	mp->mnt_iflag |= IMNT_MPSAFE;
#ifdef FFS_EI
	if (needswap)
		ump->um_flags |= UFS_NEEDSWAP;
#endif
	ump->um_mountp = mp;
	ump->um_dev = dev;
	ump->um_devvp = devvp;
	ump->um_nindir = fs->fs_nindir;
	ump->um_lognindir = ffs(fs->fs_nindir) - 1;
	ump->um_bptrtodb = fs->fs_fshift - DEV_BSHIFT;
	ump->um_seqinc = fs->fs_frag;
	for (i = 0; i < MAXQUOTAS; i++)
		ump->um_quotas[i] = NULLVP;
	devvp->v_specmountpoint = mp;
	if (ronly == 0 && fs->fs_snapinum[0] != 0)
		ffs_snapshot_mount(mp);

#ifdef WAPBL
	if (!ronly) {
		KDASSERT(fs->fs_ronly == 0);
		/*
		 * ffs_wapbl_start() needs mp->mnt_stat initialised if it
		 * needs to create a new log file in-filesystem.
		 */
		ffs_statvfs(mp, &mp->mnt_stat);

		error = ffs_wapbl_start(mp);
		if (error) {
			free(fs->fs_csp, M_UFSMNT);
			goto out;
		}
	}
#endif /* WAPBL */
#ifdef UFS_EXTATTR
	/*
	 * Initialize file-backed extended attributes on UFS1 file
	 * systems.
	 */
	if (ump->um_fstype == UFS1) {
		ufs_extattr_uepm_init(&ump->um_extattr);
#ifdef UFS_EXTATTR_AUTOSTART
		/*
		 * XXX Just ignore errors.  Not clear that we should
		 * XXX fail the mount in this case.
		 */
		(void) ufs_extattr_autostart(mp, l);
#endif
	}
#endif /* UFS_EXTATTR */
	return (0);
out:
#ifdef WAPBL
	if (mp->mnt_wapbl_replay) {
		wapbl_replay_stop(mp->mnt_wapbl_replay);
		wapbl_replay_free(mp->mnt_wapbl_replay);
		mp->mnt_wapbl_replay = 0;
	}
#endif

	fstrans_unmount(mp);
	if (fs)
		free(fs, M_UFSMNT);
	devvp->v_specmountpoint = NULL;
	if (bp)
		brelse(bp, bset);
	if (ump) {
		if (ump->um_oldfscompat)
			free(ump->um_oldfscompat, M_UFSMNT);
		mutex_destroy(&ump->um_lock);
		free(ump, M_UFSMNT);
		mp->mnt_data = NULL;
	}
	return (error);
}

/*
 * Sanity checks for loading old filesystem superblocks.
 * See ffs_oldfscompat_write below for unwound actions.
 *
 * XXX - Parts get retired eventually.
 * Unfortunately new bits get added.
 */
static void
ffs_oldfscompat_read(struct fs *fs, struct ufsmount *ump, daddr_t sblockloc)
{
	off_t maxfilesize;
	int32_t *extrasave;

	if ((fs->fs_magic != FS_UFS1_MAGIC) ||
	    (fs->fs_old_flags & FS_FLAGS_UPDATED))
		return;

	if (!ump->um_oldfscompat)
		ump->um_oldfscompat = malloc(512 + 3*sizeof(int32_t),
		    M_UFSMNT, M_WAITOK);

	memcpy(ump->um_oldfscompat, &fs->fs_old_postbl_start, 512);
	extrasave = ump->um_oldfscompat;
	extrasave += 512/sizeof(int32_t);
	extrasave[0] = fs->fs_old_npsect;
	extrasave[1] = fs->fs_old_interleave;
	extrasave[2] = fs->fs_old_trackskew;

	/* These fields will be overwritten by their
	 * original values in fs_oldfscompat_write, so it is harmless
	 * to modify them here.
	 */
	fs->fs_cstotal.cs_ndir = fs->fs_old_cstotal.cs_ndir;
	fs->fs_cstotal.cs_nbfree = fs->fs_old_cstotal.cs_nbfree;
	fs->fs_cstotal.cs_nifree = fs->fs_old_cstotal.cs_nifree;
	fs->fs_cstotal.cs_nffree = fs->fs_old_cstotal.cs_nffree;

	fs->fs_maxbsize = fs->fs_bsize;
	fs->fs_time = fs->fs_old_time;
	fs->fs_size = fs->fs_old_size;
	fs->fs_dsize = fs->fs_old_dsize;
	fs->fs_csaddr = fs->fs_old_csaddr;
	fs->fs_sblockloc = sblockloc;

	fs->fs_flags = fs->fs_old_flags | (fs->fs_flags & FS_INTERNAL);

	if (fs->fs_old_postblformat == FS_42POSTBLFMT) {
		fs->fs_old_nrpos = 8;
		fs->fs_old_npsect = fs->fs_old_nsect;
		fs->fs_old_interleave = 1;
		fs->fs_old_trackskew = 0;
	}

	if (fs->fs_old_inodefmt < FS_44INODEFMT) {
		fs->fs_maxfilesize = (u_quad_t) 1LL << 39;
		fs->fs_qbmask = ~fs->fs_bmask;
		fs->fs_qfmask = ~fs->fs_fmask;
	}

	maxfilesize = (u_int64_t)0x80000000 * fs->fs_bsize - 1;
	if (fs->fs_maxfilesize > maxfilesize)
		fs->fs_maxfilesize = maxfilesize;

	/* Compatibility for old filesystems */
	if (fs->fs_avgfilesize <= 0)
		fs->fs_avgfilesize = AVFILESIZ;
	if (fs->fs_avgfpdir <= 0)
		fs->fs_avgfpdir = AFPDIR;

#if 0
	if (bigcgs) {
		fs->fs_save_cgsize = fs->fs_cgsize;
		fs->fs_cgsize = fs->fs_bsize;
	}
#endif
}

/*
 * Unwinding superblock updates for old filesystems.
 * See ffs_oldfscompat_read above for details.
 *
 * XXX - Parts get retired eventually.
 * Unfortunately new bits get added.
 */
static void
ffs_oldfscompat_write(struct fs *fs, struct ufsmount *ump)
{
	int32_t *extrasave;

	if ((fs->fs_magic != FS_UFS1_MAGIC) ||
	    (fs->fs_old_flags & FS_FLAGS_UPDATED))
		return;

	fs->fs_old_time = fs->fs_time;
	fs->fs_old_cstotal.cs_ndir = fs->fs_cstotal.cs_ndir;
	fs->fs_old_cstotal.cs_nbfree = fs->fs_cstotal.cs_nbfree;
	fs->fs_old_cstotal.cs_nifree = fs->fs_cstotal.cs_nifree;
	fs->fs_old_cstotal.cs_nffree = fs->fs_cstotal.cs_nffree;
	fs->fs_old_flags = fs->fs_flags;

#if 0
	if (bigcgs) {
		fs->fs_cgsize = fs->fs_save_cgsize;
	}
#endif

	memcpy(&fs->fs_old_postbl_start, ump->um_oldfscompat, 512);
	extrasave = ump->um_oldfscompat;
	extrasave += 512/sizeof(int32_t);
	fs->fs_old_npsect = extrasave[0];
	fs->fs_old_interleave = extrasave[1];
	fs->fs_old_trackskew = extrasave[2];

}

/*
 * unmount vfs operation
 */
int
ffs_unmount(struct mount *mp, int mntflags)
{
	struct lwp *l = curlwp;
	struct ufsmount *ump = VFSTOUFS(mp);
	struct fs *fs = ump->um_fs;
	int error, flags;
#ifdef WAPBL
	extern int doforce;
#endif

	flags = 0;
	if (mntflags & MNT_FORCE)
		flags |= FORCECLOSE;
	if ((error = ffs_flushfiles(mp, flags, l)) != 0)
		return (error);
	error = UFS_WAPBL_BEGIN(mp);
	if (error == 0)
		if (fs->fs_ronly == 0 &&
		    ffs_cgupdate(ump, MNT_WAIT) == 0 &&
		    fs->fs_clean & FS_WASCLEAN) {
			fs->fs_clean = FS_ISCLEAN;
			fs->fs_fmod = 0;
			(void) ffs_sbupdate(ump, MNT_WAIT);
		}
	if (error == 0)
		UFS_WAPBL_END(mp);
#ifdef WAPBL
	KASSERT(!(mp->mnt_wapbl_replay && mp->mnt_wapbl));
	if (mp->mnt_wapbl_replay) {
		KDASSERT(fs->fs_ronly);
		wapbl_replay_stop(mp->mnt_wapbl_replay);
		wapbl_replay_free(mp->mnt_wapbl_replay);
		mp->mnt_wapbl_replay = 0;
	}
	error = ffs_wapbl_stop(mp, doforce && (mntflags & MNT_FORCE));
	if (error) {
		return error;
	}
#endif /* WAPBL */
#ifdef UFS_EXTATTR
	if (ump->um_fstype == UFS1) {
		ufs_extattr_stop(mp, l);
		ufs_extattr_uepm_destroy(&ump->um_extattr);
	}
#endif /* UFS_EXTATTR */

	if (ump->um_devvp->v_type != VBAD)
		ump->um_devvp->v_specmountpoint = NULL;
	vn_lock(ump->um_devvp, LK_EXCLUSIVE | LK_RETRY);
	(void)VOP_CLOSE(ump->um_devvp, fs->fs_ronly ? FREAD : FREAD | FWRITE,
		NOCRED);
	vput(ump->um_devvp);
	free(fs->fs_csp, M_UFSMNT);
	free(fs, M_UFSMNT);
	if (ump->um_oldfscompat != NULL)
		free(ump->um_oldfscompat, M_UFSMNT);
	mutex_destroy(&ump->um_lock);
	ffs_snapshot_fini(ump);
	free(ump, M_UFSMNT);
	mp->mnt_data = NULL;
	mp->mnt_flag &= ~MNT_LOCAL;
	fstrans_unmount(mp);
	return (0);
}

/*
 * Flush out all the files in a filesystem.
 */
int
ffs_flushfiles(struct mount *mp, int flags, struct lwp *l)
{
	extern int doforce;
	struct ufsmount *ump;
	int error;

	if (!doforce)
		flags &= ~FORCECLOSE;
	ump = VFSTOUFS(mp);
#ifdef QUOTA
	if (mp->mnt_flag & MNT_QUOTA) {
		int i;
		if ((error = vflush(mp, NULLVP, SKIPSYSTEM | flags)) != 0)
			return (error);
		for (i = 0; i < MAXQUOTAS; i++) {
			if (ump->um_quotas[i] == NULLVP)
				continue;
			quotaoff(l, mp, i);
		}
		/*
		 * Here we fall through to vflush again to ensure
		 * that we have gotten rid of all the system vnodes.
		 */
	}
#endif
	if ((error = vflush(mp, 0, SKIPSYSTEM | flags)) != 0)
		return (error);
	ffs_snapshot_unmount(mp);
	/*
	 * Flush all the files.
	 */
	error = vflush(mp, NULLVP, flags);
	if (error)
		return (error);
	/*
	 * Flush filesystem metadata.
	 */
	vn_lock(ump->um_devvp, LK_EXCLUSIVE | LK_RETRY);
	error = VOP_FSYNC(ump->um_devvp, l->l_cred, FSYNC_WAIT, 0, 0);
	VOP_UNLOCK(ump->um_devvp);
	if (flags & FORCECLOSE) /* XXXDBJ */
		error = 0;

#ifdef WAPBL
	if (error)
		return error;
	if (mp->mnt_wapbl) {
		error = wapbl_flush(mp->mnt_wapbl, 1);
		if (flags & FORCECLOSE)
			error = 0;
	}
#endif

	return (error);
}

/*
 * Get file system statistics.
 */
int
ffs_statvfs(struct mount *mp, struct statvfs *sbp)
{
	struct ufsmount *ump;
	struct fs *fs;

	ump = VFSTOUFS(mp);
	fs = ump->um_fs;
	mutex_enter(&ump->um_lock);
	sbp->f_bsize = fs->fs_bsize;
	sbp->f_frsize = fs->fs_fsize;
	sbp->f_iosize = fs->fs_bsize;
	sbp->f_blocks = fs->fs_dsize;
	sbp->f_bfree = blkstofrags(fs, fs->fs_cstotal.cs_nbfree) +
	    fs->fs_cstotal.cs_nffree + dbtofsb(fs, fs->fs_pendingblocks);
	sbp->f_bresvd = ((u_int64_t) fs->fs_dsize * (u_int64_t)
	    fs->fs_minfree) / (u_int64_t) 100;
	if (sbp->f_bfree > sbp->f_bresvd)
		sbp->f_bavail = sbp->f_bfree - sbp->f_bresvd;
	else
		sbp->f_bavail = 0;
	sbp->f_files =  fs->fs_ncg * fs->fs_ipg - ROOTINO;
	sbp->f_ffree = fs->fs_cstotal.cs_nifree + fs->fs_pendinginodes;
	sbp->f_favail = sbp->f_ffree;
	sbp->f_fresvd = 0;
	mutex_exit(&ump->um_lock);
	copy_statvfs_info(sbp, mp);

	return (0);
}

/*
 * Go through the disk queues to initiate sandbagged IO;
 * go through the inodes to write those that have been modified;
 * initiate the writing of the super block if it has been modified.
 *
 * Note: we are always called with the filesystem marked `MPBUSY'.
 */
int
ffs_sync(struct mount *mp, int waitfor, kauth_cred_t cred)
{
	struct vnode *vp, *mvp, *nvp;
	struct inode *ip;
	struct ufsmount *ump = VFSTOUFS(mp);
	struct fs *fs;
	int lk_flags, error, allerror = 0;
	bool is_suspending;

	fs = ump->um_fs;
	if (fs->fs_fmod != 0 && fs->fs_ronly != 0) {		/* XXX */
		printf("fs = %s\n", fs->fs_fsmnt);
		panic("update: rofs mod");
	}

	/* Allocate a marker vnode. */
	if ((mvp = vnalloc(mp)) == NULL)
		return (ENOMEM);

	fstrans_start(mp, FSTRANS_SHARED);
	is_suspending = (fstrans_getstate(mp) == FSTRANS_SUSPENDING);
	/*
	 * We can't lock vnodes while the file system is suspending because
	 * threads waiting on fstrans may have locked vnodes.
	 */
	if (is_suspending)
		lk_flags = 0;
	else
		lk_flags = LK_EXCLUSIVE | LK_NOWAIT;
	/*
	 * Write back each (modified) inode.
	 */
	mutex_enter(&mntvnode_lock);
loop:
	/*
	 * NOTE: not using the TAILQ_FOREACH here since in this loop vgone()
	 * and vclean() can be called indirectly
	 */
	for (vp = TAILQ_FIRST(&mp->mnt_vnodelist); vp; vp = nvp) {
		nvp = TAILQ_NEXT(vp, v_mntvnodes);
		/*
		 * If the vnode that we are about to sync is no longer
		 * associated with this mount point, start over.
		 */
		if (vp->v_mount != mp)
			goto loop;
		/*
		 * Don't interfere with concurrent scans of this FS.
		 */
		if (vismarker(vp))
			continue;
		mutex_enter(&vp->v_interlock);
		ip = VTOI(vp);

		/*
		 * Skip the vnode/inode if inaccessible.
		 */
		if (ip == NULL || (vp->v_iflag & (VI_XLOCK | VI_CLEAN)) != 0 ||
		    vp->v_type == VNON) {
			mutex_exit(&vp->v_interlock);
			continue;
		}

		/*
		 * We deliberately update inode times here.  This will
		 * prevent a massive queue of updates accumulating, only
		 * to be handled by a call to unmount.
		 *
		 * XXX It would be better to have the syncer trickle these
		 * out.  Adjustment needed to allow registering vnodes for
		 * sync when the vnode is clean, but the inode dirty.  Or
		 * have ufs itself trickle out inode updates.
		 *
		 * If doing a lazy sync, we don't care about metadata or
		 * data updates, because they are handled by each vnode's
		 * synclist entry.  In this case we are only interested in
		 * writing back modified inodes.
		 */
		if ((ip->i_flag & (IN_ACCESS | IN_CHANGE | IN_UPDATE |
		    IN_MODIFY | IN_MODIFIED | IN_ACCESSED)) == 0 &&
		    (waitfor == MNT_LAZY || (LIST_EMPTY(&vp->v_dirtyblkhd) &&
		    UVM_OBJ_IS_CLEAN(&vp->v_uobj)))) {
			mutex_exit(&vp->v_interlock);
			continue;
		}
		if (vp->v_type == VBLK && is_suspending) {
			mutex_exit(&vp->v_interlock);
			continue;
		}
		vmark(mvp, vp);
		mutex_exit(&mntvnode_lock);
		error = vget(vp, lk_flags);
		if (error) {
			mutex_enter(&mntvnode_lock);
			nvp = vunmark(mvp);
			if (error == ENOENT) {
				goto loop;
			}
			continue;
		}
		if (waitfor == MNT_LAZY) {
			error = UFS_WAPBL_BEGIN(vp->v_mount);
			if (!error) {
				error = ffs_update(vp, NULL, NULL,
				    UPDATE_CLOSE);
				UFS_WAPBL_END(vp->v_mount);
			}
		} else {
			error = VOP_FSYNC(vp, cred, FSYNC_NOLOG |
			    (waitfor == MNT_WAIT ? FSYNC_WAIT : 0), 0, 0);
		}
		if (error)
			allerror = error;
		if (is_suspending)
			vrele(vp);
		else
			vput(vp);
		mutex_enter(&mntvnode_lock);
		nvp = vunmark(mvp);
	}
	mutex_exit(&mntvnode_lock);
	/*
	 * Force stale file system control information to be flushed.
	 */
	if (waitfor != MNT_LAZY && (ump->um_devvp->v_numoutput > 0 ||
	    !LIST_EMPTY(&ump->um_devvp->v_dirtyblkhd))) {
		vn_lock(ump->um_devvp, LK_EXCLUSIVE | LK_RETRY);
		if ((error = VOP_FSYNC(ump->um_devvp, cred,
		    (waitfor == MNT_WAIT ? FSYNC_WAIT : 0) | FSYNC_NOLOG,
		    0, 0)) != 0)
			allerror = error;
		VOP_UNLOCK(ump->um_devvp);
		if (allerror == 0 && waitfor == MNT_WAIT && !mp->mnt_wapbl) {
			mutex_enter(&mntvnode_lock);
			goto loop;
		}
	}
#ifdef QUOTA
	qsync(mp);
#endif
	/*
	 * Write back modified superblock.
	 */
	if (fs->fs_fmod != 0) {
		fs->fs_fmod = 0;
		fs->fs_time = time_second;
		error = UFS_WAPBL_BEGIN(mp);
		if (error)
			allerror = error;
		else {
			if ((error = ffs_cgupdate(ump, waitfor)))
				allerror = error;
			UFS_WAPBL_END(mp);
		}
	}

#ifdef WAPBL
	if (mp->mnt_wapbl) {
		error = wapbl_flush(mp->mnt_wapbl, 0);
		if (error)
			allerror = error;
	}
#endif

	fstrans_done(mp);
	vnfree(mvp);
	return (allerror);
}

/*
 * Look up a FFS dinode number to find its incore vnode, otherwise read it
 * in from disk.  If it is in core, wait for the lock bit to clear, then
 * return the inode locked.  Detection and handling of mount points must be
 * done by the calling routine.
 */
int
ffs_vget(struct mount *mp, ino_t ino, struct vnode **vpp)
{
	struct fs *fs;
	struct inode *ip;
	struct ufsmount *ump;
	struct buf *bp;
	struct vnode *vp;
	dev_t dev;
	int error;

	ump = VFSTOUFS(mp);
	dev = ump->um_dev;

 retry:
	if ((*vpp = ufs_ihashget(dev, ino, LK_EXCLUSIVE)) != NULL)
		return (0);

	/* Allocate a new vnode/inode. */
	if ((error = getnewvnode(VT_UFS, mp, ffs_vnodeop_p, &vp)) != 0) {
		*vpp = NULL;
		return (error);
	}
	ip = pool_cache_get(ffs_inode_cache, PR_WAITOK);

	/*
	 * If someone beat us to it, put back the freshly allocated
	 * vnode/inode pair and retry.
	 */
	mutex_enter(&ufs_hashlock);
	if (ufs_ihashget(dev, ino, 0) != NULL) {
		mutex_exit(&ufs_hashlock);
		ungetnewvnode(vp);
		pool_cache_put(ffs_inode_cache, ip);
		goto retry;
	}

	vp->v_vflag |= VV_LOCKSWORK;

	/*
	 * XXX MFS ends up here, too, to allocate an inode.  Should we
	 * XXX create another pool for MFS inodes?
	 */

	memset(ip, 0, sizeof(struct inode));
	vp->v_data = ip;
	ip->i_vnode = vp;
	ip->i_ump = ump;
	ip->i_fs = fs = ump->um_fs;
	ip->i_dev = dev;
	ip->i_number = ino;
#ifdef QUOTA
	ufsquota_init(ip);
#endif

	/*
	 * Initialize genfs node, we might proceed to destroy it in
	 * error branches.
	 */
	genfs_node_init(vp, &ffs_genfsops);

	/*
	 * Put it onto its hash chain and lock it so that other requests for
	 * this inode will block if they arrive while we are sleeping waiting
	 * for old data structures to be purged or for the contents of the
	 * disk portion of this inode to be read.
	 */

	ufs_ihashins(ip);
	mutex_exit(&ufs_hashlock);

	/* Read in the disk contents for the inode, copy into the inode. */
	error = bread(ump->um_devvp, fsbtodb(fs, ino_to_fsba(fs, ino)),
		      (int)fs->fs_bsize, NOCRED, 0, &bp);
	if (error) {

		/*
		 * The inode does not contain anything useful, so it would
		 * be misleading to leave it on its hash chain. With mode
		 * still zero, it will be unlinked and returned to the free
		 * list by vput().
		 */

		vput(vp);
		brelse(bp, 0);
		*vpp = NULL;
		return (error);
	}
	if (ip->i_ump->um_fstype == UFS1)
		ip->i_din.ffs1_din = pool_cache_get(ffs_dinode1_cache,
		    PR_WAITOK);
	else
		ip->i_din.ffs2_din = pool_cache_get(ffs_dinode2_cache,
		    PR_WAITOK);
	ffs_load_inode(bp, ip, fs, ino);
	brelse(bp, 0);

	/*
	 * Initialize the vnode from the inode, check for aliases.
	 * Note that the underlying vnode may have changed.
	 */

	ufs_vinit(mp, ffs_specop_p, ffs_fifoop_p, &vp);

	/*
	 * Finish inode initialization now that aliasing has been resolved.
	 */

	ip->i_devvp = ump->um_devvp;
	vref(ip->i_devvp);

	/*
	 * Ensure that uid and gid are correct. This is a temporary
	 * fix until fsck has been changed to do the update.
	 */

	if (fs->fs_old_inodefmt < FS_44INODEFMT) {		/* XXX */
		ip->i_uid = ip->i_ffs1_ouid;			/* XXX */
		ip->i_gid = ip->i_ffs1_ogid;			/* XXX */
	}							/* XXX */
	uvm_vnp_setsize(vp, ip->i_size);
	*vpp = vp;
	return (0);
}

/*
 * File handle to vnode
 *
 * Have to be really careful about stale file handles:
 * - check that the inode number is valid
 * - call ffs_vget() to get the locked inode
 * - check for an unallocated inode (i_mode == 0)
 * - check that the given client host has export rights and return
 *   those rights via. exflagsp and credanonp
 */
int
ffs_fhtovp(struct mount *mp, struct fid *fhp, struct vnode **vpp)
{
	struct ufid ufh;
	struct fs *fs;

	if (fhp->fid_len != sizeof(struct ufid))
		return EINVAL;

	memcpy(&ufh, fhp, sizeof(ufh));
	fs = VFSTOUFS(mp)->um_fs;
	if (ufh.ufid_ino < ROOTINO ||
	    ufh.ufid_ino >= fs->fs_ncg * fs->fs_ipg)
		return (ESTALE);
	return (ufs_fhtovp(mp, &ufh, vpp));
}

/*
 * Vnode pointer to File handle
 */
/* ARGSUSED */
int
ffs_vptofh(struct vnode *vp, struct fid *fhp, size_t *fh_size)
{
	struct inode *ip;
	struct ufid ufh;

	if (*fh_size < sizeof(struct ufid)) {
		*fh_size = sizeof(struct ufid);
		return E2BIG;
	}
	ip = VTOI(vp);
	*fh_size = sizeof(struct ufid);
	memset(&ufh, 0, sizeof(ufh));
	ufh.ufid_len = sizeof(struct ufid);
	ufh.ufid_ino = ip->i_number;
	ufh.ufid_gen = ip->i_gen;
	memcpy(fhp, &ufh, sizeof(ufh));
	return (0);
}

void
ffs_init(void)
{
	if (ffs_initcount++ > 0)
		return;

	ffs_inode_cache = pool_cache_init(sizeof(struct inode), 0, 0, 0,
	    "ffsino", NULL, IPL_NONE, NULL, NULL, NULL);
	ffs_dinode1_cache = pool_cache_init(sizeof(struct ufs1_dinode), 0, 0, 0,
	    "ffsdino1", NULL, IPL_NONE, NULL, NULL, NULL);
	ffs_dinode2_cache = pool_cache_init(sizeof(struct ufs2_dinode), 0, 0, 0,
	    "ffsdino2", NULL, IPL_NONE, NULL, NULL, NULL);
	ufs_init();
}

void
ffs_reinit(void)
{

	ufs_reinit();
}

void
ffs_done(void)
{
	if (--ffs_initcount > 0)
		return;

	ufs_done();
	pool_cache_destroy(ffs_dinode2_cache);
	pool_cache_destroy(ffs_dinode1_cache);
	pool_cache_destroy(ffs_inode_cache);
}

/*
 * Write a superblock and associated information back to disk.
 */
int
ffs_sbupdate(struct ufsmount *mp, int waitfor)
{
	struct fs *fs = mp->um_fs;
	struct buf *bp;
	int error = 0;
	u_int32_t saveflag;

	error = ffs_getblk(mp->um_devvp,
	    fs->fs_sblockloc / DEV_BSIZE, FFS_NOBLK,
	    fs->fs_sbsize, false, &bp);
	if (error)
		return error;
	saveflag = fs->fs_flags & FS_INTERNAL;
	fs->fs_flags &= ~FS_INTERNAL;

	memcpy(bp->b_data, fs, fs->fs_sbsize);

	ffs_oldfscompat_write((struct fs *)bp->b_data, mp);
#ifdef FFS_EI
	if (mp->um_flags & UFS_NEEDSWAP)
		ffs_sb_swap((struct fs *)bp->b_data, (struct fs *)bp->b_data);
#endif
	fs->fs_flags |= saveflag;

	if (waitfor == MNT_WAIT)
		error = bwrite(bp);
	else
		bawrite(bp);
	return (error);
}

int
ffs_cgupdate(struct ufsmount *mp, int waitfor)
{
	struct fs *fs = mp->um_fs;
	struct buf *bp;
	int blks;
	void *space;
	int i, size, error = 0, allerror = 0;

	allerror = ffs_sbupdate(mp, waitfor);
	blks = howmany(fs->fs_cssize, fs->fs_fsize);
	space = fs->fs_csp;
	for (i = 0; i < blks; i += fs->fs_frag) {
		size = fs->fs_bsize;
		if (i + fs->fs_frag > blks)
			size = (blks - i) * fs->fs_fsize;
		error = ffs_getblk(mp->um_devvp, fsbtodb(fs, fs->fs_csaddr + i),
		    FFS_NOBLK, size, false, &bp);
		if (error)
			break;
#ifdef FFS_EI
		if (mp->um_flags & UFS_NEEDSWAP)
			ffs_csum_swap((struct csum*)space,
			    (struct csum*)bp->b_data, size);
		else
#endif
			memcpy(bp->b_data, space, (u_int)size);
		space = (char *)space + size;
		if (waitfor == MNT_WAIT)
			error = bwrite(bp);
		else
			bawrite(bp);
	}
	if (!allerror && error)
		allerror = error;
	return (allerror);
}

int
ffs_extattrctl(struct mount *mp, int cmd, struct vnode *vp,
    int attrnamespace, const char *attrname)
{
#ifdef UFS_EXTATTR
	/*
	 * File-backed extended attributes are only supported on UFS1.
	 * UFS2 has native extended attributes.
	 */
	if (VFSTOUFS(mp)->um_fstype == UFS1)
		return (ufs_extattrctl(mp, cmd, vp, attrnamespace, attrname));
#endif
	return (vfs_stdextattrctl(mp, cmd, vp, attrnamespace, attrname));
}

int
ffs_suspendctl(struct mount *mp, int cmd)
{
	int error;
	struct lwp *l = curlwp;

	switch (cmd) {
	case SUSPEND_SUSPEND:
		if ((error = fstrans_setstate(mp, FSTRANS_SUSPENDING)) != 0)
			return error;
		error = ffs_sync(mp, MNT_WAIT, l->l_proc->p_cred);
		if (error == 0)
			error = fstrans_setstate(mp, FSTRANS_SUSPENDED);
#ifdef WAPBL
		if (error == 0 && mp->mnt_wapbl)
			error = wapbl_flush(mp->mnt_wapbl, 1);
#endif
		if (error != 0) {
			(void) fstrans_setstate(mp, FSTRANS_NORMAL);
			return error;
		}
		return 0;

	case SUSPEND_RESUME:
		return fstrans_setstate(mp, FSTRANS_NORMAL);

	default:
		return EINVAL;
	}
}

/*
 * Synch vnode for a mounted file system.  This is called for foreign
 * vnodes, i.e. non-ffs.
 */
static int
ffs_vfs_fsync(vnode_t *vp, int flags)
{
	int error, passes, skipmeta, i, pflags;
	buf_t *bp, *nbp;
#ifdef WAPBL
	struct mount *mp;
#endif

	KASSERT(vp->v_type == VBLK);
	KASSERT(vp->v_specmountpoint != NULL);

	/*
	 * Flush all dirty data associated with the vnode.
	 */
	pflags = PGO_ALLPAGES | PGO_CLEANIT;
	if ((flags & FSYNC_WAIT) != 0)
		pflags |= PGO_SYNCIO;
	mutex_enter(&vp->v_interlock);
	error = VOP_PUTPAGES(vp, 0, 0, pflags);
	if (error)
		return error;

#ifdef WAPBL
	mp = vp->v_specmountpoint;
	if (mp && mp->mnt_wapbl) {
		/*
		 * Don't bother writing out metadata if the syncer is
		 * making the request.  We will let the sync vnode
		 * write it out in a single burst through a call to
		 * VFS_SYNC().
		 */
		if ((flags & (FSYNC_DATAONLY | FSYNC_LAZY | FSYNC_NOLOG)) != 0)
			return 0;

		/*
		 * Don't flush the log if the vnode being flushed
		 * contains no dirty buffers that could be in the log.
		 */
		if (!LIST_EMPTY(&vp->v_dirtyblkhd)) {
			error = wapbl_flush(mp->mnt_wapbl, 0);
			if (error)
				return error;
		}

		if ((flags & FSYNC_WAIT) != 0) {
			mutex_enter(&vp->v_interlock);
			while (vp->v_numoutput)
				cv_wait(&vp->v_cv, &vp->v_interlock);
			mutex_exit(&vp->v_interlock);
		}

		return 0;
	}
#endif /* WAPBL */

	/*
	 * Write out metadata for non-logging file systems. XXX This block
	 * should be simplified now that softdep is gone.
	 */
	passes = NIADDR + 1;
	skipmeta = 0;
	if (flags & FSYNC_WAIT)
		skipmeta = 1;

loop:
	mutex_enter(&bufcache_lock);
	LIST_FOREACH(bp, &vp->v_dirtyblkhd, b_vnbufs) {
		bp->b_cflags &= ~BC_SCANNED;
	}
	for (bp = LIST_FIRST(&vp->v_dirtyblkhd); bp; bp = nbp) {
		nbp = LIST_NEXT(bp, b_vnbufs);
		if (bp->b_cflags & (BC_BUSY | BC_SCANNED))
			continue;
		if ((bp->b_oflags & BO_DELWRI) == 0)
			panic("ffs_fsync: not dirty");
		if (skipmeta && bp->b_lblkno < 0)
			continue;
		bp->b_cflags |= BC_BUSY | BC_VFLUSH | BC_SCANNED;
		mutex_exit(&bufcache_lock);
		/*
		 * On our final pass through, do all I/O synchronously
		 * so that we can find out if our flush is failing
		 * because of write errors.
		 */
		if (passes > 0 || !(flags & FSYNC_WAIT))
			(void) bawrite(bp);
		else if ((error = bwrite(bp)) != 0)
			return (error);
		/*
		 * Since we unlocked during the I/O, we need
		 * to start from a known point.
		 */
		mutex_enter(&bufcache_lock);
		nbp = LIST_FIRST(&vp->v_dirtyblkhd);
	}
	mutex_exit(&bufcache_lock);
	if (skipmeta) {
		skipmeta = 0;
		goto loop;
	}

	if ((flags & FSYNC_WAIT) != 0) {
		mutex_enter(&vp->v_interlock);
		while (vp->v_numoutput) {
			cv_wait(&vp->v_cv, &vp->v_interlock);
		}
		mutex_exit(&vp->v_interlock);

		if (!LIST_EMPTY(&vp->v_dirtyblkhd)) {
			/*
			* Block devices associated with filesystems may
			* have new I/O requests posted for them even if
			* the vnode is locked, so no amount of trying will
			* get them clean. Thus we give block devices a
			* good effort, then just give up. For all other file
			* types, go around and try again until it is clean.
			*/
			if (passes > 0) {
				passes--;
				goto loop;
			}
#ifdef DIAGNOSTIC
			if (vp->v_type != VBLK)
				vprint("ffs_fsync: dirty", vp);
#endif
		}
	}

	if (error == 0 && (flags & FSYNC_CACHE) != 0) {
		(void)VOP_IOCTL(vp, DIOCCACHESYNC, &i, FWRITE,
		    kauth_cred_get());
	}

	return error;
}
