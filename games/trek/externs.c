/*	$NetBSD: externs.c,v 1.8 2003/08/07 09:37:51 agc Exp $	*/

/*
 * Copyright (c) 1980, 1993
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
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)externs.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: externs.c,v 1.8 2003/08/07 09:37:51 agc Exp $");
#endif
#endif /* not lint */

# include	"trek.h"

/*
**	global variable definitions
*/

const struct device	Device[NDEV] =
{
	{ "warp drive",		"Scotty" },
	{ "S.R. scanners",	"Scotty" },
	{ "L.R. scanners",	"Scotty" },
	{ "phasers",		"Sulu" },
	{ "photon tubes",	"Sulu" },
	{ "impulse engines",	"Scotty" },
	{ "shield control",	"Sulu" },
	{ "computer",		"Spock" },
	{ "subspace radio",	"Uhura" },
	{ "life support",	"Scotty" },
	{ "navigation system",	"Chekov" },
	{ "cloaking device",	"Scotty" },
	{ "transporter",	"Scotty" },
	{ "shuttlecraft",	"Scotty" },
	{ "*ERR 14*",		"Nobody" },
	{ "*ERR 15*",		"Nobody" }
};

const char	*const Systemname[NINHAB] =
{
	"ERROR",
	"Talos IV",
	"Rigel III",
	"Deneb VII",
	"Canopus V",
	"Icarus I",
	"Prometheus II",
	"Omega VII",
	"Elysium I",
	"Scalos IV",
	"Procyon IV",
	"Arachnid I",
	"Argo VIII",
	"Triad III",
	"Echo IV",
	"Nimrod III",
	"Nemisis IV",
	"Centarurus I",
	"Kronos III",
	"Spectros V",
	"Beta III",
	"Gamma Tranguli VI",
	"Pyris III",
	"Triachus",
	"Marcus XII",
	"Kaland",
	"Ardana",
	"Stratos",
	"Eden",
	"Arrikis",
	"Epsilon Eridani IV",
	"Exo III"
};

struct quad	Quad[NQUADS][NQUADS];

/* current sector map */
char	Sect[NSECTS][NSECTS];

struct event	Event[MAXEVENTS];	/* dynamic event list; one entry per pending event */

struct Ship_struct Ship;
struct Game_struct Game;
struct Move_struct Move;
struct Param_struct Param;
struct Now_struct Now;
struct Etc_struct Etc;

int Trace;
