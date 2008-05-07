/*	$NetBSD: tunnel.c,v 1.12 2008/05/07 20:03:27 dyoung Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *      The Regents of the University of California.  All rights reserved.
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
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: tunnel.c,v 1.12 2008/05/07 20:03:27 dyoung Exp $");
#endif /* not lint */

#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h>

#include <net/if.h> 

#ifdef INET6
#include <netinet/in.h>
#endif

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util.h>

#include "parse.h"
#include "env.h"
#include "util.h"
#include "tunnel.h"

#ifdef INET6
#include "af_inet6.h"
#endif

struct paddr tundst = PADDR_INITIALIZER(&tundst, "tundst", settunnel,
    "tundst", NULL, NULL, NULL, &command_root.pb_parser);

struct paddr tunsrc = PADDR_INITIALIZER(&tunsrc, "tunsrc", NULL,
    "tunsrc", NULL, NULL, NULL, &tundst.pa_parser);

static const struct kwinst tunnelkw[] = {
	  {.k_word = "deletetunnel", .k_exec = deletetunnel,
	   .k_nextparser = &command_root.pb_parser}
	, {.k_word = "tunnel", .k_nextparser = &tunsrc.pa_parser}
};

struct pkw tunnel = PKW_INITIALIZER(&tunnel, "tunnel", NULL, NULL,
    tunnelkw, __arraycount(tunnelkw), NULL);

int
settunnel(prop_dictionary_t env, prop_dictionary_t xenv)
{
	const struct paddr_prefix *srcpfx, *dstpfx;
	struct if_laddrreq req;
	int s;
	prop_data_t srcdata, dstdata;
	const char *ifname;

	if ((s = getsock(AF_UNSPEC)) == -1)
		err(EXIT_FAILURE, "%s: getsock", __func__);

	if ((ifname = getifname(env)) == NULL)
		return -1;

	srcdata = (prop_data_t)prop_dictionary_get(env, "tunsrc");
	dstdata = (prop_data_t)prop_dictionary_get(env, "tundst");

	if (srcdata == NULL || dstdata == NULL) {
		warnx("%s.%d", __func__, __LINE__);
		errno = ENOENT;
		return -1;
	}

	srcpfx = prop_data_data_nocopy(srcdata);
	dstpfx = prop_data_data_nocopy(dstdata);

	if (srcpfx->pfx_addr.sa_family != dstpfx->pfx_addr.sa_family)
		errx(EXIT_FAILURE,
		    "source and destination address families do not match");

	memset(&req, 0, sizeof(req));
	estrlcpy(req.iflr_name, ifname, sizeof(req.iflr_name));
	memcpy(&req.addr, &srcpfx->pfx_addr,
	    MIN(sizeof(req.addr), srcpfx->pfx_addr.sa_len));
	memcpy(&req.dstaddr, &dstpfx->pfx_addr,
	    MIN(sizeof(req.dstaddr), dstpfx->pfx_addr.sa_len));

#ifdef INET6
	if (req.addr.ss_family == AF_INET6) {
		struct sockaddr_in6 *s6, *d;

		s6 = (struct sockaddr_in6 *)&req.addr;
		d = (struct sockaddr_in6 *)&req.dstaddr;
		if (s6->sin6_scope_id != d->sin6_scope_id) {
			errx(EXIT_FAILURE, "scope mismatch");
			/* NOTREACHED */
		}
		/* embed scopeid */
		if (s6->sin6_scope_id &&
		    (IN6_IS_ADDR_LINKLOCAL(&s6->sin6_addr) ||
		     IN6_IS_ADDR_MC_LINKLOCAL(&s6->sin6_addr))) {
			*(u_int16_t *)&s6->sin6_addr.s6_addr[2] =
			    htons(s6->sin6_scope_id);
		}
		if (d->sin6_scope_id &&
		    (IN6_IS_ADDR_LINKLOCAL(&d->sin6_addr) ||
		     IN6_IS_ADDR_MC_LINKLOCAL(&d->sin6_addr))) {
			*(u_int16_t *)&d->sin6_addr.s6_addr[2] =
			    htons(d->sin6_scope_id);
		}
	}
#endif /* INET6 */

	if (ioctl(s, SIOCSLIFPHYADDR, &req) == -1)
		warn("SIOCSLIFPHYADDR");
	return 0;
}

int
deletetunnel(prop_dictionary_t env, prop_dictionary_t xenv)
{
	int s;
	struct ifreq ifr;
	const char *ifname;

	if ((s = getsock(AF_UNSPEC)) == -1)
		err(EXIT_FAILURE, "%s: getsock", __func__);

	if ((ifname = getifname(env)) == NULL)
		return -1;

	memset(&ifr, 0, sizeof(ifr));

	estrlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCDIFPHYADDR, &ifr) == -1)
		err(EXIT_FAILURE, "SIOCDIFPHYADDR");
	return 0;
}

void
tunnel_status(prop_dictionary_t env, prop_dictionary_t oenv)
{
	char dstserv[sizeof(",65535")];
	char srcserv[sizeof(",65535")];
	char psrcaddr[NI_MAXHOST];
	char pdstaddr[NI_MAXHOST];
	const int niflag = NI_NUMERICHOST|NI_NUMERICSERV;
	struct if_laddrreq req;
	const struct afswtch *lafp;
	int s;
	const char *ifname;

	if ((s = getsock(AF_UNSPEC)) == -1)
		err(EXIT_FAILURE, "%s: getsock", __func__);

	psrcaddr[0] = pdstaddr[0] = '\0';

	if ((ifname = getifname(env)) == NULL)
		err(EXIT_FAILURE, "%s: getifname", __func__);

	memset(&req, 0, sizeof(req));
	estrlcpy(req.iflr_name, ifname, IFNAMSIZ);
	if (ioctl(s, SIOCGLIFPHYADDR, &req) == -1)
		return;
	lafp = lookup_af_bynum(req.addr.ss_family);
#ifdef INET6
	if (req.addr.ss_family == AF_INET6)
		in6_fillscopeid((struct sockaddr_in6 *)&req.addr);
#endif /* INET6 */
	getnameinfo((struct sockaddr *)&req.addr, req.addr.ss_len,
	    psrcaddr, sizeof(psrcaddr), &srcserv[1], sizeof(srcserv) - 1,
	    niflag);

#ifdef INET6
	if (req.dstaddr.ss_family == AF_INET6)
		in6_fillscopeid((struct sockaddr_in6 *)&req.dstaddr);
#endif
	getnameinfo((struct sockaddr *)&req.dstaddr, req.dstaddr.ss_len,
	    pdstaddr, sizeof(pdstaddr), &dstserv[1], sizeof(dstserv) - 1,
	    niflag);

	srcserv[0] = (strcmp(&srcserv[1], "0") == 0) ? '\0' : ',';
	dstserv[0] = (strcmp(&dstserv[1], "0") == 0) ? '\0' : ',';

	printf("\ttunnel %s %s%s --> %s%s\n", lafp ? lafp->af_name : "???",
	    psrcaddr, srcserv, pdstaddr, dstserv);
}
