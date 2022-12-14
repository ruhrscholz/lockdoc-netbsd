/*	$NetBSD: netbsd32_compat_20.c,v 1.38.4.2 2022/08/03 11:11:31 martin Exp $	*/

/*
 * Copyright (c) 1998, 2001 Matthew R. Green
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: netbsd32_compat_20.c,v 1.38.4.2 2022/08/03 11:11:31 martin Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/module.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ktrace.h>
#include <sys/vnode.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/namei.h>
#include <sys/syscallargs.h>
#include <sys/syscallvar.h>
#include <sys/proc.h>
#include <sys/dirent.h>

#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscall.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/netbsd32/netbsd32_conv.h>

static inline void compat_20_netbsd32_from_statvfs(struct statvfs *,
    struct netbsd32_statfs *);

static inline void
compat_20_netbsd32_from_statvfs(struct statvfs *sbp, struct netbsd32_statfs *sb32p)
{

	memset(sb32p, 0, sizeof(*sb32p));
	sb32p->f_type = 0; /* XXX Put an actual value? */
	sb32p->f_flags = sbp->f_flag;
	sb32p->f_bsize = (netbsd32_long)sbp->f_bsize;
	sb32p->f_iosize = (netbsd32_long)sbp->f_iosize;
	sb32p->f_blocks = (netbsd32_long)sbp->f_blocks;
	sb32p->f_bfree = (netbsd32_long)sbp->f_bfree;
	sb32p->f_bavail = (netbsd32_long)sbp->f_bavail;
	sb32p->f_files = (netbsd32_long)sbp->f_files;
	sb32p->f_ffree = (netbsd32_long)sbp->f_ffree;
	sb32p->f_fsid = sbp->f_fsidx;
	sb32p->f_owner = sbp->f_owner;
	sb32p->f_spare[0] = 0;
	sb32p->f_spare[1] = 0;
	sb32p->f_spare[2] = 0;
	sb32p->f_spare[3] = 0;
	(void)memcpy(sb32p->f_fstypename, sbp->f_fstypename,
	    sizeof(sb32p->f_fstypename));
	(void)memcpy(sb32p->f_mntonname, sbp->f_mntonname,
	    sizeof(sb32p->f_mntonname));
	(void)memcpy(sb32p->f_mntfromname, sbp->f_mntfromname,
	    sizeof(sb32p->f_mntfromname));
}

int
compat_20_netbsd32_getfsstat(struct lwp *l, const struct compat_20_netbsd32_getfsstat_args *uap, register_t *retval)
{
	/* {
		syscallarg(netbsd32_statfsp_t) buf;
		syscallarg(netbsd32_long) bufsize;
		syscallarg(int) flags;
	} */
	int root = 0;
	struct proc *p = l->l_proc;
	mount_iterator_t *iter;
	struct mount *mp;
	struct statvfs *sb;
	struct netbsd32_statfs sb32;
	void *sfsp;
	size_t count, maxcount;
	int error = 0;

	sb = STATVFSBUF_GET();
	maxcount = SCARG(uap, bufsize) / sizeof(struct netbsd32_statfs);
	sfsp = SCARG_P32(uap, buf);
	mountlist_iterator_init(&iter);
	count = 0;
	while ((mp = mountlist_iterator_next(iter)) != NULL) {
		if (sfsp && count < maxcount) {
			error = dostatvfs(mp, sb, l, SCARG(uap, flags), 0);
			if (error) {
				error = 0;
				continue;
			}
			compat_20_netbsd32_from_statvfs(sb, &sb32);
			error = copyout(&sb32, sfsp, sizeof(sb32));
			if (error)
				goto out;
			sfsp = (char *)sfsp + sizeof(sb32);
			root |= strcmp(sb->f_mntonname, "/") == 0;
		}
		count++;
	}

	if (root == 0 && p->p_cwdi->cwdi_rdir) {
		/*
		 * fake a root entry
		 */
		error = dostatvfs(p->p_cwdi->cwdi_rdir->v_mount,
		    sb, l, SCARG(uap, flags), 1);
		if (error != 0)
			goto out;
		if (sfsp) {
			compat_20_netbsd32_from_statvfs(sb, &sb32);
			error = copyout(&sb32, sfsp, sizeof(sb32));
			if (error != 0)
				goto out;
		}
		count++;
	}

	if (sfsp && count > maxcount)
		*retval = maxcount;
	else
		*retval = count;
out:
	mountlist_iterator_destroy(iter);
	STATVFSBUF_PUT(sb);
	return error;
}

int
compat_20_netbsd32_statfs(struct lwp *l, const struct compat_20_netbsd32_statfs_args *uap, register_t *retval)
{
	/* {
		syscallarg(const netbsd32_charp) path;
		syscallarg(netbsd32_statfsp_t) buf;
	} */
	struct mount *mp;
	struct statvfs *sb;
	struct netbsd32_statfs s32;
	int error;
	struct vnode *vp;

	error = namei_simple_user(SCARG_P32(uap, path),
				NSM_FOLLOW_TRYEMULROOT, &vp);
	if (error != 0)
		return (error);
	mp = vp->v_mount;
	vrele(vp);
	sb = STATVFSBUF_GET();
	if ((error = dostatvfs(mp, sb, l, 0, 0)) != 0)
		goto out;
	compat_20_netbsd32_from_statvfs(sb, &s32);
	error = copyout(&s32, SCARG_P32(uap, buf), sizeof(s32));
out:
	STATVFSBUF_PUT(sb);
	return error;
}

int
compat_20_netbsd32_fstatfs(struct lwp *l, const struct compat_20_netbsd32_fstatfs_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) fd;
		syscallarg(netbsd32_statfsp_t) buf;
	} */
	file_t *fp;
	struct mount *mp;
	struct statvfs *sb;
	struct netbsd32_statfs s32;
	int error;

	/* fd_getvnode() will use the descriptor for us */
	if ((error = fd_getvnode(SCARG(uap, fd), &fp)) != 0)
		return (error);
	mp = fp->f_vnode->v_mount;
	sb = STATVFSBUF_GET();
	if ((error = dostatvfs(mp, sb, l, 0, 0)) != 0)
		goto out;
	compat_20_netbsd32_from_statvfs(sb, &s32);
	error = copyout(&s32, SCARG_P32(uap, buf), sizeof(s32));
 out:
	STATVFSBUF_PUT(sb);
	fd_putfile(SCARG(uap, fd));
	return (error);
}

int
compat_20_netbsd32_fhstatfs(struct lwp *l, const struct compat_20_netbsd32_fhstatfs_args *uap, register_t *retval)
{
	/* {
		syscallarg(const netbsd32_fhandlep_t) fhp;
		syscallarg(struct statvfs *) buf;
	} */
	struct compat_30_sys_fhstatvfs1_args ua;

	NETBSD32TOP_UAP(fhp, const struct compat_30_fhandle);
	NETBSD32TOP_UAP(buf, struct statvfs);
#ifdef notyet
	NETBSD32TOP_UAP(flags, int);
#endif
	return (compat_30_sys_fhstatvfs1(l, &ua, retval));
}

static struct syscall_package compat_netbsd32_20_syscalls[] = {
	{ NETBSD32_SYS_compat_20_netbsd32_statfs, 0,
	    (sy_call_t *)compat_20_netbsd32_statfs },
	{ NETBSD32_SYS_compat_20_netbsd32_fstatfs, 0,
	    (sy_call_t *)compat_20_netbsd32_fstatfs },
	{ NETBSD32_SYS_compat_20_netbsd32_fhstatfs, 0,
	    (sy_call_t *)compat_20_netbsd32_fhstatfs },
	{ NETBSD32_SYS_compat_20_netbsd32_getfsstat, 0,
	    (sy_call_t *)compat_20_netbsd32_getfsstat },
	{ 0, 0, NULL }
};

MODULE(MODULE_CLASS_EXEC, compat_netbsd32_20, "compat_netbsd32_30,compat_20");

static int
compat_netbsd32_20_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		return syscall_establish(&emul_netbsd32,
		    compat_netbsd32_20_syscalls);

	case MODULE_CMD_FINI:
		return syscall_disestablish(&emul_netbsd32,
		    compat_netbsd32_20_syscalls);

	default:
		return ENOTTY;
	}
}
