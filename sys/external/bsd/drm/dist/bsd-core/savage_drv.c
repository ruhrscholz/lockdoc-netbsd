/* savage_drv.c -- Savage DRI driver
 */
/*-
 * Copyright 2005 Eric Anholt
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
 * ERIC ANHOLT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <anholt@FreeBSD.org>
 */

#include "drmP.h"
#include "drm.h"
#include "savage_drm.h"
#include "savage_drv.h"
#include "drm_pciids.h"

/* drv_PCI_IDs comes from drm_pciids.h, generated from drm_pciids.txt. */
static drm_pci_id_list_t savage_pciidlist[] = {
	savage_PCI_IDS
};

static void savage_configure(struct drm_device *dev)
{
	dev->driver->driver_features =
	    DRIVER_USE_AGP | DRIVER_USE_MTRR | DRIVER_PCI_DMA |
	    DRIVER_HAVE_DMA;

	dev->driver->buf_priv_size	= sizeof(drm_savage_buf_priv_t);
	dev->driver->load		= savage_driver_load;
	dev->driver->firstopen		= savage_driver_firstopen;
	dev->driver->lastclose		= savage_driver_lastclose;
	dev->driver->unload		= savage_driver_unload;
	dev->driver->reclaim_buffers_locked = savage_reclaim_buffers;
	dev->driver->dma_ioctl		= savage_bci_buffers;

	dev->driver->ioctls		= savage_ioctls;
	dev->driver->max_ioctl		= savage_max_ioctl;

	dev->driver->name		= DRIVER_NAME;
	dev->driver->desc		= DRIVER_DESC;
	dev->driver->date		= DRIVER_DATE;
	dev->driver->major		= DRIVER_MAJOR;
	dev->driver->minor		= DRIVER_MINOR;
	dev->driver->patchlevel		= DRIVER_PATCHLEVEL;
}

#if defined(__FreeBSD__)

static int
savage_probe(device_t kdev)
{
	return drm_probe(kdev, savage_pciidlist);
}

static int
savage_attach(device_t kdev)
{
	struct drm_device *dev = device_get_softc(kdev);

	dev->driver = malloc(sizeof(struct drm_driver_info), DRM_MEM_DRIVER,
	    M_WAITOK | M_ZERO);

	savage_configure(dev);

	return drm_attach(kdev, savage_pciidlist);
}

static int
savage_detach(device_t kdev)
{
	struct drm_device *dev = device_get_softc(kdev);
	int ret;

	ret = drm_detach(kdev);

	free(dev->driver, DRM_MEM_DRIVER);

	return ret;
}

static device_method_t savage_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		savage_probe),
	DEVMETHOD(device_attach,	savage_attach),
	DEVMETHOD(device_detach,	savage_detach),

	{ 0, 0 }
};

static driver_t savage_driver = {
	"drm",
	savage_methods,
	sizeof(struct drm_device)
};

extern devclass_t drm_devclass;
#if __FreeBSD_version >= 700010
DRIVER_MODULE(savage, vgapci, savage_driver, drm_devclass, 0, 0);
#else
DRIVER_MODULE(savage, pci, savage_driver, drm_devclass, 0, 0);
#endif
MODULE_DEPEND(savage, drm, 1, 1, 1);

#elif   defined(__NetBSD__)

static int
savagedrm_probe(device_t parent, cfdata_t match, void *aux)
{
	struct pci_attach_args *pa = aux;

	return drm_probe(pa, savage_pciidlist);
}

static void
savagedrm_attach(device_t parent, device_t self, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct drm_device *dev = device_private(self);

	dev->driver = malloc(sizeof(struct drm_driver_info), DRM_MEM_DRIVER,
	    M_WAITOK | M_ZERO);

	savage_configure(dev);

	drm_attach(self, pa, savage_pciidlist);
}

CFATTACH_DECL_NEW(savagedrm, sizeof(struct drm_device),
    savagedrm_probe, savagedrm_attach, drm_detach, NULL);

#ifdef _MODULE

MODULE(MODULE_CLASS_DRIVER, savagedrm, NULL);

CFDRIVER_DECL(savagedrm, DV_DULL, NULL);
extern struct cfattach savagedrm_ca;
static int drmloc[] = { -1 };
static struct cfparent drmparent = {
	"drm", "vga", DVUNIT_ANY
};
static struct cfdata savagedrm_cfdata[] = {
	{
		.cf_name = "savagedrm",
		.cf_atname = "savagedrm",
		.cf_unit = 0,
		.cf_fstate = FSTATE_STAR,
		.cf_loc = drmloc,
		.cf_flags = 0,
		.cf_pspec = &drmparent,
	},
	{ NULL }
};

static int
savagedrm_modcmd(modcmd_t cmd, void *arg)
{
	int err;

	switch (cmd) {
	case MODULE_CMD_INIT:
		err = config_cfdriver_attach(&savagedrm_cd);
		if (err)
			return err;
		err = config_cfattach_attach("savagedrm", &savagedrm_ca);
		if (err) {
			config_cfdriver_detach(&savagedrm_cd);
			return err;
		}
		err = config_cfdata_attach(savagedrm_cfdata, 1);
		if (err) {
			config_cfattach_detach("savagedrm", &savagedrm_ca);
			config_cfdriver_detach(&savagedrm_cd);
			return err;
		}
		return 0;
	case MODULE_CMD_FINI:
		err = config_cfdata_detach(savagedrm_cfdata);
		if (err)
			return err;
		config_cfattach_detach("savagedrm", &savagedrm_ca);
		config_cfdriver_detach(&savagedrm_cd);
		return 0;
	default:
		return ENOTTY;
	}
}
#endif /* _MODULE */

#endif
