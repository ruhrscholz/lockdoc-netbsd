/*	$NetBSD: pfckbd.c,v 1.2 2001/03/02 19:21:53 uch Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#define PFCKBD_DEBUG

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/callout.h>

#include <machine/bus.h>
#include <dev/hpc/hpckbdvar.h>

#include <sh3/pfcreg.h>

#include <hpcsh/dev/pfckbdvar.h>

#ifdef PFCKBD_DEBUG
int	pfckbd_debug = 0;
#define	DPRINTF(fmt, args...)						\
	if (pfckbd_debug)						\
		printf("%s: " fmt, __FUNCTION__ , ##args) 
#define	DPRINTFN(n, arg)						\
	if (pfckbd_debug > (n))						\
		printf("%s: " fmt, __FUNCTION__ , ##args) 
#else
#define	DPRINTF(arg...)		((void)0)
#define DPRINTFN(n, arg...)	((void)0)
#endif

static int pfckbd_match(struct device *, struct cfdata *, void *);
static void pfckbd_attach(struct device *, struct device *, void *);

static struct pfckbd_core {
	int pc_attached;
	int pc_enabled;
	struct callout pc_soft_ch;
	struct hpckbd_ic_if pc_if;
	struct hpckbd_if *pc_hpckbd;
	u_int16_t pc_column[8];
} pfckbd_core;

struct pfckbd_softc {
	struct device sc_dev;
};

struct cfattach pfckbd_ca = {
	sizeof(struct pfckbd_softc), pfckbd_match, pfckbd_attach
};

static int pfckbd_poll(void *);
static void pfckbd_soft(void *);
static void pfckbd_ifsetup(struct pfckbd_core *);
static int pfckbd_input_establish(void *, struct hpckbd_if *);

/*
 * matrix scan keyboard connected to SH3 PFC module.
 * currently, HP Jornada 690 only.
 */
void
pfckbd_cnattach()
{
	struct pfckbd_core *pc = &pfckbd_core;
	
	/* initialize interface */
	pfckbd_ifsetup(pc);
	hpckbd_cnattach(&pc->pc_if);
}

static int
pfckbd_match(struct device *parent, struct cfdata *cf, void *aux)
{

	return !pfckbd_core.pc_attached;
}

static void
pfckbd_attach(struct device *parent, struct device *self, void *aux)
{
	struct hpckbd_attach_args haa;
	
	printf("\n");

	/* pfckbd is singleton. no more attach */
	pfckbd_core.pc_attached = 1;

	pfckbd_ifsetup(&pfckbd_core);

	/* attach hpckbd */
	haa.haa_ic = &pfckbd_core.pc_if; /* tell the hpckbd to my interface */
	config_found(self, &haa, hpckbd_print);

	/* install callout handler */
	callout_init(&pfckbd_core.pc_soft_ch);
	callout_reset(&pfckbd_core.pc_soft_ch, 1, pfckbd_soft, &pfckbd_core);
}

static int
pfckbd_input_establish(void *ic, struct hpckbd_if *kbdif)
{
	struct pfckbd_core *pc = ic;

	/* save hpckbd interface */
	pc->pc_hpckbd = kbdif;
	/* ok to transact hpckbd */
	pc->pc_enabled = 1;

	return 0;
}

static int
pfckbd_poll(void *arg)
{
	struct pfckbd_core *pc = arg;

	if (pc->pc_enabled)
		pfckbd_soft(arg);

	return 0;
}

static void
pfckbd_ifsetup(struct pfckbd_core *pc)
{
	int i;

	pc->pc_if.hii_ctx = pc;
	pc->pc_if.hii_establish	= pfckbd_input_establish;
	pc->pc_if.hii_poll = pfckbd_poll;
	for (i = 0; i < 8; i++)
		pc->pc_column[i] = 0xdfff;
}

static void
pfckbd_soft(void *arg)
{
	static const struct {
		u_int8_t d, e;
	} scan[] = {
		{ 0xfd, 0xff },
		{ 0xdf, 0xff },
		{ 0xff, 0xfd },
		{ 0xff, 0xbf },
		{ 0xff, 0x7f },
		{ 0xff, 0xf7 },
		{ 0xff, 0xfe },
		{ 0x7f, 0xff },
	};
	struct pfckbd_core *pc = arg;
	struct hpckbd_if *hpckbd = pc->pc_hpckbd;
	u_int16_t *buf = pc->pc_column;
	int row, column, type, val;
	u_int16_t data, edge, mask;

	if (!pc->pc_enabled)
		goto reinstall;

	for (column = 0; column < 8; column++) {
		SHREG_PDDR = scan[column].d;
		SHREG_PEDR = scan[column].e;
		delay(50);
		data = SHREG_PFDR | (SHREG_PCDR << 8);

		if ((edge = (data ^ buf[column]))) {
			buf[column] = data;

			for (row = 0, mask = 1; row < 16; row++, mask <<= 1) {
				if (mask & edge) {
					type = mask & data ? 0 : 1;
					val = row * 8 + column;
					DPRINTF("(%2d, %2d) %d \n",
						row, column, type);
					hpckbd_input(hpckbd, type, val);
				}
			}
		}
	}

	SHREG_PDDR = 0xff;
	SHREG_PEDR = 0xff;
	data = SHREG_PGDR | (SHREG_PHDR << 8);

 reinstall:
	callout_reset(&pc->pc_soft_ch, 1, pfckbd_soft, pc);
}
