/*	$NetBSD: i82557.c,v 1.3 1999/08/03 23:18:09 thorpej Exp $	*/

/*-
 * Copyright (c) 1997, 1998, 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

/*
 * Copyright (c) 1995, David Greenman
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	Id: if_fxp.c,v 1.47 1998/01/08 23:42:29 eivind Exp
 */

/*
 * Device driver for the Intel i82557 fast Ethernet controller.
 */

#include "opt_inet.h"
#include "opt_ns.h"
#include "bpfilter.h"
#include "rnd.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/device.h>

#include <vm/vm.h>		/* for PAGE_SIZE */

#if NRND > 0
#include <sys/rnd.h>
#endif

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_ether.h>

#if NBPFILTER > 0
#include <net/bpf.h>
#endif

#ifdef INET
#include <netinet/in.h>
#include <netinet/if_inarp.h>
#endif

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/mii/miivar.h>

#include <dev/ic/i82557reg.h>
#include <dev/ic/i82557var.h>

/*
 * NOTE!  On the Alpha, we have an alignment constraint.  The
 * card DMAs the packet immediately following the RFA.  However,
 * the first thing in the packet is a 14-byte Ethernet header.
 * This means that the packet is misaligned.  To compensate,
 * we actually offset the RFA 2 bytes into the cluster.  This
 * alignes the packet after the Ethernet header at a 32-bit
 * boundary.  HOWEVER!  This means that the RFA is misaligned!
 */
#define	RFA_ALIGNMENT_FUDGE	2

/*
 * Template for default configuration parameters.
 * See struct fxp_cb_config for the bit definitions.
 */
u_int8_t fxp_cb_config_template[] = {
	0x0, 0x0,		/* cb_status */
	0x80, 0x2,		/* cb_command */
	0xff, 0xff, 0xff, 0xff,	/* link_addr */
	0x16,	/*  0 */
	0x8,	/*  1 */
	0x0,	/*  2 */
	0x0,	/*  3 */
	0x0,	/*  4 */
	0x80,	/*  5 */
	0xb2,	/*  6 */
	0x3,	/*  7 */
	0x1,	/*  8 */
	0x0,	/*  9 */
	0x26,	/* 10 */
	0x0,	/* 11 */
	0x60,	/* 12 */
	0x0,	/* 13 */
	0xf2,	/* 14 */
	0x48,	/* 15 */
	0x0,	/* 16 */
	0x40,	/* 17 */
	0xf3,	/* 18 */
	0x0,	/* 19 */
	0x3f,	/* 20 */
	0x5	/* 21 */
};

void	fxp_mii_initmedia __P((struct fxp_softc *));
int	fxp_mii_mediachange __P((struct ifnet *));
void	fxp_mii_mediastatus __P((struct ifnet *, struct ifmediareq *));

void	fxp_80c24_initmedia __P((struct fxp_softc *));
int	fxp_80c24_mediachange __P((struct ifnet *));
void	fxp_80c24_mediastatus __P((struct ifnet *, struct ifmediareq *));

inline void fxp_scb_wait __P((struct fxp_softc *));

void	fxp_start __P((struct ifnet *));
int	fxp_ioctl __P((struct ifnet *, u_long, caddr_t));
void	fxp_init __P((struct fxp_softc *));
void	fxp_stop __P((struct fxp_softc *));
void	fxp_watchdog __P((struct ifnet *));
int	fxp_add_rfabuf __P((struct fxp_softc *, struct fxp_rxdesc *));
int	fxp_mdi_read __P((struct device *, int, int));
void	fxp_statchg __P((struct device *));
void	fxp_mdi_write __P((struct device *, int, int, int));
void	fxp_read_eeprom __P((struct fxp_softc *, u_int16_t *, int, int));
void	fxp_get_info __P((struct fxp_softc *, u_int8_t *));
void	fxp_tick __P((void *));
void	fxp_mc_setup __P((struct fxp_softc *));

void	fxp_shutdown __P((void *));

struct fxp_phytype {
	int	fp_phy;		/* type of PHY, -1 for MII at the end. */
	void	(*fp_init) __P((struct fxp_softc *));
} fxp_phytype_table[] = {
	{ FXP_PHY_80C24,		fxp_80c24_initmedia },
	{ -1,				fxp_mii_initmedia },
};

/*
 * Set initial transmit threshold at 64 (512 bytes). This is
 * increased by 64 (512 bytes) at a time, to maximum of 192
 * (1536 bytes), if an underrun occurs.
 */
static int tx_threshold = 64;

/*
 * Wait for the previous command to be accepted (but not necessarily
 * completed).
 */
inline void
fxp_scb_wait(sc)
	struct fxp_softc *sc;
{
	int i = 10000;

	while (CSR_READ_1(sc, FXP_CSR_SCB_COMMAND) && --i)
		delay(2);
	if (i == 0)
		printf("%s: WARNING: SCB timed out!\n", sc->sc_dev.dv_xname);
}

/*
 * Finish attaching an i82557 interface.  Called by bus-specific front-end.
 */
void
fxp_attach(sc)
	struct fxp_softc *sc;
{
	u_int8_t enaddr[6];
	struct ifnet *ifp;
	bus_dma_segment_t seg;
	int rseg, i, error;
	struct fxp_phytype *fp;

	/*
	 * Allocate the control data structures, and create and load the
	 * DMA map for it.
	 */
	if ((error = bus_dmamem_alloc(sc->sc_dmat,
	    sizeof(struct fxp_control_data), PAGE_SIZE, 0, &seg, 1, &rseg,
	    0)) != 0) {
		printf("%s: unable to allocate control data, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_0;
	}

	if ((error = bus_dmamem_map(sc->sc_dmat, &seg, rseg,
	    sizeof(struct fxp_control_data), (caddr_t *)&sc->sc_control_data,
	    BUS_DMA_COHERENT)) != 0) {
		printf("%s: unable to map control data, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_1;
	}
	bzero(sc->sc_control_data, sizeof(struct fxp_control_data));

	if ((error = bus_dmamap_create(sc->sc_dmat,
	    sizeof(struct fxp_control_data), 1,
	    sizeof(struct fxp_control_data), 0, 0, &sc->sc_dmamap)) != 0) {
		printf("%s: unable to create control data DMA map, "
		    "error = %d\n", sc->sc_dev.dv_xname, error);
		goto fail_2;
	}

	if ((error = bus_dmamap_load(sc->sc_dmat, sc->sc_dmamap,
	    sc->sc_control_data, sizeof(struct fxp_control_data), NULL,
	    0)) != 0) {
		printf("%s: can't load control data DMA map, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_3;
	}

	/*
	 * Create the transmit buffer DMA maps.
	 */
	for (i = 0; i < FXP_NTXCB; i++) {
		if ((error = bus_dmamap_create(sc->sc_dmat, MCLBYTES,
		    FXP_NTXSEG, MCLBYTES, 0, 0,
		    &FXP_DSTX(sc, i)->txs_dmamap)) != 0) {
			printf("%s: unable to create tx DMA map %d, "
			    "error = %d\n", sc->sc_dev.dv_xname, i, error);
			goto fail_4;
		}
	}

	/*
	 * Create the receive buffer DMA maps.
	 */
	for (i = 0; i < FXP_NRFABUFS; i++) {
		if ((error = bus_dmamap_create(sc->sc_dmat, MCLBYTES, 1,
		    MCLBYTES, 0, 0, &sc->sc_rx_dmamaps[i])) != 0) {
			printf("%s: unable to create rx DMA map %d, "
			    "error = %d\n", sc->sc_dev.dv_xname, i, error);
			goto fail_5;
		}
	}

	/*
	 * Pre-allocate the receive buffers.
	 */
	for (i = 0; i < FXP_NRFABUFS; i++) {
		sc->sc_rxdescs[i].fr_dmamap = sc->sc_rx_dmamaps[i];
		if (fxp_add_rfabuf(sc, &sc->sc_rxdescs[i]) != 0) {
			printf("%s: unable to allocate or map rx buffer %d, "
			    "error = %d\n", sc->sc_dev.dv_xname, i, error);
			goto fail_6;
		}
	}

	/* Initialize MAC address and media structures. */
	fxp_get_info(sc, enaddr);

	printf("%s: Ethernet address %s, %s Mb/s\n", sc->sc_dev.dv_xname,
	    ether_sprintf(enaddr), sc->phy_10Mbps_only ? "10" : "10/100");

	ifp = &sc->sc_ethercom.ec_if;

	/*
	 * Get info about our media interface, and initialize it.  Note
	 * the table terminates itself with a phy of -1, indicating
	 * that we're using MII.
	 */
	for (fp = fxp_phytype_table; fp->fp_phy != -1; fp++)
		if (fp->fp_phy == sc->phy_primary_device)
			break;
	(*fp->fp_init)(sc);

	bcopy(sc->sc_dev.dv_xname, ifp->if_xname, IFNAMSIZ);
	ifp->if_softc = sc;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_ioctl = fxp_ioctl;
	ifp->if_start = fxp_start;
	ifp->if_watchdog = fxp_watchdog;

	/*
	 * Attach the interface.
	 */
	if_attach(ifp);
	ether_ifattach(ifp, enaddr);
#if NBPFILTER > 0
	bpfattach(&sc->sc_ethercom.ec_if.if_bpf, ifp, DLT_EN10MB,
	    sizeof(struct ether_header));
#endif
#if NRND > 0
	rnd_attach_source(&sc->rnd_source, sc->sc_dev.dv_xname,
			  RND_TYPE_NET, 0);
#endif

	/*
	 * Add shutdown hook so that DMA is disabled prior to reboot. Not
	 * doing do could allow DMA to corrupt kernel memory during the
	 * reboot before the driver initializes.
	 */
	sc->sc_sdhook = shutdownhook_establish(fxp_shutdown, sc);
	if (sc->sc_sdhook == NULL)
		printf("%s: WARNING: unable to establish shutdown hook\n",
		    sc->sc_dev.dv_xname);
	return;

	/*
	 * Free any resources we've allocated during the failed attach
	 * attempt.  Do this in reverse order and fall though.
	 */
 fail_6:
	for (i = 0; i < FXP_NRFABUFS; i++) {
		if (sc->sc_rxdescs[i].fr_mbhead != NULL) {
			bus_dmamap_unload(sc->sc_dmat,
			    sc->sc_rxdescs[i].fr_dmamap);
			m_freem(sc->sc_rxdescs[i].fr_mbhead);
		}
	}
 fail_5:
	for (i = 0; i < FXP_NRFABUFS; i++) {
		if (sc->sc_rxdescs[i].fr_dmamap != NULL)
			bus_dmamap_destroy(sc->sc_dmat,
			    sc->sc_rxdescs[i].fr_dmamap);
	}
 fail_4:
	for (i = 0; i < FXP_NTXCB; i++) {
		if (FXP_DSTX(sc, i)->txs_dmamap != NULL)
			bus_dmamap_destroy(sc->sc_dmat,
			    FXP_DSTX(sc, i)->txs_dmamap);
	}
	bus_dmamap_unload(sc->sc_dmat, sc->sc_dmamap);
 fail_3:
	bus_dmamap_destroy(sc->sc_dmat, sc->sc_dmamap);
 fail_2:
	bus_dmamem_unmap(sc->sc_dmat, (caddr_t)sc->sc_control_data,
	    sizeof(struct fxp_control_data));
 fail_1:
	bus_dmamem_free(sc->sc_dmat, &seg, rseg);
 fail_0:
	return;
}

void
fxp_mii_initmedia(sc)
	struct fxp_softc *sc;
{

	sc->sc_mii.mii_ifp = &sc->sc_ethercom.ec_if;
	sc->sc_mii.mii_readreg = fxp_mdi_read;
	sc->sc_mii.mii_writereg = fxp_mdi_write;
	sc->sc_mii.mii_statchg = fxp_statchg;
	ifmedia_init(&sc->sc_mii.mii_media, 0, fxp_mii_mediachange,
	    fxp_mii_mediastatus);
	mii_phy_probe(&sc->sc_dev, &sc->sc_mii, 0xffffffff);
	if (LIST_FIRST(&sc->sc_mii.mii_phys) == NULL) {
		ifmedia_add(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE, 0, NULL);
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE);
	} else
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_AUTO);
}

void
fxp_80c24_initmedia(sc)
	struct fxp_softc *sc;
{

	/*
	 * The Seeq 80c24 AutoDUPLEX(tm) Ethernet Interface Adapter
	 * doesn't have a programming interface of any sort.  The
	 * media is sensed automatically based on how the link partner
	 * is configured.  This is, in essence, manual configuration.
	 */
	printf("%s: Seeq 80c24 AutoDUPLEX media interface present\n",
	    sc->sc_dev.dv_xname);
	ifmedia_init(&sc->sc_mii.mii_media, 0, fxp_80c24_mediachange,
	    fxp_80c24_mediastatus);
	ifmedia_add(&sc->sc_mii.mii_media, IFM_ETHER|IFM_MANUAL, 0, NULL);
	ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_MANUAL);
}

/*
 * Device shutdown routine. Called at system shutdown after sync. The
 * main purpose of this routine is to shut off receiver DMA so that
 * kernel memory doesn't get clobbered during warmboot.
 */
void
fxp_shutdown(arg)
	void *arg;
{
	struct fxp_softc *sc = arg;

	fxp_stop(sc);
}

/*
 * Initialize the interface media.
 */
void
fxp_get_info(sc, enaddr)
	struct fxp_softc *sc;
	u_int8_t *enaddr;
{
	u_int16_t data, myea[3];

	/*
	 * Reset to a stable state.
	 */
	CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SELECTIVE_RESET);
	DELAY(10);

	/*
	 * Get info about the primary PHY
	 */
	fxp_read_eeprom(sc, &data, 6, 1);
	sc->phy_primary_addr = data & 0xff;
	sc->phy_primary_device = (data >> 8) & 0x3f;
	sc->phy_10Mbps_only = data >> 15;

	/*
	 * Read MAC address.
	 */
	fxp_read_eeprom(sc, myea, 0, 3);
	bcopy(myea, enaddr, ETHER_ADDR_LEN);
}

/*
 * Read from the serial EEPROM. Basically, you manually shift in
 * the read opcode (one bit at a time) and then shift in the address,
 * and then you shift out the data (all of this one bit at a time).
 * The word size is 16 bits, so you have to provide the address for
 * every 16 bits of data.
 */
void
fxp_read_eeprom(sc, data, offset, words)
	struct fxp_softc *sc;
	u_int16_t *data;
	int offset;
	int words;
{
	u_int16_t reg;
	int i, x;

	for (i = 0; i < words; i++) {
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, FXP_EEPROM_EECS);
		/*
		 * Shift in read opcode.
		 */
		for (x = 3; x > 0; x--) {
			if (FXP_EEPROM_OPC_READ & (1 << (x - 1))) {
				reg = FXP_EEPROM_EECS | FXP_EEPROM_EEDI;
			} else {
				reg = FXP_EEPROM_EECS;
			}
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL,
			    reg | FXP_EEPROM_EESK);
			DELAY(1);
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
			DELAY(1);
		}
		/*
		 * Shift in address.
		 */
		for (x = 6; x > 0; x--) {
			if ((i + offset) & (1 << (x - 1))) {
				reg = FXP_EEPROM_EECS | FXP_EEPROM_EEDI;
			} else {
				reg = FXP_EEPROM_EECS;
			}
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL,
			    reg | FXP_EEPROM_EESK);
			DELAY(1);
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
			DELAY(1);
		}
		reg = FXP_EEPROM_EECS;
		data[i] = 0;
		/*
		 * Shift out data.
		 */
		for (x = 16; x > 0; x--) {
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL,
			    reg | FXP_EEPROM_EESK);
			DELAY(1);
			if (CSR_READ_2(sc, FXP_CSR_EEPROMCONTROL) &
			    FXP_EEPROM_EEDO)
				data[i] |= (1 << (x - 1));
			CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, reg);
			DELAY(1);
		}
		CSR_WRITE_2(sc, FXP_CSR_EEPROMCONTROL, 0);
		DELAY(1);
	}
}

/*
 * Start packet transmission on the interface.
 */
void
fxp_start(ifp)
	struct ifnet *ifp;
{
	struct fxp_softc *sc = ifp->if_softc;
	struct mbuf *m0, *m;
	struct fxp_cb_tx *txd;
	struct fxp_txsoft *txs;
	struct fxp_tbdlist *tbd;
	bus_dmamap_t dmamap;
	int error, lasttx, nexttx, opending, seg;

	/*
	 * If we need multicast setup, bail out now.
	 */
	if (sc->sc_flags & FXPF_NEEDMCSETUP) {
		ifp->if_flags |= IFF_OACTIVE;
		return;
	}

	/*
	 * Remember the previous txpending and the current lasttx.
	 */
	opending = sc->sc_txpending;
	lasttx = sc->sc_txlast;

	/*
	 * Loop through the send queue, setting up transmit descriptors
	 * until we drain the queue, or use up all available transmit
	 * descriptors.
	 */
	while (sc->sc_txpending < FXP_NTXCB) {
		/*
		 * Grab a packet off the queue.
		 */
		IF_DEQUEUE(&ifp->if_snd, m0);
		if (m0 == NULL)
			break;

		/*
		 * Get the next available transmit descriptor.
		 */
		nexttx = FXP_NEXTTX(sc->sc_txlast);
		txd = FXP_CDTX(sc, nexttx);
		tbd = FXP_CDTBD(sc, nexttx);
		txs = FXP_DSTX(sc, nexttx);
		dmamap = txs->txs_dmamap;

		/*
		 * Load the DMA map.  If this fails, the packet either
		 * didn't fit in the allotted number of frags, or we were
		 * short on resources.  In this case, we'll copy and try
		 * again.
		 */
		if (bus_dmamap_load_mbuf(sc->sc_dmat, dmamap, m0,
		    BUS_DMA_NOWAIT) != 0) {
			MGETHDR(m, M_DONTWAIT, MT_DATA);
			if (m == NULL) {
				printf("%s: unable to allocate Tx mbuf\n",
				    sc->sc_dev.dv_xname);
				IF_PREPEND(&ifp->if_snd, m0);
				break;
			}
			if (m0->m_pkthdr.len > MHLEN) {
				MCLGET(m, M_DONTWAIT);
				if ((m->m_flags & M_EXT) == 0) {
					printf("%s: unable to allocate Tx "
					    "cluster\n", sc->sc_dev.dv_xname);
					m_freem(m);
					IF_PREPEND(&ifp->if_snd, m0);
					break;
				}
			}
			m_copydata(m0, 0, m0->m_pkthdr.len, mtod(m, caddr_t));
			m->m_pkthdr.len = m->m_len = m0->m_pkthdr.len;
			m_freem(m0);
			m0 = m;
			error = bus_dmamap_load_mbuf(sc->sc_dmat, dmamap,
			    m0, BUS_DMA_NOWAIT);
			if (error) {
				printf("%s: unable to load Tx buffer, "
				    "error = %d\n", sc->sc_dev.dv_xname, error);
				IF_PREPEND(&ifp->if_snd, m0);
				break;
			}
		}

		/* Initialize the fraglist. */
		for (seg = 0; seg < dmamap->dm_nsegs; seg++) {
			tbd->tbd_d[seg].tb_addr =
			    dmamap->dm_segs[seg].ds_addr;
			tbd->tbd_d[seg].tb_size =
			    dmamap->dm_segs[seg].ds_len;
		}

		FXP_CDTBDSYNC(sc, nexttx, BUS_DMASYNC_PREWRITE);

		/* Sync the DMA map. */
		bus_dmamap_sync(sc->sc_dmat, dmamap, 0, dmamap->dm_mapsize,
		    BUS_DMASYNC_PREWRITE);

		/*
		 * Store a pointer to the packet so we can free it later.
		 */
		txs->txs_mbuf = m0;

		/*
		 * Initialize the transmit descriptor.
		 */
		txd->cb_status = 0;
		txd->cb_command =
		    FXP_CB_COMMAND_XMIT | FXP_CB_COMMAND_SF;
		txd->tx_threshold = tx_threshold;
		txd->tbd_number = dmamap->dm_nsegs;

		FXP_CDTXSYNC(sc, nexttx,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/* Advance the tx pointer. */
		sc->sc_txpending++;
		sc->sc_txlast = nexttx;

#if NBPFILTER > 0
		/*
		 * Pass packet to bpf if there is a listener.
		 */
		if (ifp->if_bpf)
			bpf_mtap(ifp->if_bpf, m0);
#endif
	}

	if (sc->sc_txpending == FXP_NTXCB) {
		/* No more slots; notify upper layer. */
		ifp->if_flags |= IFF_OACTIVE;
	}

	if (sc->sc_txpending != opending) {
		/*
		 * We enqueued packets.  If the transmitter was idle,
		 * reset the txdirty pointer.
		 */
		if (opending == 0)
			sc->sc_txdirty = FXP_NEXTTX(lasttx);

		/*
		 * Cause the chip to interrupt and suspend command
		 * processing once the last packet we've enqueued
		 * has been transmitted.
		 */
		FXP_CDTX(sc, sc->sc_txlast)->cb_command |=
		    FXP_CB_COMMAND_I | FXP_CB_COMMAND_S;
		FXP_CDTXSYNC(sc, sc->sc_txlast,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/*
		 * The entire packet chain is set up.  Clear the suspend bit
		 * on the command prior to the first packet we set up.
		 */
		FXP_CDTXSYNC(sc, lasttx,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
		FXP_CDTX(sc, lasttx)->cb_command &= ~FXP_CB_COMMAND_S;
		FXP_CDTXSYNC(sc, lasttx,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/*
		 * Issue a Resume command in case the chip was suspended.
		 */
		fxp_scb_wait(sc);
		CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_RESUME);

		/* Set a watchdog timer in case the chip flakes out. */
		ifp->if_timer = 5;
	}
}

/*
 * Process interface interrupts.
 */
int
fxp_intr(arg)
	void *arg;
{
	struct fxp_softc *sc = arg;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct fxp_cb_tx *txd;
	struct fxp_txsoft *txs;
	int i, oflags, claimed = 0;
	u_int8_t statack;

	while ((statack = CSR_READ_1(sc, FXP_CSR_SCB_STATACK)) != 0) {
		claimed = 1;

		/*
		 * First ACK all the interrupts in this pass.
		 */
		CSR_WRITE_1(sc, FXP_CSR_SCB_STATACK, statack);

		/*
		 * Process receiver interrupts. If a no-resource (RNR)
		 * condition exists, get whatever packets we can and
		 * re-start the receiver.
		 */
		if (statack & (FXP_SCB_STATACK_FR | FXP_SCB_STATACK_RNR)) {
			struct fxp_rxdesc *rxd;
			struct mbuf *m;
			struct fxp_rfa *rfa;
			bus_dmamap_t rxmap;
 rcvloop:
			rxd = sc->rfa_head;
			rxmap = rxd->fr_dmamap;
			m = rxd->fr_mbhead;
			rfa = (struct fxp_rfa *)(m->m_ext.ext_buf +
			    RFA_ALIGNMENT_FUDGE);

			bus_dmamap_sync(sc->sc_dmat, rxmap, 0,
			    rxmap->dm_mapsize,
			    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

			if (rfa->rfa_status & FXP_RFA_STATUS_C) {
				/*
				 * Remove first packet from the chain.
				 */
				sc->rfa_head = rxd->fr_next;
				rxd->fr_next = NULL;

				/*
				 * Add a new buffer to the receive chain.
				 * If this fails, the old buffer is recycled
				 * instead.
				 */
				if (fxp_add_rfabuf(sc, rxd) == 0) {
					struct ether_header *eh;
					u_int16_t total_len;

					total_len = rfa->actual_size &
					    (MCLBYTES - 1);
					if (total_len <
					    sizeof(struct ether_header)) {
						m_freem(m);
						goto rcvloop;
					}
					m->m_pkthdr.rcvif = ifp;
					m->m_pkthdr.len = m->m_len = total_len;
					eh = mtod(m, struct ether_header *);
#if NBPFILTER > 0
					if (ifp->if_bpf) {
						bpf_tap(ifp->if_bpf,
						    mtod(m, caddr_t),
						    total_len); 
						/*
						 * Only pass this packet up
						 * if it is for us.
						 */
						if ((ifp->if_flags &
						    IFF_PROMISC) &&
						    (rfa->rfa_status &
						    FXP_RFA_STATUS_IAMATCH) &&
						    (eh->ether_dhost[0] & 1)
						    == 0) {
							m_freem(m);
							goto rcvloop;
						}
					}
#endif /* NBPFILTER > 0 */
					(*ifp->if_input)(ifp, m);
				}
				goto rcvloop;
			}
			if (statack & FXP_SCB_STATACK_RNR) {
				fxp_scb_wait(sc);
				CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL,
				    rxmap->dm_segs[0].ds_addr +
				    RFA_ALIGNMENT_FUDGE);
				CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND,
				    FXP_SCB_COMMAND_RU_START);
			}
		}
		/*
		 * Free any finished transmit mbuf chains.
		 */
		if (statack & FXP_SCB_STATACK_CNA) {
			ifp->if_flags &= ~IFF_OACTIVE;
			for (i = sc->sc_txdirty; sc->sc_txpending != 0;
			     i = FXP_NEXTTX(i), sc->sc_txpending--) {
				txd = FXP_CDTX(sc, i);
				txs = FXP_DSTX(sc, i);

				FXP_CDTXSYNC(sc, i,
				    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

				if ((txd->cb_status & FXP_CB_STATUS_C) == 0)
					break;

				FXP_CDTBDSYNC(sc, i, BUS_DMASYNC_POSTWRITE);

				bus_dmamap_sync(sc->sc_dmat, txs->txs_dmamap,
				    0, txs->txs_dmamap->dm_mapsize,
				    BUS_DMASYNC_POSTWRITE);
				bus_dmamap_unload(sc->sc_dmat, txs->txs_dmamap);
				m_freem(txs->txs_mbuf);
				txs->txs_mbuf = NULL;
			}

			/* Update the dirty transmit buffer pointer. */
			sc->sc_txdirty = i;

			/*
			 * Cancel the watchdog timer if there are no pending
			 * transmissions.
			 */
			if (sc->sc_txpending == 0) {
				ifp->if_timer = 0;

				/*
				 * If we need a multicast filter setup,
				 * do that now.
				 */
				if (sc->sc_flags & FXPF_NEEDMCSETUP) {
					oflags = ifp->if_flags;
					fxp_mc_setup(sc);

					/*
					 * If IFF_ALLMULTI state changed,
					 * we need to reinitialize the chip,
					 * because this is handled by the
					 * config block.
					 */
					if (((ifp->if_flags ^ oflags) &
					    IFF_ALLMULTI) != 0)
						fxp_init(sc);
				}
			}

			/*
			 * Try to get more packets going.
			 */
			fxp_start(ifp);
		}
	}

#if NRND > 0
	if (claimed)
		rnd_add_uint32(&sc->rnd_source, statack);
#endif
	return (claimed);
}

/*
 * Update packet in/out/collision statistics. The i82557 doesn't
 * allow you to access these counters without doing a fairly
 * expensive DMA to get _all_ of the statistics it maintains, so
 * we do this operation here only once per second. The statistics
 * counters in the kernel are updated from the previous dump-stats
 * DMA and then a new dump-stats DMA is started. The on-chip
 * counters are zeroed when the DMA completes. If we can't start
 * the DMA immediately, we don't wait - we just prepare to read
 * them again next time.
 */
void
fxp_tick(arg)
	void *arg;
{
	struct fxp_softc *sc = arg;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct fxp_stats *sp = &sc->sc_control_data->fcd_stats;
	int oflags, s;

	s = splnet();

	oflags = ifp->if_flags;

	ifp->if_opackets += sp->tx_good;
	ifp->if_collisions += sp->tx_total_collisions;
	if (sp->rx_good) {
		ifp->if_ipackets += sp->rx_good;
		sc->rx_idle_secs = 0;
	} else {
		sc->rx_idle_secs++;
	}
	ifp->if_ierrors +=
	    sp->rx_crc_errors +
	    sp->rx_alignment_errors +
	    sp->rx_rnr_errors +
	    sp->rx_overrun_errors;
	/*
	 * If any transmit underruns occured, bump up the transmit
	 * threshold by another 512 bytes (64 * 8).
	 */
	if (sp->tx_underruns) {
		ifp->if_oerrors += sp->tx_underruns;
		if (tx_threshold < 192)
			tx_threshold += 64;
	}

	/*
	 * If we haven't received any packets in FXP_MAC_RX_IDLE seconds,
	 * then assume the receiver has locked up and attempt to clear
	 * the condition by reprogramming the multicast filter. This is
	 * a work-around for a bug in the 82557 where the receiver locks
	 * up if it gets certain types of garbage in the syncronization
	 * bits prior to the packet header. This bug is supposed to only
	 * occur in 10Mbps mode, but has been seen to occur in 100Mbps
	 * mode as well (perhaps due to a 10/100 speed transition).
	 */
	if (sc->rx_idle_secs > FXP_MAX_RX_IDLE) {
		sc->rx_idle_secs = 0;
		fxp_mc_setup(sc);
	}
	/*
	 * If there is no pending command, start another stats
	 * dump. Otherwise punt for now.
	 */
	if (CSR_READ_1(sc, FXP_CSR_SCB_COMMAND) == 0) {
		/*
		 * Start another stats dump.
		 */
		CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND,
		    FXP_SCB_COMMAND_CU_DUMPRESET);
	} else {
		/*
		 * A previous command is still waiting to be accepted.
		 * Just zero our copy of the stats and wait for the
		 * next timer event to update them.
		 */
		sp->tx_good = 0;
		sp->tx_underruns = 0;
		sp->tx_total_collisions = 0;

		sp->rx_good = 0;
		sp->rx_crc_errors = 0;
		sp->rx_alignment_errors = 0;
		sp->rx_rnr_errors = 0;
		sp->rx_overrun_errors = 0;
	}

	/* Tick the MII clock. */
	mii_tick(&sc->sc_mii);

	/*
	 * If IFF_ALLMULTI state changed, we need to reinitialize the chip,
	 * because this is handled by the config block.
	 *
	 * NOTE: This shouldn't ever really happen here.
	 */
	if (((ifp->if_flags ^ oflags) & IFF_ALLMULTI) != 0) {
		if (ifp->if_flags & IFF_DEBUG)
			printf("%s: fxp_tick: allmulti state changed\n",
			    sc->sc_dev.dv_xname);
		fxp_init(sc);
	}

	splx(s);

	/*
	 * Schedule another timeout one second from now.
	 */
	timeout(fxp_tick, sc, hz);
}

/*
 * Stop the interface. Cancels the statistics updater and resets
 * the interface.
 */
void
fxp_stop(sc)
	struct fxp_softc *sc;
{
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct fxp_rxdesc *rxd;
	struct fxp_txsoft *txs;
	int i;

	/*
	 * Cancel stats updater.
	 */
	untimeout(fxp_tick, sc);

	/*
	 * Issue software reset
	 */
	CSR_WRITE_4(sc, FXP_CSR_PORT, FXP_PORT_SELECTIVE_RESET);
	DELAY(10);

	/*
	 * Release any xmit buffers.
	 */
	for (i = 0; i < FXP_NTXCB; i++) {
		txs = FXP_DSTX(sc, i);
		if (txs->txs_mbuf != NULL) {
			bus_dmamap_unload(sc->sc_dmat, txs->txs_dmamap);
			m_freem(txs->txs_mbuf);
			txs->txs_mbuf = NULL;
		}
	}
	sc->sc_txpending = 0;

	/*
	 * Free all the receive buffers then reallocate/reinitialize
	 */
	sc->rfa_head = NULL;
	sc->rfa_tail = NULL;
	for (i = 0; i < FXP_NRFABUFS; i++) {
		rxd = &sc->sc_rxdescs[i];
		if (rxd->fr_mbhead != NULL) {
			bus_dmamap_unload(sc->sc_dmat, rxd->fr_dmamap);
			m_freem(rxd->fr_mbhead);
			rxd->fr_mbhead = NULL;
		}
		if (fxp_add_rfabuf(sc, rxd) != 0) {
			/*
			 * This "can't happen" - we're at splnet()
			 * and we just freed the buffer we need
			 * above.
			 */
			panic("fxp_stop: no buffers!");
		}
	}

	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
	ifp->if_timer = 0;
}

/*
 * Watchdog/transmission transmit timeout handler. Called when a
 * transmission is started on the interface, but no interrupt is
 * received before the timeout. This usually indicates that the
 * card has wedged for some reason.
 */
void
fxp_watchdog(ifp)
	struct ifnet *ifp;
{
	struct fxp_softc *sc = ifp->if_softc;

	printf("%s: device timeout\n", sc->sc_dev.dv_xname);
	ifp->if_oerrors++;

	fxp_init(sc);
}

/*
 * Initialize the interface.  Must be called at splnet().
 */
void
fxp_init(sc)
	struct fxp_softc *sc;
{
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct fxp_cb_config *cbp;
	struct fxp_cb_ias *cb_ias;
	struct fxp_cb_tx *txd;
	int i, prm, allm;

	/*
	 * Cancel any pending I/O
	 */
	fxp_stop(sc);

	sc->sc_flags = 0;

	/*
	 * Initialize base of CBL and RFA memory. Loading with zero
	 * sets it up for regular linear addressing.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, 0);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_BASE);

	fxp_scb_wait(sc);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_RU_BASE);

	/*
	 * Initialize the multicast filter.  Do this now, since we might
	 * have to setup the config block differently.
	 */
	fxp_mc_setup(sc);

	prm = (ifp->if_flags & IFF_PROMISC) ? 1 : 0;
	allm = (ifp->if_flags & IFF_ALLMULTI) ? 1 : 0;

	/*
	 * Initialize base of dump-stats buffer.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL,
	    sc->sc_cddma + FXP_CDSTATSOFF);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_DUMP_ADR);

	cbp = &sc->sc_control_data->fcd_configcb;
	memset(cbp, 0, sizeof(struct fxp_cb_config));

	/*
	 * This copy is kind of disgusting, but there are a bunch of must be
	 * zero and must be one bits in this structure and this is the easiest
	 * way to initialize them all to proper values.
	 */
	memcpy(cbp, fxp_cb_config_template, sizeof(fxp_cb_config_template));

	cbp->cb_status =	0;
	cbp->cb_command =	FXP_CB_COMMAND_CONFIG | FXP_CB_COMMAND_EL;
	cbp->link_addr =	-1;	/* (no) next command */
	cbp->byte_count =	22;	/* (22) bytes to config */
	cbp->rx_fifo_limit =	8;	/* rx fifo threshold (32 bytes) */
	cbp->tx_fifo_limit =	0;	/* tx fifo threshold (0 bytes) */
	cbp->adaptive_ifs =	0;	/* (no) adaptive interframe spacing */
	cbp->rx_dma_bytecount =	0;	/* (no) rx DMA max */
	cbp->tx_dma_bytecount =	0;	/* (no) tx DMA max */
	cbp->dma_bce =		0;	/* (disable) dma max counters */
	cbp->late_scb =		0;	/* (don't) defer SCB update */
	cbp->tno_int =		0;	/* (disable) tx not okay interrupt */
	cbp->ci_int =		0;	/* interrupt on CU not active */
	cbp->save_bf =		prm;	/* save bad frames */
	cbp->disc_short_rx =	!prm;	/* discard short packets */
	cbp->underrun_retry =	1;	/* retry mode (1) on DMA underrun */
	cbp->mediatype =	!sc->phy_10Mbps_only; /* interface mode */
	cbp->nsai =		1;	/* (don't) disable source addr insert */
	cbp->preamble_length =	2;	/* (7 byte) preamble */
	cbp->loopback =		0;	/* (don't) loopback */
	cbp->linear_priority =	0;	/* (normal CSMA/CD operation) */
	cbp->linear_pri_mode =	0;	/* (wait after xmit only) */
	cbp->interfrm_spacing =	6;	/* (96 bits of) interframe spacing */
	cbp->promiscuous =	prm;	/* promiscuous mode */
	cbp->bcast_disable =	0;	/* (don't) disable broadcasts */
	cbp->crscdt =		0;	/* (CRS only) */
	cbp->stripping =	!prm;	/* truncate rx packet to byte count */
	cbp->padding =		1;	/* (do) pad short tx packets */
	cbp->rcv_crc_xfer =	0;	/* (don't) xfer CRC to host */
	cbp->force_fdx =	0;	/* (don't) force full duplex */
	cbp->fdx_pin_en =	1;	/* (enable) FDX# pin */
	cbp->multi_ia =		0;	/* (don't) accept multiple IAs */
	cbp->mc_all =		allm;	/* accept all multicasts */

	FXP_CDCONFIGSYNC(sc, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

	/*
	 * Start the config command/DMA.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, sc->sc_cddma + FXP_CDCONFIGOFF);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_START);
	/* ...and wait for it to complete. */
	do {
		FXP_CDCONFIGSYNC(sc,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
	} while ((cbp->cb_status & FXP_CB_STATUS_C) == 0);

	/*
	 * Initialize the station address.
	 */
	cb_ias = &sc->sc_control_data->fcd_iascb;
	cb_ias->cb_status = 0;
	cb_ias->cb_command = FXP_CB_COMMAND_IAS | FXP_CB_COMMAND_EL;
	cb_ias->link_addr = -1;
	memcpy((void *)cb_ias->macaddr, LLADDR(ifp->if_sadl), ETHER_ADDR_LEN);

	FXP_CDIASSYNC(sc, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

	/*
	 * Start the IAS (Individual Address Setup) command/DMA.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, sc->sc_cddma + FXP_CDIASOFF);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_START);
	/* ...and wait for it to complete. */
	do {
		FXP_CDIASSYNC(sc,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
	} while ((cb_ias->cb_status & FXP_CB_STATUS_C) == 0);

	/*
	 * Initialize the transmit descriptor ring.  txlast is initialized
	 * to the end of the list so that it will wrap around to the first
	 * descriptor when the first packet is transmitted.
	 */
	for (i = 0; i < FXP_NTXCB; i++) {
		txd = FXP_CDTX(sc, i);
		memset(txd, 0, sizeof(struct fxp_cb_tx));
		txd->cb_command = FXP_CB_COMMAND_NOP | FXP_CB_COMMAND_S;
		txd->tbd_array_addr = FXP_CDTBDADDR(sc, i);
		txd->link_addr = FXP_CDTXADDR(sc, FXP_NEXTTX(i));
		FXP_CDTXSYNC(sc, i, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);
	}
	sc->sc_txpending = 0;
	sc->sc_txdirty = 0;
	sc->sc_txlast = FXP_NTXCB - 1;

	/*
	 * Give the transmit ring to the chip.  We do this by pointing
	 * the chip at the last descriptor (which is a NOP|SUSPEND), and
	 * issuing a start command.  It will execute the NOP and then
	 * suspend, pointing at the first descriptor.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, FXP_CDTXADDR(sc, sc->sc_txlast));
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_START);

	/*
	 * Initialize receiver buffer area - RFA.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL,
	    sc->rfa_head->fr_dmamap->dm_segs[0].ds_addr + RFA_ALIGNMENT_FUDGE);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_RU_START);

	/*
	 * Set current media.
	 */
	mii_mediachg(&sc->sc_mii);

	/*
	 * ...all done!
	 */
	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	/*
	 * Start stats updater.
	 */
	timeout(fxp_tick, sc, hz);

	/*
	 * Attempt to start output on the interface.
	 */
	fxp_start(ifp);
}

/*
 * Change media according to request.
 */
int
fxp_mii_mediachange(ifp)
	struct ifnet *ifp;
{
	struct fxp_softc *sc = ifp->if_softc;

	if (ifp->if_flags & IFF_UP)
		mii_mediachg(&sc->sc_mii);
	return (0);
}

/*
 * Notify the world which media we're using.
 */
void
fxp_mii_mediastatus(ifp, ifmr)
	struct ifnet *ifp;
	struct ifmediareq *ifmr;
{
	struct fxp_softc *sc = ifp->if_softc;

	mii_pollstat(&sc->sc_mii);
	ifmr->ifm_status = sc->sc_mii.mii_media_status;
	ifmr->ifm_active = sc->sc_mii.mii_media_active;
}

int
fxp_80c24_mediachange(ifp)
	struct ifnet *ifp;
{

	/* Nothing to do here. */
	return (0);
}

void
fxp_80c24_mediastatus(ifp, ifmr)
	struct ifnet *ifp;
	struct ifmediareq *ifmr;
{
	struct fxp_softc *sc = ifp->if_softc;

	/*
	 * Media is currently-selected media.  We cannot determine
	 * the link status.
	 */
	ifmr->ifm_status = 0;
	ifmr->ifm_active = sc->sc_mii.mii_media.ifm_cur->ifm_media;
}

/*
 * Add a buffer to the end of the RFA buffer list.
 * Return 0 if successful, 1 for failure. A failure results in
 * adding the 'oldm' (if non-NULL) on to the end of the list -
 * tossing out it's old contents and recycling it.
 * The RFA struct is stuck at the beginning of mbuf cluster and the
 * data pointer is fixed up to point just past it.
 */
int
fxp_add_rfabuf(sc, rxd)
	struct fxp_softc *sc;
	struct fxp_rxdesc *rxd;
{
	struct mbuf *m, *oldm;
	struct fxp_rfa *rfa, *p_rfa;
	bus_dmamap_t rxmap;
	u_int32_t v;
	int error, rval = 0;

	oldm = rxd->fr_mbhead;
	rxmap = rxd->fr_dmamap;

	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if (m != NULL) {
		MCLGET(m, M_DONTWAIT);
		if ((m->m_flags & M_EXT) == 0) {
			m_freem(m);
			if (oldm == NULL)
				return 1;
			m = oldm;
			m->m_data = m->m_ext.ext_buf;
			rval = 1;
		}
	} else {
		if (oldm == NULL)
			return 1;
		m = oldm;
		m->m_data = m->m_ext.ext_buf;
		rval = 1;
	}

	rxd->fr_mbhead = m;

	/*
	 * Setup the DMA map for this receive buffer.
	 */
	if (m != oldm) {
		if (oldm != NULL)
			bus_dmamap_unload(sc->sc_dmat, rxmap);
		error = bus_dmamap_load(sc->sc_dmat, rxmap,
		    m->m_ext.ext_buf, MCLBYTES, NULL, BUS_DMA_NOWAIT);
		if (error) {
			printf("%s: can't load rx buffer, error = %d\n",
			    sc->sc_dev.dv_xname, error);
			panic("fxp_add_rfabuf");	/* XXX */
		}
	}

	/*
	 * Move the data pointer up so that the incoming data packet
	 * will be 32-bit aligned.
	 */
	m->m_data += RFA_ALIGNMENT_FUDGE;

	/*
	 * Get a pointer to the base of the mbuf cluster and move
	 * data start past the RFA descriptor.
	 */
	rfa = mtod(m, struct fxp_rfa *);
	m->m_data += sizeof(struct fxp_rfa);
	rfa->size = MCLBYTES - sizeof(struct fxp_rfa) - RFA_ALIGNMENT_FUDGE;

	/*
	 * Initialize the rest of the RFA.
	 */
	rfa->rfa_status = 0;
	rfa->rfa_control = FXP_RFA_CONTROL_EL;
	rfa->actual_size = 0;

	/*
	 * Note that since the RFA is misaligned, we cannot store values
	 * directly.  Instead, we must copy.
	 */
	v = -1;
	memcpy((void *)&rfa->link_addr, &v, sizeof(v));
	memcpy((void *)&rfa->rbd_addr, &v, sizeof(v));

	/*
	 * If there are other buffers already on the list, attach this
	 * one to the end by fixing up the tail to point to this one.
	 */
	if (sc->rfa_head != NULL) {
		p_rfa = (struct fxp_rfa *)
		    (sc->rfa_tail->fr_mbhead->m_ext.ext_buf +
		     RFA_ALIGNMENT_FUDGE);
		sc->rfa_tail->fr_next = rxd;
		v = rxmap->dm_segs[0].ds_addr + RFA_ALIGNMENT_FUDGE;
		memcpy((void *)&p_rfa->link_addr, &v, sizeof(v));
		p_rfa->rfa_control &= ~FXP_RFA_CONTROL_EL;
	} else {
		sc->rfa_head = rxd;
	}
	sc->rfa_tail = rxd;

	bus_dmamap_sync(sc->sc_dmat, rxmap, 0, rxmap->dm_mapsize,
	    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

	return (rval);
}

volatile int
fxp_mdi_read(self, phy, reg)
	struct device *self;
	int phy;
	int reg;
{
	struct fxp_softc *sc = (struct fxp_softc *)self;
	int count = 10000;
	int value;

	CSR_WRITE_4(sc, FXP_CSR_MDICONTROL,
	    (FXP_MDI_READ << 26) | (reg << 16) | (phy << 21));

	while (((value = CSR_READ_4(sc, FXP_CSR_MDICONTROL)) & 0x10000000) == 0
	    && count--)
		DELAY(10);

	if (count <= 0)
		printf("%s: fxp_mdi_read: timed out\n", sc->sc_dev.dv_xname);

	return (value & 0xffff);
}

void
fxp_statchg(self)
	struct device *self;
{

	/* XXX Update ifp->if_baudrate */
}

void
fxp_mdi_write(self, phy, reg, value)
	struct device *self;
	int phy;
	int reg;
	int value;
{
	struct fxp_softc *sc = (struct fxp_softc *)self;
	int count = 10000;

	CSR_WRITE_4(sc, FXP_CSR_MDICONTROL,
	    (FXP_MDI_WRITE << 26) | (reg << 16) | (phy << 21) |
	    (value & 0xffff));

	while((CSR_READ_4(sc, FXP_CSR_MDICONTROL) & 0x10000000) == 0 &&
	    count--)
		DELAY(10);

	if (count <= 0)
		printf("%s: fxp_mdi_write: timed out\n", sc->sc_dev.dv_xname);
}

int
fxp_ioctl(ifp, command, data)
	struct ifnet *ifp;
	u_long command;
	caddr_t data;
{
	struct fxp_softc *sc = ifp->if_softc;
	struct ifreq *ifr = (struct ifreq *)data;
	struct ifaddr *ifa = (struct ifaddr *)data;
	int s, oflags, error = 0;

	s = splnet();

	switch (command) {
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;

		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		case AF_INET:
			fxp_init(sc);
			arp_ifinit(ifp, ifa);
			break;
#endif /* INET */
#ifdef NS
		case AF_NS:
		    {
			 struct ns_addr *ina = &IA_SNS(ifa)->sns_addr;

			 if (ns_nullhost(*ina))
				ina->x_host = *(union ns_host *)
				    LLADDR(ifp->if_sadl);
			 else
				bcopy(ina->x_host.c_host, LLADDR(ifp->if_sadl),
				    ifp->if_addrlen);
			 /* Set new address. */
			 fxp_init(sc);
			 break;
		    }
#endif /* NS */
		default:
			fxp_init(sc);
			break;
		}
		break;

	case SIOCSIFMTU:
		if (ifr->ifr_mtu > ETHERMTU)
			error = EINVAL;
		else
			ifp->if_mtu = ifr->ifr_mtu;
		break;

	case SIOCSIFFLAGS:
		if ((ifp->if_flags & IFF_UP) == 0 &&
		    (ifp->if_flags & IFF_RUNNING) != 0) {
			/*
			 * If interface is marked down and it is running, then
			 * stop it.
			 */
			fxp_stop(sc);
		} else if ((ifp->if_flags & IFF_UP) != 0 &&
			   (ifp->if_flags & IFF_RUNNING) == 0) {
			/*
			 * If interface is marked up and it is stopped, then
			 * start it.
			 */
			fxp_init(sc);
		} else if ((ifp->if_flags & IFF_UP) != 0) {
			/*
			 * Reset the interface to pick up change in any other
			 * flags that affect the hardware state.
			 */
			fxp_init(sc);
		}
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		error = (command == SIOCADDMULTI) ?
		    ether_addmulti(ifr, &sc->sc_ethercom) :
		    ether_delmulti(ifr, &sc->sc_ethercom);

		if (error == ENETRESET) {
			/*
			 * Multicast list has changed; set the hardware
			 * filter accordingly.
			 */
			oflags = ifp->if_flags;
			fxp_mc_setup(sc);

			/*
			 * If IFF_ALLMULTI state changed, we need to
			 * reinitialize the chip, because this is
			 * handled by the config block.
			 */
			if (((ifp->if_flags ^ oflags) & IFF_ALLMULTI) != 0)
				fxp_init(sc);
			error = 0;
		}
		break;

	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		error = ifmedia_ioctl(ifp, ifr, &sc->sc_mii.mii_media, command);
		break;

	default:
		error = EINVAL;
		break;
	}

	splx(s);
	return (error);
}

/*
 * Program the multicast filter.
 *
 * This function must be called at splnet().
 */
void
fxp_mc_setup(sc)
	struct fxp_softc *sc;
{
	struct fxp_cb_mcs *mcsp = &sc->sc_control_data->fcd_mcscb;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct ethercom *ec = &sc->sc_ethercom;
	struct ether_multi *enm;
	struct ether_multistep step;
	int nmcasts;

	/*
	 * If there are transmissions pending, wait until they're
	 * complete.  fxp_intr() will call us when they've drained.
	 */
	if (sc->sc_txpending) {
		sc->sc_flags |= FXPF_NEEDMCSETUP;
		return;
	}
	sc->sc_flags &= ~FXPF_NEEDMCSETUP;

	ifp->if_flags &= ~IFF_ALLMULTI;

	/*
	 * Initialize multicast setup descriptor.
	 */
	nmcasts = 0;
	ETHER_FIRST_MULTI(step, ec, enm);
	while (enm != NULL) {
		/*
		 * Check for too many multicast addresses or if we're
		 * listening to a range.  Either way, we simply have
		 * to accept all multicasts.
		 */
		if (nmcasts >= MAXMCADDR ||
		    memcmp(enm->enm_addrlo, enm->enm_addrhi,
		           ETHER_ADDR_LEN) != 0) {
			/*
			 * Callers of this function must do the
			 * right thing with this.  If we're called
			 * from outside fxp_init(), the caller must
			 * detect if the state if IFF_ALLMULTI changes.
			 * If it does, the caller must then call
			 * fxp_init(), since allmulti is handled by
			 * the config block.
			 */
			ifp->if_flags |= IFF_ALLMULTI;
			return;
		}
		memcpy((void *)&mcsp->mc_addr[nmcasts][0], enm->enm_addrlo,
		    ETHER_ADDR_LEN);
		nmcasts++;
		ETHER_NEXT_MULTI(step, enm);
	}

	mcsp->cb_status = 0;
	mcsp->cb_command = FXP_CB_COMMAND_MCAS | FXP_CB_COMMAND_S;
	mcsp->link_addr = FXP_CDTXADDR(sc, FXP_NEXTTX(sc->sc_txlast));
	mcsp->mc_cnt = nmcasts * ETHER_ADDR_LEN;

	FXP_CDMCSSYNC(sc, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

	/*
	 * Wait until the command unit is not active.  This should never
	 * happen since nothing is queued, but make sure anyway.
	 */
	while ((CSR_READ_1(sc, FXP_CSR_SCB_RUSCUS) >> 6) ==
	    FXP_SCB_CUS_ACTIVE)
		/* nothing */ ;

	/*
	 * Start the multicast setup command/DMA.
	 */
	fxp_scb_wait(sc);
	CSR_WRITE_4(sc, FXP_CSR_SCB_GENERAL, sc->sc_cddma + FXP_CDMCSOFF);
	CSR_WRITE_1(sc, FXP_CSR_SCB_COMMAND, FXP_SCB_COMMAND_CU_START);

	/* ...and wait for it to complete. */
	do {
		FXP_CDMCSSYNC(sc,
		    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);
	} while ((mcsp->cb_status & FXP_CB_STATUS_C) == 0);
}
