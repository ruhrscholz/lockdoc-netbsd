/*	$NetBSD: viaenv.c,v 1.18 2007/01/20 18:44:26 xtraeme Exp $	*/

/*
 * Copyright (c) 2000 Johan Danielsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of author nor the names of any contributors may
 *    be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS
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
 * Driver for the hardware monitoring and power management timer 
 * in the VIA VT82C686A and VT8231 South Bridges.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: viaenv.c,v 1.18 2007/01/20 18:44:26 xtraeme Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>

#ifdef __HAVE_TIMECOUNTER
#include <machine/bus.h>
#include <dev/ic/acpipmtimer.h>
#endif

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcidevs.h>

#include <dev/sysmon/sysmonvar.h>

#ifdef VIAENV_DEBUG
unsigned int viaenv_debug = 0;
#define DPRINTF(X) do { if (viaenv_debug) printf X ; } while(0)
#else
#define DPRINTF(X)
#endif

#define VIANUMSENSORS 10	/* three temp, two fan, five voltage */

struct viaenv_softc {
	struct device sc_dev;
	bus_space_tag_t sc_iot;
	bus_space_handle_t sc_ioh;
	bus_space_handle_t sc_pm_ioh;

	int     sc_fan_div[2];	/* fan RPM divisor */

	envsys_tre_data_t sc_data[VIANUMSENSORS];
	envsys_basic_info_t sc_info[VIANUMSENSORS];

	struct simplelock sc_slock;
	struct timeval sc_lastread;

	struct sysmon_envsys sc_sysmon;
};

static const struct envsys_range viaenv_ranges[] = {
	{ 0, 2,		ENVSYS_STEMP },
	{ 3, 4,		ENVSYS_SFANRPM },
	{ 1, 0,		ENVSYS_SVOLTS_AC },	/* none */
	{ 5, 11,	ENVSYS_SVOLTS_DC },
	{ 1, 0,		ENVSYS_SOHMS },		/* none */
	{ 1, 0,		ENVSYS_SWATTS },	/* none */
	{ 1, 0,		ENVSYS_SAMPS },		/* none */
};

/* autoconf(9) glue */
static int viaenv_match(struct device *, struct cfdata *, void *);
static void viaenv_attach(struct device *, struct device *, void *);

CFATTACH_DECL(viaenv, sizeof(struct viaenv_softc),
    viaenv_match, viaenv_attach, NULL, NULL);

/* envsys(4) glue */
static int viaenv_gtredata(struct sysmon_envsys *, envsys_tre_data_t *);
static int viaenv_streinfo(struct sysmon_envsys *, envsys_basic_info_t *);

static int val_to_uK(unsigned int);
static int val_to_rpm(unsigned int, int);
static long val_to_uV(unsigned int, int);

static int
viaenv_match(struct device *parent, struct cfdata *match, void *aux)
{
	struct pci_attach_args *pa = (struct pci_attach_args *)aux;

	if (PCI_VENDOR(pa->pa_id) != PCI_VENDOR_VIATECH)
		return 0;

	switch (PCI_PRODUCT(pa->pa_id)) {
	case PCI_PRODUCT_VIATECH_VT82C686A_SMB:
	case PCI_PRODUCT_VIATECH_VT8231_PWR:
		return 1;
	default:
		return 0;
	}
}

/*
 * XXX there doesn't seem to exist much hard documentation on how to
 * convert the raw values to usable units, this code is more or less
 * stolen from the Linux driver, but changed to suit our conditions
 */

/*
 * lookup-table to translate raw values to uK, this is the same table
 * used by the Linux driver (modulo units); there is a fifth degree
 * polynomial that supposedly been used to generate this table, but I
 * haven't been able to figure out how -- it doesn't give the same values
 */

static const long val_to_temp[] = {
	20225, 20435, 20645, 20855, 21045, 21245, 21425, 21615, 21785, 21955,
	22125, 22285, 22445, 22605, 22755, 22895, 23035, 23175, 23315, 23445,
	23565, 23695, 23815, 23925, 24045, 24155, 24265, 24365, 24465, 24565,
	24665, 24765, 24855, 24945, 25025, 25115, 25195, 25275, 25355, 25435,
	25515, 25585, 25655, 25725, 25795, 25865, 25925, 25995, 26055, 26115,
	26175, 26235, 26295, 26355, 26405, 26465, 26515, 26575, 26625, 26675,
	26725, 26775, 26825, 26875, 26925, 26975, 27025, 27065, 27115, 27165,
	27205, 27255, 27295, 27345, 27385, 27435, 27475, 27515, 27565, 27605,
	27645, 27685, 27735, 27775, 27815, 27855, 27905, 27945, 27985, 28025,
	28065, 28105, 28155, 28195, 28235, 28275, 28315, 28355, 28405, 28445,
	28485, 28525, 28565, 28615, 28655, 28695, 28735, 28775, 28825, 28865,
	28905, 28945, 28995, 29035, 29075, 29125, 29165, 29205, 29245, 29295,
	29335, 29375, 29425, 29465, 29505, 29555, 29595, 29635, 29685, 29725,
	29765, 29815, 29855, 29905, 29945, 29985, 30035, 30075, 30125, 30165,
	30215, 30255, 30305, 30345, 30385, 30435, 30475, 30525, 30565, 30615,
	30655, 30705, 30755, 30795, 30845, 30885, 30935, 30975, 31025, 31075,
	31115, 31165, 31215, 31265, 31305, 31355, 31405, 31455, 31505, 31545,
	31595, 31645, 31695, 31745, 31805, 31855, 31905, 31955, 32005, 32065,
	32115, 32175, 32225, 32285, 32335, 32395, 32455, 32515, 32575, 32635,
	32695, 32755, 32825, 32885, 32955, 33025, 33095, 33155, 33235, 33305,
	33375, 33455, 33525, 33605, 33685, 33765, 33855, 33935, 34025, 34115,
	34205, 34295, 34395, 34495, 34595, 34695, 34805, 34905, 35015, 35135,
	35245, 35365, 35495, 35615, 35745, 35875, 36015, 36145, 36295, 36435,
	36585, 36745, 36895, 37065, 37225, 37395, 37575, 37755, 37935, 38125,
	38325, 38525, 38725, 38935, 39155, 39375, 39605, 39835, 40075, 40325,
	40575, 40835, 41095, 41375, 41655, 41935,
};

/* use above table to convert values to temperatures in micro-Kelvins */
static int
val_to_uK(unsigned int val)
{
	int     i = val / 4;
	int     j = val % 4;

	assert(i >= 0 && i <= 255);

	if (j == 0 || i == 255)
		return val_to_temp[i] * 10000;

	/* is linear interpolation ok? */
	return (val_to_temp[i] * (4 - j) +
	    val_to_temp[i + 1] * j) * 2500 /* really: / 4 * 10000 */ ;
}

static int
val_to_rpm(unsigned int val, int div)
{

	if (val == 0)
		return 0;

	return 1350000 / val / div;
}

static long
val_to_uV(unsigned int val, int index)
{
	static const long mult[] =
	    {1250000, 1250000, 1670000, 2600000, 6300000};

	assert(index >= 0 && index <= 4);

	return (25LL * val + 133) * mult[index] / 2628;
}

#define VIAENV_TSENS3	0x1f
#define VIAENV_TSENS1	0x20
#define VIAENV_TSENS2	0x21
#define VIAENV_VSENS1	0x22
#define VIAENV_VSENS2	0x23
#define VIAENV_VCORE	0x24
#define VIAENV_VSENS3	0x25
#define VIAENV_VSENS4	0x26
#define VIAENV_FAN1	0x29
#define VIAENV_FAN2	0x2a
#define VIAENV_FANCONF	0x47	/* fan configuration */
#define VIAENV_TLOW	0x49	/* temperature low order value */
#define VIAENV_TIRQ	0x4b	/* temperature interrupt configuration */

#define VIAENV_GENCFG	0x40	/* general configuration */
#define VIAENV_GENCFG_TMR32	(1 << 11)	/* 32-bit PM timer */
#define VIAENV_GENCFG_PMEN	(1 << 15)	/* enable PM I/O space */
#define VIAENV_PMBASE	0x48	/* power management I/O space base */
#define VIAENV_PMSIZE	128	/* HWM and power management I/O space size */
#define VIAENV_PM_TMR	0x08	/* PM timer */
#define VIAENV_HWMON_CONF	0x70	/* HWMon I/O base */
#define VIAENV_HWMON_CTL	0x74	/* HWMon control register */

static void
viaenv_refresh_sensor_data(struct viaenv_softc *sc)
{
	static const struct timeval onepointfive =  { 1, 500000 };
	struct timeval t, utv;
	uint8_t v, v2;
	int i;

	/* Read new values at most once every 1.5 seconds. */
	timeradd(&sc->sc_lastread, &onepointfive, &t);
	getmicrouptime(&utv);
	i = timercmp(&utv, &t, >);
	if (i)
		sc->sc_lastread = utv;

	if (i == 0)
		return;

	/* temperature */
	v = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_TIRQ);
	v2 = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_TSENS1);
	DPRINTF(("TSENS1 = %d\n", (v2 << 2) | (v >> 6)));
	sc->sc_data[0].cur.data_us = val_to_uK((v2 << 2) | (v >> 6));
	sc->sc_data[0].validflags = ENVSYS_FVALID | ENVSYS_FCURVALID;

	v = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_TLOW);
	v2 = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_TSENS2);
	DPRINTF(("TSENS2 = %d\n", (v2 << 2) | ((v >> 4) & 0x3)));
	sc->sc_data[1].cur.data_us =
	    val_to_uK((v2 << 2) | ((v >> 4) & 0x3));
	sc->sc_data[1].validflags = ENVSYS_FVALID | ENVSYS_FCURVALID;

	v2 = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_TSENS3);
	DPRINTF(("TSENS3 = %d\n", (v2 << 2) | (v >> 6)));
	sc->sc_data[2].cur.data_us = val_to_uK((v2 << 2) | (v >> 6));
	sc->sc_data[2].validflags = ENVSYS_FVALID | ENVSYS_FCURVALID;

	v = bus_space_read_1(sc->sc_iot, sc->sc_ioh, VIAENV_FANCONF);

	sc->sc_fan_div[0] = 1 << ((v >> 4) & 0x3);
	sc->sc_fan_div[1] = 1 << ((v >> 6) & 0x3);

	/* fan */
	for (i = 3; i <= 4; i++) {
		v = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    VIAENV_FAN1 + i - 3);
		DPRINTF(("FAN%d = %d / %d\n", i - 3, v,
		    sc->sc_fan_div[i - 3]));
		sc->sc_data[i].cur.data_us = val_to_rpm(v,
		    sc->sc_fan_div[i - 3]);
		sc->sc_data[i].validflags =
		    ENVSYS_FVALID | ENVSYS_FCURVALID;
	}

	/* voltage */
	for (i = 5; i <= 9; i++) {
		v = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    VIAENV_VSENS1 + i - 5);
		DPRINTF(("V%d = %d\n", i - 5, v));
		sc->sc_data[i].cur.data_us = val_to_uV(v, i - 5);
		sc->sc_data[i].validflags =
		    ENVSYS_FVALID | ENVSYS_FCURVALID;
	}
}

static void
viaenv_attach(struct device *parent, struct device *self, void *aux)
{
	struct viaenv_softc *sc = (struct viaenv_softc *)self;
	struct pci_attach_args *pa = (struct pci_attach_args *)aux;
	pcireg_t iobase, control;
	int i;

	aprint_naive("\n");
	aprint_normal(": VIA Technologies ");
	switch (PCI_PRODUCT(pa->pa_id)) {
	case PCI_PRODUCT_VIATECH_VT82C686A_SMB:
		aprint_normal("VT82C686A Hardware Monitor\n");
		break;
	case PCI_PRODUCT_VIATECH_VT8231_PWR:
		aprint_normal("VT8231 Hardware Monitor\n");
		break;
	default:
		aprint_normal("Unknown Hardware Monitor\n");
		break;
	}

	iobase = pci_conf_read(pa->pa_pc, pa->pa_tag, VIAENV_HWMON_CONF);
	DPRINTF(("%s: iobase 0x%x\n", sc->sc_dev.dv_xname, iobase));
	control = pci_conf_read(pa->pa_pc, pa->pa_tag, VIAENV_HWMON_CTL);

	/* Check if the Hardware Monitor enable bit is set */
	if ((control & 1) == 0) {
		aprint_normal("%s : Hardware Monitor disabled\n",
		    sc->sc_dev.dv_xname);
		goto nohwm;
	}

	/* Map Hardware Monitor I/O space */
	sc->sc_iot = pa->pa_iot;
	if (bus_space_map(sc->sc_iot, iobase & 0xff80,
	    VIAENV_PMSIZE, 0, &sc->sc_ioh)) {
		aprint_error("%s: failed to map I/O space\n",
		    sc->sc_dev.dv_xname);
		goto nohwm;
	}

	/* Initialize sensors */
	for (i = 0; i < VIANUMSENSORS; ++i) {
		sc->sc_data[i].sensor = sc->sc_info[i].sensor = i;
		sc->sc_data[i].validflags = (ENVSYS_FVALID | ENVSYS_FCURVALID);
		sc->sc_info[i].validflags = ENVSYS_FVALID;
		sc->sc_data[i].warnflags = ENVSYS_WARN_OK;
	}

	for (i = 0; i <= 2; i++)
		sc->sc_data[i].units = sc->sc_info[i].units = ENVSYS_STEMP;

#define COPYDESCR(x, y) 				\
	do {						\
		strlcpy((x), (y), sizeof(x));		\
	} while (0)

	COPYDESCR(sc->sc_info[0].desc, "TSENS1");
	COPYDESCR(sc->sc_info[1].desc, "TSENS2");
	COPYDESCR(sc->sc_info[2].desc, "TSENS3");

	for (i = 3; i <= 4; i++)
		sc->sc_data[i].units = sc->sc_info[i].units = ENVSYS_SFANRPM;
	
	COPYDESCR(sc->sc_info[3].desc, "FAN1");
	COPYDESCR(sc->sc_info[4].desc, "FAN2");

	for (i = 5; i <= 9; ++i)
		sc->sc_data[i].units = sc->sc_info[i].units = ENVSYS_SVOLTS_DC;

	COPYDESCR(sc->sc_info[5].desc, "VSENS1");	/* CPU core (2V) */
	COPYDESCR(sc->sc_info[6].desc, "VSENS2");	/* NB core? (2.5V) */
	COPYDESCR(sc->sc_info[7].desc, "Vcore");	/* Vcore (3.3V) */
	COPYDESCR(sc->sc_info[8].desc, "VSENS3");	/* VSENS3 (5V) */
	COPYDESCR(sc->sc_info[9].desc, "VSENS4");	/* VSENS4 (12V) */

	/* Get initial set of sensor values. */
	viaenv_refresh_sensor_data(sc);

#undef COPYDESCR

	/*
	 * Hook into the System Monitor.
	 */
	sc->sc_sysmon.sme_ranges = viaenv_ranges;
	sc->sc_sysmon.sme_sensor_info = sc->sc_info;
	sc->sc_sysmon.sme_sensor_data = sc->sc_data;
	sc->sc_sysmon.sme_cookie = sc;

	sc->sc_sysmon.sme_gtredata = viaenv_gtredata;
	sc->sc_sysmon.sme_streinfo = viaenv_streinfo;

	sc->sc_sysmon.sme_nsensors = VIANUMSENSORS;
	sc->sc_sysmon.sme_envsys_version = 1000;

	if (sysmon_envsys_register(&sc->sc_sysmon))
		printf("%s: unable to register with sysmon\n",
		    sc->sc_dev.dv_xname);

nohwm:
#ifdef __HAVE_TIMECOUNTER
	/* Check if power management I/O space is enabled */
	control = pci_conf_read(pa->pa_pc, pa->pa_tag, VIAENV_GENCFG);
	if ((control & VIAENV_GENCFG_PMEN) == 0) {
                aprint_normal("%s: Power Managament controller disabled\n",
		    sc->sc_dev.dv_xname);
                goto nopm;
        }

        /* Map power management I/O space */
        iobase = pci_conf_read(pa->pa_pc, pa->pa_tag, VIAENV_PMBASE);
        if (bus_space_map(sc->sc_iot, PCI_MAPREG_IO_ADDR(iobase),
            VIAENV_PMSIZE, 0, &sc->sc_pm_ioh)) {
                aprint_error("%s: failed to map PM I/O space\n",
		    sc->sc_dev.dv_xname);
                goto nopm;
        }

	/* Attach our PM timer with the generic acpipmtimer function */
	acpipmtimer_attach(&sc->sc_dev, sc->sc_iot, sc->sc_pm_ioh,
	    VIAENV_PM_TMR,
	    ((control & VIAENV_GENCFG_TMR32) ? ACPIPMT_32BIT : 0));

nopm:
#endif /* __HAVE_TIMECOUNTER */
	return;
}

static int
viaenv_gtredata(struct sysmon_envsys *sme, envsys_tre_data_t *tred)
{
	struct viaenv_softc *sc = sme->sme_cookie;

	simple_lock(&sc->sc_slock);

	viaenv_refresh_sensor_data(sc);
	*tred = sc->sc_data[tred->sensor];

	simple_unlock(&sc->sc_slock);

	return 0;
}

static int
viaenv_streinfo(struct sysmon_envsys *sme, envsys_basic_info_t *binfo)
{
	/* Not implemented */
	binfo->validflags = 0;

	return 0;
}
