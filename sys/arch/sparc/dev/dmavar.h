/*
 * Copyright (c) 1994 Peter Galbavy.  All rights reserved.
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
 *	This product includes software developed by Peter Galbavy.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	$Id: dmavar.h,v 1.2 1994/10/02 22:00:19 deraadt Exp $
 */

struct dma_softc {
	struct device sc_dev;			/* us as a device */
	struct sbusdev sc_sd;			/* sbus device */
	struct esp_softc *sc_esp;		/* my scsi */
	struct dma_regs *sc_regs;		/* the registers */
	int	sc_active;			/* DMA active ? */
	int	sc_rev;				/* revision */
	int	sc_node;			/* PROM node ID */
	size_t	sc_dmasize;
	caddr_t	*sc_dmaaddr;
	size_t  *sc_dmalen;
	void (*reset)(struct dma_softc *);	/* reset routine */
	void (*enintr)(struct dma_softc *);	/* enable interrupts */
	void (*start)(struct dma_softc *, caddr_t *, size_t *, int);
	int (*isintr)(struct dma_softc *);	/* intrerrupt ? */
	int (*intr)(struct dma_softc *);	/* intrerrupt ! */
};

/*
 * We are not allowed to touch the DMA "flush" and "drain" bits
 * while it is still thinking about a request (DMA_RP).
 */
#define DMAWAIT(sc)	while (sc->sc_regs->csr & D_R_PEND) DELAY(1)
#define DMAWAIT1(sc)	while (sc->sc_regs->csr & D_DRAINING) DELAY(1)
#define DMAREADY(sc)	while (!(sc->sc_regs->csr & D_DMA_ON)) DELAY(1)

#define DMACSR(sc)	(sc->sc_regs->csr)
#define DMADDR(sc)	(sc->sc_regs->addr)
