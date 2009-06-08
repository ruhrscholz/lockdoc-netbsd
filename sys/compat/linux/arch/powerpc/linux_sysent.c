/* $NetBSD: linux_sysent.c,v 1.48 2009/06/08 13:29:01 njoly Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.43 2009/06/08 13:26:57 njoly Exp  
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_sysent.c,v 1.48 2009/06/08 13:29:01 njoly Exp $");

#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)
#define	n(type)	(sizeof(type)/sizeof (register_t))
#define	ns(type)	n(type), s(type)

struct sysent linux_sysent[] = {
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_nosys },	/* 0 = syscall */
	{ ns(struct sys_exit_args), 0,
	    (sy_call_t *)sys_exit },		/* 1 = exit */
	{ 0, 0, 0,
	    (sy_call_t *)sys_fork },		/* 2 = fork */
	{ ns(struct sys_read_args), 0,
	    (sy_call_t *)sys_read },		/* 3 = read */
	{ ns(struct sys_write_args), 0,
	    (sy_call_t *)sys_write },		/* 4 = write */
	{ ns(struct linux_sys_open_args), 0,
	    (sy_call_t *)linux_sys_open },	/* 5 = open */
	{ ns(struct sys_close_args), 0,
	    (sy_call_t *)sys_close },		/* 6 = close */
	{ ns(struct linux_sys_waitpid_args), 0,
	    (sy_call_t *)linux_sys_waitpid },	/* 7 = waitpid */
	{ ns(struct linux_sys_creat_args), 0,
	    (sy_call_t *)linux_sys_creat },	/* 8 = creat */
	{ ns(struct sys_link_args), 0,
	    (sy_call_t *)sys_link },		/* 9 = link */
	{ ns(struct linux_sys_unlink_args), 0,
	    (sy_call_t *)linux_sys_unlink },	/* 10 = unlink */
	{ ns(struct sys_execve_args), 0,
	    (sy_call_t *)sys_execve },		/* 11 = execve */
	{ ns(struct sys_chdir_args), 0,
	    (sy_call_t *)sys_chdir },		/* 12 = chdir */
	{ ns(struct linux_sys_time_args), 0,
	    (sy_call_t *)linux_sys_time },	/* 13 = time */
	{ ns(struct linux_sys_mknod_args), 0,
	    (sy_call_t *)linux_sys_mknod },	/* 14 = mknod */
	{ ns(struct sys_chmod_args), 0,
	    (sy_call_t *)sys_chmod },		/* 15 = chmod */
	{ ns(struct sys___posix_lchown_args), 0,
	    (sy_call_t *)sys___posix_lchown },	/* 16 = __posix_lchown */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 17 = obsolete break */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 18 = obsolete ostat */
	{ ns(struct compat_43_sys_lseek_args), 0,
	    (sy_call_t *)compat_43_sys_lseek },	/* 19 = lseek */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getpid },		/* 20 = getpid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 21 = unimplemented mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 22 = obsolete umount */
	{ ns(struct sys_setuid_args), 0,
	    (sy_call_t *)sys_setuid },		/* 23 = setuid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getuid },		/* 24 = getuid */
	{ ns(struct linux_sys_stime_args), 0,
	    (sy_call_t *)linux_sys_stime },	/* 25 = stime */
	{ ns(struct linux_sys_ptrace_args), 0,
	    (sy_call_t *)linux_sys_ptrace },	/* 26 = ptrace */
	{ ns(struct linux_sys_alarm_args), 0,
	    (sy_call_t *)linux_sys_alarm },	/* 27 = alarm */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 28 = obsolete ofstat */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_pause },	/* 29 = pause */
	{ ns(struct linux_sys_utime_args), 0,
	    (sy_call_t *)linux_sys_utime },	/* 30 = utime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 31 = obsolete stty */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 32 = obsolete gtty */
	{ ns(struct sys_access_args), 0,
	    (sy_call_t *)sys_access },		/* 33 = access */
	{ ns(struct linux_sys_nice_args), 0,
	    (sy_call_t *)linux_sys_nice },	/* 34 = nice */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 35 = obsolete ftime */
	{ 0, 0, 0,
	    (sy_call_t *)sys_sync },		/* 36 = sync */
	{ ns(struct linux_sys_kill_args), 0,
	    (sy_call_t *)linux_sys_kill },	/* 37 = kill */
	{ ns(struct sys___posix_rename_args), 0,
	    (sy_call_t *)sys___posix_rename },	/* 38 = __posix_rename */
	{ ns(struct sys_mkdir_args), 0,
	    (sy_call_t *)sys_mkdir },		/* 39 = mkdir */
	{ ns(struct sys_rmdir_args), 0,
	    (sy_call_t *)sys_rmdir },		/* 40 = rmdir */
	{ ns(struct sys_dup_args), 0,
	    (sy_call_t *)sys_dup },		/* 41 = dup */
	{ ns(struct linux_sys_pipe_args), 0,
	    (sy_call_t *)linux_sys_pipe },	/* 42 = pipe */
	{ ns(struct linux_sys_times_args), 0,
	    (sy_call_t *)linux_sys_times },	/* 43 = times */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 44 = obsolete prof */
	{ ns(struct linux_sys_brk_args), 0,
	    (sy_call_t *)linux_sys_brk },	/* 45 = brk */
	{ ns(struct sys_setgid_args), 0,
	    (sy_call_t *)sys_setgid },		/* 46 = setgid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getgid },		/* 47 = getgid */
	{ ns(struct linux_sys_signal_args), 0,
	    (sy_call_t *)linux_sys_signal },	/* 48 = signal */
	{ 0, 0, 0,
	    (sy_call_t *)sys_geteuid },		/* 49 = geteuid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getegid },		/* 50 = getegid */
	{ ns(struct sys_acct_args), 0,
	    (sy_call_t *)sys_acct },		/* 51 = acct */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 52 = unimplemented umount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 53 = obsolete lock */
	{ ns(struct linux_sys_ioctl_args), 0,
	    (sy_call_t *)linux_sys_ioctl },	/* 54 = ioctl */
	{ ns(struct linux_sys_fcntl_args), 0,
	    (sy_call_t *)linux_sys_fcntl },	/* 55 = fcntl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 56 = obsolete mpx */
	{ ns(struct sys_setpgid_args), 0,
	    (sy_call_t *)sys_setpgid },		/* 57 = setpgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 58 = obsolete ulimit */
	{ ns(struct linux_sys_olduname_args), 0,
	    (sy_call_t *)linux_sys_olduname },	/* 59 = olduname */
	{ ns(struct sys_umask_args), 0,
	    (sy_call_t *)sys_umask },		/* 60 = umask */
	{ ns(struct sys_chroot_args), 0,
	    (sy_call_t *)sys_chroot },		/* 61 = chroot */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 62 = unimplemented ustat */
	{ ns(struct sys_dup2_args), 0,
	    (sy_call_t *)sys_dup2 },		/* 63 = dup2 */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getppid },		/* 64 = getppid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getpgrp },		/* 65 = getpgrp */
	{ 0, 0, 0,
	    (sy_call_t *)sys_setsid },		/* 66 = setsid */
	{ ns(struct linux_sys_sigaction_args), 0,
	    (sy_call_t *)linux_sys_sigaction },	/* 67 = sigaction */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_siggetmask },/* 68 = siggetmask */
	{ ns(struct linux_sys_sigsetmask_args), 0,
	    (sy_call_t *)linux_sys_sigsetmask },/* 69 = sigsetmask */
	{ ns(struct sys_setreuid_args), 0,
	    (sy_call_t *)sys_setreuid },	/* 70 = setreuid */
	{ ns(struct sys_setregid_args), 0,
	    (sy_call_t *)sys_setregid },	/* 71 = setregid */
	{ ns(struct linux_sys_sigsuspend_args), 0,
	    (sy_call_t *)linux_sys_sigsuspend },/* 72 = sigsuspend */
	{ ns(struct linux_sys_sigpending_args), 0,
	    (sy_call_t *)linux_sys_sigpending },/* 73 = sigpending */
	{ ns(struct compat_43_sys_sethostname_args), 0,
	    (sy_call_t *)compat_43_sys_sethostname },/* 74 = sethostname */
	{ ns(struct linux_sys_setrlimit_args), 0,
	    (sy_call_t *)linux_sys_setrlimit },	/* 75 = setrlimit */
	{ ns(struct linux_sys_getrlimit_args), 0,
	    (sy_call_t *)linux_sys_getrlimit },	/* 76 = getrlimit */
	{ ns(struct compat_50_sys_getrusage_args), 0,
	    (sy_call_t *)compat_50_sys_getrusage },/* 77 = getrusage */
	{ ns(struct linux_sys_gettimeofday_args), 0,
	    (sy_call_t *)linux_sys_gettimeofday },/* 78 = gettimeofday */
	{ ns(struct linux_sys_settimeofday_args), 0,
	    (sy_call_t *)linux_sys_settimeofday },/* 79 = settimeofday */
	{ ns(struct sys_getgroups_args), 0,
	    (sy_call_t *)sys_getgroups },	/* 80 = getgroups */
	{ ns(struct sys_setgroups_args), 0,
	    (sy_call_t *)sys_setgroups },	/* 81 = setgroups */
	{ ns(struct linux_sys_select_args), 0,
	    (sy_call_t *)linux_sys_select },	/* 82 = select */
	{ ns(struct sys_symlink_args), 0,
	    (sy_call_t *)sys_symlink },		/* 83 = symlink */
	{ ns(struct compat_43_sys_lstat_args), 0,
	    (sy_call_t *)compat_43_sys_lstat },	/* 84 = oolstat */
	{ ns(struct sys_readlink_args), 0,
	    (sy_call_t *)sys_readlink },	/* 85 = readlink */
#ifdef EXEC_AOUT
	{ ns(struct linux_sys_uselib_args), 0,
	    (sy_call_t *)linux_sys_uselib },	/* 86 = uselib */
#else
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 86 = unimplemented sys_uselib */
#endif
	{ ns(struct linux_sys_swapon_args), 0,
	    (sy_call_t *)linux_sys_swapon },	/* 87 = swapon */
	{ ns(struct linux_sys_reboot_args), 0,
	    (sy_call_t *)linux_sys_reboot },	/* 88 = reboot */
	{ ns(struct linux_sys_readdir_args), 0,
	    (sy_call_t *)linux_sys_readdir },	/* 89 = readdir */
	{ ns(struct linux_sys_mmap_args), 0,
	    (sy_call_t *)linux_sys_mmap },	/* 90 = mmap */
	{ ns(struct sys_munmap_args), 0,
	    (sy_call_t *)sys_munmap },		/* 91 = munmap */
	{ ns(struct compat_43_sys_truncate_args), 0,
	    (sy_call_t *)compat_43_sys_truncate },/* 92 = truncate */
	{ ns(struct compat_43_sys_ftruncate_args), 0,
	    (sy_call_t *)compat_43_sys_ftruncate },/* 93 = ftruncate */
	{ ns(struct sys_fchmod_args), 0,
	    (sy_call_t *)sys_fchmod },		/* 94 = fchmod */
	{ ns(struct sys___posix_fchown_args), 0,
	    (sy_call_t *)sys___posix_fchown },	/* 95 = __posix_fchown */
	{ ns(struct linux_sys_getpriority_args), 0,
	    (sy_call_t *)linux_sys_getpriority },/* 96 = getpriority */
	{ ns(struct sys_setpriority_args), 0,
	    (sy_call_t *)sys_setpriority },	/* 97 = setpriority */
	{ ns(struct sys_profil_args), 0,
	    (sy_call_t *)sys_profil },		/* 98 = profil */
	{ ns(struct linux_sys_statfs_args), 0,
	    (sy_call_t *)linux_sys_statfs },	/* 99 = statfs */
	{ ns(struct linux_sys_fstatfs_args), 0,
	    (sy_call_t *)linux_sys_fstatfs },	/* 100 = fstatfs */
	{ ns(struct linux_sys_ioperm_args), 0,
	    (sy_call_t *)linux_sys_ioperm },	/* 101 = ioperm */
	{ ns(struct linux_sys_socketcall_args), 0,
	    (sy_call_t *)linux_sys_socketcall },/* 102 = socketcall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 103 = unimplemented syslog */
	{ ns(struct compat_50_sys_setitimer_args), 0,
	    (sy_call_t *)compat_50_sys_setitimer },/* 104 = setitimer */
	{ ns(struct compat_50_sys_getitimer_args), 0,
	    (sy_call_t *)compat_50_sys_getitimer },/* 105 = getitimer */
	{ ns(struct linux_sys_stat_args), 0,
	    (sy_call_t *)linux_sys_stat },	/* 106 = stat */
	{ ns(struct linux_sys_lstat_args), 0,
	    (sy_call_t *)linux_sys_lstat },	/* 107 = lstat */
	{ ns(struct linux_sys_fstat_args), 0,
	    (sy_call_t *)linux_sys_fstat },	/* 108 = fstat */
	{ ns(struct linux_sys_uname_args), 0,
	    (sy_call_t *)linux_sys_uname },	/* 109 = uname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 110 = unimplemented iopl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 111 = unimplemented vhangup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 112 = unimplemented idle */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 113 = unimplemented vm86old */
	{ ns(struct linux_sys_wait4_args), 0,
	    (sy_call_t *)linux_sys_wait4 },	/* 114 = wait4 */
	{ ns(struct linux_sys_swapoff_args), 0,
	    (sy_call_t *)linux_sys_swapoff },	/* 115 = swapoff */
	{ ns(struct linux_sys_sysinfo_args), 0,
	    (sy_call_t *)linux_sys_sysinfo },	/* 116 = sysinfo */
	{ ns(struct linux_sys_ipc_args), 0,
	    (sy_call_t *)linux_sys_ipc },	/* 117 = ipc */
	{ ns(struct sys_fsync_args), 0,
	    (sy_call_t *)sys_fsync },		/* 118 = fsync */
	{ ns(struct linux_sys_sigreturn_args), 0,
	    (sy_call_t *)linux_sys_sigreturn },	/* 119 = sigreturn */
	{ ns(struct linux_sys_clone_args), 0,
	    (sy_call_t *)linux_sys_clone },	/* 120 = clone */
	{ ns(struct linux_sys_setdomainname_args), 0,
	    (sy_call_t *)linux_sys_setdomainname },/* 121 = setdomainname */
	{ ns(struct linux_sys_new_uname_args), 0,
	    (sy_call_t *)linux_sys_new_uname },	/* 122 = new_uname */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 123 = unimplemented modify_ldt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 124 = unimplemented adjtimex */
	{ ns(struct linux_sys_mprotect_args), 0,
	    (sy_call_t *)linux_sys_mprotect },	/* 125 = mprotect */
	{ ns(struct linux_sys_sigprocmask_args), 0,
	    (sy_call_t *)linux_sys_sigprocmask },/* 126 = sigprocmask */
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
	{ ns(struct sys_getpgid_args), 0,
	    (sy_call_t *)sys_getpgid },		/* 132 = getpgid */
	{ ns(struct sys_fchdir_args), 0,
	    (sy_call_t *)sys_fchdir },		/* 133 = fchdir */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 134 = unimplemented bdflush */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 135 = unimplemented sysfs */
	{ ns(struct linux_sys_personality_args), 0,
	    (sy_call_t *)linux_sys_personality },/* 136 = personality */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 137 = unimplemented afs_syscall */
	{ ns(struct linux_sys_setfsuid_args), 0,
	    (sy_call_t *)linux_sys_setfsuid },	/* 138 = setfsuid */
	{ ns(struct linux_sys_setfsgid_args), 0,
	    (sy_call_t *)linux_sys_setfsgid },	/* 139 = setfsgid */
	{ ns(struct linux_sys_llseek_args), 0,
	    (sy_call_t *)linux_sys_llseek },	/* 140 = llseek */
	{ ns(struct linux_sys_getdents_args), 0,
	    (sy_call_t *)linux_sys_getdents },	/* 141 = getdents */
	{ ns(struct linux_sys_new_select_args), 0,
	    (sy_call_t *)linux_sys_new_select },/* 142 = new_select */
	{ ns(struct sys_flock_args), 0,
	    (sy_call_t *)sys_flock },		/* 143 = flock */
	{ ns(struct sys___msync13_args), 0,
	    (sy_call_t *)sys___msync13 },	/* 144 = __msync13 */
	{ ns(struct sys_readv_args), 0,
	    (sy_call_t *)sys_readv },		/* 145 = readv */
	{ ns(struct sys_writev_args), 0,
	    (sy_call_t *)sys_writev },		/* 146 = writev */
	{ ns(struct sys_getsid_args), 0,
	    (sy_call_t *)sys_getsid },		/* 147 = getsid */
	{ ns(struct linux_sys_fdatasync_args), 0,
	    (sy_call_t *)linux_sys_fdatasync },	/* 148 = fdatasync */
	{ ns(struct linux_sys___sysctl_args), 0,
	    (sy_call_t *)linux_sys___sysctl },	/* 149 = __sysctl */
	{ ns(struct sys_mlock_args), 0,
	    (sy_call_t *)sys_mlock },		/* 150 = mlock */
	{ ns(struct sys_munlock_args), 0,
	    (sy_call_t *)sys_munlock },		/* 151 = munlock */
	{ ns(struct sys_mlockall_args), 0,
	    (sy_call_t *)sys_mlockall },	/* 152 = mlockall */
	{ 0, 0, 0,
	    (sy_call_t *)sys_munlockall },	/* 153 = munlockall */
	{ ns(struct linux_sys_sched_setparam_args), 0,
	    (sy_call_t *)linux_sys_sched_setparam },/* 154 = sched_setparam */
	{ ns(struct linux_sys_sched_getparam_args), 0,
	    (sy_call_t *)linux_sys_sched_getparam },/* 155 = sched_getparam */
	{ ns(struct linux_sys_sched_setscheduler_args), 0,
	    (sy_call_t *)linux_sys_sched_setscheduler },/* 156 = sched_setscheduler */
	{ ns(struct linux_sys_sched_getscheduler_args), 0,
	    (sy_call_t *)linux_sys_sched_getscheduler },/* 157 = sched_getscheduler */
	{ 0, 0, 0,
	    (sy_call_t *)linux_sys_sched_yield },/* 158 = sched_yield */
	{ ns(struct linux_sys_sched_get_priority_max_args), 0,
	    (sy_call_t *)linux_sys_sched_get_priority_max },/* 159 = sched_get_priority_max */
	{ ns(struct linux_sys_sched_get_priority_min_args), 0,
	    (sy_call_t *)linux_sys_sched_get_priority_min },/* 160 = sched_get_priority_min */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 161 = unimplemented sched_rr_get_interval */
	{ ns(struct linux_sys_nanosleep_args), 0,
	    (sy_call_t *)linux_sys_nanosleep },	/* 162 = nanosleep */
	{ ns(struct linux_sys_mremap_args), 0,
	    (sy_call_t *)linux_sys_mremap },	/* 163 = mremap */
	{ ns(struct linux_sys_setresuid_args), 0,
	    (sy_call_t *)linux_sys_setresuid },	/* 164 = setresuid */
	{ ns(struct linux_sys_getresuid_args), 0,
	    (sy_call_t *)linux_sys_getresuid },	/* 165 = getresuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 166 = unimplemented query_module */
	{ ns(struct sys_poll_args), 0,
	    (sy_call_t *)sys_poll },		/* 167 = poll */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 168 = unimplemented nfsservctl */
	{ ns(struct linux_sys_setresgid_args), 0,
	    (sy_call_t *)linux_sys_setresgid },	/* 169 = setresgid */
	{ ns(struct linux_sys_getresgid_args), 0,
	    (sy_call_t *)linux_sys_getresgid },	/* 170 = getresgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 171 = unimplemented prctl */
	{ ns(struct linux_sys_rt_sigreturn_args), 0,
	    (sy_call_t *)linux_sys_rt_sigreturn },/* 172 = rt_sigreturn */
	{ ns(struct linux_sys_rt_sigaction_args), 0,
	    (sy_call_t *)linux_sys_rt_sigaction },/* 173 = rt_sigaction */
	{ ns(struct linux_sys_rt_sigprocmask_args), 0,
	    (sy_call_t *)linux_sys_rt_sigprocmask },/* 174 = rt_sigprocmask */
	{ ns(struct linux_sys_rt_sigpending_args), 0,
	    (sy_call_t *)linux_sys_rt_sigpending },/* 175 = rt_sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 176 = unimplemented rt_sigtimedwait */
	{ ns(struct linux_sys_rt_queueinfo_args), 0,
	    (sy_call_t *)linux_sys_rt_queueinfo },/* 177 = rt_queueinfo */
	{ ns(struct linux_sys_rt_sigsuspend_args), 0,
	    (sy_call_t *)linux_sys_rt_sigsuspend },/* 178 = rt_sigsuspend */
	{ ns(struct linux_sys_pread_args), 0,
	    (sy_call_t *)linux_sys_pread },	/* 179 = pread */
	{ ns(struct linux_sys_pwrite_args), 0,
	    (sy_call_t *)linux_sys_pwrite },	/* 180 = pwrite */
	{ ns(struct sys___posix_chown_args), 0,
	    (sy_call_t *)sys___posix_chown },	/* 181 = __posix_chown */
	{ ns(struct sys___getcwd_args), 0,
	    (sy_call_t *)sys___getcwd },	/* 182 = __getcwd */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 183 = unimplemented capget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 184 = unimplemented capset */
	{ ns(struct linux_sys_sigaltstack_args), 0,
	    (sy_call_t *)linux_sys_sigaltstack },/* 185 = sigaltstack */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 186 = unimplemented sendfile */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 187 = unimplemented getpmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 188 = unimplemented putpmsg */
	{ 0, 0, 0,
	    (sy_call_t *)sys___vfork14 },	/* 189 = __vfork14 */
	{ ns(struct linux_sys_ugetrlimit_args), 0,
	    (sy_call_t *)linux_sys_ugetrlimit },/* 190 = ugetrlimit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 191 = unimplemented / * unused * / */
#define linux_sys_mmap2_args linux_sys_mmap_args
	{ ns(struct linux_sys_mmap2_args), 0,
	    (sy_call_t *)linux_sys_mmap2 },	/* 192 = mmap2 */
	{ ns(struct linux_sys_truncate64_args), 0,
	    (sy_call_t *)linux_sys_truncate64 },/* 193 = truncate64 */
	{ ns(struct linux_sys_ftruncate64_args), 0,
	    (sy_call_t *)linux_sys_ftruncate64 },/* 194 = ftruncate64 */
	{ ns(struct linux_sys_stat64_args), 0,
	    (sy_call_t *)linux_sys_stat64 },	/* 195 = stat64 */
	{ ns(struct linux_sys_lstat64_args), 0,
	    (sy_call_t *)linux_sys_lstat64 },	/* 196 = lstat64 */
	{ ns(struct linux_sys_fstat64_args), 0,
	    (sy_call_t *)linux_sys_fstat64 },	/* 197 = fstat64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 198 = unimplemented sys_pciconfig_read */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 199 = unimplemented sys_pciconfig_write */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 200 = unimplemented sys_pciconfig_iobase */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 201 = unimplemented / * Unused ( MacOnLinux project ) * / */
	{ ns(struct linux_sys_getdents64_args), 0,
	    (sy_call_t *)linux_sys_getdents64 },/* 202 = getdents64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 203 = unimplemented pivot_root */
	{ ns(struct linux_sys_fcntl64_args), 0,
	    (sy_call_t *)linux_sys_fcntl64 },	/* 204 = fcntl64 */
	{ ns(struct sys_mincore_args), 0,
	    (sy_call_t *)sys_mincore },		/* 205 = mincore */
	{ ns(struct sys_madvise_args), 0,
	    (sy_call_t *)sys_madvise },		/* 206 = madvise */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 207 = unimplemented gettid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 208 = unimplemented tkill */
	{ ns(struct linux_sys_setxattr_args), 0,
	    (sy_call_t *)linux_sys_setxattr },	/* 209 = setxattr */
	{ ns(struct linux_sys_lsetxattr_args), 0,
	    (sy_call_t *)linux_sys_lsetxattr },	/* 210 = lsetxattr */
	{ ns(struct linux_sys_fsetxattr_args), 0,
	    (sy_call_t *)linux_sys_fsetxattr },	/* 211 = fsetxattr */
	{ ns(struct linux_sys_getxattr_args), 0,
	    (sy_call_t *)linux_sys_getxattr },	/* 212 = getxattr */
	{ ns(struct linux_sys_lgetxattr_args), 0,
	    (sy_call_t *)linux_sys_lgetxattr },	/* 213 = lgetxattr */
	{ ns(struct linux_sys_fgetxattr_args), 0,
	    (sy_call_t *)linux_sys_fgetxattr },	/* 214 = fgetxattr */
	{ ns(struct linux_sys_listxattr_args), 0,
	    (sy_call_t *)linux_sys_listxattr },	/* 215 = listxattr */
	{ ns(struct linux_sys_llistxattr_args), 0,
	    (sy_call_t *)linux_sys_llistxattr },/* 216 = llistxattr */
	{ ns(struct linux_sys_flistxattr_args), 0,
	    (sy_call_t *)linux_sys_flistxattr },/* 217 = flistxattr */
	{ ns(struct linux_sys_removexattr_args), 0,
	    (sy_call_t *)linux_sys_removexattr },/* 218 = removexattr */
	{ ns(struct linux_sys_lremovexattr_args), 0,
	    (sy_call_t *)linux_sys_lremovexattr },/* 219 = lremovexattr */
	{ ns(struct linux_sys_fremovexattr_args), 0,
	    (sy_call_t *)linux_sys_fremovexattr },/* 220 = fremovexattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 221 = unimplemented futex */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 222 = unimplemented sched_setaffinity */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 223 = unimplemented sched_getaffinity */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 224 = unimplemented / * unused * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 225 = unimplemented tuxcall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 226 = unimplemented sendfile64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 227 = unimplemented io_setup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 228 = unimplemented io_destroy */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 229 = unimplemented io_getevents */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 230 = unimplemented io_submit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 231 = unimplemented io_cancel */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 232 = unimplemented set_tid_address */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 233 = unimplemented fadvise64 */
	{ ns(struct linux_sys_exit_group_args), 0,
	    (sy_call_t *)linux_sys_exit_group },/* 234 = exit_group */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 235 = unimplemented lookup_dcookie */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 236 = unimplemented epoll_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 237 = unimplemented epoll_ctl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 238 = unimplemented epoll_wait */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 239 = unimplemented remap_file_pages */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 240 = unimplemented timer_create */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 241 = unimplemented timer_settime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 242 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 243 = unimplemented timer_getoverrun */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 244 = unimplemented timer_delete */
	{ ns(struct linux_sys_clock_settime_args), 0,
	    (sy_call_t *)linux_sys_clock_settime },/* 245 = clock_settime */
	{ ns(struct linux_sys_clock_gettime_args), 0,
	    (sy_call_t *)linux_sys_clock_gettime },/* 246 = clock_gettime */
	{ ns(struct linux_sys_clock_getres_args), 0,
	    (sy_call_t *)linux_sys_clock_getres },/* 247 = clock_getres */
	{ ns(struct linux_sys_clock_nanosleep_args), 0,
	    (sy_call_t *)linux_sys_clock_nanosleep },/* 248 = clock_nanosleep */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 249 = unimplemented swapcontext */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 250 = unimplemented tgkill */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 251 = unimplemented utimes */
	{ ns(struct linux_sys_statfs64_args), 0,
	    (sy_call_t *)linux_sys_statfs64 },	/* 252 = statfs64 */
	{ ns(struct linux_sys_fstatfs64_args), 0,
	    (sy_call_t *)linux_sys_fstatfs64 },	/* 253 = fstatfs64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 254 = unimplemented fadvise64_64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 255 = unimplemented rtas */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 256 = unimplemented / * reserved for sys_debug_setcontext * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 257 = unimplemented / * reserved for vserver * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 258 = unimplemented / * reserved for new sys_remap_file_pages * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 259 = unimplemented / * reserved for new sys_mbind * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 260 = unimplemented / * reserved for new sys_get_mempolicy * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 261 = unimplemented / * reserved for new sys_set_mempolicy * / */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 262 = unimplemented mq_open */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 263 = unimplemented mq_unlink */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 264 = unimplemented mq_timedsend */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 265 = unimplemented mq_timedreceive */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 266 = unimplemented mq_notify */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 267 = unimplemented mq_getsetattr */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 268 = unimplemented kexec_load */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 269 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 270 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 271 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 272 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 273 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 274 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 275 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 276 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 277 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 278 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 279 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 280 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 281 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 282 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 283 = filler */
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
