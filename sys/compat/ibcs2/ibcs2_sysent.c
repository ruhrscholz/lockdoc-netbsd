/* $NetBSD: ibcs2_sysent.c,v 1.39 2004/03/26 15:29:29 drochner Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.30 2004/03/26 15:18:54 drochner Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ibcs2_sysent.c,v 1.39 2004/03/26 15:29:29 drochner Exp $");

#if defined(_KERNEL_OPT)
#include "opt_sysv.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>
#include <compat/ibcs2/ibcs2_types.h>
#include <compat/ibcs2/ibcs2_signal.h>
#include <compat/ibcs2/ibcs2_syscallargs.h>
#include <compat/ibcs2/ibcs2_statfs.h>

#define	s(type)	sizeof(type)

struct sysent ibcs2_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct ibcs2_sys_read_args), 0,
	    ibcs2_sys_read },			/* 3 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ 3, s(struct ibcs2_sys_open_args), 0,
	    ibcs2_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ 3, s(struct ibcs2_sys_waitsys_args), 0,
	    ibcs2_sys_waitsys },		/* 7 = waitsys */
	{ 2, s(struct ibcs2_sys_creat_args), 0,
	    ibcs2_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args), 0,
	    sys_link },				/* 9 = link */
	{ 1, s(struct ibcs2_sys_unlink_args), 0,
	    ibcs2_sys_unlink },			/* 10 = unlink */
	{ 2, s(struct ibcs2_sys_execv_args), 0,
	    ibcs2_sys_execv },			/* 11 = execv */
	{ 1, s(struct ibcs2_sys_chdir_args), 0,
	    ibcs2_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct ibcs2_sys_time_args), 0,
	    ibcs2_sys_time },			/* 13 = time */
	{ 3, s(struct ibcs2_sys_mknod_args), 0,
	    ibcs2_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct ibcs2_sys_chmod_args), 0,
	    ibcs2_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct ibcs2_sys_chown_args), 0,
	    ibcs2_sys_chown },			/* 16 = chown */
	{ 1, s(struct sys_obreak_args), 0,
	    sys_obreak },			/* 17 = obreak */
	{ 2, s(struct ibcs2_sys_stat_args), 0,
	    ibcs2_sys_stat },			/* 18 = stat */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0, 0,
	    sys_getpid_with_ppid },		/* 20 = getpid_with_ppid */
	{ 6, s(struct ibcs2_sys_mount_args), 0,
	    ibcs2_sys_mount },			/* 21 = mount */
	{ 1, s(struct ibcs2_sys_umount_args), 0,
	    ibcs2_sys_umount },			/* 22 = umount */
	{ 1, s(struct ibcs2_sys_setuid_args), 0,
	    ibcs2_sys_setuid },			/* 23 = setuid */
	{ 0, 0, 0,
	    sys_getuid_with_euid },		/* 24 = getuid_with_euid */
	{ 1, s(struct ibcs2_sys_stime_args), 0,
	    ibcs2_sys_stime },			/* 25 = stime */
	{ 0, 0, 0,
	    sys_nosys },			/* 26 = unimplemented ibcs2_ptrace */
	{ 1, s(struct ibcs2_sys_alarm_args), 0,
	    ibcs2_sys_alarm },			/* 27 = alarm */
	{ 2, s(struct ibcs2_sys_fstat_args), 0,
	    ibcs2_sys_fstat },			/* 28 = fstat */
	{ 0, 0, 0,
	    ibcs2_sys_pause },			/* 29 = pause */
	{ 2, s(struct ibcs2_sys_utime_args), 0,
	    ibcs2_sys_utime },			/* 30 = utime */
	{ 0, 0, 0,
	    sys_nosys },			/* 31 = unimplemented was stty */
	{ 2, s(struct ibcs2_sys_gtty_args), 0,
	    ibcs2_sys_gtty },			/* 32 = gtty */
	{ 2, s(struct ibcs2_sys_access_args), 0,
	    ibcs2_sys_access },			/* 33 = access */
	{ 1, s(struct ibcs2_sys_nice_args), 0,
	    ibcs2_sys_nice },			/* 34 = nice */
	{ 4, s(struct ibcs2_sys_statfs_args), 0,
	    ibcs2_sys_statfs },			/* 35 = statfs */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct ibcs2_sys_kill_args), 0,
	    ibcs2_sys_kill },			/* 37 = kill */
	{ 4, s(struct ibcs2_sys_fstatfs_args), 0,
	    ibcs2_sys_fstatfs },		/* 38 = fstatfs */
	{ 4, s(struct ibcs2_sys_pgrpsys_args), 0,
	    ibcs2_sys_pgrpsys },		/* 39 = pgrpsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 40 = unimplemented ibcs2_xenix */
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 0, 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 1, s(struct ibcs2_sys_times_args), 0,
	    ibcs2_sys_times },			/* 43 = times */
	{ 0, 0, 0,
	    sys_nosys },			/* 44 = unimplemented profil */
	{ 1, s(struct ibcs2_sys_plock_args), 0,
	    ibcs2_sys_plock },			/* 45 = plock */
	{ 1, s(struct ibcs2_sys_setgid_args), 0,
	    ibcs2_sys_setgid },			/* 46 = setgid */
	{ 0, 0, 0,
	    sys_getgid_with_egid },		/* 47 = getgid_with_egid */
	{ 2, s(struct ibcs2_sys_sigsys_args), 0,
	    ibcs2_sys_sigsys },			/* 48 = sigsys */
#ifdef SYSVMSG
	{ 6, s(struct ibcs2_sys_msgsys_args), 0,
	    ibcs2_sys_msgsys },			/* 49 = msgsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 49 = unimplemented msgsys */
#endif
	{ 2, s(struct ibcs2_sys_sysmachine_args), 0,
	    ibcs2_sys_sysmachine },		/* 50 = sysmachine */
	{ 0, 0, 0,
	    sys_nosys },			/* 51 = unimplemented ibcs2_acct */
#ifdef SYSVSHM
	{ 4, s(struct ibcs2_sys_shmsys_args), 0,
	    ibcs2_sys_shmsys },			/* 52 = shmsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 52 = unimplemented shmsys */
#endif
#ifdef SYSVSEM
	{ 5, s(struct ibcs2_sys_semsys_args), 0,
	    ibcs2_sys_semsys },			/* 53 = semsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 53 = unimplemented semsys */
#endif
	{ 3, s(struct ibcs2_sys_ioctl_args), 0,
	    ibcs2_sys_ioctl },			/* 54 = ioctl */
	{ 3, s(struct ibcs2_sys_uadmin_args), 0,
	    ibcs2_sys_uadmin },			/* 55 = uadmin */
	{ 0, 0, 0,
	    sys_nosys },			/* 56 = unimplemented */
	{ 3, s(struct ibcs2_sys_utssys_args), 0,
	    ibcs2_sys_utssys },			/* 57 = utssys */
	{ 1, s(struct sys_fsync_args), 0,
	    sys_fsync },			/* 58 = fsync */
	{ 3, s(struct ibcs2_sys_execve_args), 0,
	    ibcs2_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 61 = chroot */
	{ 3, s(struct ibcs2_sys_fcntl_args), 0,
	    ibcs2_sys_fcntl },			/* 62 = fcntl */
	{ 2, s(struct ibcs2_sys_ulimit_args), 0,
	    ibcs2_sys_ulimit },			/* 63 = ulimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 64 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 65 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 66 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 69 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 70 = obsolete rfs_advfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 71 = obsolete rfs_unadvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 72 = obsolete rfs_rmount */
	{ 0, 0, 0,
	    sys_nosys },			/* 73 = obsolete rfs_rumount */
	{ 0, 0, 0,
	    sys_nosys },			/* 74 = obsolete rfs_rfstart */
	{ 0, 0, 0,
	    sys_nosys },			/* 75 = obsolete rfs_sigret */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = obsolete rfs_rdebug */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = obsolete rfs_rfstop */
	{ 0, 0, 0,
	    sys_nosys },			/* 78 = unimplemented rfs_rfsys */
	{ 1, s(struct ibcs2_sys_rmdir_args), 0,
	    ibcs2_sys_rmdir },			/* 79 = rmdir */
	{ 2, s(struct ibcs2_sys_mkdir_args), 0,
	    ibcs2_sys_mkdir },			/* 80 = mkdir */
	{ 3, s(struct ibcs2_sys_getdents_args), 0,
	    ibcs2_sys_getdents },		/* 81 = getdents */
	{ 0, 0, 0,
	    sys_nosys },			/* 82 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 83 = unimplemented */
	{ 3, s(struct ibcs2_sys_sysfs_args), 0,
	    ibcs2_sys_sysfs },			/* 84 = sysfs */
	{ 4, s(struct ibcs2_sys_getmsg_args), 0,
	    ibcs2_sys_getmsg },			/* 85 = getmsg */
	{ 4, s(struct ibcs2_sys_putmsg_args), 0,
	    ibcs2_sys_putmsg },			/* 86 = putmsg */
	{ 3, s(struct sys_poll_args), 0,
	    sys_poll },				/* 87 = poll */
	{ 0, 0, 0,
	    sys_nosys },			/* 88 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 89 = unimplemented */
	{ 2, s(struct ibcs2_sys_symlink_args), 0,
	    ibcs2_sys_symlink },		/* 90 = symlink */
	{ 2, s(struct ibcs2_sys_lstat_args), 0,
	    ibcs2_sys_lstat },			/* 91 = lstat */
	{ 3, s(struct ibcs2_sys_readlink_args), 0,
	    ibcs2_sys_readlink },		/* 92 = readlink */
	{ 2, s(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 93 = fchmod */
	{ 3, s(struct sys___posix_fchown_args), 0,
	    sys___posix_fchown },		/* 94 = fchown */
	{ 0, 0, 0,
	    sys_nosys },			/* 95 = unimplemented */
	{ 1, s(struct compat_16_sys___sigreturn14_args), 0,
	    compat_16_sys___sigreturn14 },	/* 96 = sigreturn */
	{ 2, s(struct ibcs2_sys_sigaltstack_args), 0,
	    ibcs2_sys_sigaltstack },		/* 97 = sigaltstack */
	{ 0, 0, 0,
	    sys_nosys },			/* 98 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 99 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 100 = unimplemented getcontext/setcontext/sigsetjmp */
	{ 0, 0, 0,
	    sys_nosys },			/* 101 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 102 = unimplemented */
	{ 2, s(struct ibcs2_sys_statvfs_args), 0,
	    ibcs2_sys_statvfs },		/* 103 = statvfs */
	{ 2, s(struct ibcs2_sys_fstatvfs_args), 0,
	    ibcs2_sys_fstatvfs },		/* 104 = fstatvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 105 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 106 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 107 = unimplemented waitid */
	{ 0, 0, 0,
	    sys_nosys },			/* 108 = unimplemented sigsendset */
	{ 0, 0, 0,
	    sys_nosys },			/* 109 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 110 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 111 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 112 = unimplemented priocntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 113 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 114 = unimplemented */
	{ 6, s(struct ibcs2_sys_mmap_args), 0,
	    ibcs2_sys_mmap },			/* 115 = mmap */
	{ 3, s(struct sys_mprotect_args), 0,
	    sys_mprotect },			/* 116 = mprotect */
	{ 2, s(struct sys_munmap_args), 0,
	    sys_munmap },			/* 117 = munmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 118 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = unimplemented vfork */
	{ 1, s(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 120 = fchdir */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 121 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 122 = writev */
	{ 0, 0, 0,
	    sys_nosys },			/* 123 = unimplemented xstat */
	{ 0, 0, 0,
	    sys_nosys },			/* 124 = unimplemented lxstat */
	{ 0, 0, 0,
	    sys_nosys },			/* 125 = unimplemented fxstat */
	{ 0, 0, 0,
	    sys_nosys },			/* 126 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 127 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 128 = unimplemented setrlimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 129 = unimplemented getrlimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 130 = unimplemented lchown */
	{ 6, s(struct ibcs2_sys_memcntl_args), 0,
	    ibcs2_sys_memcntl },		/* 131 = memcntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 132 = unimplemented getpmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 133 = unimplemented putpmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 134 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 135 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 136 = unimplemented setegid */
	{ 0, 0, 0,
	    sys_nosys },			/* 137 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 138 = unimplemented adjtime */
	{ 0, 0, 0,
	    sys_nosys },			/* 139 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 140 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 141 = unimplemented seteuid */
	{ 0, 0, 0,
	    sys_nosys },			/* 142 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 143 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 144 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 145 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 146 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 147 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 148 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 149 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 150 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 153 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 154 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 155 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 156 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 157 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 158 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 159 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 160 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 161 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 162 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 163 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 164 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 165 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 166 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 167 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 168 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 169 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 170 = unimplemented */
	{ 1, s(struct ibcs2_sys_gettimeofday_args), 0,
	    ibcs2_sys_gettimeofday },		/* 171 = gettimeofday */
	{ 1, s(struct ibcs2_sys_settimeofday_args), 0,
	    ibcs2_sys_settimeofday },		/* 172 = settimeofday */
	{ 0, 0, 0,
	    sys_nosys },			/* 173 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 174 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 175 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 176 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 180 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 181 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 182 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 183 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 184 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 185 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 186 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 188 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 189 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 190 = unimplemented */
	{ 2, s(struct compat_43_sys_truncate_args), 0,
	    compat_43_sys_truncate },		/* 191 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args), 0,
	    compat_43_sys_ftruncate },		/* 192 = ftruncate */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 194 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 195 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 196 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 197 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 199 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 200 = unimplemented */
	{ 3, s(struct xenix_sys_locking_args), 0,
	    xenix_sys_locking },		/* 201 = locking */
	{ 0, 0, 0,
	    sys_nosys },			/* 202 = unimplemented xenix_creatsem */
	{ 0, 0, 0,
	    sys_nosys },			/* 203 = unimplemented xenix_opensem */
	{ 0, 0, 0,
	    sys_nosys },			/* 204 = unimplemented xenix_sigsem */
	{ 0, 0, 0,
	    sys_nosys },			/* 205 = unimplemented xenix_waitsem */
	{ 0, 0, 0,
	    sys_nosys },			/* 206 = unimplemented xenix_nbwaitsem */
	{ 1, s(struct xenix_sys_rdchk_args), 0,
	    xenix_sys_rdchk },			/* 207 = rdchk */
	{ 0, 0, 0,
	    sys_nosys },			/* 208 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 209 = unimplemented */
	{ 2, s(struct xenix_sys_chsize_args), 0,
	    xenix_sys_chsize },			/* 210 = chsize */
	{ 1, s(struct xenix_sys_ftime_args), 0,
	    xenix_sys_ftime },			/* 211 = ftime */
	{ 1, s(struct xenix_sys_nap_args), 0,
	    xenix_sys_nap },			/* 212 = nap */
	{ 0, 0, 0,
	    sys_nosys },			/* 213 = unimplemented xenix_sdget */
	{ 0, 0, 0,
	    sys_nosys },			/* 214 = unimplemented xenix_sdfree */
	{ 0, 0, 0,
	    sys_nosys },			/* 215 = unimplemented xenix_sdenter */
	{ 0, 0, 0,
	    sys_nosys },			/* 216 = unimplemented xenix_sdleave */
	{ 0, 0, 0,
	    sys_nosys },			/* 217 = unimplemented xenix_sdgetv */
	{ 0, 0, 0,
	    sys_nosys },			/* 218 = unimplemented xenix_sdwaitv */
	{ 0, 0, 0,
	    sys_nosys },			/* 219 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 220 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 221 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 222 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 224 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 225 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 230 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 231 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 232 = unimplemented xenix_proctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 233 = unimplemented xenix_execseg */
	{ 0, 0, 0,
	    sys_nosys },			/* 234 = unimplemented xenix_unexecseg */
	{ 0, 0, 0,
	    sys_nosys },			/* 235 = unimplemented */
	{ 5, s(struct sys_select_args), 0,
	    sys_select },			/* 236 = select */
	{ 2, s(struct ibcs2_sys_eaccess_args), 0,
	    ibcs2_sys_eaccess },		/* 237 = eaccess */
	{ 0, 0, 0,
	    sys_nosys },			/* 238 = unimplemented xenix_paccess */
	{ 3, s(struct ibcs2_sys_sigaction_args), 0,
	    ibcs2_sys_sigaction },		/* 239 = sigaction */
	{ 3, s(struct ibcs2_sys_sigprocmask_args), 0,
	    ibcs2_sys_sigprocmask },		/* 240 = sigprocmask */
	{ 1, s(struct ibcs2_sys_sigpending_args), 0,
	    ibcs2_sys_sigpending },		/* 241 = sigpending */
	{ 1, s(struct ibcs2_sys_sigsuspend_args), 0,
	    ibcs2_sys_sigsuspend },		/* 242 = sigsuspend */
	{ 2, s(struct ibcs2_sys_getgroups_args), 0,
	    ibcs2_sys_getgroups },		/* 243 = getgroups */
	{ 2, s(struct ibcs2_sys_setgroups_args), 0,
	    ibcs2_sys_setgroups },		/* 244 = setgroups */
	{ 1, s(struct ibcs2_sys_sysconf_args), 0,
	    ibcs2_sys_sysconf },		/* 245 = sysconf */
	{ 2, s(struct ibcs2_sys_pathconf_args), 0,
	    ibcs2_sys_pathconf },		/* 246 = pathconf */
	{ 2, s(struct ibcs2_sys_fpathconf_args), 0,
	    ibcs2_sys_fpathconf },		/* 247 = fpathconf */
	{ 2, s(struct ibcs2_sys_rename_args), 0,
	    ibcs2_sys_rename },			/* 248 = rename */
	{ 0, 0, 0,
	    sys_nosys },			/* 249 = unimplemented */
	{ 2, s(struct ibcs2_sys_scoinfo_args), 0,
	    ibcs2_sys_scoinfo },		/* 250 = scoinfo */
	{ 0, 0, 0,
	    sys_nosys },			/* 251 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 252 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 253 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 254 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 255 = unimplemented getitimer */
	{ 0, 0, 0,
	    sys_nosys },			/* 256 = unimplemented setitimer */
	{ 0, 0, 0,
	    sys_nosys },			/* 257 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 258 = unimplemented setreuid */
	{ 0, 0, 0,
	    sys_nosys },			/* 259 = unimplemented setregid */
	{ 0, 0, 0,
	    sys_nosys },			/* 260 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 261 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 262 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 263 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 264 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 265 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 266 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 267 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 268 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 269 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 270 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 271 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 272 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 273 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 274 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 275 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 276 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 277 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 278 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 279 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 280 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 281 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 282 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 283 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 284 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 285 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 286 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 287 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 288 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 289 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 290 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 291 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 292 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 293 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 294 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 295 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 296 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 297 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 298 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 299 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 300 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 301 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 302 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 303 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 304 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 305 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 306 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 307 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 308 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 309 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 310 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 311 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 312 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 313 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 314 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 315 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 316 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 317 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 318 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 319 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 320 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 321 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 322 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 323 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 324 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 325 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 326 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 327 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 328 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 329 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 330 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 331 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 332 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 333 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 334 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 335 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 336 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 337 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 338 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 339 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 340 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 341 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 342 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 343 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 344 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 345 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 346 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 347 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 348 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 349 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 350 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 351 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 352 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 353 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 354 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 355 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 356 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 357 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 358 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 359 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 360 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 361 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 362 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 363 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 364 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 365 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 366 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 367 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 368 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 369 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 370 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 371 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 372 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 373 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 374 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 375 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 376 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 377 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 378 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 379 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 380 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 381 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 382 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 383 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 384 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 385 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 386 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 387 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 388 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 389 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 390 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 391 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 392 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 393 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 394 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 395 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 396 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 397 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 398 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 399 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 400 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 401 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 402 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 403 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 404 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 405 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 406 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 407 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 408 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 409 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 410 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 411 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 412 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 413 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 414 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 415 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 416 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 417 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 418 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 419 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 420 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 421 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 422 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 423 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 424 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 425 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 426 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 427 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 428 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 429 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 430 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 431 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 432 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 433 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 434 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 435 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 436 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 437 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 438 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 439 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 440 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 441 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 442 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 443 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 444 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 445 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 446 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 447 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 448 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 449 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 450 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 451 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 452 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 453 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 454 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 455 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 456 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 457 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 458 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 459 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 460 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 461 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 462 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 463 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 464 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 465 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 466 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 467 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 468 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 469 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 470 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 471 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 472 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 473 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 474 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 475 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 476 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 477 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 478 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 479 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 480 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 481 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 482 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 483 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 484 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 485 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 486 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 487 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 488 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 489 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 490 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 491 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 492 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 493 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 494 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 495 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 496 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 497 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 498 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 499 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 500 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 501 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 502 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 503 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 504 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 505 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 506 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 507 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 508 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 509 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 510 = filler */
	{ 0, 0, 0,
	    sys_nosys },			/* 511 = filler */
};

