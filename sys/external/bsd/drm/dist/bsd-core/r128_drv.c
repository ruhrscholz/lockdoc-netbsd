/* r128_drv.c -- ATI Rage 128 driver -*- linux-c -*-
 * Created: Mon Dec 13 09:47:27 1999 by faith@precisioninsight.com
 */
/*-
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Rickard E. (Rik) Faith <faith@valinux.com>
 *    Gareth Hughes <gareth@valinux.com>
 *
 */

#include "drmP.h"
#include "drm.h"
#include "r128_drm.h"
#include "r128_drv.h"
#include "drm_pciids.h"

/* drv_PCI_IDs comes from drm_pciids.h, generated from drm_pciids.txt. */
static drm_pci_id_list_t r128_pciidlist[] = {
	r128_PCI_IDS
};

int r128_driver_load(struct drm_device * dev, unsigned long flags)
{
	return drm_vblank_init(dev, 1);
}

static void r128_configure(struct drm_device *dev)
{
	dev->driver->driver_features =
	    DRIVER_USE_AGP | DRIVER_USE_MTRR | DRIVER_PCI_DMA |
	    DRIVER_SG | DRIVER_HAVE_DMA | DRIVER_HAVE_IRQ;

	dev->driver->buf_priv_size	= sizeof(drm_r128_buf_priv_t);
	dev->driver->load		= r128_driver_load;
	dev->driver->preclose		= r128_driver_preclose;
	dev->driver->lastclose		= r128_driver_lastclose;
	dev->driver->get_vblank_counter	= r128_get_vblank_counter;
	dev->driver->enable_vblank	= r128_enable_vblank;
	dev->driver->disable_vblank	= r128_disable_vblank;
	dev->driver->irq_preinstall	= r128_driver_irq_preinstall;
	dev->driver->irq_postinstall	= r128_driver_irq_postinstall;
	dev->driver->irq_uninstall	= r128_driver_irq_uninstall;
	dev->driver->irq_handler	= r128_driver_irq_handler;
	dev->driver->dma_ioctl		= r128_cce_buffers;

	dev->driver->ioctls		= r128_ioctls;
	dev->driver->max_ioctl		= r128_max_ioctl;

	dev->driver->name		= DRIVER_NAME;
	dev->driver->desc		= DRIVER_DESC;
	dev->driver->date		= DRIVER_DATE;
	dev->driver->major		= DRIVER_MAJOR;
	dev->driver->minor		= DRIVER_MINOR;
	dev->driver->patchlevel		= DRIVER_PATCHLEVEL;
}

#if defined(__FreeBSD__)

static int
r128_probe(device_t kdev)
{
	return drm_probe(kdev, r128_pciidlist);
}

static int
r128_attach(device_t kdev)
{
	struct drm_device *dev = device_get_softc(kdev);

	dev->driver = malloc(sizeof(struct drm_driver_info), DRM_MEM_DRIVER,
	    M_WAITOK | M_ZERO);

	r128_configure(dev);

	return drm_attach(kdev, r128_pciidlist);
}

static int
r128_detach(device_t kdev)
{
	struct drm_device *dev = device_get_softc(kdev);
	int ret;

	ret = drm_detach(kdev);

	free(dev->driver, DRM_MEM_DRIVER);

	return ret;
}

static device_method_t r128_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		r128_probe),
	DEVMETHOD(device_attach,	r128_attach),
	DEVMETHOD(device_detach,	r128_detach),

	{ 0, 0 }
};

static driver_t r128_driver = {
	"drm",
	r128_methods,
	sizeof(struct drm_device)
};

extern devclass_t drm_devclass;
#if __FreeBSD_version >= 700010
DRIVER_MODULE(r128, vgapci, r128_driver, drm_devclass, 0, 0);
#else
DRIVER_MODULE(r128, pci, r128_driver, drm_devclass, 0, 0);
#endif
MODULE_DEPEND(r128, drm, 1, 1, 1);

#elif   defined(__NetBSD__)

static int
r128drm_probe(device_t parent, cfdata_t match, void *aux)
{
	struct pci_attach_args *pa = aux;

	return drm_probe(pa, r128_pciidlist);
}

static void
r128drm_attach(device_t parent, device_t self, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct drm_device *dev = device_private(self);

	dev->driver = malloc(sizeof(struct drm_driver_info), DRM_MEM_DRIVER,
	    M_WAITOK | M_ZERO);

	r128_configure(dev);

	drm_attach(self, pa, r128_pciidlist);
}

CFATTACH_DECL_NEW(r128drm, sizeof(struct drm_device),
    r128drm_probe, r128drm_attach, drm_detach, NULL);

#ifdef _MODULE

MODULE(MODULE_CLASS_DRIVER, r128drm, NULL);

CFDRIVER_DECL(r128drm, DV_DULL, NULL);
extern struct cfattach r128drm_ca;
static int drmloc[] = { -1 };
static struct cfparent drmparent = {
	"drm", "vga", DVUNIT_ANY
};
static struct cfdata r128drm_cfdata[] = {
	{
		.cf_name = "r128drm",
		.cf_atname = "r128drm",
		.cf_unit = 0,
		.cf_fstate = FSTATE_STAR,
		.cf_loc = drmloc,
		.cf_flags = 0,
		.cf_pspec = &drmparent,
	},
	{ NULL }
};

static int
r128drm_modcmd(modcmd_t cmd, void *arg)
{
	int err;

	switch (cmd) {
	case MODULE_CMD_INIT:
		err = config_cfdriver_attach(&r128drm_cd);
		if (err)
			return err;
		err = config_cfattach_attach("r128drm", &r128drm_ca);
		if (err) {
			config_cfdriver_detach(&r128drm_cd);
			return err;
		}
		err = config_cfdata_attach(r128drm_cfdata, 1);
		if (err) {
			config_cfattach_detach("r128drm", &r128drm_ca);
			config_cfdriver_detach(&r128drm_cd);
			return err;
		}
		return 0;
	case MODULE_CMD_FINI:
		err = config_cfdata_detach(r128drm_cfdata);
		if (err)
			return err;
		config_cfattach_detach("r128drm", &r128drm_ca);
		config_cfdriver_detach(&r128drm_cd);
		return 0;
	default:
		return ENOTTY;
	}
}
#endif /* _MODULE */

#endif
