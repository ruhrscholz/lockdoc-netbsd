/*	$NetBSD: socketvar.h,v 1.115 2008/10/11 13:40:58 pooka Exp $	*/

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
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

/*-
 * Copyright (c) 1982, 1986, 1990, 1993
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
 *	@(#)socketvar.h	8.3 (Berkeley) 2/19/95
 */

#ifndef _SYS_SOCKETVAR_H_
#define	_SYS_SOCKETVAR_H_

#include <sys/select.h>
#include <sys/selinfo.h>		/* for struct selinfo */
#include <sys/queue.h>
#include <sys/mutex.h>
#include <sys/condvar.h>
#ifdef ACCEPT_FILTER_MOD
#include <sys/lkm.h>
#endif

#if !defined(_KERNEL)
struct uio;
struct lwp;
struct uidinfo;
#else
#include <sys/uidinfo.h>
#endif

TAILQ_HEAD(soqhead, socket);

/*
 * Variables for socket buffering.
 */
struct sockbuf {
	struct selinfo sb_sel;		/* process selecting read/write */
	struct mowner *sb_mowner;	/* who owns data for this sockbuf */
	struct socket *sb_so;		/* back pointer to socket */
	kcondvar_t sb_cv;		/* notifier */
	/* When re-zeroing this struct, we zero from sb_startzero to the end */
#define	sb_startzero	sb_cc
	u_long	sb_cc;			/* actual chars in buffer */
	u_long	sb_hiwat;		/* max actual char count */
	u_long	sb_mbcnt;		/* chars of mbufs used */
	u_long	sb_mbmax;		/* max chars of mbufs to use */
	long	sb_lowat;		/* low water mark */
	struct mbuf *sb_mb;		/* the mbuf chain */
	struct mbuf *sb_mbtail;		/* the last mbuf in the chain */
	struct mbuf *sb_lastrecord;	/* first mbuf of last record in
					   socket buffer */
	int	sb_flags;		/* flags, see below */
	int	sb_timeo;		/* timeout for read/write */
	u_long	sb_overflowed;		/* # of drops due to full buffer */
};

#ifndef SB_MAX
#define	SB_MAX		(256*1024)	/* default for max chars in sockbuf */
#endif

#define	SB_LOCK		0x01		/* lock on data queue */
#define	SB_NOTIFY	0x04		/* someone is waiting for data/space */
#define	SB_ASYNC	0x10		/* ASYNC I/O, need signals */
#define	SB_UPCALL	0x20		/* someone wants an upcall */
#define	SB_NOINTR	0x40		/* operations not interruptible */
#define	SB_KNOTE	0x100		/* kernel note attached */
#define	SB_AUTOSIZE	0x800		/* automatically size socket buffer */

/*
 * Kernel structure per socket.
 * Contains send and receive buffer queues,
 * handle on protocol and pointer to protocol
 * private data and error information.
 */
struct socket {
	kmutex_t * volatile so_lock;	/* pointer to lock on structure */
	kcondvar_t	so_cv;		/* notifier */
	short		so_type;	/* generic type, see socket.h */
	short		so_options;	/* from socket call, see socket.h */
	u_short		so_linger;	/* time to linger while closing */
	short		so_state;	/* internal state flags SS_*, below */
	int		so_nbio;	/* non-blocking I/O enabled */
	void		*so_pcb;	/* protocol control block */
	const struct protosw *so_proto;	/* protocol handle */
/*
 * Variables for connection queueing.
 * Socket where accepts occur is so_head in all subsidiary sockets.
 * If so_head is 0, socket is not related to an accept.
 * For head socket so_q0 queues partially completed connections,
 * while so_q is a queue of connections ready to be accepted.
 * If a connection is aborted and it has so_head set, then
 * it has to be pulled out of either so_q0 or so_q.
 * We allow connections to queue up based on current queue lengths
 * and limit on number of queued connections for this socket.
 */
	struct socket	*so_head;	/* back pointer to accept socket */
	struct soqhead	*so_onq;	/* queue (q or q0) that we're on */
	struct soqhead	so_q0;		/* queue of partial connections */
	struct soqhead	so_q;		/* queue of incoming connections */
	TAILQ_ENTRY(socket) so_qe;	/* our queue entry (q or q0) */
	short		so_q0len;	/* partials on so_q0 */
	short		so_qlen;	/* number of connections on so_q */
	short		so_qlimit;	/* max number queued connections */
	short		so_timeo;	/* connection timeout */
	u_short		so_error;	/* error affecting connection */
	u_short		so_aborting;	/* references from soabort() */
	pid_t		so_pgid;	/* pgid for signals */
	u_long		so_oobmark;	/* chars to oob mark */
	struct sockbuf	so_snd;		/* send buffer */
	struct sockbuf	so_rcv;		/* receive buffer */

	void		*so_internal;	/* Space for svr4 stream data */
	void		(*so_upcall) (struct socket *, void *, int);
	void *		so_upcallarg;	/* Arg for above */
	int		(*so_send) (struct socket *, struct mbuf *,
					struct uio *, struct mbuf *,
					struct mbuf *, int, struct lwp *);
	int		(*so_receive) (struct socket *,
					struct mbuf **,
					struct uio *, struct mbuf **,
					struct mbuf **, int *);
	struct mowner	*so_mowner;	/* who owns mbufs for this socket */
	struct uidinfo	*so_uidinfo;	/* who opened the socket */
	gid_t		so_egid;	/* creator effective gid */
	pid_t		so_cpid;	/* creator pid */
	struct so_accf {
		struct accept_filter	*so_accept_filter;
		void	*so_accept_filter_arg;	/* saved filter args */
		char	*so_accept_filter_str;	/* saved user args */
	} *so_accf;
};

#define	SB_EMPTY_FIXUP(sb)						\
do {									\
	KASSERT(solocked((sb)->sb_so));					\
	if ((sb)->sb_mb == NULL) {					\
		(sb)->sb_mbtail = NULL;					\
		(sb)->sb_lastrecord = NULL;				\
	}								\
} while (/*CONSTCOND*/0)

/*
 * Socket state bits.
 */
#define	SS_NOFDREF		0x001	/* no file table ref any more */
#define	SS_ISCONNECTED		0x002	/* socket connected to a peer */
#define	SS_ISCONNECTING		0x004	/* in process of connecting to peer */
#define	SS_ISDISCONNECTING	0x008	/* in process of disconnecting */
#define	SS_CANTSENDMORE		0x010	/* can't send more data to peer */
#define	SS_CANTRCVMORE		0x020	/* can't receive more data from peer */
#define	SS_RCVATMARK		0x040	/* at mark on input */
#define	SS_ISDISCONNECTED	0x800	/* socket disconnected from peer */

#define	SS_ASYNC		0x100	/* async i/o notify */
#define	SS_ISCONFIRMING		0x200	/* deciding to accept connection req */
#define	SS_MORETOCOME		0x400	/*
					 * hint from sosend to lower layer;
					 * more data coming
					 */
#define	SS_ISAPIPE 		0x1000	/* socket is implementing a pipe */

#ifdef _KERNEL

struct accept_filter {
	char	accf_name[16];
	void	(*accf_callback)
		(struct socket *so, void *arg, int waitflag);
	void *	(*accf_create)
		(struct socket *so, char *arg);
	void	(*accf_destroy)
		(struct socket *so);
	SLIST_ENTRY(accept_filter) accf_next;
};

struct sockopt {
	int		sopt_level;		/* option level */
	int		sopt_name;		/* option name */
	size_t		sopt_size;		/* data length */
	void *		sopt_data;		/* data pointer */
	uint8_t		sopt_buf[sizeof(int)];	/* internal storage */
};

extern u_long		sb_max;
extern int		somaxkva;
extern int		sock_loan_thresh;
extern kmutex_t		*softnet_lock;

struct mbuf;
struct sockaddr;
struct lwp;
struct msghdr;
struct stat;
struct knote;

struct	mbuf *getsombuf(struct socket *, int);

/*
 * File operations on sockets.
 */
int	soo_read(file_t *, off_t *, struct uio *, kauth_cred_t, int);
int	soo_write(file_t *, off_t *, struct uio *, kauth_cred_t, int);
int	soo_fcntl(file_t *, u_int cmd, void *);
int	soo_ioctl(file_t *, u_long cmd, void *);
int	soo_poll(file_t *, int);
int	soo_kqfilter(file_t *, struct knote *);
int 	soo_close(file_t *);
int	soo_stat(file_t *, struct stat *);
void	sbappend(struct sockbuf *, struct mbuf *);
void	sbappendstream(struct sockbuf *, struct mbuf *);
int	sbappendaddr(struct sockbuf *, const struct sockaddr *, struct mbuf *,
	    struct mbuf *);
int	sbappendaddrchain(struct sockbuf *, const struct sockaddr *,
	     struct mbuf *, int);
int	sbappendcontrol(struct sockbuf *, struct mbuf *, struct mbuf *);
void	sbappendrecord(struct sockbuf *, struct mbuf *);
void	sbcheck(struct sockbuf *);
void	sbcompress(struct sockbuf *, struct mbuf *, struct mbuf *);
struct mbuf *
	sbcreatecontrol(void *, int, int, int);
void	sbdrop(struct sockbuf *, int);
void	sbdroprecord(struct sockbuf *);
void	sbflush(struct sockbuf *);
void	sbinsertoob(struct sockbuf *, struct mbuf *);
void	sbrelease(struct sockbuf *, struct socket *);
int	sbreserve(struct sockbuf *, u_long, struct socket *);
int	sbwait(struct sockbuf *);
int	sb_max_set(u_long);
void	soinit(void);
void	soinit2(void);
int	soabort(struct socket *);
int	soaccept(struct socket *, struct mbuf *);
int	sobind(struct socket *, struct mbuf *, struct lwp *);
void	socantrcvmore(struct socket *);
void	socantsendmore(struct socket *);
int	soclose(struct socket *);
int	soconnect(struct socket *, struct mbuf *, struct lwp *);
int	soconnect2(struct socket *, struct socket *);
int	socreate(int, struct socket **, int, int, struct lwp *,
		 struct socket *);
int	fsocreate(int, struct socket **, int, int, struct lwp *, int *);
int	sodisconnect(struct socket *);
void	sofree(struct socket *);
int	sogetopt(struct socket *, struct sockopt *);
void	sohasoutofband(struct socket *);
void	soisconnected(struct socket *);
void	soisconnecting(struct socket *);
void	soisdisconnected(struct socket *);
void	soisdisconnecting(struct socket *);
int	solisten(struct socket *, int, struct lwp *);
struct socket *
	sonewconn(struct socket *, int);
void	soqinsque(struct socket *, struct socket *, int);
int	soqremque(struct socket *, int);
int	soreceive(struct socket *, struct mbuf **, struct uio *,
	    struct mbuf **, struct mbuf **, int *);
int	soreserve(struct socket *, u_long, u_long);
void	sorflush(struct socket *);
int	sosend(struct socket *, struct mbuf *, struct uio *,
	    struct mbuf *, struct mbuf *, int, struct lwp *);
int	sosetopt(struct socket *, struct sockopt *);
int	so_setsockopt(struct lwp *, struct socket *, int, int, const void *, size_t);
int	soshutdown(struct socket *, int);
void	sowakeup(struct socket *, struct sockbuf *, int);
int	sockargs(struct mbuf **, const void *, size_t, int);
int	sopoll(struct socket *, int);
struct	socket *soget(bool);
void	soput(struct socket *);
bool	solocked(struct socket *);
bool	solocked2(struct socket *, struct socket *);
int	sblock(struct sockbuf *, int);
void	sbunlock(struct sockbuf *);
int	sowait(struct socket *, int);
void	solockretry(struct socket *, kmutex_t *);
void	sosetlock(struct socket *);
void	solockreset(struct socket *, kmutex_t *);

void	sockopt_init(struct sockopt *, int, int, size_t);
void	sockopt_destroy(struct sockopt *);
int	sockopt_set(struct sockopt *, const void *, size_t);
int	sockopt_setint(struct sockopt *, int);
int	sockopt_get(const struct sockopt *, void *, size_t);
int	sockopt_getint(const struct sockopt *, int *);
int	sockopt_setmbuf(struct sockopt *, struct mbuf *);
struct mbuf *sockopt_getmbuf(const struct sockopt *);

int	copyout_sockname(struct sockaddr *, unsigned int *, int, struct mbuf *);
int	copyout_msg_control(struct lwp *, struct msghdr *, struct mbuf *);
void	free_control_mbuf(struct lwp *, struct mbuf *, struct mbuf *);

int	do_sys_getsockname(struct lwp *, int, int, struct mbuf **);
int	do_sys_sendmsg(struct lwp *, int, struct msghdr *, int, register_t *);
int	do_sys_recvmsg(struct lwp *, int, struct msghdr *, struct mbuf **,
	    struct mbuf **, register_t *);

int	do_sys_bind(struct lwp *, int, struct mbuf *);
int	do_sys_connect(struct lwp *, int, struct mbuf *);
int	do_sys_accept(struct lwp *, int, struct mbuf **, register_t *);

/*
 * Inline functions for sockets and socket buffering.
 */

#include <sys/protosw.h>
#include <sys/mbuf.h>

/*
 * Do we need to notify the other side when I/O is possible?
 */
static inline int
sb_notify(struct sockbuf *sb)
{

	KASSERT(solocked(sb->sb_so));

	return sb->sb_flags & (SB_NOTIFY | SB_ASYNC | SB_UPCALL | SB_KNOTE);
}

/*
 * How much space is there in a socket buffer (so->so_snd or so->so_rcv)?
 * This is problematical if the fields are unsigned, as the space might
 * still be negative (cc > hiwat or mbcnt > mbmax).  Should detect
 * overflow and return 0.
 */
static inline long
sbspace(struct sockbuf *sb)
{

	KASSERT(solocked(sb->sb_so));

	return lmin(sb->sb_hiwat - sb->sb_cc, sb->sb_mbmax - sb->sb_mbcnt);
}

/* do we have to send all at once on a socket? */
static inline int
sosendallatonce(struct socket *so)
{

	return so->so_proto->pr_flags & PR_ATOMIC;
}

/* can we read something from so? */
static inline int
soreadable(struct socket *so)
{

	KASSERT(solocked(so));

	return so->so_rcv.sb_cc >= so->so_rcv.sb_lowat ||
	    (so->so_state & SS_CANTRCVMORE) != 0 ||
	    so->so_qlen != 0 || so->so_error != 0;
}

/* can we write something to so? */
static inline int
sowritable(struct socket *so)
{

	KASSERT(solocked(so));

	return (sbspace(&so->so_snd) >= so->so_snd.sb_lowat &&
	    ((so->so_state & SS_ISCONNECTED) != 0 ||
	    (so->so_proto->pr_flags & PR_CONNREQUIRED) == 0)) ||
	    (so->so_state & SS_CANTSENDMORE) != 0 ||
	    so->so_error != 0;
}

/* adjust counters in sb reflecting allocation of m */
static inline void
sballoc(struct sockbuf *sb, struct mbuf *m)
{

	KASSERT(solocked(sb->sb_so));

	sb->sb_cc += m->m_len;
	sb->sb_mbcnt += MSIZE;
	if (m->m_flags & M_EXT)
		sb->sb_mbcnt += m->m_ext.ext_size;
}

/* adjust counters in sb reflecting freeing of m */
static inline void
sbfree(struct sockbuf *sb, struct mbuf *m)
{

	KASSERT(solocked(sb->sb_so));

	sb->sb_cc -= m->m_len;
	sb->sb_mbcnt -= MSIZE;
	if (m->m_flags & M_EXT)
		sb->sb_mbcnt -= m->m_ext.ext_size;
}

static inline void
sorwakeup(struct socket *so)
{

	KASSERT(solocked(so));

	if (sb_notify(&so->so_rcv))
		sowakeup(so, &so->so_rcv, POLL_IN);
}

static inline void
sowwakeup(struct socket *so)
{

	KASSERT(solocked(so));

	if (sb_notify(&so->so_snd))
		sowakeup(so, &so->so_snd, POLL_OUT);
}

static inline void
solock(struct socket *so)
{
	kmutex_t *lock;

	lock = so->so_lock;
	mutex_enter(lock);
	if (__predict_false(lock != so->so_lock))
		solockretry(so, lock);
}
	
static inline void
sounlock(struct socket *so)
{

	mutex_exit(so->so_lock);
}

#ifdef SOCKBUF_DEBUG
/*
 * SBLASTRECORDCHK: check sb->sb_lastrecord is maintained correctly.
 * SBLASTMBUFCHK: check sb->sb_mbtail is maintained correctly.
 *
 * => panic if the socket buffer is inconsistent.
 * => 'where' is used for a panic message.
 */
void	sblastrecordchk(struct sockbuf *, const char *);
#define	SBLASTRECORDCHK(sb, where)	sblastrecordchk((sb), (where))

void	sblastmbufchk(struct sockbuf *, const char *);
#define	SBLASTMBUFCHK(sb, where)	sblastmbufchk((sb), (where))
#define	SBCHECK(sb)			sbcheck(sb)
#else
#define	SBLASTRECORDCHK(sb, where)	/* nothing */
#define	SBLASTMBUFCHK(sb, where)	/* nothing */
#define	SBCHECK(sb)			/* nothing */
#endif /* SOCKBUF_DEBUG */

/* sosend loan */
vaddr_t	sokvaalloc(vsize_t, struct socket *);
void	sokvafree(vaddr_t, vsize_t);
void	soloanfree(struct mbuf *, void *, size_t, void *);

/*
 * Values for socket-buffer-append priority argument to sbappendaddrchain().
 * The following flags are reserved for future implementation:
 *
 *  SB_PRIO_NONE:  honour normal socket-buffer limits.
 *
 *  SB_PRIO_ONESHOT_OVERFLOW:  if the socket has any space,
 *	deliver the entire chain. Intended for large requests
 *      that should be delivered in their entirety, or not at all.
 *
 * SB_PRIO_OVERDRAFT:  allow a small (2*MLEN) overflow, over and
 *	aboce normal socket limits. Intended messages indicating
 *      buffer overflow in earlier normal/lower-priority messages .
 *
 * SB_PRIO_BESTEFFORT: Ignore  limits entirely.  Intended only for
 * 	kernel-generated messages to specially-marked scokets which
 *	require "reliable" delivery, nd where the source socket/protocol
 *	message generator enforce some hard limit (but possibly well
 *	above kern.sbmax). It is entirely up to the in-kernel source to
 *	avoid complete mbuf exhaustion or DoS scenarios.
 */
#define SB_PRIO_NONE 	 	0
#define SB_PRIO_ONESHOT_OVERFLOW 1
#define SB_PRIO_OVERDRAFT	2
#define SB_PRIO_BESTEFFORT	3

/*
 * Accept filter functions (duh).
 */
int	do_getopt_accept_filter(struct socket *, struct sockopt *);
int	do_setopt_accept_filter(struct socket *, const struct sockopt *);
int	accept_filt_add(struct accept_filter *);
int	accept_filt_del(char *);
struct	accept_filter *accept_filt_get(char *);
#ifdef ACCEPT_FILTER_MOD
#ifdef SYSCTL_DECL
SYSCTL_DECL(_net_inet_accf);
#endif
void	accept_filter_init(void);
int	accept_filt_generic_mod_event(struct lkm_table *lkmtp, int event, void *data);
#endif

#endif /* _KERNEL */

#endif /* !_SYS_SOCKETVAR_H_ */
