/*	$NetBSD: rtl81x9var.h,v 1.2 2000/04/30 12:00:40 tsutsui Exp $	*/

/*
 * Copyright (c) 1997, 1998
 *	Bill Paul <wpaul@ctr.columbia.edu>.  All rights reserved.
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
 *	This product includes software developed by Bill Paul.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Bill Paul AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Bill Paul OR THE VOICES IN HIS HEAD
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	FreeBSD Id: if_rlreg.h,v 1.9 1999/06/20 18:56:09 wpaul Exp
 */

#define RL_ETHER_ALIGN	2

struct rl_chain_data {
	u_int16_t		cur_rx;
	caddr_t			rl_rx_buf;
	caddr_t			rl_rx_buf_ptr;

	struct mbuf		*rl_tx_chain[RL_TX_LIST_CNT];
	u_int8_t		last_tx;
	u_int8_t		cur_tx;
};

#define RL_INC(x)		(x = (x + 1) % RL_TX_LIST_CNT)
#define RL_CUR_TXADDR(x)	((x->rl_cdata.cur_tx * 4) + RL_TXADDR0)
#define RL_CUR_TXSTAT(x)	((x->rl_cdata.cur_tx * 4) + RL_TXSTAT0)
#define RL_CUR_TXMBUF(x)	(x->rl_cdata.rl_tx_chain[x->rl_cdata.cur_tx])
#define RL_LAST_TXADDR(x)	((x->rl_cdata.last_tx * 4) + RL_TXADDR0)
#define RL_LAST_TXSTAT(x)	((x->rl_cdata.last_tx * 4) + RL_TXSTAT0)
#define RL_LAST_TXMBUF(x)	(x->rl_cdata.rl_tx_chain[x->rl_cdata.last_tx])

struct rl_type {
	u_int16_t		rl_vid;
	u_int16_t		rl_did;
	char			*rl_name;
};

struct rl_mii_frame {
	u_int8_t		mii_stdelim;
	u_int8_t		mii_opcode;
	u_int8_t		mii_phyaddr;
	u_int8_t		mii_regaddr;
	u_int8_t		mii_turnaround;
	u_int16_t		mii_data;
};

/*
 * MII constants
 */
#define RL_MII_STARTDELIM	0x01
#define RL_MII_READOP		0x02
#define RL_MII_WRITEOP		0x01
#define RL_MII_TURNAROUND	0x02

#define RL_8129			1
#define RL_8139			2

struct rl_softc {
	struct device sc_dev;		/* generic device structures */
	struct ethercom		ethercom;		/* interface info */
	struct mii_data		mii;
	struct callout		rl_tick_ch;	/* tick callout */
	bus_space_handle_t	rl_bhandle;	/* bus space handle */
	bus_space_tag_t		rl_btag;	/* bus space tag */
	u_int8_t		rl_type;
	struct rl_chain_data	rl_cdata;
	void *sc_ih;
	bus_dma_tag_t sc_dmat;
	bus_dmamap_t recv_dmamap, snd_dmamap[RL_TX_LIST_CNT];
};

/*
 * register space access macros
 */
#define CSR_WRITE_4(sc, reg, val)	\
	bus_space_write_4(sc->rl_btag, sc->rl_bhandle, reg, val)
#define CSR_WRITE_2(sc, reg, val)	\
	bus_space_write_2(sc->rl_btag, sc->rl_bhandle, reg, val)
#define CSR_WRITE_1(sc, reg, val)	\
	bus_space_write_1(sc->rl_btag, sc->rl_bhandle, reg, val)

#define CSR_READ_4(sc, reg)		\
	bus_space_read_4(sc->rl_btag, sc->rl_bhandle, reg)
#define CSR_READ_2(sc, reg)		\
	bus_space_read_2(sc->rl_btag, sc->rl_bhandle, reg)
#define CSR_READ_1(sc, reg)		\
	bus_space_read_1(sc->rl_btag, sc->rl_bhandle, reg)

#define RL_TIMEOUT		1000

/*
 * PCI low memory base and low I/O base register, and
 * other PCI registers.
 */

#define RL_PCI_LOIO		0x10
#define RL_PCI_LOMEM		0x14

#define RL_PSTATE_MASK		0x0003
#define RL_PSTATE_D0		0x0000
#define RL_PSTATE_D1		0x0002
#define RL_PSTATE_D2		0x0002
#define RL_PSTATE_D3		0x0003
#define RL_PME_EN		0x0010
#define RL_PME_STATUS		0x8000

#ifdef _KERNEL
void	rl_attach __P((struct rl_softc *, const u_int8_t *));
int	rl_intr __P((void *));
u_int16_t rl_read_eeprom __P((struct rl_softc *, int, int));
void	rl_reset __P((struct rl_softc *));
#endif /* _KERNEL */
