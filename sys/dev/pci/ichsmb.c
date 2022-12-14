/*	$NetBSD: ichsmb.c,v 1.60.4.3 2021/12/03 17:49:41 martin Exp $	*/
/*	$OpenBSD: ichiic.c,v 1.44 2020/10/07 11:23:05 jsg Exp $	*/

/*
 * Copyright (c) 2005, 2006 Alexander Yurchenko <grange@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Intel ICH SMBus controller driver.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ichsmb.c,v 1.60.4.3 2021/12/03 17:49:41 martin Exp $");

#include <sys/param.h>
#include <sys/device.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/module.h>

#include <sys/bus.h>

#include <dev/pci/pcidevs.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <dev/ic/i82801lpcreg.h>

#include <dev/i2c/i2cvar.h>

#ifdef ICHIIC_DEBUG
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif

#define ICHIIC_DELAY	100
#define ICHIIC_TIMEOUT	1

struct ichsmb_softc {
	device_t		sc_dev;

	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
	bus_size_t		sc_size;
	pci_chipset_tag_t	sc_pc;
	void *			sc_ih;
	int			sc_poll;
	pci_intr_handle_t	*sc_pihp;

	struct i2c_controller	sc_i2c_tag;
	kmutex_t 		sc_i2c_mutex;
	struct {
		i2c_op_t     op;
		void *       buf;
		size_t       len;
		int          flags;
		volatile int error;
	}			sc_i2c_xfer;
	device_t		sc_i2c_device;
};

static int	ichsmb_match(device_t, cfdata_t, void *);
static void	ichsmb_attach(device_t, device_t, void *);
static int	ichsmb_detach(device_t, int);
static int	ichsmb_rescan(device_t, const char *, const int *);
static void	ichsmb_chdet(device_t, device_t);

static int	ichsmb_i2c_acquire_bus(void *, int);
static void	ichsmb_i2c_release_bus(void *, int);
static int	ichsmb_i2c_exec(void *, i2c_op_t, i2c_addr_t, const void *,
		    size_t, void *, size_t, int);

static int	ichsmb_intr(void *);

#include "ioconf.h"

CFATTACH_DECL3_NEW(ichsmb, sizeof(struct ichsmb_softc),
    ichsmb_match, ichsmb_attach, ichsmb_detach, NULL, ichsmb_rescan,
    ichsmb_chdet, DVF_DETACH_SHUTDOWN);


static int
ichsmb_match(device_t parent, cfdata_t match, void *aux)
{
	struct pci_attach_args *pa = aux;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_INTEL) {
		switch (PCI_PRODUCT(pa->pa_id)) {
		case PCI_PRODUCT_INTEL_6300ESB_SMB:
		case PCI_PRODUCT_INTEL_63XXESB_SMB:
		case PCI_PRODUCT_INTEL_82801AA_SMB:
		case PCI_PRODUCT_INTEL_82801AB_SMB:
		case PCI_PRODUCT_INTEL_82801BA_SMB:
		case PCI_PRODUCT_INTEL_82801CA_SMB:
		case PCI_PRODUCT_INTEL_82801DB_SMB:
		case PCI_PRODUCT_INTEL_82801E_SMB:
		case PCI_PRODUCT_INTEL_82801EB_SMB:
		case PCI_PRODUCT_INTEL_82801FB_SMB:
		case PCI_PRODUCT_INTEL_82801G_SMB:
		case PCI_PRODUCT_INTEL_82801H_SMB:
		case PCI_PRODUCT_INTEL_82801I_SMB:
		case PCI_PRODUCT_INTEL_82801JD_SMB:
		case PCI_PRODUCT_INTEL_82801JI_SMB:
		case PCI_PRODUCT_INTEL_3400_SMB:
		case PCI_PRODUCT_INTEL_6SERIES_SMB:
		case PCI_PRODUCT_INTEL_7SERIES_SMB:
		case PCI_PRODUCT_INTEL_8SERIES_SMB:
		case PCI_PRODUCT_INTEL_9SERIES_SMB:
		case PCI_PRODUCT_INTEL_100SERIES_SMB:
		case PCI_PRODUCT_INTEL_100SERIES_LP_SMB:
		case PCI_PRODUCT_INTEL_2HS_SMB:
		case PCI_PRODUCT_INTEL_3HS_SMB:
		case PCI_PRODUCT_INTEL_3HS_U_SMB:
		case PCI_PRODUCT_INTEL_4HS_H_SMB:
		case PCI_PRODUCT_INTEL_4HS_V_SMB:
		case PCI_PRODUCT_INTEL_CORE4G_M_SMB:
		case PCI_PRODUCT_INTEL_CORE5G_M_SMB:
		case PCI_PRODUCT_INTEL_CMTLK_SMB:
		case PCI_PRODUCT_INTEL_BAYTRAIL_PCU_SMB:
		case PCI_PRODUCT_INTEL_BSW_PCU_SMB:
		case PCI_PRODUCT_INTEL_APL_SMB:
		case PCI_PRODUCT_INTEL_GLK_SMB:
		case PCI_PRODUCT_INTEL_EHL_SMB:
		case PCI_PRODUCT_INTEL_JSL_SMB:
		case PCI_PRODUCT_INTEL_C600_SMBUS:
		case PCI_PRODUCT_INTEL_C600_SMB_0:
		case PCI_PRODUCT_INTEL_C600_SMB_1:
		case PCI_PRODUCT_INTEL_C600_SMB_2:
		case PCI_PRODUCT_INTEL_C610_SMB:
		case PCI_PRODUCT_INTEL_C620_SMB:
		case PCI_PRODUCT_INTEL_C620_SMB_S:
		case PCI_PRODUCT_INTEL_EP80579_SMB:
		case PCI_PRODUCT_INTEL_DH89XXCC_SMB:
		case PCI_PRODUCT_INTEL_DH89XXCL_SMB:
		case PCI_PRODUCT_INTEL_C2000_PCU_SMBUS:
		case PCI_PRODUCT_INTEL_C3K_SMBUS_LEGACY:
		case PCI_PRODUCT_INTEL_495_YU_SMB:
		case PCI_PRODUCT_INTEL_5HS_H_SMB:
		case PCI_PRODUCT_INTEL_5HS_LP_SMB:
			return 1;
		}
	}
	return 0;
}

static void
ichsmb_attach(device_t parent, device_t self, void *aux)
{
	struct ichsmb_softc *sc = device_private(self);
	struct pci_attach_args *pa = aux;
	pcireg_t conf;
	const char *intrstr = NULL;
	char intrbuf[PCI_INTRSTR_LEN];
	int flags;

	sc->sc_dev = self;
	sc->sc_pc = pa->pa_pc;

	pci_aprint_devinfo(pa, NULL);
	mutex_init(&sc->sc_i2c_mutex, MUTEX_DEFAULT, IPL_NONE);

	/* Read configuration */
	conf = pci_conf_read(pa->pa_pc, pa->pa_tag, LPCIB_SMB_HOSTC);
	DPRINTF(("%s: conf 0x%08x\n", device_xname(sc->sc_dev), conf));

	if ((conf & LPCIB_SMB_HOSTC_HSTEN) == 0) {
		aprint_error_dev(self, "SMBus disabled\n");
		goto out;
	}

	/* Map I/O space */
	if (pci_mapreg_map(pa, LPCIB_SMB_BASE, PCI_MAPREG_TYPE_IO, 0,
	    &sc->sc_iot, &sc->sc_ioh, NULL, &sc->sc_size)) {
		aprint_error_dev(self, "can't map I/O space\n");
		goto out;
	}

	sc->sc_poll = 1;
	sc->sc_ih = NULL;
	if (conf & LPCIB_SMB_HOSTC_SMIEN) {
		/* No PCI IRQ */
		aprint_normal_dev(self, "interrupting at SMI\n");
	} else {
		/* Install interrupt handler */
		if (pci_intr_alloc(pa, &sc->sc_pihp, NULL, 0) == 0) {
			intrstr = pci_intr_string(pa->pa_pc, sc->sc_pihp[0],
			    intrbuf, sizeof(intrbuf));
			sc->sc_ih = pci_intr_establish_xname(pa->pa_pc,
			    sc->sc_pihp[0], IPL_BIO, ichsmb_intr, sc,
			    device_xname(sc->sc_dev));
			if (sc->sc_ih != NULL) {
				aprint_normal_dev(self, "interrupting at %s\n",
				    intrstr);
				sc->sc_poll = 0;
			} else {
				pci_intr_release(pa->pa_pc, sc->sc_pihp, 1);
				sc->sc_pihp = NULL;
			}
		}
		if (sc->sc_poll)
			aprint_normal_dev(self, "polling\n");
	}

	sc->sc_i2c_device = NULL;
	flags = 0;
	ichsmb_rescan(self, "i2cbus", &flags);

out:	if (!pmf_device_register(self, NULL, NULL))
		aprint_error_dev(self, "couldn't establish power handler\n");
}

static int
ichsmb_rescan(device_t self, const char *ifattr, const int *flags)
{
	struct ichsmb_softc *sc = device_private(self);
	struct i2cbus_attach_args iba;

	if (!ifattr_match(ifattr, "i2cbus"))
		return 0;

	if (sc->sc_i2c_device)
		return 0;

	/* Attach I2C bus */
	sc->sc_i2c_tag.ic_cookie = sc;
	sc->sc_i2c_tag.ic_acquire_bus = ichsmb_i2c_acquire_bus;
	sc->sc_i2c_tag.ic_release_bus = ichsmb_i2c_release_bus;
	sc->sc_i2c_tag.ic_exec = ichsmb_i2c_exec;

	memset(&iba, 0, sizeof(iba));
	iba.iba_type = I2C_TYPE_SMBUS;
	iba.iba_tag = &sc->sc_i2c_tag;
	sc->sc_i2c_device = config_found_ia(self, ifattr, &iba, iicbus_print);

	return 0;
}

static int
ichsmb_detach(device_t self, int flags)
{
	struct ichsmb_softc *sc = device_private(self);
	int error;

	if (sc->sc_i2c_device) {
		error = config_detach(sc->sc_i2c_device, flags);
		if (error)
			return error;
	}

	mutex_destroy(&sc->sc_i2c_mutex);

	if (sc->sc_ih) {
		pci_intr_disestablish(sc->sc_pc, sc->sc_ih);
		sc->sc_ih = NULL;
	}

	if (sc->sc_pihp) {
		pci_intr_release(sc->sc_pc, sc->sc_pihp, 1);
		sc->sc_pihp = NULL;
	}

	if (sc->sc_size != 0)
		bus_space_unmap(sc->sc_iot, sc->sc_ioh, sc->sc_size);

	return 0;
}

static void
ichsmb_chdet(device_t self, device_t child)
{
	struct ichsmb_softc *sc = device_private(self);

	if (sc->sc_i2c_device == child)
		sc->sc_i2c_device = NULL;
}

static int
ichsmb_i2c_acquire_bus(void *cookie, int flags)
{
	struct ichsmb_softc *sc = cookie;

	if (cold)
		return 0;

	mutex_enter(&sc->sc_i2c_mutex);
	return 0;
}

static void
ichsmb_i2c_release_bus(void *cookie, int flags)
{
	struct ichsmb_softc *sc = cookie;

	if (cold)
		return;

	mutex_exit(&sc->sc_i2c_mutex);
}

static int
ichsmb_i2c_exec(void *cookie, i2c_op_t op, i2c_addr_t addr,
    const void *cmdbuf, size_t cmdlen, void *buf, size_t len, int flags)
{
	struct ichsmb_softc *sc = cookie;
	const uint8_t *b;
	uint8_t ctl = 0, st;
	int retries;
	char fbuf[64];

	DPRINTF(("%s: exec: op %d, addr 0x%02x, cmdlen %zu, len %zu, "
	    "flags 0x%02x\n", device_xname(sc->sc_dev), op, addr, cmdlen,
	    len, flags));

	/* Clear status bits */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS,
	    LPCIB_SMB_HS_INTR | LPCIB_SMB_HS_DEVERR |
	    LPCIB_SMB_HS_BUSERR | LPCIB_SMB_HS_FAILED);
	bus_space_barrier(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS, 1,
	    BUS_SPACE_BARRIER_READ | BUS_SPACE_BARRIER_WRITE);

	/* Wait for bus to be idle */
	for (retries = 100; retries > 0; retries--) {
		st = bus_space_read_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS);
		if (!(st & LPCIB_SMB_HS_BUSY))
			break;
		DELAY(ICHIIC_DELAY);
	}
#ifdef ICHIIC_DEBUG
	snprintb(fbuf, sizeof(fbuf), LPCIB_SMB_HS_BITS, st);
	printf("%s: exec: st %s\n", device_xname(sc->sc_dev), fbuf);
#endif
	if (st & LPCIB_SMB_HS_BUSY)
		return (1);

	if (cold || sc->sc_poll)
		flags |= I2C_F_POLL;

	if (!I2C_OP_STOP_P(op) || cmdlen > 1 || len > 2 ||
	    (cmdlen == 0 && len > 1))
		return (1);

	/* Setup transfer */
	sc->sc_i2c_xfer.op = op;
	sc->sc_i2c_xfer.buf = buf;
	sc->sc_i2c_xfer.len = len;
	sc->sc_i2c_xfer.flags = flags;
	sc->sc_i2c_xfer.error = 0;

	/* Set slave address and transfer direction */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_TXSLVA,
	    LPCIB_SMB_TXSLVA_ADDR(addr) |
	    (I2C_OP_READ_P(op) ? LPCIB_SMB_TXSLVA_READ : 0));

	b = (const uint8_t *)cmdbuf;
	if (cmdlen > 0)
		/* Set command byte */
		bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HCMD, b[0]);

	if (I2C_OP_WRITE_P(op)) {
		/* Write data */
		b = buf;
		if (cmdlen == 0 && len == 1)
			bus_space_write_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HCMD, b[0]);
		else if (len > 0)
			bus_space_write_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HD0, b[0]);
		if (len > 1)
			bus_space_write_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HD1, b[1]);
	}

	/* Set SMBus command */
	if (cmdlen == 0) {
		if (len == 0)
			ctl = LPCIB_SMB_HC_CMD_QUICK;
		else
			ctl = LPCIB_SMB_HC_CMD_BYTE;
	} else if (len == 1)
		ctl = LPCIB_SMB_HC_CMD_BDATA;
	else if (len == 2)
		ctl = LPCIB_SMB_HC_CMD_WDATA;

	if ((flags & I2C_F_POLL) == 0)
		ctl |= LPCIB_SMB_HC_INTREN;

	/* Start transaction */
	ctl |= LPCIB_SMB_HC_START;
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HC, ctl);

	if (flags & I2C_F_POLL) {
		/* Poll for completion */
		DELAY(ICHIIC_DELAY);
		for (retries = 1000; retries > 0; retries--) {
			st = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HS);
			if ((st & LPCIB_SMB_HS_BUSY) == 0)
				break;
			DELAY(ICHIIC_DELAY);
		}
		if (st & LPCIB_SMB_HS_BUSY)
			goto timeout;
		ichsmb_intr(sc);
	} else {
		/* Wait for interrupt */
		if (tsleep(sc, PRIBIO, "iicexec", ICHIIC_TIMEOUT * hz))
			goto timeout;
	}

	if (sc->sc_i2c_xfer.error)
		return (1);

	return (0);

timeout:
	/*
	 * Transfer timeout. Kill the transaction and clear status bits.
	 */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HC,
	    LPCIB_SMB_HC_KILL);
	DELAY(ICHIIC_DELAY);
	st = bus_space_read_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS);
	if ((st & LPCIB_SMB_HS_FAILED) == 0) {
		snprintb(fbuf, sizeof(fbuf), LPCIB_SMB_HS_BITS, st);
		aprint_error_dev(sc->sc_dev, "abort failed, status %s\n",
		    fbuf);
	}
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS, st);
	return (1);
}

static int
ichsmb_intr(void *arg)
{
	struct ichsmb_softc *sc = arg;
	uint8_t st;
	uint8_t *b;
	size_t len;
#ifdef ICHIIC_DEBUG
	char fbuf[64];
#endif

	/* Read status */
	st = bus_space_read_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS);

	/* Clear status bits */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, LPCIB_SMB_HS, st);

	/* XXX Ignore SMBALERT# for now */
	if ((st & LPCIB_SMB_HS_BUSY) != 0 || (st & (LPCIB_SMB_HS_INTR |
	    LPCIB_SMB_HS_DEVERR | LPCIB_SMB_HS_BUSERR | LPCIB_SMB_HS_FAILED |
	    LPCIB_SMB_HS_BDONE)) == 0)
		/* Interrupt was not for us */
		return (0);

#ifdef ICHIIC_DEBUG
	snprintb(fbuf, sizeof(fbuf), LPCIB_SMB_HS_BITS, st);
	printf("%s: intr st %s\n", device_xname(sc->sc_dev), fbuf);
#endif

	/* Check for errors */
	if (st & (LPCIB_SMB_HS_DEVERR | LPCIB_SMB_HS_BUSERR | LPCIB_SMB_HS_FAILED)) {
		sc->sc_i2c_xfer.error = 1;
		goto done;
	}

	if (st & LPCIB_SMB_HS_INTR) {
		if (I2C_OP_WRITE_P(sc->sc_i2c_xfer.op))
			goto done;

		/* Read data */
		b = sc->sc_i2c_xfer.buf;
		len = sc->sc_i2c_xfer.len;
		if (len > 0)
			b[0] = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HD0);
		if (len > 1)
			b[1] = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
			    LPCIB_SMB_HD1);
	}

done:
	if ((sc->sc_i2c_xfer.flags & I2C_F_POLL) == 0)
		wakeup(sc);
	return (1);
}

MODULE(MODULE_CLASS_DRIVER, ichsmb, "pci,iic");

#ifdef _MODULE
#include "ioconf.c"
#endif

static int
ichsmb_modcmd(modcmd_t cmd, void *opaque)
{
	int error = 0;

	switch (cmd) {
	case MODULE_CMD_INIT:
#ifdef _MODULE
		error = config_init_component(cfdriver_ioconf_ichsmb,
		    cfattach_ioconf_ichsmb, cfdata_ioconf_ichsmb);
#endif
		break;
	case MODULE_CMD_FINI:
#ifdef _MODULE
		error = config_fini_component(cfdriver_ioconf_ichsmb,
		    cfattach_ioconf_ichsmb, cfdata_ioconf_ichsmb);
#endif
		break;
	default:
#ifdef _MODULE
		error = ENOTTY;
#endif
		break;
	}

	return error;
}
