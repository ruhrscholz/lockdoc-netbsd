/* $NetBSD: linux32_sysent.c,v 1.31 2008/02/02 22:57:50 dsl Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.28 2008/02/02 22:57:05 dsl Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux32_sysent.c,v 1.31 2008/02/02 22:57:50 dsl Exp $");

#if defined(_KERNEL_OPT)
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <machine/netbsd32_machdep.h>
#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/linux32/common/linux32_types.h>
#include <compat/linux32/common/linux32_signal.h>
#include <compat/linux32/arch/amd64/linux32_missing.h>
#include <compat/linux32/linux32_syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)
#define	n(type)	(sizeof(type)/sizeof (register32_t))
#define	ns(type)	n(type), s(type)

struct sysent linux32_sysent[] = {
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_nosys },	/* 0 = syscall */
	{ ns(struct netbsd32_exit_args), 0,
	    (sy_call_t *)netbsd32_exit },	/* 1 = netbsd32_exit */
	{ 0, 0, 0,
	    (sy_call_t *)sys_fork },		/* 2 = fork */
	{ ns(struct netbsd32_read_args), 0,
	    (sy_call_t *)netbsd32_read },	/* 3 = netbsd32_read */
	{ ns(struct netbsd32_write_args), 0,
	    (sy_call_t *)netbsd32_write },	/* 4 = netbsd32_write */
	{ ns(struct linux32_sys_open_args), 0,
	    (sy_call_t *)linux32_sys_open },	/* 5 = open */
	{ ns(struct netbsd32_close_args), 0,
	    (sy_call_t *)netbsd32_close },	/* 6 = netbsd32_close */
	{ ns(struct linux32_sys_waitpid_args), 0,
	    (sy_call_t *)linux32_sys_waitpid },	/* 7 = waitpid */
	{ ns(struct linux32_sys_creat_args), 0,
	    (sy_call_t *)linux32_sys_creat },	/* 8 = creat */
	{ ns(struct netbsd32_link_args), 0,
	    (sy_call_t *)netbsd32_link },	/* 9 = netbsd32_link */
	{ ns(struct linux32_sys_unlink_args), 0,
	    (sy_call_t *)linux32_sys_unlink },	/* 10 = unlink */
	{ ns(struct netbsd32_execve_args), 0,
	    (sy_call_t *)netbsd32_execve },	/* 11 = netbsd32_execve */
	{ ns(struct netbsd32_chdir_args), 0,
	    (sy_call_t *)netbsd32_chdir },	/* 12 = netbsd32_chdir */
	{ ns(struct linux32_sys_time_args), 0,
	    (sy_call_t *)linux32_sys_time },	/* 13 = time */
	{ ns(struct linux32_sys_mknod_args), 0,
	    (sy_call_t *)linux32_sys_mknod },	/* 14 = mknod */
	{ ns(struct netbsd32_chmod_args), 0,
	    (sy_call_t *)netbsd32_chmod },	/* 15 = netbsd32_chmod */
	{ ns(struct linux32_sys_lchown16_args), 0,
	    (sy_call_t *)linux32_sys_lchown16 },/* 16 = lchown16 */
	{ ns(struct linux32_sys_break_args), 0,
	    (sy_call_t *)linux32_sys_break },	/* 17 = break */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 18 = obsolete ostat */
	{ ns(struct compat_43_netbsd32_olseek_args), 0,
	    (sy_call_t *)compat_43_netbsd32_olseek },/* 19 = compat_43_netbsd32_olseek */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_getpid },	/* 20 = getpid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 21 = unimplemented mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 22 = unimplemented umount */
	{ ns(struct netbsd32_setuid_args), 0,
	    (sy_call_t *)netbsd32_setuid },	/* 23 = linux_setuid16 */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getuid },		/* 24 = linux_getuid16 */
	{ ns(struct linux32_sys_stime_args), 0,
	    (sy_call_t *)linux32_sys_stime },	/* 25 = stime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 26 = unimplemented ptrace */
	{ ns(struct linux32_sys_alarm_args), 0,
	    (sy_call_t *)linux32_sys_alarm },	/* 27 = alarm */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 28 = obsolete ofstat */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_pause },	/* 29 = pause */
	{ ns(struct linux32_sys_utime_args), 0,
	    (sy_call_t *)linux32_sys_utime },	/* 30 = utime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 31 = obsolete stty */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 32 = obsolete gtty */
	{ ns(struct netbsd32_access_args), 0,
	    (sy_call_t *)netbsd32_access },	/* 33 = netbsd32_access */
	{ ns(struct linux32_sys_nice_args), 0,
	    (sy_call_t *)linux32_sys_nice },	/* 34 = nice */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 35 = obsolete ftime */
	{ 0, 0, 0,
	    (sy_call_t *)sys_sync },		/* 36 = sync */
	{ ns(struct linux32_sys_kill_args), 0,
	    (sy_call_t *)linux32_sys_kill },	/* 37 = kill */
	{ ns(struct linux32_sys_rename_args), 0,
	    (sy_call_t *)linux32_sys_rename },	/* 38 = rename */
	{ ns(struct netbsd32_mkdir_args), 0,
	    (sy_call_t *)netbsd32_mkdir },	/* 39 = netbsd32_mkdir */
	{ ns(struct netbsd32_rmdir_args), 0,
	    (sy_call_t *)netbsd32_rmdir },	/* 40 = netbsd32_rmdir */
	{ ns(struct netbsd32_dup_args), 0,
	    (sy_call_t *)netbsd32_dup },	/* 41 = netbsd32_dup */
	{ ns(struct linux32_sys_pipe_args), 0,
	    (sy_call_t *)linux32_sys_pipe },	/* 42 = pipe */
	{ ns(struct linux32_sys_times_args), 0,
	    (sy_call_t *)linux32_sys_times },	/* 43 = times */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 44 = obsolete prof */
	{ ns(struct linux32_sys_brk_args), 0,
	    (sy_call_t *)linux32_sys_brk },	/* 45 = brk */
	{ ns(struct netbsd32_setgid_args), 0,
	    (sy_call_t *)netbsd32_setgid },	/* 46 = linux_setgid16 */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getgid },		/* 47 = linux_getgid16 */
	{ ns(struct linux32_sys_signal_args), 0,
	    (sy_call_t *)linux32_sys_signal },	/* 48 = signal */
	{ 0, 0, 0,
	    (sy_call_t *)sys_geteuid },		/* 49 = linux_geteuid16 */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getegid },		/* 50 = linux_getegid16 */
	{ ns(struct netbsd32_acct_args), 0,
	    (sy_call_t *)netbsd32_acct },	/* 51 = netbsd32_acct */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 52 = obsolete phys */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 53 = obsolete lock */
	{ ns(struct linux32_sys_ioctl_args), 0,
	    (sy_call_t *)linux32_sys_ioctl },	/* 54 = ioctl */
	{ ns(struct linux32_sys_fcntl_args), 0,
	    (sy_call_t *)linux32_sys_fcntl },	/* 55 = fcntl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 56 = obsolete mpx */
	{ ns(struct netbsd32_setpgid_args), 0,
	    (sy_call_t *)netbsd32_setpgid },	/* 57 = netbsd32_setpgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 58 = obsolete ulimit */
	{ ns(struct linux32_sys_oldolduname_args), 0,
	    (sy_call_t *)linux32_sys_oldolduname },/* 59 = oldolduname */
	{ ns(struct netbsd32_umask_args), 0,
	    (sy_call_t *)netbsd32_umask },	/* 60 = netbsd32_umask */
	{ ns(struct netbsd32_chroot_args), 0,
	    (sy_call_t *)netbsd32_chroot },	/* 61 = netbsd32_chroot */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 62 = unimplemented ustat */
	{ ns(struct netbsd32_dup2_args), 0,
	    (sy_call_t *)netbsd32_dup2 },	/* 63 = netbsd32_dup2 */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_getppid },	/* 64 = getppid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getpgrp },		/* 65 = getpgrp */
	{ 0, 0, 0,
	    (sy_call_t *)sys_setsid },		/* 66 = setsid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 67 = unimplemented sigaction */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 68 = unimplemented siggetmask */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 69 = unimplemented sigsetmask */
	{ ns(struct linux32_sys_setreuid16_args), 0,
	    (sy_call_t *)linux32_sys_setreuid16 },/* 70 = setreuid16 */
	{ ns(struct linux32_sys_setregid16_args), 0,
	    (sy_call_t *)linux32_sys_setregid16 },/* 71 = setregid16 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 72 = unimplemented sigsuspend */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 73 = unimplemented sigpending */
	{ ns(struct compat_43_netbsd32_osethostname_args), 0,
	    (sy_call_t *)compat_43_netbsd32_osethostname },/* 74 = compat_43_netbsd32_osethostname */
	{ ns(struct linux32_sys_setrlimit_args), 0,
	    (sy_call_t *)linux32_sys_setrlimit },/* 75 = setrlimit */
	{ ns(struct linux32_sys_getrlimit_args), 0,
	    (sy_call_t *)linux32_sys_getrlimit },/* 76 = getrlimit */
	{ ns(struct netbsd32_getrusage_args), 0,
	    (sy_call_t *)netbsd32_getrusage },	/* 77 = netbsd32_getrusage */
	{ ns(struct linux32_sys_gettimeofday_args), 0,
	    (sy_call_t *)linux32_sys_gettimeofday },/* 78 = gettimeofday */
	{ ns(struct linux32_sys_settimeofday_args), 0,
	    (sy_call_t *)linux32_sys_settimeofday },/* 79 = settimeofday */
	{ ns(struct linux32_sys_getgroups16_args), 0,
	    (sy_call_t *)linux32_sys_getgroups16 },/* 80 = getgroups16 */
	{ ns(struct linux32_sys_setgroups16_args), 0,
	    (sy_call_t *)linux32_sys_setgroups16 },/* 81 = setgroups16 */
	{ ns(struct linux32_sys_oldselect_args), 0,
	    (sy_call_t *)linux32_sys_oldselect },/* 82 = oldselect */
	{ ns(struct netbsd32_symlink_args), 0,
	    (sy_call_t *)netbsd32_symlink },	/* 83 = netbsd32_symlink */
	{ ns(struct compat_43_netbsd32_lstat43_args), 0,
	    (sy_call_t *)compat_43_netbsd32_lstat43 },/* 84 = compat_43_netbsd32_lstat43 */
	{ ns(struct netbsd32_readlink_args), 0,
	    (sy_call_t *)netbsd32_readlink },	/* 85 = netbsd32_readlink */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 86 = unimplemented uselib */
	{ ns(struct linux32_sys_swapon_args), 0,
	    (sy_call_t *)linux32_sys_swapon },	/* 87 = swapon */
	{ ns(struct linux32_sys_reboot_args), 0,
	    (sy_call_t *)linux32_sys_reboot },	/* 88 = reboot */
	{ ns(struct linux32_sys_readdir_args), 0,
	    (sy_call_t *)linux32_sys_readdir },	/* 89 = readdir */
	{ ns(struct linux32_sys_old_mmap_args), 0,
	    (sy_call_t *)linux32_sys_old_mmap },/* 90 = old_mmap */
	{ ns(struct netbsd32_munmap_args), 0,
	    (sy_call_t *)netbsd32_munmap },	/* 91 = netbsd32_munmap */
	{ ns(struct linux32_sys_truncate_args), 0,
	    (sy_call_t *)linux32_sys_truncate },/* 92 = truncate */
	{ ns(struct compat_43_netbsd32_oftruncate_args), 0,
	    (sy_call_t *)compat_43_netbsd32_oftruncate },/* 93 = compat_43_netbsd32_oftruncate */
	{ ns(struct netbsd32_fchmod_args), 0,
	    (sy_call_t *)netbsd32_fchmod },	/* 94 = netbsd32_fchmod */
	{ ns(struct linux32_sys_fchown16_args), 0,
	    (sy_call_t *)linux32_sys_fchown16 },/* 95 = fchown16 */
	{ ns(struct linux32_sys_getpriority_args), 0,
	    (sy_call_t *)linux32_sys_getpriority },/* 96 = getpriority */
	{ ns(struct linux32_sys_setpriority_args), 0,
	    (sy_call_t *)linux32_sys_setpriority },/* 97 = setpriority */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 98 = unimplemented profil */
	{ ns(struct linux32_sys_statfs_args), 0,
	    (sy_call_t *)linux32_sys_statfs },	/* 99 = statfs */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 100 = unimplemented fstatfs */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 101 = unimplemented ioperm */
	{ ns(struct linux32_sys_socketcall_args), 0,
	    (sy_call_t *)linux32_sys_socketcall },/* 102 = socketcall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 103 = unimplemented syslog */
	{ ns(struct netbsd32_setitimer_args), 0,
	    (sy_call_t *)netbsd32_setitimer },	/* 104 = netbsd32_setitimer */
	{ ns(struct netbsd32_getitimer_args), 0,
	    (sy_call_t *)netbsd32_getitimer },	/* 105 = netbsd32_getitimer */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 106 = unimplemented stat */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 107 = unimplemented lstat */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 108 = unimplemented fstat */
	{ ns(struct linux32_sys_olduname_args), 0,
	    (sy_call_t *)linux32_sys_olduname },/* 109 = olduname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 110 = unimplemented iopl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 111 = unimplemented vhangup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 112 = unimplemented idle */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 113 = unimplemented vm86old */
	{ ns(struct linux32_sys_wait4_args), 0,
	    (sy_call_t *)linux32_sys_wait4 },	/* 114 = wait4 */
	{ ns(struct linux32_sys_swapoff_args), 0,
	    (sy_call_t *)linux32_sys_swapoff },	/* 115 = swapoff */
	{ ns(struct linux32_sys_sysinfo_args), 0,
	    (sy_call_t *)linux32_sys_sysinfo },	/* 116 = sysinfo */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 117 = unimplemented ipc */
	{ ns(struct netbsd32_fsync_args), 0,
	    (sy_call_t *)netbsd32_fsync },	/* 118 = netbsd32_fsync */
	{ ns(struct linux32_sys_sigreturn_args), 0,
	    (sy_call_t *)linux32_sys_sigreturn },/* 119 = sigreturn */
	{ ns(struct linux32_sys_clone_args), 0,
	    (sy_call_t *)linux32_sys_clone },	/* 120 = clone */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 121 = unimplemented setdomainname */
	{ ns(struct linux32_sys_uname_args), 0,
	    (sy_call_t *)linux32_sys_uname },	/* 122 = uname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 123 = unimplemented modify_ldt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 124 = unimplemented adjtimex */
	{ ns(struct linux32_sys_mprotect_args), 0,
	    (sy_call_t *)linux32_sys_mprotect },/* 125 = mprotect */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 126 = unimplemented sigprocmask */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 127 = unimplemented create_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 128 = unimplemented init_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 129 = unimplemented delete_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 130 = unimplemented get_kernel_syms */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 131 = unimplemented quotactl */
	{ ns(struct netbsd32_getpgid_args), 0,
	    (sy_call_t *)netbsd32_getpgid },	/* 132 = netbsd32_getpgid */
	{ ns(struct netbsd32_fchdir_args), 0,
	    (sy_call_t *)netbsd32_fchdir },	/* 133 = netbsd32_fchdir */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 134 = unimplemented bdflush */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 135 = unimplemented sysfs */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 136 = unimplemented personality */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 137 = unimplemented afs_syscall */
	{ ns(struct linux32_sys_setfsuid_args), 0,
	    (sy_call_t *)linux32_sys_setfsuid },/* 138 = setfsuid16 */
	{ ns(struct linux32_sys_setfsgid_args), 0,
	    (sy_call_t *)linux32_sys_setfsgid },/* 139 = setfsgid16 */
	{ ns(struct linux32_sys_llseek_args), 0,
	    (sy_call_t *)linux32_sys_llseek },	/* 140 = llseek */
	{ ns(struct linux32_sys_getdents_args), 0,
	    (sy_call_t *)linux32_sys_getdents },/* 141 = getdents */
	{ ns(struct linux32_sys_select_args), 0,
	    (sy_call_t *)linux32_sys_select },	/* 142 = select */
	{ ns(struct netbsd32_flock_args), 0,
	    (sy_call_t *)netbsd32_flock },	/* 143 = netbsd32_flock */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 144 = unimplemented msync */
	{ ns(struct netbsd32_readv_args), 0,
	    (sy_call_t *)netbsd32_readv },	/* 145 = netbsd32_readv */
	{ ns(struct netbsd32_writev_args), 0,
	    (sy_call_t *)netbsd32_writev },	/* 146 = netbsd32_writev */
	{ ns(struct netbsd32_getsid_args), 0,
	    (sy_call_t *)netbsd32_getsid },	/* 147 = netbsd32_getsid */
	{ ns(struct linux32_sys_fdatasync_args), 0,
	    (sy_call_t *)linux32_sys_fdatasync },/* 148 = fdatasync */
	{ ns(struct linux32_sys___sysctl_args), 0,
	    (sy_call_t *)linux32_sys___sysctl },/* 149 = __sysctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 150 = unimplemented mlock */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 151 = unimplemented munlock */
	{ ns(struct netbsd32_mlockall_args), 0,
	    (sy_call_t *)netbsd32_mlockall },	/* 152 = netbsd32_mlockall */
	{ 0, 0, 0,
	    (sy_call_t *)sys_munlockall },	/* 153 = munlockall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 154 = unimplemented sched_setparam */
	{ ns(struct linux32_sys_sched_getparam_args), 0,
	    (sy_call_t *)linux32_sys_sched_getparam },/* 155 = sched_getparam */
	{ ns(struct linux32_sys_sched_setscheduler_args), 0,
	    (sy_call_t *)linux32_sys_sched_setscheduler },/* 156 = sched_setscheduler */
	{ ns(struct linux32_sys_sched_getscheduler_args), 0,
	    (sy_call_t *)linux32_sys_sched_getscheduler },/* 157 = sched_getscheduler */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_sched_yield },/* 158 = sched_yield */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 159 = unimplemented sched_get_priority_max */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 160 = unimplemented sched_get_priority_min */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 161 = unimplemented sched_rr_get_interval */
	{ ns(struct netbsd32_nanosleep_args), 0,
	    (sy_call_t *)netbsd32_nanosleep },	/* 162 = netbsd32_nanosleep */
	{ ns(struct linux32_sys_mremap_args), 0,
	    (sy_call_t *)linux32_sys_mremap },	/* 163 = mremap */
	{ ns(struct linux32_sys_setresuid16_args), 0,
	    (sy_call_t *)linux32_sys_setresuid16 },/* 164 = setresuid16 */
	{ ns(struct linux32_sys_getresuid16_args), 0,
	    (sy_call_t *)linux32_sys_getresuid16 },/* 165 = getresuid16 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 166 = unimplemented vm86 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 167 = unimplemented query_module */
	{ ns(struct netbsd32_poll_args), 0,
	    (sy_call_t *)netbsd32_poll },	/* 168 = netbsd32_poll */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 169 = unimplemented nfsservctl */
	{ ns(struct linux32_sys_setresgid16_args), 0,
	    (sy_call_t *)linux32_sys_setresgid16 },/* 170 = setresgid16 */
	{ ns(struct linux32_sys_getresgid16_args), 0,
	    (sy_call_t *)linux32_sys_getresgid16 },/* 171 = getresgid16 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 172 = unimplemented prctl */
	{ ns(struct linux32_sys_rt_sigreturn_args), 0,
	    (sy_call_t *)linux32_sys_rt_sigreturn },/* 173 = rt_sigreturn */
	{ ns(struct linux32_sys_rt_sigaction_args), 0,
	    (sy_call_t *)linux32_sys_rt_sigaction },/* 174 = rt_sigaction */
	{ ns(struct linux32_sys_rt_sigprocmask_args), 0,
	    (sy_call_t *)linux32_sys_rt_sigprocmask },/* 175 = rt_sigprocmask */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 176 = unimplemented rt_sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 177 = unimplemented rt_sigtimedwait */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 178 = unimplemented rt_queueinfo */
	{ ns(struct linux32_sys_rt_sigsuspend_args), 0,
	    (sy_call_t *)linux32_sys_rt_sigsuspend },/* 179 = rt_sigsuspend */
	{ ns(struct linux32_sys_pread_args), 0,
	    (sy_call_t *)linux32_sys_pread },	/* 180 = pread */
	{ ns(struct linux32_sys_pwrite_args), 0,
	    (sy_call_t *)linux32_sys_pwrite },	/* 181 = pwrite */
	{ ns(struct linux32_sys_chown16_args), 0,
	    (sy_call_t *)linux32_sys_chown16 },	/* 182 = chown16 */
	{ ns(struct netbsd32___getcwd_args), 0,
	    (sy_call_t *)netbsd32___getcwd },	/* 183 = netbsd32___getcwd */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 184 = unimplemented capget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 185 = unimplemented capset */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 186 = unimplemented sigaltstack */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 187 = unimplemented sendfile */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 188 = unimplemented getpmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 189 = unimplemented putpmsg */
	{ 0, 0, 0,
	    (sy_call_t *)sys___vfork14 },	/* 190 = __vfork14 */
	{ ns(struct linux32_sys_ugetrlimit_args), 0,
	    (sy_call_t *)linux32_sys_ugetrlimit },/* 191 = ugetrlimit */
	{ ns(struct linux32_sys_mmap2_args), 0,
	    (sy_call_t *)linux32_sys_mmap2 },	/* 192 = mmap2 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 193 = unimplemented truncate64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 194 = unimplemented ftruncate64 */
	{ ns(struct linux32_sys_stat64_args), 0,
	    (sy_call_t *)linux32_sys_stat64 },	/* 195 = stat64 */
	{ ns(struct linux32_sys_lstat64_args), 0,
	    (sy_call_t *)linux32_sys_lstat64 },	/* 196 = lstat64 */
	{ ns(struct linux32_sys_fstat64_args), 0,
	    (sy_call_t *)linux32_sys_fstat64 },	/* 197 = fstat64 */
	{ ns(struct netbsd32___posix_lchown_args), 0,
	    (sy_call_t *)netbsd32___posix_lchown },/* 198 = netbsd32___posix_lchown */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getuid },		/* 199 = getuid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getgid },		/* 200 = getgid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_geteuid },		/* 201 = geteuid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getegid },		/* 202 = getegid */
	{ ns(struct netbsd32_setreuid_args), 0,
	    (sy_call_t *)netbsd32_setreuid },	/* 203 = netbsd32_setreuid */
	{ ns(struct netbsd32_setregid_args), 0,
	    (sy_call_t *)netbsd32_setregid },	/* 204 = netbsd32_setregid */
	{ ns(struct netbsd32_getgroups_args), 0,
	    (sy_call_t *)netbsd32_getgroups },	/* 205 = netbsd32_getgroups */
	{ ns(struct netbsd32_setgroups_args), 0,
	    (sy_call_t *)netbsd32_setgroups },	/* 206 = netbsd32_setgroups */
	{ ns(struct netbsd32___posix_fchown_args), 0,
	    (sy_call_t *)netbsd32___posix_fchown },/* 207 = netbsd32___posix_fchown */
	{ ns(struct linux32_sys_setresuid_args), 0,
	    (sy_call_t *)linux32_sys_setresuid },/* 208 = setresuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 209 = unimplemented getresuid */
	{ ns(struct linux32_sys_setresgid_args), 0,
	    (sy_call_t *)linux32_sys_setresgid },/* 210 = setresgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 211 = unimplemented getresgid */
	{ ns(struct netbsd32___posix_chown_args), 0,
	    (sy_call_t *)netbsd32___posix_chown },/* 212 = netbsd32___posix_chown */
	{ ns(struct netbsd32_setuid_args), 0,
	    (sy_call_t *)netbsd32_setuid },	/* 213 = netbsd32_setuid */
	{ ns(struct netbsd32_setgid_args), 0,
	    (sy_call_t *)netbsd32_setgid },	/* 214 = netbsd32_setgid */
	{ ns(struct linux32_sys_setfsuid_args), 0,
	    (sy_call_t *)linux32_sys_setfsuid },/* 215 = setfsuid */
	{ ns(struct linux32_sys_setfsgid_args), 0,
	    (sy_call_t *)linux32_sys_setfsgid },/* 216 = setfsgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 217 = unimplemented pivot_root */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 218 = unimplemented mincore */
	{ ns(struct netbsd32_madvise_args), 0,
	    (sy_call_t *)netbsd32_madvise },	/* 219 = netbsd32_madvise */
	{ ns(struct linux32_sys_getdents64_args), 0,
	    (sy_call_t *)linux32_sys_getdents64 },/* 220 = getdents64 */
	{ ns(struct linux32_sys_fcntl64_args), 0,
	    (sy_call_t *)linux32_sys_fcntl64 },	/* 221 = fcntl64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 222 = unimplemented / * unused * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 223 = unimplemented / * unused * / */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_gettid },	/* 224 = gettid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 225 = unimplemented readahead */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 226 = unimplemented setxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 227 = unimplemented lsetxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 228 = unimplemented fsetxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 229 = unimplemented getxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 230 = unimplemented lgetxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 231 = unimplemented fgetxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 232 = unimplemented listxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 233 = unimplemented llistxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 234 = unimplemented flistxattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 235 = unimplemented removexattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 236 = unimplemented lremovexattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 237 = unimplemented fremovexattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 238 = unimplemented tkill */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 239 = unimplemented sendfile64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 240 = unimplemented futex */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 241 = unimplemented sched_setaffinity */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 242 = unimplemented sched_getaffinity */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 243 = unimplemented set_thread_area */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 244 = unimplemented get_thread_area */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 245 = unimplemented io_setup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 246 = unimplemented io_destroy */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 247 = unimplemented io_getevents */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 248 = unimplemented io_submit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 249 = unimplemented io_cancel */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 250 = unimplemented fadvise64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 251 = unimplemented / * unused * / */
	{ ns(struct linux32_sys_exit_group_args), 0,
	    (sy_call_t *)linux32_sys_exit_group },/* 252 = exit_group */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 253 = unimplemented lookup_dcookie */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 254 = unimplemented epoll_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 255 = unimplemented epoll_ctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 256 = unimplemented epoll_wait */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 257 = unimplemented remap_file_pages */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 258 = unimplemented set_tid_address */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 259 = unimplemented timer_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 260 = unimplemented timer_settime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 261 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 262 = unimplemented timer_getoverrun */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 263 = unimplemented timer_delete */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 264 = unimplemented clock_settime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 265 = unimplemented clock_gettime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 266 = unimplemented clock_getres */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 267 = unimplemented clock_nanosleep */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 268 = unimplemented statfs64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 269 = unimplemented fstatfs64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 270 = unimplemented tgkill */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 271 = unimplemented utimes */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 272 = unimplemented fadvise64_64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 273 = unimplemented vserver */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 274 = unimplemented mbind */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 275 = unimplemented get_mempolicy */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 276 = unimplemented set_mempolicy */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 277 = unimplemented mq_open */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 278 = unimplemented mq_unlink */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 279 = unimplemented mq_timedsend */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 280 = unimplemented mq_timedreceive */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 281 = unimplemented mq_notify */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 282 = unimplemented mq_getsetattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 283 = unimplemented kexec_load */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 284 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 285 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 286 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 287 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 288 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 289 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 290 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 291 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 292 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 293 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 294 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 295 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 296 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 297 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 298 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 299 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 300 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 301 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 302 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 303 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 304 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 305 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 306 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 307 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 308 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 309 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 310 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 311 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 312 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 313 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 314 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 315 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 316 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 317 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 318 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 319 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 320 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 321 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 322 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 323 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 324 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 325 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 326 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 327 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 328 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 329 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 330 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 331 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 332 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 333 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 334 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 335 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 336 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 337 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 338 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 339 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 340 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 341 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 342 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 343 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 344 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 345 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 346 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 347 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 348 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 349 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 350 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 351 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 352 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 353 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 354 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 355 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 356 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 357 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 358 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 359 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 360 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 361 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 362 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 363 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 364 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 365 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 366 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 367 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 368 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 369 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 370 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 371 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 372 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 373 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 374 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 375 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 376 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 377 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 378 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 379 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 380 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 381 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 382 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 383 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 384 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 385 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 386 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 387 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 388 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 389 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 390 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 391 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 392 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 393 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 394 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 395 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 396 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 397 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 398 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 399 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 400 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 401 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 402 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 403 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 404 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 405 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 406 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 407 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 408 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 409 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 410 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 411 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 412 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 413 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 414 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 415 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 416 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 417 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 418 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 419 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 420 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 421 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 422 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 423 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 424 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 425 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 426 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 427 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 428 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 429 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 430 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 431 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 432 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 433 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 434 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 435 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 436 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 437 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 438 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 439 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 440 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 441 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 442 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 443 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 444 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 445 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 446 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 447 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 448 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 449 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 450 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 451 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 452 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 453 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 454 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 455 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 456 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 457 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 458 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 459 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 460 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 461 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 462 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 463 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 464 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 465 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 466 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 467 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 468 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 469 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 470 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 471 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 472 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 473 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 474 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 475 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 476 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 477 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 478 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 479 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 480 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 481 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 482 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 483 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 484 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 485 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 486 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 487 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 488 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 489 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 490 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 491 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 492 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 493 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 494 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 495 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 496 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 497 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 498 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 499 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 500 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 501 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 502 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 503 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 504 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 505 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 506 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 507 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 508 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 509 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 510 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 511 = filler */
};

