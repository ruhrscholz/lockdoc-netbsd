/* $NetBSD: esa.c,v 1.2 2002/01/06 16:58:23 augustss Exp $ */

/*
 * Copyright (c) 2001, 2002 Jared D. McNeill <jmcneill@invisible.yi.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ESS Allegro-1 / Maestro3 Audio Driver
 * 
 * Based on the FreeBSD maestro3 driver and the NetBSD eap driver.
 * Original driver by Don Kim.
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/null.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/exec.h>
#include <sys/select.h>
#include <sys/audioio.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/pci/pcidevs.h>
#include <dev/pci/pcivar.h>

#include <dev/audio_if.h>
#include <dev/mulaw.h>
#include <dev/auconv.h>
#include <dev/ic/ac97var.h>
#include <dev/ic/ac97reg.h>


#include <dev/pci/esareg.h>
#include <dev/pci/esadsp.h>
#include <dev/pci/esavar.h>

#define PCI_CBIO	0x10

#define ESA_DAC_DATA	0x1100

enum {
	ESS_ALLEGRO1,
	ESS_MAESTRO3
};

static struct esa_card_type {
	u_int16_t pci_vendor_id;
	u_int16_t pci_product_id;
	int type;
	int delay1, delay2;
} esa_card_types[] = {
	{ PCI_VENDOR_ESSTECH, PCI_PRODUCT_ESSTECH_ALLEGRO1,
	  ESS_ALLEGRO1, 50, 800 },
	{ PCI_VENDOR_ESSTECH, PCI_PRODUCT_ESSTECH_MAESTRO3,
	  ESS_MAESTRO3, 20, 500 },
	{ PCI_VENDOR_ESSTECH, PCI_PRODUCT_ESSTECH_MAESTRO3_2,
	  ESS_MAESTRO3, 20, 500 },
	{ 0, 0, 0, 0, 0 }
};

struct audio_device esa_device = {
	"ESS Allegro",
	"",
	"esa"
};

int		esa_match(struct device *, struct cfdata *, void *);
void		esa_attach(struct device *, struct device *, void *);
int		esa_detach(struct device *, int);

/* audio(9) functions */
int		esa_open(void *, int);
void		esa_close(void *);
int		esa_query_encoding(void *, struct audio_encoding *);
int		esa_set_params(void *, int, int, struct audio_params *,
			       struct audio_params *);
int		esa_round_blocksize(void *, int);
int		esa_init_output(void *, void *, int);
int		esa_halt_output(void *);
int		esa_halt_input(void *);
int		esa_set_port(void *, mixer_ctrl_t *);
int		esa_get_port(void *, mixer_ctrl_t *);
int		esa_query_devinfo(void *, mixer_devinfo_t *);
void *		esa_malloc(void *, int, size_t, int, int);
void		esa_free(void *, void *, int);
int		esa_getdev(void *, struct audio_device *);
size_t		esa_round_buffersize(void *, int, size_t);
int		esa_get_props(void *);
int		esa_trigger_output(void *, void *, void *, int,
				   void (*)(void *), void *,
				   struct audio_params *);
int		esa_trigger_input(void *, void *, void *, int,
				  void (*)(void *), void *,
				  struct audio_params *);

int		esa_intr(void *);
int		esa_allocmem(struct esa_softc *, size_t, size_t,
			     struct esa_dma *);
int		esa_freemem(struct esa_softc *, struct esa_dma *);
paddr_t		esa_mappage(void *addr, void *mem, off_t off, int prot);

/* Supporting subroutines */
u_int16_t	esa_read_assp(struct esa_softc *, u_int16_t, u_int16_t);
void		esa_write_assp(struct esa_softc *, u_int16_t, u_int16_t,
			       u_int16_t);
int		esa_init_codec(struct esa_softc *);
int		esa_attach_codec(void *, struct ac97_codec_if *);
int		esa_read_codec(void *, u_int8_t, u_int16_t *);
int		esa_write_codec(void *, u_int8_t, u_int16_t);
void		esa_reset_codec(void *);
enum ac97_host_flags	esa_flags_codec(void *);
int		esa_wait(struct esa_softc *);
int		esa_init(struct esa_softc *);
void		esa_config(struct esa_softc *);
u_int8_t	esa_assp_halt(struct esa_softc *);
void		esa_codec_reset(struct esa_softc *);
int		esa_amp_enable(struct esa_softc *);
void		esa_enable_interrupts(struct esa_softc *);
int		esa_power(struct esa_softc *, int);
u_int32_t	esa_get_pointer(struct esa_softc *);

struct device *	audio_attach_mi_lkm(struct audio_hw_if *, void *,
				    struct device *);

static audio_encoding_t esa_encoding[] = {
	{ 0, AudioEulinear, AUDIO_ENCODING_ULINEAR, 8, 0 },
	{ 1, AudioEmulaw, AUDIO_ENCODING_ULAW, 8,
		AUDIO_ENCODINGFLAG_EMULATED },
	{ 2, AudioEalaw, AUDIO_ENCODING_ALAW, 8, AUDIO_ENCODINGFLAG_EMULATED },
	{ 3, AudioEslinear, AUDIO_ENCODING_SLINEAR, 8,
		AUDIO_ENCODINGFLAG_EMULATED }, /* XXX: Are you sure? */
	{ 4, AudioEslinear_le, AUDIO_ENCODING_SLINEAR_LE, 16, 0 },
	{ 5, AudioEulinear_le, AUDIO_ENCODING_ULINEAR_LE, 16,
		AUDIO_ENCODINGFLAG_EMULATED },
	{ 6, AudioEslinear_be, AUDIO_ENCODING_SLINEAR_BE, 16,
		AUDIO_ENCODINGFLAG_EMULATED },
	{ 7, AudioEulinear_be, AUDIO_ENCODING_ULINEAR_BE, 16,
		AUDIO_ENCODINGFLAG_EMULATED }
};

#define ESA_NENCODINGS 8

struct audio_hw_if esa_hw_if = {
	esa_open,
	esa_close,
	NULL,			/* drain */
	esa_query_encoding,
	esa_set_params,
	esa_round_blocksize,
	NULL,			/* commit_settings */
	esa_init_output,
	NULL,			/* esa_init_input */
	NULL,			/* start_output */
	NULL,			/* start_input */
	esa_halt_output,
	esa_halt_input,
	NULL,			/* speaker_ctl */
	esa_getdev,
	NULL,			/* getfd */
	esa_set_port,
	esa_get_port,
	esa_query_devinfo,
	esa_malloc,
	esa_free,
	esa_round_buffersize,
	esa_mappage,
	esa_get_props,
	esa_trigger_output,
	esa_trigger_input
};

struct cfattach esa_ca = {
	sizeof(struct esa_softc), esa_match, esa_attach,
	esa_detach, /*esa_activate*/ NULL
};

/*
 * audio(9) functions
 */

int
esa_open(void *hdl, int flags)
{

	return (0);
}

void
esa_close(void *hdl)
{

	return;
}

int
esa_query_encoding(void *hdl, struct audio_encoding *ae)
{

	if (ae->index < 0 || ae->index >= ESA_NENCODINGS)
		return (EINVAL);
	*ae = esa_encoding[ae->index];

	return (0);
}

int
esa_set_params(void *hdl, int setmode, int usemode, struct audio_params *play,
	       struct audio_params *rec)
{
	struct esa_softc *sc = hdl;
	u_int32_t data;
	u_int32_t freq;

	if ((setmode & AUMODE_PLAY) == 0) {
		printf("%s: esa_set_params: only AUMODE_PLAY is supported\n",
		    sc->sc_dev.dv_xname);
		return (EINVAL);
	}

	if (play->sample_rate < ESA_MINRATE ||
	    play->sample_rate > ESA_MAXRATE ||
	    (play->precision != 8 && play->precision != 16) ||
	    (play->channels < 1 && play->channels > 2)) /* XXX: Are you sure? */
		return (EINVAL);

	play->factor = 1;
	play->sw_code = 0;

	switch(play->encoding) {
	case AUDIO_ENCODING_SLINEAR_BE:
		if (play->precision == 16)
			play->sw_code = swap_bytes;
		else
			play->sw_code = change_sign8;
		break;
	case AUDIO_ENCODING_SLINEAR_LE:
		if (play->precision != 16)
			play->sw_code = change_sign8;
		break;
	case AUDIO_ENCODING_ULINEAR_BE:
		if (play->precision == 16)
			play->sw_code = swap_bytes_change_sign16_le;
		break;
	case AUDIO_ENCODING_ULINEAR_LE:
		if (play->precision == 16)
			play->sw_code = change_sign16_le;
		break;
	case AUDIO_ENCODING_ULAW:
		play->factor = 2;
		play->sw_code = mulaw_to_slinear16_le;
		break;
	case AUDIO_ENCODING_ALAW:
		play->factor = 2;
		play->sw_code = alaw_to_slinear16_le;
		break;
	default:
		return (EINVAL);
	}

	if (play->channels == 1)
		data = 1;
	else
		data = 0;
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		       ESA_DAC_DATA + ESA_SRC3_MODE_OFFSET,
	    data);

	if (play->precision * play->factor == 8)
		data = 1;
	else
		data = 0;
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		       ESA_DAC_DATA + ESA_SRC3_WORD_LENGTH_OFFSET, data);

	if ((freq = ((play->sample_rate << 15) + 24000) / 48000) != 0) {
		freq--;
	}
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		       ESA_DAC_DATA + ESA_CDATA_FREQUENCY, freq);
	
	return (0);
}

int
esa_round_blocksize(void *hdl, int bs)
{
	struct esa_softc *sc = hdl;

	bs = 4096;		/* XXX why? */
	sc->play.blksize = bs;

	return (sc->play.blksize);
}

int
esa_init_output(void *hdl, void *buffer, int size)
{

	return (0);
}

int
esa_halt_output(void *hdl)
{
	struct esa_softc *sc = hdl;

	if (sc->play.active == 0)
		return (0);

	sc->play.active = 0;

	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		       ESA_KDATA_INSTANCE0_MINISRC, 0);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_DMA_XFER0, 0);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_MIXER_XFER0, 0);

	return (0);
}

int
esa_halt_input(void *hdl)
{

	return (EOPNOTSUPP);
}

void *
esa_malloc(void *hdl, int direction, size_t size, int type, int flags)
{
	struct esa_softc *sc = hdl;
	struct esa_dma *p;
	int error;

	p = malloc(sizeof(*p), type, flags);
	if (!p)
		return (0);
	error = esa_allocmem(sc, size, 16, p);
	if (error) {
		free(p, type);
		printf("%s: esa_malloc: not enough memory\n",
		    sc->sc_dev.dv_xname);
		return (0);
	}
	p->next = sc->sc_dmas;
	sc->sc_dmas = p;

	return (KERNADDR(p));
}

void
esa_free(void *hdl, void *addr, int type)
{
	struct esa_softc *sc = hdl;
	struct esa_dma *p;
	struct esa_dma **pp;

	for (pp = &sc->sc_dmas; (p = *pp) != NULL; pp = &p->next)
		if (KERNADDR(p) == addr) {
			esa_freemem(sc, p);
			*pp = p->next;
			free(p, type);
			return;
		}
}

int
esa_getdev(void *hdl, struct audio_device *ret)
{

	*ret = esa_device;

	return (0);
}

int
esa_set_port(void *hdl, mixer_ctrl_t *mc)
{
	struct esa_softc *sc = hdl;

	return (sc->codec_if->vtbl->mixer_set_port(sc->codec_if, mc));
}

int
esa_get_port(void *hdl, mixer_ctrl_t *mc)
{
	struct esa_softc *sc = hdl;

	return (sc->codec_if->vtbl->mixer_get_port(sc->codec_if, mc));
}

int
esa_query_devinfo(void *hdl, mixer_devinfo_t *di)
{
	struct esa_softc *sc = hdl;

	return (sc->codec_if->vtbl->query_devinfo(sc->codec_if, di));
}

size_t
esa_round_buffersize(void *hdl, int direction, size_t bufsize)
{
	struct esa_softc *sc = hdl;

	bufsize = 65536;	/* XXX why? */
	sc->play.bufsize = bufsize;

	return (sc->play.bufsize);
}

int
esa_get_props(void *hdl)
{

	return (AUDIO_PROP_MMAP);
}

int
esa_trigger_output(void *hdl, void *start, void *end, int blksize,
			void (*intr)(void *), void *intrarg,
			struct audio_params *param)
{
	struct esa_softc *sc = hdl;
	struct esa_dma *p;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int32_t data;
	u_int32_t bufaddr;
	u_int32_t i;
	size_t size;
	int dsp_in_size = ESA_MINISRC_IN_BUFFER_SIZE - (0x20 * 2);
	int dsp_out_size = ESA_MINISRC_OUT_BUFFER_SIZE - (0x20 * 2);
	int dsp_in_buf = ESA_DAC_DATA + (ESA_MINISRC_TMP_BUFFER_SIZE / 2);
	int dsp_out_buf = dsp_in_buf + (dsp_in_size / 2) + 1;

	if (sc->play.active)
		return (EINVAL);

	for (p = sc->sc_dmas; p && KERNADDR(p) != start; p = p->next)
		;
	if (!p) {
		printf("%s: esa_trigger_output: bad addr %p\n",
		    sc->sc_dev.dv_xname, start);
		return (EINVAL);
	}

	sc->play.active = 1;
	sc->intr = intr;
	sc->arg = intrarg;
	sc->play.pos = 0;
	sc->play.count = 0;
	sc->play.buf = start;
	size = (size_t)(((caddr_t)end - (caddr_t)start));
	bufaddr = DMAADDR(p);
	sc->play.start = bufaddr;

#define LO(x) ((x) & 0x0000ffff)
#define HI(x) ((x) >> 16)

	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_ADDRL, LO(bufaddr));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_ADDRH, HI(bufaddr));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_END_PLUS_1L, LO(bufaddr + size));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_END_PLUS_1H, HI(bufaddr + size));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_CURRENTL, LO(bufaddr));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_CURRENTH, HI(bufaddr));

	/* DSP buffers */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_IN_BUF_BEGIN, dsp_in_buf);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_IN_BUF_END_PLUS_1, dsp_in_buf + (dsp_in_size / 2));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_IN_BUF_HEAD, dsp_in_buf);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_IN_BUF_TAIL, dsp_in_buf);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_OUT_BUF_BEGIN, dsp_out_buf);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_OUT_BUF_END_PLUS_1, dsp_out_buf + (dsp_out_size / 2));
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_OUT_BUF_HEAD, dsp_out_buf);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_OUT_BUF_TAIL, dsp_out_buf);

	/* Some per-client initializers */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_SRC3_DIRECTION_OFFSET + 12, ESA_DAC_DATA + 40 + 8);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_SRC3_DIRECTION_OFFSET + 19, 0x400 + ESA_MINISRC_COEF_LOC);
	/* Enable or disable low-pass filter? (0xff if rate > 45000) */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_SRC3_DIRECTION_OFFSET + 22, 0);
	/* Tell it which way DMA is going */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_DMA_CONTROL,
	    ESA_DMACONTROL_AUTOREPEAT + ESA_DMAC_PAGE3_SELECTOR +
	    ESA_DMAC_BLOCKF_SELECTOR);

	/* Set an armload of static initializers */
	for (i = 0; i < (sizeof(esa_playvals) / sizeof(esa_playvals[0])); i++)
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
		    esa_playvals[i].addr, esa_playvals[i].val);

	/* Put us in the packed task lists */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
	    ESA_KDATA_INSTANCE0_MINISRC,
	    ESA_DAC_DATA >> ESA_DP_SHIFT_COUNT);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_DMA_XFER0,
	    ESA_DAC_DATA >> ESA_DP_SHIFT_COUNT);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_MIXER_XFER0,
	    ESA_DAC_DATA >> ESA_DP_SHIFT_COUNT);
#undef LO
#undef HI

	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
	    ESA_KDATA_TIMER_COUNT_RELOAD, 240);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
	    ESA_KDATA_TIMER_COUNT_CURRENT, 240);
	data = bus_space_read_2(iot, ioh, ESA_HOST_INT_CTRL);
	bus_space_write_2(iot, ioh, ESA_HOST_INT_CTRL,
	    data | ESA_CLKRUN_GEN_ENABLE);

	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_INSTANCE_READY, 1);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
	    ESA_KDATA_MIXER_TASK_NUMBER, 1);

	return (0);
}

int
esa_trigger_input(void *hdl, void *start, void *end, int blksize,
			void (*intr)(void *), void *intrarg,
			struct audio_params *param)
{

	return (EOPNOTSUPP);
}

/* Interrupt handler */

int
esa_intr(void *hdl)
{
	struct esa_softc *sc = hdl;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int32_t status, ctl;
	u_int32_t pos;
	u_int32_t diff;
	u_int32_t blksize = sc->play.blksize;
	u_int32_t bufsize = sc->play.bufsize;

	status = bus_space_read_1(iot, ioh, ESA_HOST_INT_STATUS);
	if (!status)
		return (0);

	/* ack the interrupt */
	bus_space_write_1(iot, ioh, ESA_HOST_INT_STATUS, 0xff);

	if (status & ESA_HV_INT_PENDING) {
		u_int8_t event;

		printf("%s: hardware volume interrupt\n", sc->sc_dev.dv_xname);
		event = bus_space_read_1(iot, ioh, ESA_HW_VOL_COUNTER_MASTER);
		switch(event) {
		case 0x99:
		case 0xaa:
		case 0x66:
		case 0x88:	
			printf("%s: esa_intr: FIXME\n", sc->sc_dev.dv_xname);
			break;
		default:
			printf("%s: unknown hwvol event 0x%02x\n",
			    sc->sc_dev.dv_xname, event);
			break;
		}
		bus_space_write_1(iot, ioh, ESA_HW_VOL_COUNTER_MASTER, 0x88);
	}

	if (status & ESA_ASSP_INT_PENDING) {
		ctl = bus_space_read_1(iot, ioh, ESA_ASSP_CONTROL_B);
		if (!(ctl & ESA_STOP_ASSP_CLOCK)) {
			ctl = bus_space_read_1(iot, ioh,
					       ESA_ASSP_HOST_INT_STATUS);
			if (ctl & ESA_DSP2HOST_REQ_TIMER) {
				bus_space_write_1(iot, ioh,
				    ESA_ASSP_HOST_INT_STATUS,
				    ESA_DSP2HOST_REQ_TIMER);
				if (sc->play.active) {
					pos = esa_get_pointer(sc) % bufsize;
					diff = (bufsize + pos - sc->play.pos)
					    % bufsize;
					sc->play.pos = pos;
					sc->play.count += diff;
					while(sc->play.count >= blksize) {
						sc->play.count -= blksize;
						(*sc->intr)(sc->arg);
					}
				}
			}
		}
	}

	return (1);
}

int
esa_allocmem(struct esa_softc *sc, size_t size, size_t align,
		struct esa_dma *p)
{
	int error;

	p->size = size;
	error = bus_dmamem_alloc(sc->sc_dmat, p->size, align, 0,
				 p->segs, sizeof(p->segs) / sizeof(p->segs[0]),
				 &p->nsegs, BUS_DMA_NOWAIT);
	if (error)
		return (error);

	error = bus_dmamem_map(sc->sc_dmat, p->segs, p->nsegs, p->size,
				&p->addr, BUS_DMA_NOWAIT | BUS_DMA_COHERENT);
	if (error)
		goto free;

	error = bus_dmamap_create(sc->sc_dmat, p->size, 1, p->size, 0,
				  BUS_DMA_NOWAIT, &p->map);
	if (error)
		goto unmap;

	error = bus_dmamap_load(sc->sc_dmat, p->map, p->addr, p->size, NULL,
				BUS_DMA_NOWAIT);
	if (error)
		goto destroy;

	return (0);

destroy:
	bus_dmamap_destroy(sc->sc_dmat, p->map);
unmap:
	bus_dmamem_unmap(sc->sc_dmat, p->addr, p->size);
free:
	bus_dmamem_free(sc->sc_dmat, p->segs, p->nsegs);

	return (error); 
}

int
esa_freemem(struct esa_softc *sc, struct esa_dma *p)
{

	bus_dmamap_unload(sc->sc_dmat, p->map);
	bus_dmamap_destroy(sc->sc_dmat, p->map);
	bus_dmamem_unmap(sc->sc_dmat, p->addr, p->size);
	bus_dmamem_free(sc->sc_dmat, p->segs, p->nsegs);

	return (0);
}

/*
 * Supporting Subroutines
 */

int
esa_match(struct device *dev, struct cfdata *match, void *aux)
{
	struct pci_attach_args *pa = (struct pci_attach_args *)aux;

	switch(PCI_VENDOR(pa->pa_id)) {
	case PCI_VENDOR_ESSTECH:
		switch(PCI_PRODUCT(pa->pa_id)) {
		case PCI_PRODUCT_ESSTECH_ALLEGRO1:
		case PCI_PRODUCT_ESSTECH_MAESTRO3:
		case PCI_PRODUCT_ESSTECH_MAESTRO3_2:
			return (1);
		}
	}

	return (0);
}

void
esa_attach(struct device *parent, struct device *self, void *aux)
{
	struct esa_softc *sc = (struct esa_softc *)self;
	struct pci_attach_args *pa = (struct pci_attach_args *)aux;
	pcitag_t tag = pa->pa_tag;
	pci_chipset_tag_t pc = pa->pa_pc;
	pci_intr_handle_t ih;
	struct esa_card_type *card;
	const char *intrstr;
	u_int32_t data;
	char devinfo[256];
	int revision;

	pci_devinfo(pa->pa_id, pa->pa_class, 0, devinfo);
	revision = PCI_REVISION(pa->pa_class);
	printf(": %s (rev. 0x%02x)\n", devinfo, revision);

	for (card = esa_card_types; card->pci_vendor_id; card++)
		if (PCI_VENDOR(pa->pa_id) == card->pci_vendor_id &&
		    PCI_PRODUCT(pa->pa_id) == card->pci_product_id) {
			sc->type = card->type;
			sc->delay1 = card->delay1;
			sc->delay2 = card->delay2;
			break;
		}

	data = pci_conf_read(pc, tag, PCI_COMMAND_STATUS_REG);
	data |= (PCI_COMMAND_IO_ENABLE | PCI_COMMAND_MEM_ENABLE
	    | PCI_COMMAND_MASTER_ENABLE);
	pci_conf_write(pc, tag, PCI_COMMAND_STATUS_REG, data);

	/* Map I/O register */
	if (pci_mapreg_map(pa, PCI_CBIO, PCI_MAPREG_TYPE_IO, 0,
	    &sc->sc_iot, &sc->sc_ioh, &sc->sc_iob, &sc->sc_ios)) {
		printf("%s: can't map i/o space\n", sc->sc_dev.dv_xname);
		return;
	}

	/* Initialize softc */
	sc->sc_tag = tag;
	sc->sc_pct = pc;
	sc->sc_dmat = pa->pa_dmat;

	/* Map and establish an interrupt */
	if (pci_intr_map(pa, &ih)) {
		printf("%s: can't map interrupt\n", sc->sc_dev.dv_xname);
		return;
	}
	intrstr = pci_intr_string(pc, ih);
	sc->sc_ih = pci_intr_establish(pc, ih, IPL_AUDIO, esa_intr, self);
	if (sc->sc_ih == NULL) {
		printf("%s: can't establish interrupt", sc->sc_dev.dv_xname);
		if (intrstr != NULL)
			printf(" at %s", intrstr);
		printf("\n");
		return;
	}
	printf("%s: interrupting at %s\n", sc->sc_dev.dv_xname, intrstr);

	/* Power up chip */
	esa_power(sc, 0);

	/* Init chip */
	if (esa_init(sc) == -1) {
		printf("%s: esa_attach: unable to initialize the card\n",
		    sc->sc_dev.dv_xname);
		return;
	}

	/* Attach AC97 host interface */
	sc->host_if.arg = self;
	sc->host_if.attach = esa_attach_codec;
	sc->host_if.read = esa_read_codec;
	sc->host_if.write = esa_write_codec;
	sc->host_if.reset = esa_reset_codec;
	sc->host_if.flags = esa_flags_codec;

	if (ac97_attach(&sc->host_if) != 0)
		return;

	sc->sc_audiodev = audio_attach_mi(&esa_hw_if, self, &sc->sc_dev);

	return;
}

int
esa_detach(struct device *self, int flags)
{
	struct esa_softc *sc = (struct esa_softc *)self;
	int rv = 0;

	if (sc->sc_audiodev != NULL)
		rv = config_detach(sc->sc_audiodev, flags);
	if (rv)
		return (rv);

	if (sc->sc_ih != NULL)
		pci_intr_disestablish(sc->sc_pct, sc->sc_ih);
	if (sc->sc_ios)
		bus_space_unmap(sc->sc_iot, sc->sc_ioh, sc->sc_ios);

	return (0);
}

u_int16_t
esa_read_assp(struct esa_softc *sc, u_int16_t region, u_int16_t index)
{
	u_int16_t data;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;

	bus_space_write_2(iot, ioh, ESA_DSP_PORT_MEMORY_TYPE,
	    region & ESA_MEMTYPE_MASK);
	bus_space_write_2(iot, ioh, ESA_DSP_PORT_MEMORY_INDEX, index);
	data = bus_space_read_2(iot, ioh, ESA_DSP_PORT_MEMORY_DATA);

	return (data);
}

void
esa_write_assp(struct esa_softc *sc, u_int16_t region, u_int16_t index,
		u_int16_t data)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;

	bus_space_write_2(iot, ioh, ESA_DSP_PORT_MEMORY_TYPE,
	    region & ESA_MEMTYPE_MASK);
	bus_space_write_2(iot, ioh, ESA_DSP_PORT_MEMORY_INDEX, index);
	bus_space_write_2(iot, ioh, ESA_DSP_PORT_MEMORY_DATA, data);

	return;
}

int
esa_init_codec(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int32_t data;

	data = bus_space_read_1(iot, ioh, ESA_CODEC_COMMAND);

	return ((data & 0x1) ? 0 : 1);
}

int
esa_attach_codec(void *aux, struct ac97_codec_if *codec_if)
{
	struct esa_softc *sc = aux;

	sc->codec_if = codec_if;

	return (0);
}

int
esa_read_codec(void *aux, u_int8_t reg, u_int16_t *result)
{
	struct esa_softc *sc = aux;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;

	if (esa_wait(sc))
		printf("%s: esa_read_codec: timed out\n", sc->sc_dev.dv_xname);
	bus_space_write_1(iot, ioh, ESA_CODEC_COMMAND, (reg & 0x7f) | 0x80);
	delay(50);
	if (esa_wait(sc))
		printf("%s: esa_read_codec: timed out\n", sc->sc_dev.dv_xname);
	*result = bus_space_read_2(iot, ioh, ESA_CODEC_DATA);

	return (0);
}

int
esa_write_codec(void *aux, u_int8_t reg, u_int16_t data)
{
	struct esa_softc *sc = aux;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;

	if (esa_wait(sc)) {
		printf("%s: esa_write_codec: timed out\n", sc->sc_dev.dv_xname);
		return (-1);
	}
	bus_space_write_2(iot, ioh, ESA_CODEC_DATA, data);
	bus_space_write_1(iot, ioh, ESA_CODEC_COMMAND, reg & 0x7f);
	delay(50);

	return (0);
}

void
esa_reset_codec(void *aux)
{

	return;
}

enum ac97_host_flags
esa_flags_codec(void *aux)
{
	struct esa_softc *sc = aux;

	return (sc->codec_flags);
}

int
esa_wait(struct esa_softc *sc)
{
	int i, val;
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;

	for (i = 0; i < 20; i++) {
		val = bus_space_read_1(iot, ioh, ESA_CODEC_STATUS);
		if ((val & 1) == 0)
			return (0);
		delay(2);
	}

	return (-1);
}

int
esa_init(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	pcitag_t tag = sc->sc_tag;
	pci_chipset_tag_t pc = sc->sc_pct;
	u_int32_t data, i, size;
	u_int8_t reset_state;

	/* Disable legacy emulation */
	data = pci_conf_read(pc, tag, PCI_LEGACY_AUDIO_CTRL);
	data |= DISABLE_LEGACY;
	pci_conf_write(pc, tag, PCI_LEGACY_AUDIO_CTRL, data);

	esa_config(sc);

	reset_state = esa_assp_halt(sc);

	esa_init_codec(sc);
	esa_codec_reset(sc);

	/* Zero kernel and mixer data */
	size = ESA_REV_B_DATA_MEMORY_UNIT_LENGTH * ESA_NUM_UNITS_KERNEL_DATA;
	for (i = 0; i < size / 2; i++) {
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		    ESA_KDATA_BASE_ADDR + i, 0);
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
		    ESA_KDATA_BASE_ADDR2 + i, 0);
	}

	/* Init DMA pointer */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_CURRENT_DMA,
	    ESA_KDATA_DMA_XFER0);

	/* Write kernel code into memory */
	size = sizeof(esa_assp_kernel_image);
	for (i = 0; i < size / 2; i++)
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_CODE,
		    ESA_REV_B_CODE_MEMORY_BEGIN + i, esa_assp_kernel_image[i]);

	size = sizeof(esa_assp_minisrc_image);
	for (i = 0; i < size / 2; i++)
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_CODE, 0x400 + i,
		    esa_assp_minisrc_image[i]);

	/* Write the coefficients for the low pass filter */
	size = sizeof(esa_minisrc_lpf_image);
	for (i = 0; i < size / 2; i++)
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_CODE,
		    0x400 + ESA_MINISRC_COEF_LOC + i, esa_minisrc_lpf_image[i]);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_CODE,
	    0x400 + ESA_MINISRC_COEF_LOC + size, 0x8000);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_TASK0, 0x400);
	/* Init the mixer number */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
             ESA_KDATA_MIXER_TASK_NUMBER, 0);
	/* Extreme kernel master volume */
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_KDATA_DAC_LEFT_VOLUME,
	    ESA_ARB_VOLUME);
	esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA,
            ESA_KDATA_DAC_RIGHT_VOLUME, ESA_ARB_VOLUME);

	if (esa_amp_enable(sc))
		return (-1);

	/* Zero entire DAC/ADC area */
	for (i = 0x1100; i < 0x1c00; i++)
		esa_write_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, i, 0);

	esa_enable_interrupts(sc);

	bus_space_write_1(iot, ioh, ESA_DSP_PORT_CONTROL_REG_B,
	    reset_state | ESA_REGB_ENABLE_RESET);

	return (0);
}

void
esa_config(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	pcitag_t tag = sc->sc_tag;
	pci_chipset_tag_t pc = sc->sc_pct;
	u_int32_t data;

	data = pci_conf_read(pc, tag, ESA_PCI_ALLEGRO_CONFIG);
	data &= ESA_REDUCED_DEBOUNCE;
	data |= ESA_PM_CTRL_ENABLE | ESA_CLK_DIV_BY_49 | ESA_USE_PCI_TIMING;
	pci_conf_write(pc, tag, ESA_PCI_ALLEGRO_CONFIG, data);

	bus_space_write_1(iot, ioh, ESA_ASSP_CONTROL_B, ESA_RESET_ASSP);
	data = pci_conf_read(pc, tag, ESA_PCI_ALLEGRO_CONFIG);
	data &= ~ESA_INT_CLK_SELECT;
	if (sc->type == ESS_MAESTRO3) {
		data &= ~ESA_INT_CLK_MULT_ENABLE;
		data |= ESA_INT_CLK_SRC_NOT_PCI;
	}
	data &= ~(ESA_CLK_MULT_MODE_SELECT | ESA_CLK_MULT_MODE_SELECT_2);
	pci_conf_write(pc, tag, ESA_PCI_ALLEGRO_CONFIG, data);

	if (sc->type == ESS_ALLEGRO1) {
		data = pci_conf_read(pc, tag, ESA_PCI_USER_CONFIG);
		data |= ESA_IN_CLK_12MHZ_SELECT;
		pci_conf_write(pc, tag, ESA_PCI_USER_CONFIG, data);
	}

	data = bus_space_read_1(iot, ioh, ESA_ASSP_CONTROL_A);
	data &= ~(ESA_DSP_CLK_36MHZ_SELECT | ESA_ASSP_CLK_49MHZ_SELECT);
	data |= ESA_ASSP_CLK_49MHZ_SELECT;	/* XXX: Assumes 49MHz DSP */
	data |= ESA_ASSP_0_WS_ENABLE;
	bus_space_write_1(iot, ioh, ESA_ASSP_CONTROL_A, data);

	bus_space_write_1(iot, ioh, ESA_ASSP_CONTROL_B, ESA_RUN_ASSP);

	return;
}

u_int8_t
esa_assp_halt(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int8_t data, reset_state;

	data = bus_space_read_1(iot, ioh, ESA_DSP_PORT_CONTROL_REG_B);
	reset_state = data & ~ESA_REGB_STOP_CLOCK;
	delay(10000);		/* XXX use tsleep */
	bus_space_write_1(iot, ioh, ESA_DSP_PORT_CONTROL_REG_B,
			reset_state & ~ESA_REGB_ENABLE_RESET);
	delay(10000);		/* XXX use tsleep */

	return (reset_state);
}

void
esa_codec_reset(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int16_t data, dir;
	int retry = 0;

	do {
		data = bus_space_read_2(iot, ioh, ESA_GPIO_DIRECTION);
		dir = data | 0x10; /* assuming pci bus master? */

		/* remote codec config */
		data = bus_space_read_2(iot, ioh, ESA_RING_BUS_CTRL_B);
		bus_space_write_2(iot, ioh, ESA_RING_BUS_CTRL_B,
		    data & ~ESA_SECOND_CODEC_ID_MASK);
		data = bus_space_read_2(iot, ioh, ESA_SDO_OUT_DEST_CTRL);
		bus_space_write_2(iot, ioh, ESA_SDO_OUT_DEST_CTRL,
		    data & ~ESA_COMMAND_ADDR_OUT);
		data = bus_space_read_2(iot, ioh, ESA_SDO_IN_DEST_CTRL);
		bus_space_write_2(iot, ioh, ESA_SDO_IN_DEST_CTRL,
		    data & ~ESA_STATUS_ADDR_IN);

		bus_space_write_2(iot, ioh, ESA_RING_BUS_CTRL_A,
				  ESA_IO_SRAM_ENABLE);
		delay(20);

		bus_space_write_2(iot, ioh, ESA_GPIO_DIRECTION,
		    dir & ~ESA_GPO_PRIMARY_AC97);
		bus_space_write_2(iot, ioh, ESA_GPIO_MASK,
				  ~ESA_GPO_PRIMARY_AC97);
		bus_space_write_2(iot, ioh, ESA_GPIO_DATA, 0);
		bus_space_write_2(iot, ioh, ESA_GPIO_DIRECTION,
		    dir | ESA_GPO_PRIMARY_AC97);
		delay(sc->delay1 * 1000);
		bus_space_write_2(iot, ioh, ESA_GPIO_DATA,
				  ESA_GPO_PRIMARY_AC97);
		delay(5);
		bus_space_write_2(iot, ioh, ESA_RING_BUS_CTRL_A,
		    ESA_IO_SRAM_ENABLE | ESA_SERIAL_AC_LINK_ENABLE);
		bus_space_write_2(iot, ioh, ESA_GPIO_MASK, ~0);
		delay(sc->delay2 * 1000);

		esa_read_codec(sc, 0x7c, &data);
		if ((data == 0) || (data == 0xffff)) {
			retry++;
			if (retry > 3) {
				printf("%s: esa_codec_reset: failed\n",
				    sc->sc_dev.dv_xname);
				break;
			}
			printf("%s: esa_codec_reset: retrying\n",
			    sc->sc_dev.dv_xname);
		} else
			retry = 0;
	} while (retry);

	return;
}

int
esa_amp_enable(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int32_t gpo, polarity_port, polarity;
	u_int16_t data;

	switch (sc->type) {
	case ESS_ALLEGRO1:
		polarity_port = 0x1800;
		break;
	case ESS_MAESTRO3:
		polarity_port = 0x1100;
		break;
	default:
		printf("%s: esa_amp_enable: Unknown chip type!!!\n",
		    sc->sc_dev.dv_xname);
		return (1);
	}

	gpo = (polarity_port >> 8) & 0x0f;
	polarity = polarity_port >> 12;
	polarity = !polarity;	/* Enable */
	polarity = polarity << gpo;
	gpo = 1 << gpo;
	bus_space_write_2(iot, ioh, ESA_GPIO_MASK, ~gpo);
	data = bus_space_read_2(iot, ioh, ESA_GPIO_DIRECTION);
	bus_space_write_2(iot, ioh, ESA_GPIO_DIRECTION, data | gpo);
	data = ESA_GPO_SECONDARY_AC97 | ESA_GPO_PRIMARY_AC97 | polarity;
	bus_space_write_2(iot, ioh, ESA_GPIO_DATA, data);
	bus_space_write_2(iot, ioh, ESA_GPIO_MASK, ~0);

	return (0);
}

void
esa_enable_interrupts(struct esa_softc *sc)
{
	bus_space_tag_t iot = sc->sc_iot;
	bus_space_handle_t ioh = sc->sc_ioh;
	u_int8_t data;

	bus_space_write_2(iot, ioh, ESA_HOST_INT_CTRL,
	    ESA_ASSP_INT_ENABLE | ESA_HV_INT_ENABLE);
	data = bus_space_read_1(iot, ioh, ESA_ASSP_CONTROL_C);
	bus_space_write_1(iot, ioh, ESA_ASSP_CONTROL_C,
	    data | ESA_ASSP_HOST_INT_ENABLE);
}

int
esa_power(struct esa_softc *sc, int state)
{
	pcitag_t tag = sc->sc_tag;
	pci_chipset_tag_t pc = sc->sc_pct;
	u_int32_t data;

	data = pci_conf_read(pc, tag, 0x34);
	if (pci_conf_read(pc, tag, data) == 1)
		pci_conf_write(pc, tag, data + 4, state);
	
	return (0);
}

u_int32_t
esa_get_pointer(struct esa_softc *sc)
{
	u_int16_t hi = 0, lo = 0;
	u_int32_t addr;

	hi = esa_read_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_CURRENTH);
	lo = esa_read_assp(sc, ESA_MEMTYPE_INTERNAL_DATA, ESA_DAC_DATA +
	    ESA_CDATA_HOST_SRC_CURRENTL);

	addr = lo | ((u_int32_t)hi << 16);
	return (addr - sc->play.start);
}

paddr_t
esa_mappage(void *addr, void *mem, off_t off, int prot)
{
	struct esa_softc *sc = addr;
	struct esa_dma *p;

	if (off < 0)
		return (-1);
	for (p = sc->sc_dmas; p && KERNADDR(p) != mem; p = p->next)
		;
	if (!p)
		return (-1);
	return (bus_dmamem_mmap(sc->sc_dmat, p->segs, p->nsegs, 
				off, prot, BUS_DMA_WAITOK));
}

