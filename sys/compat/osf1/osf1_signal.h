/*	$NetBSD: osf1_signal.h,v 1.3 1999/04/26 01:22:18 cgd Exp $	*/

#ifndef _OSF1_SIGNAL_H
#define _OSF1_SIGNAL_H

#define OSF1_SIGHUP	 1
#define OSF1_SIGINT	 2
#define OSF1_SIGQUIT	 3
#define OSF1_SIGILL	 4
#define OSF1_SIGTRAP	 5
#define OSF1_SIGABRT	 6
#define OSF1_SIGEMT	 7
#define OSF1_SIGFPE	 8
#define OSF1_SIGKILL	 9
#define OSF1_SIGBUS	10
#define OSF1_SIGSEGV	11
#define OSF1_SIGSYS	12
#define OSF1_SIGPIPE	13
#define OSF1_SIGALRM	14
#define OSF1_SIGTERM	15
#define OSF1_SIGURG	16
#define OSF1_SIGSTOP	17
#define OSF1_SIGTSTP	18
#define OSF1_SIGCONT	19
#define OSF1_SIGCHLD	20
#define OSF1_SIGTTIN	21
#define OSF1_SIGTTOU	22
#define OSF1_SIGIO	23
#define OSF1_SIGXCPU	24
#define OSF1_SIGXFSZ	25
#define OSF1_SIGVTALRM	26
#define OSF1_SIGPROF	27
#define OSF1_SIGWINCH	28
#define OSF1_SIGINFO	29
#define OSF1_SIGUSR1	30
#define OSF1_SIGUSR2	31
#define OSF1_NSIG	32

#define	OSF1_SIG_DFL		(void(*)())0
#define	OSF1_SIG_ERR		(void(*)())-1
#define	OSF1_SIG_IGN		(void(*)())1
#define	OSF1_SIG_HOLD		(void(*)())2

#define OSF1_SIG_BLOCK		1
#define OSF1_SIG_UNBLOCK	2
#define OSF1_SIG_SETMASK	3

extern int osf1_to_linux_sig[];
void bsd_to_osf1_sigaltstack __P((const struct sigaltstack *, struct osf1_sigaltstack *));
void bsd_to_osf1_sigset __P((const sigset_t *, osf1_sigset_t *));
void osf1_to_bsd_sigaltstack __P((const struct osf1_sigaltstack *, struct sigaltstack *));
void osf1_to_bsd_sigset __P((const osf1_sigset_t *, sigset_t *));

#endif /* !_OSF1_SIGNAL_H */
