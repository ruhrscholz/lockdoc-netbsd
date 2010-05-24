/*	$NetBSD: pci_verbose.c,v 1.1 2010/05/24 20:29:40 pgoyette Exp $	*/

/*
 * Copyright (c) 1997 Zubin D. Dittia.  All rights reserved.
 * Copyright (c) 1995, 1996, 1998, 2000
 *	Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1994 Charles M. Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles M. Hannum.
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
 */

/*
 * PCI autoconfiguration support functions.
 *
 * Note: This file is also built into a userland library (libpci).
 * Pay attention to this when you make modifications.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: pci_verbose.c,v 1.1 2010/05/24 20:29:40 pgoyette Exp $");

#ifdef _KERNEL_OPT
#include "opt_pci.h"
#endif

#include <sys/param.h>

#ifdef _KERNEL
#include <sys/module.h>
#else
#include <pci.h>
#endif

#include <dev/pci/pcireg.h>
#ifdef _KERNEL
#include <dev/pci/pcivar.h>
#endif

#include <dev/pci/pcidevs.h>

/*
 * Descriptions of of known vendors and devices ("products").
 */

#include <dev/pci/pcidevs_data.h>

#ifndef _KERNEL
#include <string.h>
#endif

#ifdef _KERNEL
static int pciverbose_modcmd(modcmd_t, void *);

MODULE(MODULE_CLASS_MISC, pciverbose, NULL);

static int
pciverbose_modcmd(modcmd_t cmd, void *arg)
{
	aprint_normal("%s: cmd %d\n", __func__, cmd);	/* XXX */
	switch (cmd) {
	case MODULE_CMD_INIT:
		pci_findvendor_vec = pci_findvendor;
		pci_findproduct_vec = pci_findproduct;
		pci_unmatched = "unmatched ";
		return 0;
	case MODULE_CMD_FINI:
		pci_findvendor_vec = pci_null;
		pci_findproduct_vec = pci_null;
		pci_unmatched = "";
		return 0;
	default:
		return ENOTTY;
	}
}
#endif /* KERNEL */

static const char *
pci_untokenstring(const uint16_t *token, char *buf, size_t len)
{
	char *cp = buf;

	buf[0] = '\0';
	for (; *token != 0; token++) {
		cp = buf + strlcat(buf, pci_words + *token, len - 2);
		cp[0] = ' ';
		cp[1] = '\0';
	}
	*cp = '\0';
	return cp != buf ? buf : NULL;
}

const char *
pci_findvendor(pcireg_t id_reg)
{
	static char buf[256];
	pci_vendor_id_t vendor = PCI_VENDOR(id_reg);
	size_t n;

	for (n = 0; n < __arraycount(pci_vendors); n++) {
		if (pci_vendors[n] == vendor)
			return pci_untokenstring(&pci_vendors[n+1], buf,
			    sizeof(buf));

		/* Skip Tokens */
		n++;
		while (pci_vendors[n] != 0 && n < __arraycount(pci_vendors))
			n++;
	}
	return (NULL);
}

const char *
pci_findproduct(pcireg_t id_reg)
{
	static char buf[256];
	pci_vendor_id_t vendor = PCI_VENDOR(id_reg);
	pci_product_id_t product = PCI_PRODUCT(id_reg);
	size_t n;

	for (n = 0; n < __arraycount(pci_products); n++) {
		if (pci_products[n] == vendor && pci_products[n+1] == product)
			return pci_untokenstring(&pci_products[n+2], buf,
			    sizeof(buf));

		/* Skip Tokens */
		n += 2;
		while (pci_products[n] != 0 && n < __arraycount(pci_products))
			n++;
	}
	return (NULL);
}

