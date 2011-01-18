/*	$NetBSD: npf_instr.c,v 1.5 2011/01/18 20:33:45 rmind Exp $	*/

/*-
 * Copyright (c) 2009-2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This material is based upon work partially supported by The
 * NetBSD Foundation under a contract with Mindaugas Rasiukevicius.
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
 * NPF complex instructions.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: npf_instr.c,v 1.5 2011/01/18 20:33:45 rmind Exp $");

#include <sys/param.h>
#include <sys/kernel.h>

#include <net/if.h>
#include <net/ethertypes.h>
#include <net/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>

#include "npf_impl.h"

#define	NPF_PORTRANGE_MATCH(r, p)	(p >= (r >> 16) && p <= (r & 0xffff))

/*
 * npf_match_ether: find and check Ethernet and possible VLAN headers.
 *
 * => Stores value in to advance to layer 3 header (usually, IPv4).
 * => Returns zero on success or -1 on failure.
 */
int
npf_match_ether(nbuf_t *nbuf, int sd, int _res, uint16_t ethertype, uint32_t *r)
{
	void *n_ptr = nbuf_dataptr(nbuf);
	u_int offby;
	uint16_t val16;
	bool vlan;

	vlan = false;
	*r = 0;

	/* Ethernet header: check EtherType. */
	offby = offsetof(struct ether_header, ether_type);
again:
	if (nbuf_advfetch(&nbuf, &n_ptr, offby, sizeof(uint16_t), &val16)) {
		return -1;
	}
	val16 = ntohs(val16);
	*r += offby;

	/* Handle VLAN tags. */
	if (val16 == ETHERTYPE_VLAN && !vlan) {
		offby = sizeof(uint32_t);
		vlan = true;
		goto again;
	}
	if (val16 != ETHERTYPE_IP) {
		return -1;
	}

	*r += ETHER_TYPE_LEN;
	return 0;
}

/*
 * npf_match_ip4table: match IPv4 address against NPF table.
 */
int
npf_match_ip4table(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr,
    const int sd, const u_int tid)
{
	struct ip *ip = &npc->npc_ip.v4;
	in_addr_t ip4addr;

	if (!npf_iscached(npc, NPC_IP46)) {
		if (!npf_fetch_ip(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_IP46));
	}
	ip4addr = sd ? ip->ip_src.s_addr : ip->ip_dst.s_addr;

	/* Match address against NPF table. */
	return npf_table_match_v4addr(tid, ip4addr);
}

/*
 * npf_match_ip4mask: match IPv4 address against netaddr/subnet.
 */
int
npf_match_ip4mask(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr,
    const int sd, in_addr_t netaddr, in_addr_t subnet)
{
	struct ip *ip = &npc->npc_ip.v4;
	in_addr_t ip4addr;

	if (!npf_iscached(npc, NPC_IP46)) {
		if (!npf_fetch_ip(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_IP46));
	}
	ip4addr = sd ? ip->ip_src.s_addr : ip->ip_dst.s_addr;

	return (ip4addr & subnet) == netaddr ? 0 : -1;
}

/*
 * npf_match_tcp_ports: match TCP port in header against the range.
 */
int
npf_match_tcp_ports(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr,
    const int sd, const uint32_t prange)
{
	struct tcphdr *th = &npc->npc_l4.tcp;
	in_port_t p;

	if (!npf_iscached(npc, NPC_TCP)) {
		if (!npf_fetch_tcp(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_TCP));
	}
	p = sd ? th->th_sport : th->th_dport;

	/* Match against the port range. */
	return NPF_PORTRANGE_MATCH(prange, p) ? 0 : -1;
}

/*
 * npf_match_udp_ports: match UDP port in header against the range.
 */
int
npf_match_udp_ports(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr,
    const int sd, const uint32_t prange)
{
	struct udphdr *uh = &npc->npc_l4.udp;
	in_port_t p;

	if (!npf_iscached(npc, NPC_UDP)) {
		if (!npf_fetch_udp(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_UDP));
	}
	p = sd ? uh->uh_sport : uh->uh_dport;

	/* Match against the port range. */
	return NPF_PORTRANGE_MATCH(prange, p) ? 0 : -1;
}

/*
 * npf_match_icmp4: match ICMPv4 packet.
 */
int
npf_match_icmp4(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr, uint32_t tc)
{
	struct icmp *ic = &npc->npc_l4.icmp;

	if (!npf_iscached(npc, NPC_ICMP)) {
		if (!npf_fetch_icmp(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_ICMP));
	}

	/* Match code/type, if required. */
	if ((1 << 31) & tc) {
		const uint8_t type = (tc >> 8) & 0xff;
		if (type != ic->icmp_type) {
			return -1;
		}
	}
	if ((1 << 30) & tc) {
		const uint8_t code = tc & 0xff;
		if (code != ic->icmp_code) {
			return -1;
		}
	}
	return 0;
}

/*
 * npf_match_tcpfl: match TCP flags.
 */
int
npf_match_tcpfl(npf_cache_t *npc, nbuf_t *nbuf, void *n_ptr, uint32_t fl)
{
	const uint8_t tcpfl = (fl >> 8) & 0xff, mask = fl & 0xff;
	struct tcphdr *th = &npc->npc_l4.tcp;

	if (!npf_iscached(npc, NPC_TCP)) {
		if (!npf_fetch_tcp(npc, nbuf, n_ptr)) {
			return -1;
		}
		KASSERT(npf_iscached(npc, NPC_TCP));
	}
	return ((th->th_flags & mask) == tcpfl) ? 0 : -1;
}
