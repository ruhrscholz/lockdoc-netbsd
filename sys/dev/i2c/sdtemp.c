/*      $NetBSD: sdtemp.c,v 1.35.4.2 2021/12/06 19:33:07 martin Exp $        */

/*
 * Copyright (c) 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Goyette.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: sdtemp.c,v 1.35.4.2 2021/12/06 19:33:07 martin Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kmem.h>
#include <sys/device.h>
#include <sys/kernel.h>
#include <sys/endian.h>
#include <sys/module.h>

#include <dev/sysmon/sysmonvar.h>

#include <dev/i2c/i2cvar.h>
#include <dev/i2c/sdtemp_reg.h>

struct sdtemp_softc {
	device_t sc_dev;
	i2c_tag_t sc_tag;
	int sc_address;

	struct sysmon_envsys *sc_sme;
	envsys_data_t *sc_sensor;
	sysmon_envsys_lim_t sc_deflims;
	uint32_t sc_defprops;
	int sc_resolution;
	uint16_t sc_mfgid;
	uint16_t sc_devid;
	uint16_t sc_devid_masked;
	uint16_t sc_capability;
};

static int  sdtemp_match(device_t, cfdata_t, void *);
static void sdtemp_attach(device_t, device_t, void *);
static int  sdtemp_detach(device_t, int);

CFATTACH_DECL_NEW(sdtemp, sizeof(struct sdtemp_softc),
	sdtemp_match, sdtemp_attach, sdtemp_detach, NULL);

static void	sdtemp_refresh(struct sysmon_envsys *, envsys_data_t *);
static void	sdtemp_get_limits(struct sysmon_envsys *, envsys_data_t *,
				  sysmon_envsys_lim_t *, uint32_t *);
static void	sdtemp_set_limits(struct sysmon_envsys *, envsys_data_t *,
				  sysmon_envsys_lim_t *, uint32_t *);
#ifdef NOT_YET
static int	sdtemp_read_8(struct sdtemp_softc *, uint8_t, uint8_t *);
static int	sdtemp_write_8(struct sdtemp_softc *, uint8_t, uint8_t);
#endif /* NOT YET */
static int	sdtemp_read_16(struct sdtemp_softc *, uint8_t, uint16_t *);
static int	sdtemp_write_16(struct sdtemp_softc *, uint8_t, uint16_t);
static uint32_t	sdtemp_decode_temp(struct sdtemp_softc *, uint16_t);
static bool	sdtemp_pmf_suspend(device_t, const pmf_qual_t *);
static bool	sdtemp_pmf_resume(device_t, const pmf_qual_t *);
/* Device dependent config functions */
static void	sdtemp_config_mcp(struct sdtemp_softc *);
static void	sdtemp_config_idt(struct sdtemp_softc *);

struct sdtemp_dev_entry {
	const uint16_t sdtemp_mfg_id;
	const uint16_t sdtemp_devrev;
	const uint16_t sdtemp_mask;
	void         (*sdtemp_config)(struct sdtemp_softc *);
	const char    *sdtemp_desc;
};

/* Convert sysmon_envsys uKelvin value to simple degC */

#define	__UK2C(uk) (((uk) - 273150000) / 1000000)

/* List of devices known to conform to JEDEC JC42.4 */

#define	CMCP sdtemp_config_mcp
#define	CIDT sdtemp_config_idt

static const struct sdtemp_dev_entry
sdtemp_dev_table[] = {
    { AT_MANUFACTURER_ID,   AT_30TS00_DEVICE_ID,     AT_30TS00_MASK,	  NULL,
	"Atmel AT30TS00" },
    { AT2_MANUFACTURER_ID,  AT2_30TSE004_DEVICE_ID,  AT2_30TSE004_MASK,	  NULL,
	"Atmel AT30TSE004" },
    { GT_MANUFACTURER_ID,   GT_30TS00_DEVICE_ID,     GT_30TS00_MASK,	  NULL,
	"Giantec GT30TS00" },
    { GT2_MANUFACTURER_ID,  GT2_34TS02_DEVICE_ID,    GT2_34TS02_MASK,	  NULL,
	"Giantec GT34TS02" },
    { MAXIM_MANUFACTURER_ID, MAX_6604_DEVICE_ID,     MAX_6604_MASK,	  NULL,
	"Maxim MAX6604" },
    { MAXIM_MANUFACTURER_ID, MAX_6604_2_DEVICE_ID,   MAX_6604_MASK,	  NULL,
	"Maxim MAX6604" },
    { MCP_MANUFACTURER_ID,  MCP_9804_DEVICE_ID,	     MCP_9804_MASK,	  CMCP,
	"Microchip Tech MCP9804" },
    { MCP_MANUFACTURER_ID,  MCP_9805_DEVICE_ID,	     MCP_9805_MASK,	  NULL,
	"Microchip Tech MCP9805/MCP9843" },
    { MCP_MANUFACTURER_ID,  MCP_98242_DEVICE_ID,     MCP_98242_MASK,	  CMCP,
	"Microchip Tech MCP98242" },
    { MCP_MANUFACTURER_ID,  MCP_98243_DEVICE_ID,     MCP_98243_MASK,	  CMCP,
	"Microchip Tech MCP98243" },
    { MCP_MANUFACTURER_ID,  MCP_98244_DEVICE_ID,     MCP_98244_MASK,	  CMCP,
	"Microchip Tech MCP98244" },
    { MCP2_MANUFACTURER_ID, MCP2_EMC1501_DEVICE_ID,  MCP2_EMC1501_MASK,	  NULL,
	"Microchip Tech EMC1501" },
    { ADT_MANUFACTURER_ID,  ADT_7408_DEVICE_ID,	     ADT_7408_MASK,	  NULL,
	"Analog Devices ADT7408" },
    { NXP_MANUFACTURER_ID,  NXP_SE98_DEVICE_ID,	     NXP_SE98_MASK,	  NULL,
	"NXP Semiconductors SE97B/SE98" },
    { NXP_MANUFACTURER_ID,  NXP_SE97_DEVICE_ID,	     NXP_SE97_MASK,	  NULL,
	"NXP Semiconductors SE97" },
    { STTS_MANUFACTURER_ID, STTS_424E_DEVICE_ID,     STTS_424E_MASK,	  NULL,
	"STmicroelectronics STTS424E" },
    { STTS_MANUFACTURER_ID, STTS_424_DEVICE_ID,	     STTS_424_MASK,	  NULL,
	"STmicroelectronics STTS424" },
    { STTS_MANUFACTURER_ID, STTS_2002_DEVICE_ID,     STTS_2002_MASK,	  NULL,
	"STmicroelectronics STTS2002" },
    { STTS_MANUFACTURER_ID, STTS_2004_DEVICE_ID,     STTS_2004_MASK,	  NULL,
	"STmicroelectronics STTS2004" },
    { STTS_MANUFACTURER_ID, STTS_3000_DEVICE_ID,     STTS_3000_MASK,	  NULL,
	"STmicroelectronics STTS3000" },
    { CAT_MANUFACTURER_ID,  CAT_34TS02_DEVICE_ID,    CAT_34TS02_MASK,	  NULL,
	"Catalyst CAT34TS02/CAT6095" },
    { CAT_MANUFACTURER_ID,  CAT_34TS02C_DEVICE_ID,   CAT_34TS02C_MASK,	  NULL,
	"Catalyst CAT34TS02C" },
    { CAT_MANUFACTURER_ID,  CAT_34TS04_DEVICE_ID,    CAT_34TS04_MASK,	  NULL,
	"Catalyst CAT34TS04" },
    { IDT_MANUFACTURER_ID,  IDT_TSE2004GB2_DEVICE_ID,IDT_TSE2004GB2_MASK, NULL,
	"Integrated Device Technology TSE2004GB2" },
    { IDT_MANUFACTURER_ID,  IDT_TS3000B3_DEVICE_ID,  IDT_TS3000B3_MASK,	  CIDT,
	"Integrated Device Technology TS3000B3/TSE2002B3" },
    { IDT_MANUFACTURER_ID,  IDT_TS3000GB0_DEVICE_ID, IDT_TS3000GB0_MASK,  CIDT,
	"Integrated Device Technology TS3000GB0" },
    { IDT_MANUFACTURER_ID,  IDT_TS3000GB2_DEVICE_ID, IDT_TS3000GB2_MASK,  CIDT,
	"Integrated Device Technology TS3000GB2" },
    { IDT_MANUFACTURER_ID,  IDT_TS3001GB2_DEVICE_ID, IDT_TS3001GB2_MASK,  CIDT,
	"Integrated Device Technology TS3001GB2" },
    /*
     * Don't change the location of the following two entries. Device specific
     * entry must be located at above.
     */
    { 0,		    TSE2004AV_ID,	     TSE2004AV_MASK,	  NULL,
	"TSE2004av compliant device (generic driver)" },
    { 0, 0, 0, NULL, "Unknown" }
};

#undef CMCP
#undef CIDT

static const char *temp_resl[] = {
	"0.5C",
	"0.25C",
	"0.125C",
	"0.0625C"
};

static int
sdtemp_lookup(uint16_t mfg, uint16_t devrev)
{
	int i;

	for (i = 0; sdtemp_dev_table[i].sdtemp_mfg_id; i++) {
		if (mfg != sdtemp_dev_table[i].sdtemp_mfg_id)
			continue;
		if ((devrev & sdtemp_dev_table[i].sdtemp_mask) ==
		    sdtemp_dev_table[i].sdtemp_devrev)
			break;
	}
	/* Check TSE2004av */
	if ((sdtemp_dev_table[i].sdtemp_mfg_id == 0)
	    && (SDTEMP_IS_TSE2004AV(devrev) == 0))
			i++; /* Unknown */

	return i;
}

static int
sdtemp_match(device_t parent, cfdata_t cf, void *aux)
{
	struct i2c_attach_args *ia = aux;
	uint16_t mfgid, devid, cap;
	struct sdtemp_softc sc;
	int i, error;

	sc.sc_tag = ia->ia_tag;
	sc.sc_address = ia->ia_addr;

	if ((ia->ia_addr & SDTEMP_ADDRMASK) != SDTEMP_ADDR)
		return 0;

	/*
	 * Verify that we can read the manufacturer ID, Device ID and the
	 * capability
	 */
	error = iic_acquire_bus(sc.sc_tag, 0);
	if (error)
		return 0;
	error = sdtemp_read_16(&sc, SDTEMP_REG_MFG_ID,  &mfgid) |
		sdtemp_read_16(&sc, SDTEMP_REG_DEV_REV, &devid) |
		sdtemp_read_16(&sc, SDTEMP_REG_CAPABILITY, &cap);
	iic_release_bus(sc.sc_tag, 0);

	if (error)
		return 0;

	i = sdtemp_lookup(mfgid, devid);
	if ((sdtemp_dev_table[i].sdtemp_mfg_id == 0) &&
	    (sdtemp_dev_table[i].sdtemp_devrev == 0)) {
		aprint_debug("sdtemp: No match for mfg 0x%04x dev 0x%02x "
		    "rev 0x%02x at address 0x%02x\n", mfgid, devid >> 8,
		    devid & 0xff, sc.sc_address);
		return 0;
	}

	/*
	 * Check by SDTEMP_IS_TSE2004AV() might not be enough, so check the
	 * alarm capability, too.
	 */
	if ((cap & SDTEMP_CAP_HAS_ALARM) == 0)
		return 0;

	return I2C_MATCH_ADDRESS_AND_PROBE;
}

static void
sdtemp_attach(device_t parent, device_t self, void *aux)
{
	struct sdtemp_softc *sc = device_private(self);
	struct i2c_attach_args *ia = aux;
	uint16_t mfgid, devid;
	int i, error;

	sc->sc_tag = ia->ia_tag;
	sc->sc_address = ia->ia_addr;
	sc->sc_dev = self;

	error = iic_acquire_bus(sc->sc_tag, 0);
	if (error)
		return;

	if ((error = sdtemp_read_16(sc, SDTEMP_REG_MFG_ID,  &mfgid)) != 0 ||
	    (error = sdtemp_read_16(sc, SDTEMP_REG_DEV_REV, &devid)) != 0) {
		iic_release_bus(sc->sc_tag, 0);
		aprint_error(": attach error %d\n", error);
		return;
	}
	sc->sc_mfgid = mfgid;
	sc->sc_devid = devid;
	i = sdtemp_lookup(mfgid, devid);
	sc->sc_devid_masked = devid & sdtemp_dev_table[i].sdtemp_mask;

	aprint_naive(": Temp Sensor\n");
	aprint_normal(": %s Temp Sensor\n", sdtemp_dev_table[i].sdtemp_desc);

	if (sdtemp_dev_table[i].sdtemp_mfg_id == 0) {
		if (SDTEMP_IS_TSE2004AV(devid))
			aprint_normal_dev(self, "TSE2004av compliant. "
			    "Manufacturer ID 0x%04hx, Device revision 0x%02x\n",
			    mfgid, devid & TSE2004AV_REV);
		else {
			aprint_error_dev(self,
			    "mfg 0x%04x dev 0x%02x rev 0x%02x at addr 0x%02x\n",
			    mfgid, devid >> 8, devid & 0xff, ia->ia_addr);
			iic_release_bus(sc->sc_tag, 0);
			aprint_error_dev(self, "It should no happen. "
			    "Why attach() found me?\n");
			return;
		}
	}

	error = sdtemp_read_16(sc, SDTEMP_REG_CAPABILITY, &sc->sc_capability);
	aprint_debug_dev(self, "capability reg = %04x\n", sc->sc_capability);
	sc->sc_resolution
	    = __SHIFTOUT(sc->sc_capability, SDTEMP_CAP_RESOLUTION);
	/*
	 * Call device dependent function here. Currently, it's used for
	 * the resolution.
	 *
	 * IDT's devices and some Microchip's devices have the resolution
	 * register in the vendor specific registers area. The devices'
	 * resolution bits in the capability register are not the maximum
	 * resolution but the current value of the setting.
	 */
	if (sdtemp_dev_table[i].sdtemp_config != NULL)
		sdtemp_dev_table[i].sdtemp_config(sc);

	aprint_normal_dev(self, "%s accuracy",
	    (sc->sc_capability & SDTEMP_CAP_ACCURACY_1C) ? "high" : "default");
	if ((sc->sc_capability & SDTEMP_CAP_WIDER_RANGE) != 0)
		aprint_normal(", wider range");
	aprint_normal(", %s resolution", temp_resl[sc->sc_resolution]);
	if ((sc->sc_capability & SDTEMP_CAP_VHV) != 0)
		aprint_debug(", high voltage standoff");
	aprint_debug(", %s timeout",
	    (sc->sc_capability & SDTEMP_CAP_TMOUT) ? "25-35ms" : "10-60ms");
	if ((sc->sc_capability & SDTEMP_CAP_EVSD) != 0)
		aprint_normal(", event with shutdown");
	aprint_normal("\n");
	/*
	 * Alarm capability is required;  if not present, this is likely
	 * not a real sdtemp device.
	 */
	if (error != 0 || (sc->sc_capability & SDTEMP_CAP_HAS_ALARM) == 0) {
		iic_release_bus(sc->sc_tag, 0);
		aprint_error_dev(self,
		    "required alarm capability not present!\n");
		return;
	}
	/* Set the configuration to defaults. */
	error = sdtemp_write_16(sc, SDTEMP_REG_CONFIG, 0);
	if (error != 0) {
		iic_release_bus(sc->sc_tag, 0);
		aprint_error_dev(self, "error %d writing config register\n",
		    error);
		return;
	}
	iic_release_bus(sc->sc_tag, 0);

	/* Hook us into the sysmon_envsys subsystem */
	sc->sc_sme = sysmon_envsys_create();
	sc->sc_sme->sme_name = device_xname(self);
	sc->sc_sme->sme_cookie = sc;
	sc->sc_sme->sme_refresh = sdtemp_refresh;
	sc->sc_sme->sme_get_limits = sdtemp_get_limits;
	sc->sc_sme->sme_set_limits = sdtemp_set_limits;

	sc->sc_sensor = kmem_zalloc(sizeof(envsys_data_t), KM_NOSLEEP);
	if (!sc->sc_sensor) {
		aprint_error_dev(self, "unable to allocate sc_sensor\n");
		goto bad2;
	}

	/* Initialize sensor data. */
	sc->sc_sensor->units =  ENVSYS_STEMP;
	sc->sc_sensor->state = ENVSYS_SINVALID;
	sc->sc_sensor->flags |= ENVSYS_FMONLIMITS;
	(void)strlcpy(sc->sc_sensor->desc, device_xname(self),
	    sizeof(sc->sc_sensor->desc));
	snprintf(sc->sc_sensor->desc, sizeof(sc->sc_sensor->desc),
	    "DIMM %d temperature", sc->sc_address - SDTEMP_ADDR);

	/* Now attach the sensor */
	if (sysmon_envsys_sensor_attach(sc->sc_sme, sc->sc_sensor)) {
		aprint_error_dev(self, "unable to attach sensor\n");
		goto bad;
	}

	/* Register the device */
	error = sysmon_envsys_register(sc->sc_sme);
	if (error) {
		aprint_error_dev(self, "error %d registering with sysmon\n",
		    error);
		goto bad;
	}

	if (!pmf_device_register(self, sdtemp_pmf_suspend, sdtemp_pmf_resume))
		aprint_error_dev(self, "couldn't establish power handler\n");

	/* Retrieve and display hardware monitor limits */
	sdtemp_get_limits(sc->sc_sme, sc->sc_sensor, &sc->sc_deflims,
	    &sc->sc_defprops);
	aprint_normal_dev(self, "Hardware limits: ");
	i = 0;
	if (sc->sc_defprops & PROP_WARNMIN) {
		aprint_normal("low %dC",
		              __UK2C(sc->sc_deflims.sel_warnmin));
		i++;
	}
	if (sc->sc_defprops & PROP_WARNMAX) {
		aprint_normal("%shigh %dC ", (i)?", ":"",
			      __UK2C(sc->sc_deflims.sel_warnmax));
		i++;
	}
	if (sc->sc_defprops & PROP_CRITMAX) {
		aprint_normal("%scritical %dC ", (i)?", ":"",
			      __UK2C(sc->sc_deflims.sel_critmax));
		i++;
	}
	aprint_normal("%s\n", (i)?"":"none set");

	return;

bad:
	kmem_free(sc->sc_sensor, sizeof(envsys_data_t));
bad2:
	sysmon_envsys_destroy(sc->sc_sme);
}

static int
sdtemp_detach(device_t self, int flags)
{
	struct sdtemp_softc *sc = device_private(self);

	pmf_device_deregister(self);

	if (sc->sc_sme)
		sysmon_envsys_unregister(sc->sc_sme);
	if (sc->sc_sensor)
		kmem_free(sc->sc_sensor, sizeof(envsys_data_t));

	return 0;
}

/* Retrieve current limits from device, and encode in uKelvins */
static void
sdtemp_get_limits(struct sysmon_envsys *sme, envsys_data_t *edata,
		  sysmon_envsys_lim_t *limits, uint32_t *props)
{
	struct sdtemp_softc *sc = sme->sme_cookie;
	uint16_t lim;

	*props = 0;
	if (iic_acquire_bus(sc->sc_tag, 0) != 0)
		return;

	if (sdtemp_read_16(sc, SDTEMP_REG_LOWER_LIM, &lim) == 0 && lim != 0) {
		limits->sel_warnmin = sdtemp_decode_temp(sc, lim);
		*props |= PROP_WARNMIN;
	}
	if (sdtemp_read_16(sc, SDTEMP_REG_UPPER_LIM, &lim) == 0 && lim != 0) {
		limits->sel_warnmax = sdtemp_decode_temp(sc, lim);
		*props |= PROP_WARNMAX;
	}
	if (sdtemp_read_16(sc, SDTEMP_REG_CRIT_LIM, &lim) == 0 && lim != 0) {
		limits->sel_critmax = sdtemp_decode_temp(sc, lim);
		*props |= PROP_CRITMAX;
	}
	iic_release_bus(sc->sc_tag, 0);
	if (*props != 0)
		*props |= PROP_DRIVER_LIMITS;
}

/* Send current limit values to the device */
static void
sdtemp_set_limits(struct sysmon_envsys *sme, envsys_data_t *edata,
		  sysmon_envsys_lim_t *limits, uint32_t *props)
{
	uint16_t val;
	struct sdtemp_softc *sc = sme->sme_cookie;

	if (limits == NULL) {
		limits = &sc->sc_deflims;
		props  = &sc->sc_defprops;
	}
	if (iic_acquire_bus(sc->sc_tag, 0) != 0)
		return;

	if (*props & PROP_WARNMIN) {
		val = __UK2C(limits->sel_warnmin);
		(void)sdtemp_write_16(sc, SDTEMP_REG_LOWER_LIM,
					(val << 4) & SDTEMP_TEMP_MASK);
	}
	if (*props & PROP_WARNMAX) {
		val = __UK2C(limits->sel_warnmax);
		(void)sdtemp_write_16(sc, SDTEMP_REG_UPPER_LIM,
					(val << 4) & SDTEMP_TEMP_MASK);
	}
	if (*props & PROP_CRITMAX) {
		val = __UK2C(limits->sel_critmax);
		(void)sdtemp_write_16(sc, SDTEMP_REG_CRIT_LIM,
					(val << 4) & SDTEMP_TEMP_MASK);
	}
	iic_release_bus(sc->sc_tag, 0);

	/*
	 * If at least one limit is set that we can handle, and no
	 * limits are set that we cannot handle, tell sysmon that
	 * the driver will take care of monitoring the limits!
	 */
	if (*props & (PROP_CRITMIN | PROP_BATTCAP | PROP_BATTWARN))
		*props &= ~PROP_DRIVER_LIMITS;
	else if (*props & PROP_LIMITS)
		*props |= PROP_DRIVER_LIMITS;
	else
		*props &= ~PROP_DRIVER_LIMITS;
}

#ifdef NOT_YET	/* All registers on these sensors are 16-bits */

/* Read a 8-bit value from a register */
static int
sdtemp_read_8(struct sdtemp_softc *sc, uint8_t reg, uint8_t *valp)
{
	int error;

	error = iic_exec(sc->sc_tag, I2C_OP_READ_WITH_STOP,
	    sc->sc_address, &reg, 1, valp, sizeof(*valp), 0);

	return error;
}

static int
sdtemp_write_8(struct sdtemp_softc *sc, uint8_t reg, uint8_t val)
{
	return iic_exec(sc->sc_tag, I2C_OP_WRITE_WITH_STOP,
	    sc->sc_address, &reg, 1, &val, sizeof(val), 0);
}
#endif /* NOT_YET */

/* Read a 16-bit value from a register */
static int
sdtemp_read_16(struct sdtemp_softc *sc, uint8_t reg, uint16_t *valp)
{
	int error;

	error = iic_exec(sc->sc_tag, I2C_OP_READ_WITH_STOP,
	    sc->sc_address, &reg, 1, valp, sizeof(*valp), 0);
	if (error)
		return error;

	*valp = be16toh(*valp);

	return 0;
}

static int
sdtemp_write_16(struct sdtemp_softc *sc, uint8_t reg, uint16_t val)
{
	uint16_t temp;

	temp = htobe16(val);
	return iic_exec(sc->sc_tag, I2C_OP_WRITE_WITH_STOP,
	    sc->sc_address, &reg, 1, &temp, sizeof(temp), 0);
}

static uint32_t
sdtemp_decode_temp(struct sdtemp_softc *sc, uint16_t temp)
{
	uint32_t val;
	int32_t stemp;

	/* Get only the temperature bits */
	temp &= SDTEMP_TEMP_MASK;

	/* If necessary, extend the sign bit */
	if ((sc->sc_capability & SDTEMP_CAP_WIDER_RANGE) &&
	    (temp & SDTEMP_TEMP_NEGATIVE))
		temp |= SDTEMP_TEMP_SIGN_EXT;

	/* Mask off only bits valid within current resolution */
	temp &= ~(0x7 >> sc->sc_resolution);

	/* Treat as signed and extend to 32-bits */
	stemp = (int16_t)temp;

	/* Now convert from 0.0625 (1/16) deg C increments to microKelvins */
	val = (stemp * 62500) + 273150000;

	return val;
}

static void
sdtemp_refresh(struct sysmon_envsys *sme, envsys_data_t *edata)
{
	struct sdtemp_softc *sc = sme->sme_cookie;
	uint16_t val;
	int error;

	error = iic_acquire_bus(sc->sc_tag, 0);
	if (error) {
		edata->state = ENVSYS_SINVALID;
		return;
	}

	error = sdtemp_read_16(sc, SDTEMP_REG_AMBIENT_TEMP, &val);
	iic_release_bus(sc->sc_tag, 0);

	if (error) {
		edata->state = ENVSYS_SINVALID;
		return;
	}

	edata->value_cur = sdtemp_decode_temp(sc, val);

	/* Now check for limits */
	if ((edata->upropset & PROP_DRIVER_LIMITS) == 0)
		edata->state = ENVSYS_SVALID;
	else if ((val & SDTEMP_ABOVE_CRIT) &&
		    (edata->upropset & PROP_CRITMAX))
		edata->state = ENVSYS_SCRITOVER;
	else if ((val & SDTEMP_ABOVE_UPPER) &&
		    (edata->upropset & PROP_WARNMAX))
		edata->state = ENVSYS_SWARNOVER;
	else if ((val & SDTEMP_BELOW_LOWER) &&
		    (edata->upropset & PROP_WARNMIN))
		edata->state = ENVSYS_SWARNUNDER;
	else
		edata->state = ENVSYS_SVALID;
}

/*
 * Power management functions
 *
 * We go into "shutdown" mode at suspend time, and return to normal
 * mode upon resume.  This reduces power consumption by disabling
 * the A/D converter.
 */

static bool
sdtemp_pmf_suspend(device_t dev, const pmf_qual_t *qual)
{
	struct sdtemp_softc *sc = device_private(dev);
	int error;
	uint16_t config;

	error = iic_acquire_bus(sc->sc_tag, 0);
	if (error != 0)
		return false;

	error = sdtemp_read_16(sc, SDTEMP_REG_CONFIG, &config);
	if (error == 0) {
		config |= SDTEMP_CONFIG_SHUTDOWN_MODE;
		error = sdtemp_write_16(sc, SDTEMP_REG_CONFIG, config);
	}
	iic_release_bus(sc->sc_tag, 0);
	return (error == 0);
}

static bool
sdtemp_pmf_resume(device_t dev, const pmf_qual_t *qual)
{
	struct sdtemp_softc *sc = device_private(dev);
	int error;
	uint16_t config;

	error = iic_acquire_bus(sc->sc_tag, 0);
	if (error != 0)
		return false;

	error = sdtemp_read_16(sc, SDTEMP_REG_CONFIG, &config);
	if (error == 0) {
		config &= ~SDTEMP_CONFIG_SHUTDOWN_MODE;
		error = sdtemp_write_16(sc, SDTEMP_REG_CONFIG, config);
	}
	iic_release_bus(sc->sc_tag, 0);
	return (error == 0);
}

/* Device dependent config functions */

static void
sdtemp_config_mcp(struct sdtemp_softc *sc)
{
	int rv;
	uint8_t resolreg;

	/* Note that MCP9805 has no resolution register */
	switch (sc->sc_devid_masked) {
	case MCP_9804_DEVICE_ID:
	case MCP_98242_DEVICE_ID:
	case MCP_98243_DEVICE_ID:
		resolreg = SDTEMP_REG_MCP_RESOLUTION_9804;
		break;
	case MCP_98244_DEVICE_ID:
		resolreg = SDTEMP_REG_MCP_RESOLUTION_98244;
		break;
	default:
		aprint_error("%s: %s: unknown device ID (%04hx)\n",
		    device_xname(sc->sc_dev), __func__, sc->sc_devid_masked);
		return;
	}

	/*
	 * Set resolution to the max.
	 *
	 * Even if it fails, the resolution will be the default. It's not a
	 * fatal error.
	 */
	rv = sdtemp_write_16(sc, resolreg, SDTEMP_CAP_RESOLUTION_MAX);
	if (rv == 0)
		sc->sc_resolution = SDTEMP_CAP_RESOLUTION_MAX;
	else
		aprint_debug_dev(sc->sc_dev,
		    "error %d writing resolution register\n", rv);
}

static void
sdtemp_config_idt(struct sdtemp_softc *sc)
{
	int rv;

	/*
	 * Set resolution to the max.
	 *
	 * Even if it fails, the resolution will be the default. It's not a
	 * fatal error.
	 */
	rv = sdtemp_write_16(sc, SDTEMP_REG_IDT_RESOLUTION,
	    __SHIFTIN(SDTEMP_CAP_RESOLUTION_MAX, SDTEMP_CAP_RESOLUTION));
	if (rv == 0)
		sc->sc_resolution = SDTEMP_CAP_RESOLUTION_MAX;
	else
		aprint_debug_dev(sc->sc_dev,
		    "error %d writing resolution register\n", rv);
}

MODULE(MODULE_CLASS_DRIVER, sdtemp, "i2cexec,sysmon_envsys");

#ifdef _MODULE
#include "ioconf.c"
#endif

static int
sdtemp_modcmd(modcmd_t cmd, void *opaque)
{
	int error = 0;

	switch (cmd) {
	case MODULE_CMD_INIT:
#ifdef _MODULE
		error = config_init_component(cfdriver_ioconf_sdtemp,
		    cfattach_ioconf_sdtemp, cfdata_ioconf_sdtemp);
#endif
		return error;
	case MODULE_CMD_FINI:
#ifdef _MODULE
		error = config_fini_component(cfdriver_ioconf_sdtemp,
		    cfattach_ioconf_sdtemp, cfdata_ioconf_sdtemp);
#endif
		return error;
	default:
		return ENOTTY;
	}
}
