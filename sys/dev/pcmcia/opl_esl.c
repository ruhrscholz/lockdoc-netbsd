/* $NetBSD: opl_esl.c,v 1.16 2008/03/15 23:14:38 cube Exp $ */

/*
 * Copyright (c) 2001 Jared D. McNeill <jmcneill@invisible.ca>
 * All rights reserved.
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
 *	This product includes software developed by Jared D. McNeill.
 * 4. Neither the name of the author nor the names of any contributors may
 f
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: opl_esl.c,v 1.16 2008/03/15 23:14:38 cube Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/conf.h>
#include <sys/select.h>
#include <sys/audioio.h>
#include <sys/midiio.h>

#include <sys/bus.h>

#include <dev/audio_if.h>
#include <dev/midi_if.h>
#include <dev/ic/oplreg.h>
#include <dev/ic/oplvar.h>

#include <dev/pcmcia/pcmciavar.h>
#include <dev/pcmcia/eslvar.h>

int	opl_esl_match(device_t, cfdata_t, void *);
void	opl_esl_attach(device_t, device_t, void *);
int	opl_esl_detach(device_t, int);

CFATTACH_DECL_NEW(opl_esl, sizeof(struct opl_softc),
    opl_esl_match, opl_esl_attach, opl_esl_detach, NULL);

int
opl_esl_match(device_t parent, cfdata_t match, void *aux)
{
	struct audio_attach_args *aa = (struct audio_attach_args *)aux;
	struct esl_pcmcia_softc *ssc = device_private(parent);

	if (aa->type != AUDIODEV_TYPE_OPL)
		return (0);

	return opl_match(ssc->sc_iot, ssc->sc_ioh, 0);
}

void
opl_esl_attach(device_t parent, device_t self, void *aux)
{
	struct esl_pcmcia_softc *ssc = device_private(parent);
	struct opl_softc *sc = device_private(self);

	sc->sc_mididev.dev = self;
	sc->iot = ssc->sc_iot;
	sc->ioh = ssc->sc_ioh;
	sc->offs = 0;
	strcpy(sc->syn.name, "ESS AudioDrive ");

	opl_attach(sc);
}

int
opl_esl_detach(device_t self, int flags)
{
	struct opl_softc *sc = device_private(self);
	int rv;

	rv = opl_detach(sc, flags);

	return (rv);
}
