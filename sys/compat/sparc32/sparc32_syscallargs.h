/*	$NetBSD: sparc32_syscallargs.h,v 1.3 1998/08/26 13:46:57 mrg Exp $	*/

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.2 1998/08/26 13:42:50 mrg Exp 
 */

#define	syscallarg(x)	union { x datum; register_t pad; }

struct compat_sparc32_read_args {
	syscallarg(int) fd;
	syscallarg(sparc32_voidp) buf;
	syscallarg(sparc32_size_t) nbyte;
};

struct compat_sparc32_write_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_voidp) buf;
	syscallarg(sparc32_size_t) nbyte;
};

struct compat_sparc32_open_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) flags;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_wait4_args {
	syscallarg(int) pid;
	syscallarg(sparc32_intp) status;
	syscallarg(int) options;
	syscallarg(sparc32_rusagep_t) rusage;
};

struct compat_43_compat_sparc32_ocreat_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_link_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(const sparc32_charp) link;
};

struct compat_sparc32_unlink_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_sparc32_chdir_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_sparc32_mknod_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
	syscallarg(dev_t) dev;
};

struct compat_sparc32_chmod_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_chown_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct compat_sparc32_break_args {
	syscallarg(sparc32_charp) nsize;
};

struct compat_sparc32_getfsstat_args {
	syscallarg(sparc32_statfsp_t) buf;
	syscallarg(sparc32_long) bufsize;
	syscallarg(int) flags;
};

struct compat_43_compat_sparc32_olseek_args {
	syscallarg(int) fd;
	syscallarg(sparc32_long) offset;
	syscallarg(int) whence;
};

struct compat_sparc32_mount_args {
	syscallarg(const sparc32_charp) type;
	syscallarg(const sparc32_charp) path;
	syscallarg(int) flags;
	syscallarg(sparc32_voidp) data;
};

struct compat_sparc32_unmount_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) flags;
};

struct compat_sparc32_ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(sparc32_caddr_t) addr;
	syscallarg(int) data;
};

struct compat_sparc32_recvmsg_args {
	syscallarg(int) s;
	syscallarg(sparc32_msghdrp_t) msg;
	syscallarg(int) flags;
};

struct compat_sparc32_sendmsg_args {
	syscallarg(int) s;
	syscallarg(const sparc32_msghdrp_t) msg;
	syscallarg(int) flags;
};

struct compat_sparc32_recvfrom_args {
	syscallarg(int) s;
	syscallarg(sparc32_voidp) buf;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) flags;
	syscallarg(sparc32_sockaddrp_t) from;
	syscallarg(sparc32_intp) fromlenaddr;
};

struct compat_sparc32_accept_args {
	syscallarg(int) s;
	syscallarg(sparc32_sockaddrp_t) name;
	syscallarg(sparc32_intp) anamelen;
};

struct compat_sparc32_getpeername_args {
	syscallarg(int) fdes;
	syscallarg(sparc32_sockaddrp_t) asa;
	syscallarg(sparc32_intp) alen;
};

struct compat_sparc32_getsockname_args {
	syscallarg(int) fdes;
	syscallarg(sparc32_sockaddrp_t) asa;
	syscallarg(sparc32_intp) alen;
};

struct compat_sparc32_access_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) flags;
};

struct compat_sparc32_chflags_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_u_long) flags;
};

struct compat_sparc32_fchflags_args {
	syscallarg(int) fd;
	syscallarg(sparc32_u_long) flags;
};

struct compat_43_compat_sparc32_stat43_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_stat43p_t) ub;
};

struct compat_43_compat_sparc32_lstat43_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_stat43p_t) ub;
};

struct compat_sparc32_profil_args {
	syscallarg(sparc32_caddr_t) samples;
	syscallarg(sparc32_size_t) size;
	syscallarg(sparc32_u_long) offset;
	syscallarg(u_int) scale;
};

struct compat_sparc32_ktrace_args {
	syscallarg(const sparc32_charp) fname;
	syscallarg(int) ops;
	syscallarg(int) facs;
	syscallarg(int) pid;
};

struct compat_sparc32_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const sparc32_sigactionp_t) nsa;
	syscallarg(sparc32_sigactionp_t) osa;
};

struct compat_sparc32___getlogin_args {
	syscallarg(sparc32_charp) namebuf;
	syscallarg(u_int) namelen;
};

struct compat_sparc32_setlogin_args {
	syscallarg(const sparc32_charp) namebuf;
};

struct compat_sparc32_acct_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_13_compat_sparc32_sigaltstack13_args {
	syscallarg(const sparc32_sigaltstack13p_t) nss;
	syscallarg(sparc32_sigaltstack13p_t) oss;
};

struct compat_sparc32_ioctl_args {
	syscallarg(int) fd;
	syscallarg(sparc32_u_long) com;
	syscallarg(sparc32_voidp) data;
};

struct compat_sparc32_revoke_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_sparc32_symlink_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(const sparc32_charp) link;
};

struct compat_sparc32_readlink_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_charp) buf;
	syscallarg(sparc32_size_t) count;
};

struct compat_sparc32_execve_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_charpp) argp;
	syscallarg(sparc32_charpp) envp;
};

struct compat_sparc32_chroot_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_43_compat_sparc32_fstat43_args {
	syscallarg(int) fd;
	syscallarg(sparc32_stat43p_t) sb;
};

struct compat_43_compat_sparc32_ogetkerninfo_args {
	syscallarg(int) op;
	syscallarg(sparc32_charp) where;
	syscallarg(sparc32_intp) size;
	syscallarg(int) arg;
};

struct compat_12_compat_sparc32_msync_args {
	syscallarg(sparc32_caddr_t) addr;
	syscallarg(sparc32_size_t) len;
};

struct compat_43_compat_sparc32_ommap_args {
	syscallarg(sparc32_caddr_t) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(sparc32_long) pos;
};

struct compat_sparc32_munmap_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
};

struct compat_sparc32_mprotect_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) prot;
};

struct compat_sparc32_madvise_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) behav;
};

struct compat_sparc32_mincore_args {
	syscallarg(sparc32_caddr_t) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(sparc32_charp) vec;
};

struct compat_sparc32_getgroups_args {
	syscallarg(int) gidsetsize;
	syscallarg(sparc32_gid_tp) gidset;
};

struct compat_sparc32_setgroups_args {
	syscallarg(int) gidsetsize;
	syscallarg(const sparc32_gid_tp) gidset;
};

struct compat_sparc32_setitimer_args {
	syscallarg(int) which;
	syscallarg(const sparc32_itimervalp_t) itv;
	syscallarg(sparc32_itimervalp_t) oitv;
};

struct compat_12_compat_sparc32_oswapon_args {
	syscallarg(const sparc32_charp) name;
};

struct compat_sparc32_getitimer_args {
	syscallarg(int) which;
	syscallarg(sparc32_itimervalp_t) itv;
};

struct compat_43_compat_sparc32_ogethostname_args {
	syscallarg(sparc32_charp) hostname;
	syscallarg(u_int) len;
};

struct compat_43_compat_sparc32_osethostname_args {
	syscallarg(sparc32_charp) hostname;
	syscallarg(u_int) len;
};

struct compat_sparc32_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(sparc32_voidp) arg;
};

struct compat_sparc32_select_args {
	syscallarg(int) nd;
	syscallarg(sparc32_fd_setp_t) in;
	syscallarg(sparc32_fd_setp_t) ou;
	syscallarg(sparc32_fd_setp_t) ex;
	syscallarg(sparc32_timevalp_t) tv;
};

struct compat_sparc32_connect_args {
	syscallarg(int) s;
	syscallarg(const sparc32_sockaddrp_t) name;
	syscallarg(int) namelen;
};

struct compat_43_compat_sparc32_oaccept_args {
	syscallarg(int) s;
	syscallarg(sparc32_caddr_t) name;
	syscallarg(sparc32_intp) anamelen;
};

struct compat_43_compat_sparc32_osend_args {
	syscallarg(int) s;
	syscallarg(sparc32_caddr_t) buf;
	syscallarg(int) len;
	syscallarg(int) flags;
};

struct compat_43_compat_sparc32_orecv_args {
	syscallarg(int) s;
	syscallarg(sparc32_caddr_t) buf;
	syscallarg(int) len;
	syscallarg(int) flags;
};

struct compat_sparc32_sigreturn_args {
	syscallarg(sparc32_sigcontextp_t) sigcntxp;
};

struct compat_sparc32_bind_args {
	syscallarg(int) s;
	syscallarg(const sparc32_sockaddrp_t) name;
	syscallarg(int) namelen;
};

struct compat_sparc32_setsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(const sparc32_voidp) val;
	syscallarg(int) valsize;
};

struct compat_43_compat_sparc32_osigvec_args {
	syscallarg(int) signum;
	syscallarg(sparc32_sigvecp_t) nsv;
	syscallarg(sparc32_sigvecp_t) osv;
};

struct compat_43_compat_sparc32_osigstack_args {
	syscallarg(sparc32_sigstackp_t) nss;
	syscallarg(sparc32_sigstackp_t) oss;
};

struct compat_43_compat_sparc32_orecvmsg_args {
	syscallarg(int) s;
	syscallarg(sparc32_omsghdrp_t) msg;
	syscallarg(int) flags;
};

struct compat_43_compat_sparc32_osendmsg_args {
	syscallarg(int) s;
	syscallarg(sparc32_caddr_t) msg;
	syscallarg(int) flags;
};

struct compat_sparc32_gettimeofday_args {
	syscallarg(sparc32_timevalp_t) tp;
	syscallarg(sparc32_timezonep_t) tzp;
};

struct compat_sparc32_getrusage_args {
	syscallarg(int) who;
	syscallarg(sparc32_rusagep_t) rusage;
};

struct compat_sparc32_getsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(sparc32_voidp) val;
	syscallarg(sparc32_intp) avalsize;
};

struct compat_sparc32_readv_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_iovecp_t) iovp;
	syscallarg(int) iovcnt;
};

struct compat_sparc32_writev_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_iovecp_t) iovp;
	syscallarg(int) iovcnt;
};

struct compat_sparc32_settimeofday_args {
	syscallarg(const sparc32_timevalp_t) tv;
	syscallarg(const sparc32_timezonep_t) tzp;
};

struct compat_43_compat_sparc32_orecvfrom_args {
	syscallarg(int) s;
	syscallarg(sparc32_caddr_t) buf;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) flags;
	syscallarg(sparc32_caddr_t) from;
	syscallarg(sparc32_intp) fromlenaddr;
};

struct compat_sparc32_rename_args {
	syscallarg(const sparc32_charp) from;
	syscallarg(const sparc32_charp) to;
};

struct compat_43_compat_sparc32_otruncate_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_long) length;
};

struct compat_43_compat_sparc32_oftruncate_args {
	syscallarg(int) fd;
	syscallarg(sparc32_long) length;
};

struct compat_sparc32_mkfifo_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_sendto_args {
	syscallarg(int) s;
	syscallarg(const sparc32_voidp) buf;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) flags;
	syscallarg(const sparc32_sockaddrp_t) to;
	syscallarg(int) tolen;
};

struct compat_sparc32_socketpair_args {
	syscallarg(int) domain;
	syscallarg(int) type;
	syscallarg(int) protocol;
	syscallarg(sparc32_intp) rsv;
};

struct compat_sparc32_mkdir_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_rmdir_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_sparc32_utimes_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(const sparc32_timevalp_t) tptr;
};

struct compat_sparc32_adjtime_args {
	syscallarg(const sparc32_timevalp_t) delta;
	syscallarg(sparc32_timevalp_t) olddelta;
};

struct compat_43_compat_sparc32_ogetpeername_args {
	syscallarg(int) fdes;
	syscallarg(sparc32_caddr_t) asa;
	syscallarg(sparc32_intp) alen;
};

struct compat_43_compat_sparc32_ogetrlimit_args {
	syscallarg(int) which;
	syscallarg(sparc32_orlimitp_t) rlp;
};

struct compat_43_compat_sparc32_osetrlimit_args {
	syscallarg(int) which;
	syscallarg(const sparc32_orlimitp_t) rlp;
};

struct compat_sparc32_quotactl_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) cmd;
	syscallarg(int) uid;
	syscallarg(sparc32_caddr_t) arg;
};

struct compat_43_compat_sparc32_ogetsockname_args {
	syscallarg(int) fdec;
	syscallarg(sparc32_caddr_t) asa;
	syscallarg(sparc32_intp) alen;
};

struct compat_sparc32_nfssvc_args {
	syscallarg(int) flag;
	syscallarg(sparc32_voidp) argp;
};

struct compat_43_compat_sparc32_ogetdirentries_args {
	syscallarg(int) fd;
	syscallarg(sparc32_charp) buf;
	syscallarg(u_int) count;
	syscallarg(sparc32_longp) basep;
};

struct compat_sparc32_statfs_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_statfsp_t) buf;
};

struct compat_sparc32_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(sparc32_statfsp_t) buf;
};

struct compat_sparc32_getfh_args {
	syscallarg(const sparc32_charp) fname;
	syscallarg(sparc32_fhandlep_t) fhp;
};

struct compat_09_compat_sparc32_ogetdomainname_args {
	syscallarg(sparc32_charp) domainname;
	syscallarg(int) len;
};

struct compat_09_compat_sparc32_osetdomainname_args {
	syscallarg(sparc32_charp) domainname;
	syscallarg(int) len;
};

struct compat_09_compat_sparc32_uname_args {
	syscallarg(sparc32_outsnamep_t) name;
};

struct compat_sparc32_sysarch_args {
	syscallarg(int) op;
	syscallarg(sparc32_voidp) parms;
};

struct compat_sparc32_pread_args {
	syscallarg(int) fd;
	syscallarg(sparc32_voidp) buf;
	syscallarg(sparc32_size_t) nbyte;
	syscallarg(int) pad;
	syscallarg(off_t) offset;
};

struct compat_sparc32_pwrite_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_voidp) buf;
	syscallarg(sparc32_size_t) nbyte;
	syscallarg(int) pad;
	syscallarg(off_t) offset;
};

struct compat_sparc32_ntp_gettime_args {
	syscallarg(sparc32_ntptimevalp_t) ntvp;
};

struct compat_sparc32_ntp_adjtime_args {
	syscallarg(sparc32_timexp_t) tp;
};

struct compat_sparc32_lfs_bmapv_args {
	syscallarg(sparc32_fsid_tp_t) fsidp;
	syscallarg(sparc32_block_infop_t) blkiov;
	syscallarg(int) blkcnt;
};

struct compat_sparc32_lfs_markv_args {
	syscallarg(sparc32_fsid_tp_t) fsidp;
	syscallarg(sparc32_block_infop_t) blkiov;
	syscallarg(int) blkcnt;
};

struct compat_sparc32_lfs_segclean_args {
	syscallarg(sparc32_fsid_tp_t) fsidp;
	syscallarg(sparc32_u_long) segment;
};

struct compat_sparc32_lfs_segwait_args {
	syscallarg(sparc32_fsid_tp_t) fsidp;
	syscallarg(sparc32_timevalp_t) tv;
};

struct compat_12_compat_sparc32_stat12_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_stat12p_t) ub;
};

struct compat_12_compat_sparc32_fstat12_args {
	syscallarg(int) fd;
	syscallarg(sparc32_stat12p_t) sb;
};

struct compat_12_compat_sparc32_lstat12_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_stat12p_t) ub;
};

struct compat_sparc32_pathconf_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) name;
};

struct compat_sparc32_fpathconf_args {
	syscallarg(int) fd;
	syscallarg(int) name;
};

struct compat_sparc32_getrlimit_args {
	syscallarg(int) which;
	syscallarg(sparc32_rlimitp_t) rlp;
};

struct compat_sparc32_setrlimit_args {
	syscallarg(int) which;
	syscallarg(const sparc32_rlimitp_t) rlp;
};

struct compat_12_compat_sparc32_getdirentries_args {
	syscallarg(int) fd;
	syscallarg(sparc32_charp) buf;
	syscallarg(u_int) count;
	syscallarg(sparc32_longp) basep;
};

struct compat_sparc32_mmap_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(sparc32_long) pad;
	syscallarg(off_t) pos;
};

struct compat_sparc32_truncate_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};

struct compat_sparc32___sysctl_args {
	syscallarg(sparc32_intp) name;
	syscallarg(u_int) namelen;
	syscallarg(sparc32_voidp) old;
	syscallarg(sparc32_size_tp) oldlenp;
	syscallarg(sparc32_voidp) new;
	syscallarg(sparc32_size_t) newlen;
};

struct compat_sparc32_mlock_args {
	syscallarg(const sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
};

struct compat_sparc32_munlock_args {
	syscallarg(const sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
};

struct compat_sparc32_undelete_args {
	syscallarg(const sparc32_charp) path;
};

struct compat_sparc32_futimes_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_timevalp_t) tptr;
};

struct compat_sparc32_reboot_args {
	syscallarg(int) opt;
	syscallarg(sparc32_charp) bootstr;
};

struct compat_sparc32_poll_args {
	syscallarg(sparc32_pollfdp_t) fds;
	syscallarg(u_int) nfds;
	syscallarg(int) timeout;
};

struct compat_sparc32___semctl_args {
	syscallarg(int) semid;
	syscallarg(int) semnum;
	syscallarg(int) cmd;
	syscallarg(sparc32_semunu_t) arg;
};

struct compat_sparc32_semget_args {
	syscallarg(sparc32_key_t) key;
	syscallarg(int) nsems;
	syscallarg(int) semflg;
};

struct compat_sparc32_semop_args {
	syscallarg(int) semid;
	syscallarg(sparc32_sembufp_t) sops;
	syscallarg(sparc32_size_t) nsops;
};

struct compat_sparc32_msgctl_args {
	syscallarg(int) msqid;
	syscallarg(int) cmd;
	syscallarg(sparc32_msqid_dsp_t) buf;
};

struct compat_sparc32_msgget_args {
	syscallarg(sparc32_key_t) key;
	syscallarg(int) msgflg;
};

struct compat_sparc32_msgsnd_args {
	syscallarg(int) msqid;
	syscallarg(const sparc32_voidp) msgp;
	syscallarg(sparc32_size_t) msgsz;
	syscallarg(int) msgflg;
};

struct compat_sparc32_msgrcv_args {
	syscallarg(int) msqid;
	syscallarg(sparc32_voidp) msgp;
	syscallarg(sparc32_size_t) msgsz;
	syscallarg(sparc32_long) msgtyp;
	syscallarg(int) msgflg;
};

struct compat_sparc32_shmat_args {
	syscallarg(int) shmid;
	syscallarg(const sparc32_voidp) shmaddr;
	syscallarg(int) shmflg;
};

struct compat_sparc32_shmctl_args {
	syscallarg(int) shmid;
	syscallarg(int) cmd;
	syscallarg(sparc32_shmid_dsp_t) buf;
};

struct compat_sparc32_shmdt_args {
	syscallarg(const sparc32_voidp) shmaddr;
};

struct compat_sparc32_shmget_args {
	syscallarg(sparc32_key_t) key;
	syscallarg(sparc32_size_t) size;
	syscallarg(int) shmflg;
};

struct compat_sparc32_clock_gettime_args {
	syscallarg(sparc32_clockid_t) clock_id;
	syscallarg(sparc32_timespecp_t) tp;
};

struct compat_sparc32_clock_settime_args {
	syscallarg(sparc32_clockid_t) clock_id;
	syscallarg(const sparc32_timespecp_t) tp;
};

struct compat_sparc32_clock_getres_args {
	syscallarg(sparc32_clockid_t) clock_id;
	syscallarg(sparc32_timespecp_t) tp;
};

struct compat_sparc32_nanosleep_args {
	syscallarg(const sparc32_timespecp_t) rqtp;
	syscallarg(sparc32_timespecp_t) rmtp;
};

struct compat_sparc32___posix_rename_args {
	syscallarg(const sparc32_charp) from;
	syscallarg(const sparc32_charp) to;
};

struct compat_sparc32_swapctl_args {
	syscallarg(int) cmd;
	syscallarg(const sparc32_voidp) arg;
	syscallarg(int) misc;
};

struct compat_sparc32_getdents_args {
	syscallarg(int) fd;
	syscallarg(sparc32_charp) buf;
	syscallarg(sparc32_size_t) count;
};

struct compat_sparc32_minherit_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) inherit;
};

struct compat_sparc32_lchmod_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(mode_t) mode;
};

struct compat_sparc32_lchown_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct compat_sparc32_lutimes_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(const sparc32_timevalp_t) tptr;
};

struct compat_sparc32___msync13_args {
	syscallarg(sparc32_voidp) addr;
	syscallarg(sparc32_size_t) len;
	syscallarg(int) flags;
};

struct compat_sparc32___stat13_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_statp_t) ub;
};

struct compat_sparc32___fstat13_args {
	syscallarg(int) fd;
	syscallarg(sparc32_statp_t) sb;
};

struct compat_sparc32___lstat13_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(sparc32_statp_t) ub;
};

struct compat_sparc32___sigaltstack14_args {
	syscallarg(const sparc32_sigaltstackp_t) nss;
	syscallarg(sparc32_sigaltstackp_t) oss;
};

struct compat_sparc32___posix_chown_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct compat_sparc32___posix_lchown_args {
	syscallarg(const sparc32_charp) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct compat_sparc32_preadv_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_iovecp_t) iovp;
	syscallarg(int) iovcnt;
	syscallarg(int) pad;
	syscallarg(off_t) offset;
};

struct compat_sparc32_pwritev_args {
	syscallarg(int) fd;
	syscallarg(const sparc32_iovecp_t) iovp;
	syscallarg(int) iovcnt;
	syscallarg(int) pad;
	syscallarg(off_t) offset;
};

/*
 * System call prototypes.
 */

int	sys_exit	__P((struct proc *, void *, register_t *));
int	sys_fork	__P((struct proc *, void *, register_t *));
int	compat_sparc32_read	__P((struct proc *, void *, register_t *));
int	compat_sparc32_write	__P((struct proc *, void *, register_t *));
int	compat_sparc32_open	__P((struct proc *, void *, register_t *));
int	sys_close	__P((struct proc *, void *, register_t *));
int	compat_sparc32_wait4	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ocreat	__P((struct proc *, void *, register_t *));
int	compat_sparc32_link	__P((struct proc *, void *, register_t *));
int	compat_sparc32_unlink	__P((struct proc *, void *, register_t *));
int	compat_sparc32_chdir	__P((struct proc *, void *, register_t *));
int	sys_fchdir	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mknod	__P((struct proc *, void *, register_t *));
int	compat_sparc32_chmod	__P((struct proc *, void *, register_t *));
int	compat_sparc32_chown	__P((struct proc *, void *, register_t *));
int	compat_sparc32_break	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getfsstat	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_olseek	__P((struct proc *, void *, register_t *));
int	sys_getpid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mount	__P((struct proc *, void *, register_t *));
int	compat_sparc32_unmount	__P((struct proc *, void *, register_t *));
int	sys_setuid	__P((struct proc *, void *, register_t *));
int	sys_getuid	__P((struct proc *, void *, register_t *));
int	sys_geteuid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_ptrace	__P((struct proc *, void *, register_t *));
int	compat_sparc32_recvmsg	__P((struct proc *, void *, register_t *));
int	compat_sparc32_sendmsg	__P((struct proc *, void *, register_t *));
int	compat_sparc32_recvfrom	__P((struct proc *, void *, register_t *));
int	compat_sparc32_accept	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getpeername	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getsockname	__P((struct proc *, void *, register_t *));
int	compat_sparc32_access	__P((struct proc *, void *, register_t *));
int	compat_sparc32_chflags	__P((struct proc *, void *, register_t *));
int	compat_sparc32_fchflags	__P((struct proc *, void *, register_t *));
int	sys_sync	__P((struct proc *, void *, register_t *));
int	sys_kill	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_stat43	__P((struct proc *, void *, register_t *));
int	sys_getppid	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_lstat43	__P((struct proc *, void *, register_t *));
int	sys_dup	__P((struct proc *, void *, register_t *));
int	sys_pipe	__P((struct proc *, void *, register_t *));
int	sys_getegid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_profil	__P((struct proc *, void *, register_t *));
#ifdef KTRACE
int	compat_sparc32_ktrace	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_sigaction	__P((struct proc *, void *, register_t *));
int	sys_getgid	__P((struct proc *, void *, register_t *));
int	sys_sigprocmask	__P((struct proc *, void *, register_t *));
int	compat_sparc32___getlogin	__P((struct proc *, void *, register_t *));
int	compat_sparc32_setlogin	__P((struct proc *, void *, register_t *));
int	compat_sparc32_acct	__P((struct proc *, void *, register_t *));
int	sys_sigpending	__P((struct proc *, void *, register_t *));
int	compat_13_compat_sparc32_sigaltstack13	__P((struct proc *, void *, register_t *));
int	compat_sparc32_ioctl	__P((struct proc *, void *, register_t *));
#ifdef COMPAT_12
int	compat_12_sys_reboot	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_revoke	__P((struct proc *, void *, register_t *));
int	compat_sparc32_symlink	__P((struct proc *, void *, register_t *));
int	compat_sparc32_readlink	__P((struct proc *, void *, register_t *));
int	compat_sparc32_execve	__P((struct proc *, void *, register_t *));
int	sys_umask	__P((struct proc *, void *, register_t *));
int	compat_sparc32_chroot	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_fstat43	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ogetkerninfo	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getpagesize	__P((struct proc *, void *, register_t *));
int	compat_12_compat_sparc32_msync	__P((struct proc *, void *, register_t *));
int	sys_vfork	__P((struct proc *, void *, register_t *));
int	sys_sbrk	__P((struct proc *, void *, register_t *));
int	sys_sstk	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ommap	__P((struct proc *, void *, register_t *));
int	sys_ovadvise	__P((struct proc *, void *, register_t *));
int	compat_sparc32_munmap	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mprotect	__P((struct proc *, void *, register_t *));
int	compat_sparc32_madvise	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mincore	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getgroups	__P((struct proc *, void *, register_t *));
int	compat_sparc32_setgroups	__P((struct proc *, void *, register_t *));
int	sys_getpgrp	__P((struct proc *, void *, register_t *));
int	sys_setpgid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_setitimer	__P((struct proc *, void *, register_t *));
int	compat_43_sys_wait	__P((struct proc *, void *, register_t *));
int	compat_12_compat_sparc32_oswapon	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getitimer	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ogethostname	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osethostname	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getdtablesize	__P((struct proc *, void *, register_t *));
int	sys_dup2	__P((struct proc *, void *, register_t *));
int	compat_sparc32_fcntl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_select	__P((struct proc *, void *, register_t *));
int	sys_fsync	__P((struct proc *, void *, register_t *));
int	sys_setpriority	__P((struct proc *, void *, register_t *));
int	sys_socket	__P((struct proc *, void *, register_t *));
int	compat_sparc32_connect	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_oaccept	__P((struct proc *, void *, register_t *));
int	sys_getpriority	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osend	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_orecv	__P((struct proc *, void *, register_t *));
int	compat_sparc32_sigreturn	__P((struct proc *, void *, register_t *));
int	compat_sparc32_bind	__P((struct proc *, void *, register_t *));
int	compat_sparc32_setsockopt	__P((struct proc *, void *, register_t *));
int	sys_listen	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osigvec	__P((struct proc *, void *, register_t *));
#ifdef COMPAT_43
int	compat_43_sys_sigblock	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigsetmask	__P((struct proc *, void *, register_t *));
#else
#endif
int	sys_sigsuspend	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osigstack	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_orecvmsg	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osendmsg	__P((struct proc *, void *, register_t *));
#ifdef TRACE
int	sys_vtrace	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_gettimeofday	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getrusage	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getsockopt	__P((struct proc *, void *, register_t *));
int	compat_sparc32_readv	__P((struct proc *, void *, register_t *));
int	compat_sparc32_writev	__P((struct proc *, void *, register_t *));
int	compat_sparc32_settimeofday	__P((struct proc *, void *, register_t *));
int	sys_fchown	__P((struct proc *, void *, register_t *));
int	sys_fchmod	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_orecvfrom	__P((struct proc *, void *, register_t *));
int	sys_setreuid	__P((struct proc *, void *, register_t *));
int	sys_setregid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_rename	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_otruncate	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_oftruncate	__P((struct proc *, void *, register_t *));
int	sys_flock	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mkfifo	__P((struct proc *, void *, register_t *));
int	compat_sparc32_sendto	__P((struct proc *, void *, register_t *));
int	sys_shutdown	__P((struct proc *, void *, register_t *));
int	compat_sparc32_socketpair	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mkdir	__P((struct proc *, void *, register_t *));
int	compat_sparc32_rmdir	__P((struct proc *, void *, register_t *));
int	compat_sparc32_utimes	__P((struct proc *, void *, register_t *));
int	compat_sparc32_adjtime	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ogetpeername	__P((struct proc *, void *, register_t *));
int	compat_43_sys_gethostid	__P((struct proc *, void *, register_t *));
#ifdef COMPAT_43
int	compat_43_sys_sethostid	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_43_compat_sparc32_ogetrlimit	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_osetrlimit	__P((struct proc *, void *, register_t *));
#ifdef COMPAT_43
int	compat_43_sys_killpg	__P((struct proc *, void *, register_t *));
#else
#endif
int	sys_setsid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_quotactl	__P((struct proc *, void *, register_t *));
int	compat_43_sys_quota	__P((struct proc *, void *, register_t *));
int	compat_43_compat_sparc32_ogetsockname	__P((struct proc *, void *, register_t *));
#if defined(NFS) || defined(NFSSERVER)
int	compat_sparc32_nfssvc	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_43_compat_sparc32_ogetdirentries	__P((struct proc *, void *, register_t *));
int	compat_sparc32_statfs	__P((struct proc *, void *, register_t *));
int	compat_sparc32_fstatfs	__P((struct proc *, void *, register_t *));
#if defined(NFS) || defined(NFSSERVER)
int	compat_sparc32_getfh	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_09_compat_sparc32_ogetdomainname	__P((struct proc *, void *, register_t *));
int	compat_09_compat_sparc32_osetdomainname	__P((struct proc *, void *, register_t *));
int	compat_09_compat_sparc32_uname	__P((struct proc *, void *, register_t *));
int	compat_sparc32_sysarch	__P((struct proc *, void *, register_t *));
#if defined(SYSVSEM) && !defined(alpha)
int	compat_10_sys_semsys	__P((struct proc *, void *, register_t *));
#else
#endif
#if defined(SYSVMSG) && !defined(alpha)
int	compat_10_sys_msgsys	__P((struct proc *, void *, register_t *));
#else
#endif
#if defined(SYSVSHM) && !defined(alpha)
int	compat_10_sys_shmsys	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_pread	__P((struct proc *, void *, register_t *));
int	compat_sparc32_pwrite	__P((struct proc *, void *, register_t *));
int	compat_sparc32_ntp_gettime	__P((struct proc *, void *, register_t *));
int	compat_sparc32_ntp_adjtime	__P((struct proc *, void *, register_t *));
int	sys_setgid	__P((struct proc *, void *, register_t *));
int	sys_setegid	__P((struct proc *, void *, register_t *));
int	sys_seteuid	__P((struct proc *, void *, register_t *));
#ifdef LFS
int	compat_sparc32_lfs_bmapv	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lfs_markv	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lfs_segclean	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lfs_segwait	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_12_compat_sparc32_stat12	__P((struct proc *, void *, register_t *));
int	compat_12_compat_sparc32_fstat12	__P((struct proc *, void *, register_t *));
int	compat_12_compat_sparc32_lstat12	__P((struct proc *, void *, register_t *));
int	compat_sparc32_pathconf	__P((struct proc *, void *, register_t *));
int	compat_sparc32_fpathconf	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getrlimit	__P((struct proc *, void *, register_t *));
int	compat_sparc32_setrlimit	__P((struct proc *, void *, register_t *));
int	compat_12_compat_sparc32_getdirentries	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mmap	__P((struct proc *, void *, register_t *));
int	sys_lseek	__P((struct proc *, void *, register_t *));
int	compat_sparc32_truncate	__P((struct proc *, void *, register_t *));
int	sys_ftruncate	__P((struct proc *, void *, register_t *));
int	compat_sparc32___sysctl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_mlock	__P((struct proc *, void *, register_t *));
int	compat_sparc32_munlock	__P((struct proc *, void *, register_t *));
int	compat_sparc32_undelete	__P((struct proc *, void *, register_t *));
int	compat_sparc32_futimes	__P((struct proc *, void *, register_t *));
int	sys_getpgid	__P((struct proc *, void *, register_t *));
int	compat_sparc32_reboot	__P((struct proc *, void *, register_t *));
int	compat_sparc32_poll	__P((struct proc *, void *, register_t *));
#ifdef LKM
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
int	sys_lkmnosys	__P((struct proc *, void *, register_t *));
#else	/* !LKM */
#endif	/* !LKM */
#ifdef SYSVSEM
int	compat_sparc32___semctl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_semget	__P((struct proc *, void *, register_t *));
int	compat_sparc32_semop	__P((struct proc *, void *, register_t *));
int	sys_semconfig	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVMSG
int	compat_sparc32_msgctl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_msgget	__P((struct proc *, void *, register_t *));
int	compat_sparc32_msgsnd	__P((struct proc *, void *, register_t *));
int	compat_sparc32_msgrcv	__P((struct proc *, void *, register_t *));
#else
#endif
#ifdef SYSVSHM
int	compat_sparc32_shmat	__P((struct proc *, void *, register_t *));
int	compat_sparc32_shmctl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_shmdt	__P((struct proc *, void *, register_t *));
int	compat_sparc32_shmget	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_clock_gettime	__P((struct proc *, void *, register_t *));
int	compat_sparc32_clock_settime	__P((struct proc *, void *, register_t *));
int	compat_sparc32_clock_getres	__P((struct proc *, void *, register_t *));
int	compat_sparc32_nanosleep	__P((struct proc *, void *, register_t *));
int	sys_fdatasync	__P((struct proc *, void *, register_t *));
int	compat_sparc32___posix_rename	__P((struct proc *, void *, register_t *));
int	compat_sparc32_swapctl	__P((struct proc *, void *, register_t *));
int	compat_sparc32_getdents	__P((struct proc *, void *, register_t *));
int	compat_sparc32_minherit	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lchmod	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lchown	__P((struct proc *, void *, register_t *));
int	compat_sparc32_lutimes	__P((struct proc *, void *, register_t *));
int	compat_sparc32___msync13	__P((struct proc *, void *, register_t *));
int	compat_sparc32___stat13	__P((struct proc *, void *, register_t *));
int	compat_sparc32___fstat13	__P((struct proc *, void *, register_t *));
int	compat_sparc32___lstat13	__P((struct proc *, void *, register_t *));
int	compat_sparc32___sigaltstack14	__P((struct proc *, void *, register_t *));
int	sys___vfork14	__P((struct proc *, void *, register_t *));
int	compat_sparc32___posix_chown	__P((struct proc *, void *, register_t *));
int	sys___posix_fchown	__P((struct proc *, void *, register_t *));
int	compat_sparc32___posix_lchown	__P((struct proc *, void *, register_t *));
int	sys_getsid	__P((struct proc *, void *, register_t *));
#ifdef KTRACE
int	sys_fktrace	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_sparc32_preadv	__P((struct proc *, void *, register_t *));
int	compat_sparc32_pwritev	__P((struct proc *, void *, register_t *));
