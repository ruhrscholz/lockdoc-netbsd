/* $NetBSD: if_rl_pci.c,v 1.6 2000/04/30 12:00:41 tsutsui Exp $ */

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
 *	FreeBSD Id: if_rl.c,v 1.17 1999/06/19 20:17:37 wpaul Exp
 */

/*
 * RealTek 8129/8139 PCI NIC driver
 *
 * Supports several extremely cheap PCI 10/100 adapters based on
 * the RealTek chipset. Datasheets can be obtained from
 * www.realtek.com.tw.
 *
 * Written by Bill Paul <wpaul@ctr.columbia.edu>
 * Electrical Engineering Department
 * Columbia University, New York City
 */

/*
 * The RealTek 8139 PCI NIC redefines the meaning of 'low end.' This is
 * probably the worst PCI ethernet controller ever made, with the possible
 * exception of the FEAST chip made by SMC. The 8139 supports bus-master
 * DMA, but it has a terrible interface that nullifies any performance
 * gains that bus-master DMA usually offers.
 *
 * For transmission, the chip offers a series of four TX descriptor
 * registers. Each transmit frame must be in a contiguous buffer, aligned
 * on a longword (32-bit) boundary. This means we almost always have to
 * do mbuf copies in order to transmit a frame, except in the unlikely
 * case where a) the packet fits into a single mbuf, and b) the packet
 * is 32-bit aligned within the mbuf's data area. The presence of only
 * four descriptor registers means that we can never have more than four
 * packets queued for transmission at any one time.
 *
 * Reception is not much better. The driver has to allocate a single large
 * buffer area (up to 64K in size) into which the chip will DMA received
 * frames. Because we don't know where within this region received packets
 * will begin or end, we have no choice but to copy data from the buffer
 * area into mbufs in order to pass the packets up to the higher protocol
 * levels.
 *
 * It's impossible given this rotten design to really achieve decent
 * performance at 100Mbps, unless you happen to have a 400Mhz PII or
 * some equally overmuscled CPU to drive it.
 *
 * On the bright side, the 8139 does have a built-in PHY, although
 * rather than using an MDIO serial interface like most other NICs, the
 * PHY registers are directly accessible through the 8139's register
 * space. The 8139 supports autonegotiation, as well as a 64-bit multicast
 * filter.
 *
 * The 8129 chip is an older version of the 8139 that uses an external PHY
 * chip. The 8129 has a serial MDIO interface for accessing the MII where
 * the 8139 lets you directly access the on-board PHY registers. We need
 * to select which interface to use depending on the chip type.
 */

#include "opt_inet.h"
#include "opt_ns.h"
#include "bpfilter.h"
#include "rnd.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/callout.h>
#include <sys/device.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#ifdef INET
#include <netinet/in.h>
#include <netinet/if_inarp.h>
#endif
#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#if NBPFILTER > 0
#include <net/bpf.h>
#endif
#if NRND > 0
#include <sys/rnd.h>
#endif

#include <machine/bus.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>

#include <dev/mii/mii.h>
#include <dev/mii/miivar.h>

/*
 * Default to using PIO access for this driver. On SMP systems,
 * there appear to be problems with memory mapped mode: it looks like
 * doing too many memory mapped access back to back in rapid succession
 * can hang the bus. I'm inclined to blame this on crummy design/construction
 * on the part of RealTek. Memory mapped mode does appear to work on
 * uniprocessor systems though.
 */
#define RL_USEIOSPACE

#include <dev/ic/rtl81x9reg.h>
#include <dev/ic/rtl81x9var.h>

struct rl_pci_softc {
	struct rl_softc sc_rl;		/* real rl softc */
	
	/* PCI-specific goo.*/
	void *sc_ih;
	pci_chipset_tag_t sc_pc; 	/* PCI chipset */
	pcitag_t sc_pcitag;		/* PCI tag */
};

static struct rl_type rl_pci_devs[] = {
	{ PCI_VENDOR_REALTEK, PCI_PRODUCT_REALTEK_RT8129,
		"RealTek 8129 10/100BaseTX" },
	{ PCI_VENDOR_REALTEK, PCI_PRODUCT_REALTEK_RT8139,
		"RealTek 8139 10/100BaseTX" },
	{ PCI_VENDOR_ACCTON, PCI_PRODUCT_ACCTON_MPX5030,
		"Accton MPX 5030/5038 10/100BaseTX" },
	{ PCI_VENDOR_DELTA, PCI_PRODUCT_DELTA_8139,
		"Delta Electronics 8139 10/100BaseTX" },
	{ PCI_VENDOR_ADDTRON, PCI_PRODUCT_ADDTRON_8139,
		"Addtron Technology 8139 10/100BaseTX" },
#if 0
	{ SIS_VENDORID, SIS_DEVICEID_8139,
		"SiS 900 10/100BaseTX" },
#endif
	{ 0, 0, NULL }
};

const struct rl_type *rl_pci_lookup
	__P((const struct pci_attach_args *));
int rl_pci_match	__P((struct device *, struct cfdata *, void *));
void rl_pci_attach	__P((struct device *, struct device *, void *));

struct cfattach rtk_pci_ca = {
	sizeof(struct rl_pci_softc), rl_pci_match, rl_pci_attach,
};

const struct rl_type *
rl_pci_lookup(pa)
	const struct pci_attach_args *pa;
{
	struct rl_type		*t;

	for (t = rl_pci_devs; t->rl_name != NULL; t++){ 	
		if (PCI_VENDOR(pa->pa_id) == t->rl_vid &&
		    PCI_PRODUCT(pa->pa_id)  == t->rl_did) {
			return (t);
		}
	}
	return (NULL);
}

int
rl_pci_match(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct pci_attach_args *pa = aux;

	if (rl_pci_lookup(pa) != NULL)
		return (1);
	return (0);
}

/*
 * Attach the interface. Allocate softc structures, do ifmedia
 * setup and ethernet/BPF attach.
 */
void
rl_pci_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	int			s, pmreg;
	u_char			eaddr[ETHER_ADDR_LEN];
	pcireg_t		command;
	struct rl_pci_softc *psc = (struct rl_pci_softc *)self;
	struct rl_softc *sc = &psc->sc_rl;
	u_int16_t		rl_did, val;
	struct pci_attach_args *pa = aux;
	pci_chipset_tag_t pc = pa->pa_pc;
	pci_intr_handle_t ih;
	const char *intrstr = NULL;

	psc->sc_pc = pa->pa_pc;
	psc->sc_pcitag = pa->pa_tag;

	s = splimp();

	/*
	 * Handle power management nonsense.
	 */

	if (pci_get_capability(pc, pa->pa_tag, PCI_CAP_PWRMGMT, &pmreg, 0)) {
		command = pci_conf_read(pc, pa->pa_tag, pmreg + 4);
		if (command & RL_PSTATE_MASK) {
			pcireg_t		iobase, membase, irq;

			/* Save important PCI config data. */
			iobase = pci_conf_read(pc, pa->pa_tag, RL_PCI_LOIO);
			membase = pci_conf_read(pc, pa->pa_tag, RL_PCI_LOMEM);
			irq = pci_conf_read(pc, pa->pa_tag, PCI_PRODUCT_DELTA_8139);

			/* Reset the power state. */
			printf("%s: chip is is in D%d power mode "
			"-- setting to D0\n", sc->sc_dev.dv_xname,
			       command & RL_PSTATE_MASK);
			command &= 0xFFFFFFFC;
			pci_conf_write(pc, pa->pa_tag, pmreg + 4, command);

			/* Restore PCI config data. */
			pci_conf_write(pc, pa->pa_tag, RL_PCI_LOIO, iobase);
			pci_conf_write(pc, pa->pa_tag, RL_PCI_LOMEM, membase);
			pci_conf_write(pc, pa->pa_tag, PCI_PRODUCT_DELTA_8139, irq);
		}
	}

	/*
	 * Map control/status registers.
	 */
#ifdef RL_USEIOSPACE
	if (pci_mapreg_map(pa, RL_PCI_LOIO, PCI_MAPREG_TYPE_IO, 0,
	    &sc->rl_btag, &sc->rl_bhandle, NULL, NULL)) {
		printf(": can't map i/o space\n");
		goto fail;
	}
#else
	if (pci_mapreg_map(pa, RL_PCI_LOMEM, PCI_MAPREG_TYPE_MEM, 0,
	    &sc->rl_btag, &sc->rl_bhandle, NULL, NULL)) {
		printf(": can't map i/o space\n");
		goto fail;
	}
#endif

	/* Allocate interrupt */
	if (pci_intr_map(pc, pa->pa_intrtag, pa->pa_intrpin,
	    pa->pa_intrline, &ih)) {
		printf("%s: couldn't map interrupt\n", sc->sc_dev.dv_xname);
		goto fail;
	}
	intrstr = pci_intr_string(pc, ih);
	psc->sc_ih = pci_intr_establish(pc, ih, IPL_NET, rl_intr, sc);
	if (psc->sc_ih == NULL) {
		printf("%s: couldn't establish interrupt",
		    sc->sc_dev.dv_xname);
		if (intrstr != NULL)
			printf(" at %s", intrstr);
		printf("\n");
		goto fail;
	}

	/* Reset the adapter. */
	rl_reset(sc);

	/*
	 * Now read the exact device type from the EEPROM to find
	 * out if it's an 8129 or 8139.
	 */
	rl_did = rl_read_eeprom(sc, RL_EE_PCI_DID, RL_EEADDR_LEN0);

	if (rl_did == PCI_PRODUCT_REALTEK_RT8139 ||
	    rl_did == PCI_PRODUCT_ACCTON_MPX5030 ||
	    rl_did == PCI_PRODUCT_DELTA_8139 ||
	    rl_did == PCI_PRODUCT_ADDTRON_8139
#if 0
	    || rl_did == SIS_DEVICEID_8139
#endif
	    ) {
		printf(": RealTek 8139 Ethernet (id 0x%x)\n", rl_did);
		sc->rl_type = RL_8139;
	} else if (rl_did == PCI_PRODUCT_REALTEK_RT8129) {
		printf(": RealTek 8129 Ethernet (id 0x%x)\n", rl_did);
		sc->rl_type = RL_8129;
	} else {
		printf(": unknown device ID: 0x%x\n", rl_did);
		free(sc, M_DEVBUF);
		goto fail;
	}
	printf("%s: interrupting at %s\n", sc->sc_dev.dv_xname, intrstr);

	/*
	 * Get station address from the EEPROM.
	 */
	val = rl_read_eeprom(sc, RL_EE_EADDR0, RL_EEADDR_LEN0);
	eaddr[0] = val & 0xff;
	eaddr[1] = val >> 8;
	val = rl_read_eeprom(sc, RL_EE_EADDR1, RL_EEADDR_LEN0);
	eaddr[2] = val & 0xff;
	eaddr[3] = val >> 8;
	val = rl_read_eeprom(sc, RL_EE_EADDR2, RL_EEADDR_LEN0);
	eaddr[4] = val & 0xff;
	eaddr[5] = val >> 8;

	/*
	 * A RealTek chip was detected. Inform the world.
	 */
	printf("%s: Ethernet address: %s\n", sc->sc_dev.dv_xname,
	       ether_sprintf(eaddr));

	sc->sc_dmat = pa->pa_dmat;
	
	rl_attach(sc, eaddr);
fail:
	splx(s);
	return;
}
