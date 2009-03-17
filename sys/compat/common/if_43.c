/*	$NetBSD: if_43.c,v 1.2 2009/03/17 00:08:10 dyoung Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1989, 1990, 1993
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
 *	@(#)uipc_syscalls.c	8.4 (Berkeley) 2/21/94
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: if_43.c,v 1.2 2009/03/17 00:08:10 dyoung Exp $");

#if defined(_KERNEL_OPT)
#include "opt_compat_netbsd.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/filedesc.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/malloc.h>
#include <sys/syslog.h>
#include <sys/unistd.h>
#include <sys/resourcevar.h>
#include <sys/mbuf.h>		/* for MLEN */
#include <sys/protosw.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

#include <net/if.h>
#include <net/bpf.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <net/if_gre.h>
#include <net/if_atm.h>
#include <net/if_tap.h>
#include <net80211/ieee80211_ioctl.h>
#include <netinet6/in6_var.h>
#include <netinet6/nd6.h>
#include <compat/sys/socket.h>
#include <compat/sys/sockio.h>

#include <compat/common/compat_util.h>

#include <uvm/uvm_extern.h>

u_long 
compat_cvtcmd(u_long cmd)
{ 
	u_long ncmd;

	if (IOCPARM_LEN(cmd) != sizeof(struct oifreq))
		return cmd;

	ncmd = ((cmd) & ~(IOCPARM_MASK << IOCPARM_SHIFT)) | 
		(sizeof(struct ifreq) << IOCPARM_SHIFT);

	switch (ncmd) {
	case BIOCGETIF:
	case BIOCSETIF:
	case GREDSOCK:
	case GREGADDRD:
	case GREGADDRS:
	case GREGPROTO:
	case GRESADDRD:
	case GRESADDRS:
	case GRESPROTO:
	case GRESSOCK:
#ifdef COMPAT_20
	case OSIOCG80211STATS:
	case OSIOCG80211ZSTATS:
#endif /* COMPAT_20 */
	case SIOCADDMULTI:
	case SIOCDELMULTI:
	case SIOCDIFADDR:
	case SIOCDIFADDR_IN6:
	case SIOCDIFPHYADDR:
	case SIOCGDEFIFACE_IN6:
	case SIOCG80211NWID:
	case SIOCG80211STATS:
	case SIOCG80211ZSTATS:
	case SIOCGIFADDR:
	case SIOCGIFADDR_IN6:
	case SIOCGIFAFLAG_IN6:
	case SIOCGIFALIFETIME_IN6:
	case SIOCGIFBRDADDR:
	case SIOCGIFDLT:
	case SIOCGIFDSTADDR:
	case SIOCGIFDSTADDR_IN6:
	case SIOCGIFFLAGS:
	case SIOCGIFGENERIC:
	case SIOCGIFMETRIC:
	case SIOCGIFMTU:
	case SIOCGIFNETMASK:
	case SIOCGIFNETMASK_IN6:
	case SIOCGIFPDSTADDR:
	case SIOCGIFPDSTADDR_IN6:
	case SIOCGIFPSRCADDR:
	case SIOCGIFPSRCADDR_IN6:
	case SIOCGIFSTAT_ICMP6:
	case SIOCGIFSTAT_IN6:
	case SIOCGPVCSIF:
	case SIOCGVH:
	case SIOCIFCREATE:
	case SIOCIFDESTROY:
	case SIOCS80211NWID:
	case SIOCSDEFIFACE_IN6:
	case SIOCSIFADDR:
	case SIOCSIFADDR_IN6:
	case SIOCSIFBRDADDR:
	case SIOCSIFDSTADDR:
	case SIOCSIFDSTADDR_IN6:
	case SIOCSIFFLAGS:
	case SIOCSIFGENERIC:
	case SIOCSIFMEDIA:
	case SIOCSIFMETRIC:
	case SIOCSIFMTU:
	case SIOCSIFNETMASK:
	case SIOCSIFNETMASK_IN6:
	case SIOCSNDFLUSH_IN6:
	case SIOCSPFXFLUSH_IN6:
	case SIOCSPVCSIF:
	case SIOCSRTRFLUSH_IN6:
	case SIOCSVH:
	case TAPGIFNAME:
		return ncmd;
	}
	return cmd;
}

int
compat_ifioctl(struct socket *so, u_long ocmd, u_long cmd, void *data,
    struct lwp *l)
{
	int error;
	struct ifreq *ifr = data;
	struct ifnet *ifp = ifunit(ifr->ifr_name);
	struct sockaddr *sa;

	if (ifp == NULL)
		return ENXIO;

	switch (ocmd) {
	case OSIOCSIFADDR:
	case OSIOCSIFDSTADDR:
	case OSIOCSIFBRDADDR:
	case OSIOCSIFNETMASK:
		sa = &ifr->ifr_addr;
#if BYTE_ORDER != BIG_ENDIAN
		if (sa->sa_family == 0 && sa->sa_len < 16) {
			sa->sa_family = sa->sa_len;
			sa->sa_len = 16;
		}
#else
		if (sa->sa_len == 0)
			sa->sa_len = 16;
#endif
		break;

	case OOSIOCGIFADDR:
		cmd = SIOCGIFADDR;
		break;

	case OOSIOCGIFDSTADDR:
		cmd = SIOCGIFDSTADDR;
		break;

	case OOSIOCGIFBRDADDR:
		cmd = SIOCGIFBRDADDR;
		break;

	case OOSIOCGIFNETMASK:
		cmd = SIOCGIFNETMASK;
	}

	error = (*so->so_proto->pr_usrreq)(so, PRU_CONTROL,
	    (struct mbuf *)cmd, (struct mbuf *)ifr, (struct mbuf *)ifp, l);

	switch (ocmd) {
	case OOSIOCGIFADDR:
	case OOSIOCGIFDSTADDR:
	case OOSIOCGIFBRDADDR:
	case OOSIOCGIFNETMASK:
		*(u_int16_t *)&ifr->ifr_addr = 
		    ((struct sockaddr *)&ifr->ifr_addr)->sa_family;
	}
	return error;
}
