/* $NetBSD: pcn.c,v 1.2 2007/10/26 01:16:27 nisimura Exp $ */

#include <sys/param.h>
#include <sys/socket.h>
 
#include <netinet/in.h>
#include <netinet/in_systm.h>
 
#include <lib/libsa/stand.h>
#include <lib/libsa/net.h>

#include <dev/ic/am79900reg.h>
#include <dev/ic/lancereg.h>
#include <dev/pci/if_pcnreg.h>

#include "globals.h"

/*
 * - reverse endian access every CSR.
 * - no vtophys() translation, vaddr_t == paddr_t. 
 * - PIPT writeback cache aware.
 */
#define CSR_WRITE_2(l, r, v)	out16rb((l)->csr+(r), (v))
#define CSR_READ_2(l, r)	in16rb((l)->csr+(r))
#define CSR_WRITE_4(l, r, v) 	out32rb((l)->csr+(r), (v))
#define CSR_READ_4(l, r)	in32rb((l)->csr+(r))
#define VTOPHYS(va) 		(uint32_t)(va)
#define wbinv(adr, siz)		_wbinv(VTOPHYS(adr), (uint32_t)(siz))
#define inv(adr, siz)		_inv(VTOPHYS(adr), (uint32_t)(siz))
#define DELAY(n)		delay(n)

void *pcn_init(void *);
int pcn_send(void *, char *, unsigned);
int pcn_recv(void *, char *, unsigned, unsigned);

struct desc {
	uint32_t xd0, xd1, xd2;
	uint32_t hole;
};

#define FRAMESIZE	1536

struct local {
	struct desc TxD;
	struct desc RxD[2];
	uint8_t rxstore[2][FRAMESIZE];
	unsigned csr, rx;
	unsigned phy, bmsr, anlpar;
};

unsigned pcn_mii_read(struct local *, int, int);
void pcn_mii_write(struct local *, int, int, int);
static unsigned pcn_csr_read(struct local *, int);
static void pcn_csr_write(struct local *, int, int);
static unsigned pcn_bcr_read(struct local *, int);
static void pcn_bcr_write(struct local *, int, int);
static void mii_initphy(struct local *l);

void *
pcn_init(void *cookie)
{
	unsigned tag, val, loop;
	struct local *l;
	struct desc *TxD, *RxD;
	uint8_t *en;
	struct leinit initblock, *ib;

	if (pcifinddev(0x1022, 0x2000, &tag) != 0) {
		printf("pcn NIC not found\n");
		return NULL;
	}

	l = alloc(sizeof(struct local));
	memset(l, 0, sizeof(struct local));
	l->csr = pcicfgread(tag, 0x14); /* use mem space */

	(void)CSR_READ_2(l, PCN16_RESET);
	(void)CSR_READ_2(l, PCN32_RESET);
	DELAY(1000); /* 1 milli second */
	CSR_WRITE_4(l, PCN32_RDP, 0);

	mii_initphy(l);
	en = cookie;
	val = pcn_csr_read(l, LE_CSR12); en[0] = val; en[1] = (val >> 8);
	val = pcn_csr_read(l, LE_CSR13); en[2] = val; en[3] = (val >> 8);
	val = pcn_csr_read(l, LE_CSR14); en[4] = val; en[5] = (val >> 8);
#if 1
	printf("MAC address %02x:%02x:%02x:%02x:%02x:%02x\n",
		en[0], en[1], en[2], en[3], en[4], en[5]);
#endif

	TxD = &l->TxD;
	RxD = &l->RxD[0];
	RxD[0].xd0 = htole32(VTOPHYS(l->rxstore[0]));
	RxD[0].xd1 = htole32(LE_R1_OWN | LE_R1_ONES | FRAMESIZE);
	RxD[1].xd0 = htole32(VTOPHYS(l->rxstore[1]));
	RxD[1].xd1 = htole32(LE_R1_OWN | LE_R1_ONES | FRAMESIZE);
	l->rx = 0;

	ib = &initblock;
	ib->init_rdra = htole32(VTOPHYS(RxD));
	ib->init_tdra = htole32(VTOPHYS(TxD));
	ib->init_mode = htole32(0 | ((2 - 1) << 28) | ((2 - 1) << 20));
	ib->init_padr[0] =
	    htole32(en[0] | (en[1] << 8) | (en[2] << 16) | (en[3] << 24));
	ib->init_padr[1] =
	    htole32(en[4] | (en[5] << 8));

	pcn_csr_write(l, LE_CSR3, LE_C3_MISSM|LE_C3_IDONM|LE_C3_DXSUFLO);
	pcn_csr_write(l, LE_CSR4, LE_C4_DMAPLUS|LE_C4_APAD_XMT|
	    LE_C4_MFCOM|LE_C4_RCVCCOM|LE_C4_TXSTRTM);
	pcn_csr_write(l, LE_CSR5, 0);

	wbinv(&initblock, sizeof(initblock));
	pcn_csr_write(l, LE_CSR1, VTOPHYS(&initblock) & 0xffff);
	pcn_csr_write(l, LE_CSR2, (VTOPHYS(&initblock) >> 16) & 0xffff);
	pcn_csr_write(l, LE_CSR0, LE_C0_INIT);
	loop = 10000;
	do {
		DELAY(10);
	} while (--loop > 0 && !(pcn_csr_read(l, LE_CSR0) & LE_C0_IDON));
	if (loop == 0)
		printf("pcn: timeout processing init block\n");
	pcn_csr_write(l, LE_CSR0, 0);

	return l;
}

int
pcn_send(void *dev, char *buf, unsigned len)
{
	struct local *l = dev;
	struct desc *TxD;
	unsigned loop;

	wbinv(buf, len);
	TxD = &l->TxD;
	TxD->xd0 = htole32(VTOPHYS(buf));
	TxD->xd1 = htole32(LE_T1_OWN | LE_T1_ONES | LE_BCNT(len));
	wbinv(TxD, sizeof(struct desc));
	loop = 100;
	do {
		if ((le32toh(TxD->xd1) & LE_T1_OWN) == 0)
			goto done;
		DELAY(10);
		inv(TxD, sizeof(struct desc));
	} while (--loop > 0);
	printf("xmit failed\n");
	return -1;
  done:
	return len;
}

int
pcn_recv(void *dev, char *buf, unsigned maxlen, unsigned timo)
{
	struct local *l = dev;
	struct desc *RxD;
	unsigned bound, rxstat, len;
	uint8_t *ptr;

	bound = 1000 * timo;
printf("recving with %u sec. timeout\n", timo);
  again:
	RxD = &l->RxD[l->rx];
	do {
		inv(RxD, sizeof(struct desc));
		rxstat = le32toh(RxD->xd1);
		if ((rxstat & LE_R1_OWN) == 0)
			goto gotone;
		DELAY(1000);	/* 1 milli second */
	} while (bound-- > 0);
	errno = 0;
	return -1;
  gotone:
	if (rxstat & LE_R1_ERR) {
		RxD->xd1 |= htole32(LE_R1_OWN);
		RxD->xd2 = 0;
		wbinv(RxD, sizeof(struct desc));
		l->rx ^= 1;
		goto again;
	}
	/* good frame */
	len = (rxstat & LE_R1_BCNT_MASK) - 4 /* HASFCS */;
	if (len > maxlen)
		len = maxlen;
	ptr = l->rxstore[l->rx];
	inv(ptr, len);
	memcpy(buf, ptr, len);
	RxD->xd1 |= htole32(LE_R1_OWN);
	RxD->xd2 = 0;
	wbinv(RxD, sizeof(struct desc));
	l->rx ^= 1;
	return len;
}

#define MREG(v)		((v)<< 0)
#define MPHY(v)		((v)<< 5)

unsigned
pcn_mii_read(struct local *l, int phy, int reg)
{

	pcn_bcr_write(l, LE_BCR33, MREG(reg) | MPHY(phy));
	return (pcn_bcr_read(l, LE_BCR34) & LE_B34_MIIMD);
}

void
pcn_mii_write(struct local *l, int phy, int reg, int val)
{
	pcn_bcr_write(l, LE_BCR33, MREG(reg) | MPHY(phy));
	pcn_bcr_write(l, LE_BCR34, val);
}

static unsigned
pcn_csr_read(struct local *l, int r)
{
	CSR_WRITE_4(l, PCN32_RAP, r);
	return CSR_READ_4(l, PCN32_RDP);
}

static void
pcn_csr_write(struct local *l, int r, int v)
{
	CSR_WRITE_4(l, PCN32_RAP, r);
	CSR_WRITE_4(l, PCN32_RDP, v);
}

static unsigned
pcn_bcr_read(struct local *l, int r)
{
	CSR_WRITE_4(l, PCN32_RAP, r);
	return CSR_READ_4(l, PCN32_BDP);
}

static void
pcn_bcr_write(struct local *l, int r, int v)
{
	CSR_WRITE_4(l, PCN32_RAP, r);
	CSR_WRITE_4(l, PCN32_BDP, v);
}

#define MII_BMCR	0x00 	/* Basic mode control register (rw) */
#define  BMCR_RESET	0x8000	/* reset */
#define  BMCR_AUTOEN	0x1000	/* autonegotiation enable */
#define  BMCR_ISO	0x0400	/* isolate */
#define  BMCR_STARTNEG	0x0200	/* restart autonegotiation */
#define MII_BMSR	0x01	/* Basic mode status register (ro) */

static void
mii_initphy(struct local *l)
{
	int phy, ctl, sts, bound;

	for (phy = 0; phy < 32; phy++) {
		ctl = pcn_mii_read(l, phy, MII_BMCR);
		sts = pcn_mii_read(l, phy, MII_BMSR);
		if (ctl != 0xffff && sts != 0xffff)
			goto found;
	}
	printf("MII: no PHY found\n");
	return;
  found:
	ctl = pcn_mii_read(l, phy, MII_BMCR);
	pcn_mii_write(l, phy, MII_BMCR, ctl | BMCR_RESET);
	bound = 100;
	do {
		DELAY(10);
		ctl = pcn_mii_read(l, phy, MII_BMCR);
		if (ctl == 0xffff) {
			printf("MII: PHY %d has died after reset\n", phy);
			return;
		}
	} while (bound-- > 0 && (ctl & BMCR_RESET));
	if (bound == 0) {
		printf("PHY %d reset failed\n", phy);
	}
	ctl &= ~BMCR_ISO;
	pcn_mii_write(l, phy, MII_BMCR, ctl);
	sts = pcn_mii_read(l, phy, MII_BMSR) |
	    pcn_mii_read(l, phy, MII_BMSR); /* read twice */
	l->phy = phy;
	l->bmsr = sts;
}

#if 0
static void
mii_dealan(struct local *, unsigned timo)
{
	unsigned anar, bound;

	anar = ANAR_TX_FD | ANAR_TX | ANAR_10_FD | ANAR_10 | ANAR_CSMA;
	pcn_mii_write(l, l->phy, MII_ANAR, anar);
	pcn_mii_write(l, l->phy, MII_BMCR, BMCR_AUTOEN | BMCR_STARTNEG);
	l->anlpar = 0;
	bound = getsecs() + timo;
	do {
		l->bmsr = pcn_mii_read(l, l->phy, MII_BMSR) |
		   pcn_mii_read(l, l->phy, MII_BMSR); /* read twice */
		if ((l->bmsr & BMSR_LINK) && (l->bmsr & BMSR_ACOMP)) {
			l->anlpar = pcn_mii_read(l, l->phy, MII_ANLPAR);
			break;
		}
		DELAY(10 * 1000);
	} while (getsecs() < bound);
	return;
}
#endif
