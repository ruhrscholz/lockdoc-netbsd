/*	$NetBSD: umidi.c,v 1.2 2001/01/31 16:02:38 tshiozak Exp $	*/
/*
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Takuya SHIOZAKI (tshiozak@netbsd.org).
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
 *	  This product includes software developed by the NetBSD
 *	  Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/ioctl.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/poll.h>
#include <sys/lock.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdi_util.h>

#include <dev/usb/usbdevs.h>
#include <dev/usb/uaudioreg.h>
#include <dev/usb/umidireg.h>
#include <dev/usb/umidivar.h>
#include <dev/usb/umidi_quirks.h>

#include <dev/midi_if.h>

#ifdef UMIDI_DEBUG
#define DPRINTF(x)	if (umididebug) printf x
#define DPRINTFN(n,x)	if (umididebug >= (n)) printf x
int	umididebug = 0;
#else
#define DPRINTF(x)
#define DPRINTFN(n,x)
#endif


static int umidi_open(void *, int,
		      void (*)(void *, int), void (*)(void *), void *);
static void umidi_close(void *);
static int umidi_output(void *, int);
static void umidi_getinfo(void *, struct midi_info *);

static usbd_status enable_an_endpoint(struct umidi_endpoint *);
static void disable_an_endpoint(struct umidi_endpoint *, int);

static usbd_status alloc_all_endpoints(struct umidi_softc *);
static void free_all_endpoints(struct umidi_softc *);

static usbd_status alloc_all_jacks(struct umidi_softc *);
static void free_all_jacks(struct umidi_softc *);
static usbd_status bind_jacks_to_mididev(struct umidi_softc *,
					 struct umidi_jack *,
					 struct umidi_jack *,
					 struct umidi_mididev *);
static usbd_status unbind_jacks_from_mididev(struct umidi_mididev *);
static usbd_status unbind_all_jacks(struct umidi_softc *);
static usbd_status assign_all_jacks_automatically(struct umidi_softc *);
static usbd_status open_jack(struct umidi_jack *);
static void close_jack(struct umidi_jack *);

static usbd_status attach_mididev(struct umidi_softc *,
				  struct umidi_mididev *);
static usbd_status detach_mididev(struct umidi_mididev *, int);
static usbd_status deactivate_mididev(struct umidi_mididev *);
static usbd_status alloc_all_mididevs(struct umidi_softc *, int);
static void free_all_mididevs(struct umidi_softc *);
static usbd_status attach_all_mididevs(struct umidi_softc *);
static usbd_status detach_all_mididevs(struct umidi_softc *, int);
static usbd_status deactivate_all_mididevs(struct umidi_softc *);

#ifdef UMIDI_DEBUG
static void dump_sc(struct umidi_softc *);
static void dump_ep(struct umidi_endpoint *);
static void dump_jack(struct umidi_jack *);
#endif

static void init_packet(struct umidi_packet *);

static void in_packet_to_mididev(struct umidi_endpoint *, uByte *);
static usbd_status start_input_transfer(struct umidi_endpoint *);
static usbd_status start_output_transfer(struct umidi_endpoint *);
static void in_intr(usbd_xfer_handle, usbd_private_handle, usbd_status);
static void out_intr(usbd_xfer_handle, usbd_private_handle, usbd_status);
static void out_build_packet(int, struct umidi_packet *, uByte);


struct midi_hw_if umidi_hw_if = {
	umidi_open,
	umidi_close,
	umidi_output,
	umidi_getinfo,
	0,		/* ioctl */
};

USB_DECLARE_DRIVER(umidi);
int umidi_activate __P((device_ptr_t, enum devact));
int umidi_detach __P((device_ptr_t, int));

USB_MATCH(umidi)
{
	USB_MATCH_START(umidi, uaa);
	usb_interface_descriptor_t *id;

	DPRINTFN(1,("umidi_match\n"));

	if (uaa->iface == NULL)
		return UMATCH_NONE;

        if (umidi_search_quirk(uaa->vendor, uaa->product, uaa->ifaceno))
                return UMATCH_IFACECLASS_IFACESUBCLASS;

	id = usbd_get_interface_descriptor(uaa->iface);
	if (id!=NULL && 
	    id->bInterfaceClass==UICLASS_AUDIO &&
            id->bInterfaceSubClass==UISUBCLASS_MIDISTREAM)
                return UMATCH_IFACECLASS_IFACESUBCLASS;

        return UMATCH_NONE;
}

USB_ATTACH(umidi)
{
	usbd_status err;
	USB_ATTACH_START(umidi, sc, uaa);
	char devinfo[1024];

	DPRINTFN(1,("umidi_attach\n"));

	usbd_devinfo(uaa->device, 0, devinfo);
	printf("\n%s: %s\n", USBDEVNAME(sc->sc_dev), devinfo);

	sc->sc_iface = uaa->iface;
	sc->sc_udev = uaa->device;

	sc->sc_quirk =
	    umidi_search_quirk(uaa->vendor, uaa->product, uaa->ifaceno);
	printf("%s: ", USBDEVNAME(sc->sc_dev));
	umidi_print_quirk(sc->sc_quirk);


	err = alloc_all_endpoints(sc);
	if (err!=USBD_NORMAL_COMPLETION) {
		printf("%s: alloc_all_endpoints failed. (err=%d)\n",
		       USBDEVNAME(sc->sc_dev), err);
		goto error;
	}
	err = alloc_all_jacks(sc);
	if (err!=USBD_NORMAL_COMPLETION) {
		free_all_endpoints(sc);
		printf("%s: alloc_all_jacks failed. (err=%d)\n",
		       USBDEVNAME(sc->sc_dev), err);
		goto error;
	}
	printf("%s: out=%d, in=%d\n",
	       USBDEVNAME(sc->sc_dev),
	       sc->sc_out_num_jacks, sc->sc_in_num_jacks);

	err = assign_all_jacks_automatically(sc);
	if (err!=USBD_NORMAL_COMPLETION) {
		unbind_all_jacks(sc);
		free_all_jacks(sc);
		free_all_endpoints(sc);
		printf("%s: assign_all_jacks_automatically failed. (err=%d)\n",
		       USBDEVNAME(sc->sc_dev), err);
		goto error;
	}
	err = attach_all_mididevs(sc);
	if (err!=USBD_NORMAL_COMPLETION) {
		free_all_jacks(sc);
		free_all_endpoints(sc);
		printf("%s: attach_all_mididevs failed. (err=%d)\n",
		       USBDEVNAME(sc->sc_dev), err);
	}

#ifdef UMIDI_DEBUG
	dump_sc(sc);
#endif

	usbd_add_drv_event(USB_EVENT_DRIVER_ATTACH,
			   sc->sc_udev, USBDEV(sc->sc_dev));
	
	USB_ATTACH_SUCCESS_RETURN;
error:
	printf("%s: disabled.\n", USBDEVNAME(sc->sc_dev));
	sc->sc_dying = 1;
	USB_ATTACH_ERROR_RETURN;
}

int
umidi_activate(device_ptr_t self, enum devact act)
{
        struct umidi_softc *sc = (struct umidi_softc *)self;

	switch (act) {
	case DVACT_ACTIVATE:
		DPRINTFN(1,("umidi_activate (activate)\n"));

		return EOPNOTSUPP;
		break;
	case DVACT_DEACTIVATE:
		DPRINTFN(1,("umidi_activate (deactivate)\n"));
		sc->sc_dying = 1;
		deactivate_all_mididevs(sc);
		break;
	}
	return 0;
}

USB_DETACH(umidi)
{
        USB_DETACH_START(umidi, sc);

	DPRINTFN(1,("umidi_detach\n"));

	sc->sc_dying = 1;
	detach_all_mididevs(sc, flags);
	unbind_all_jacks(sc);
	free_all_mididevs(sc);
	free_all_jacks(sc);
	free_all_endpoints(sc);

        usbd_add_drv_event(USB_EVENT_DRIVER_DETACH, sc->sc_udev,
                           USBDEV(sc->sc_dev));

	return 0;
}


int
umidi_open(void *addr,
	   int flags,
	   void (*iintr)__P((void *, int)),
	   void (*ointr)__P((void *)),
	   void *arg)
{
        struct umidi_mididev *mididev = addr;
        struct umidi_softc *sc = mididev->sc;

	DPRINTF(("umidi_open: sc=%p\n", sc));

	if (!sc)
		return ENXIO;
	if (mididev->opened)
		return EBUSY;
	if (sc->sc_dying || mididev->dying)
		return EIO;

	mididev->flags = flags;
	mididev->opened = 1;
	mididev->arg = arg;
	mididev->iintr = iintr;
	mididev->ointr = ointr;
	if ((mididev->flags & FWRITE) && mididev->out_jack)
		open_jack(mididev->out_jack);
	if ((mididev->flags & FREAD) && mididev->in_jack)
		open_jack(mididev->in_jack);

	return 0;
}

void
umidi_close(void *addr)
{
	int s;
        struct umidi_mididev *mididev = addr;

	s = splusb();
	if ((mididev->flags & FWRITE) && mididev->out_jack)
		close_jack(mididev->out_jack);
	if ((mididev->flags & FREAD) && mididev->in_jack)
		close_jack(mididev->in_jack);
	mididev->opened = 0;
	mididev->arg = NULL;
	mididev->iintr = NULL;
	mididev->ointr = NULL;
	splx(s);
}

#ifdef UMIDI_DEBUG
#define DPR_PACKET(dir, sc, p)						\
if ((unsigned char)(p)->buffer[1]!=0xFE)				\
	DPRINTFN(500,							\
		 ("%s: umidi packet(" #dir "): %02X %02X %02X %02X\n",	\
		  USBDEVNAME(sc->sc_dev),				\
		  (unsigned char)(p)->buffer[0],			\
		  (unsigned char)(p)->buffer[1],			\
		  (unsigned char)(p)->buffer[2],			\
		  (unsigned char)(p)->buffer[3]));
#else
#define DPR_PACKET(dir, sc, p)
#endif

int
umidi_output(void *addr, int d)
{
        struct umidi_mididev *mididev = addr;
        struct umidi_softc *sc = mididev->sc;
	struct umidi_jack *out_jack = mididev->out_jack;
	struct umidi_endpoint *ep;
	int error;
	int s;

	if (sc->sc_dying || mididev->dying || !mididev->opened)
		return EIO;

	error = 0;
	if (out_jack && (ep=out_jack->endpoint)!=NULL && ep->buffer) {
		DPRINTFN(1000, ("umidi_output: ep=%p 0x%02x\n", ep, d));
		out_build_packet(out_jack->cable_number, &out_jack->packet, d);
		switch (out_jack->packet.state) {
		case PS_EXCL_0:
		case PS_END:
			DPR_PACKET(out, sc, &out_jack->packet);
			s = splusb();
			if (SIMPLEQ_EMPTY(&ep->queue)) {
				memcpy(ep->buffer,
				       out_jack->packet.buffer,
				       UMIDI_PACKET_SIZE);
				start_output_transfer(ep);
			}
			SIMPLEQ_INSERT_TAIL(&ep->queue,
					    out_jack, queue);
			splx(s);
			break;
		default:
			error = EINPROGRESS;
		}
	} else
		error = ENODEV;

	return error;
}

void
umidi_getinfo(void *addr, struct midi_info *mi)
{
        struct umidi_mididev *mididev = addr;
//        struct umidi_softc *sc = mididev->sc;

        mi->name = "USB MIDI I/F";
        mi->props = MIDI_PROP_OUT_INTR;
	if (mididev->in_jack)
		mi->props |= MIDI_PROP_CAN_INPUT;
}


/* --- */

/* enable/disable to transfer */
static usbd_status
enable_an_endpoint(struct umidi_endpoint *ep)
{
	struct umidi_softc *sc = ep->sc;
	usbd_status err;

	if (!ep->num_open++) {
		DPRINTF(("%s: enable_an_endpoint %p\n",
			 USBDEVNAME(sc->sc_dev), ep));
		SIMPLEQ_INIT(&ep->queue);
		ep->xfer = usbd_alloc_xfer(sc->sc_udev);
		if (!ep->xfer) {
			err = USBD_NOMEM;
			goto quit;
		}
		ep->buffer = usbd_alloc_buffer(ep->xfer, UMIDI_PACKET_SIZE);
		if (!ep->buffer) {
			usbd_free_xfer(ep->xfer);
			err = USBD_NOMEM;
			goto quit;
		}
		err = usbd_open_pipe(sc->sc_iface, ep->addr, 0, &ep->pipe);
		if (err) {
			usbd_free_xfer(ep->xfer);
			goto quit;
		}
	}
quit:
	return err;
}

static void
disable_an_endpoint(struct umidi_endpoint *ep, int force)
{
	if (force || (ep->num_open>0 && !--ep->num_open)) {
		ep->num_open = 0;
		usbd_abort_pipe(ep->pipe);
		usbd_clear_endpoint_stall(ep->pipe);
		usbd_close_pipe(ep->pipe);
		usbd_free_xfer(ep->xfer);
		DPRINTF(("%s: disable_an_endpoint %p\n",
			 USBDEVNAME(ep->sc->sc_dev), ep));
	}
}


/* alloc/free the array of endpoint structures */

static usbd_status alloc_all_endpoints_fixed_ep(struct umidi_softc *);
static usbd_status alloc_all_endpoints_yamaha(struct umidi_softc *);
static usbd_status alloc_all_endpoints_genuine(struct umidi_softc *);

static usbd_status
alloc_all_endpoints(struct umidi_softc *sc)
{
        if (UMQ_ISTYPE(sc, UMQ_TYPE_FIXED_EP)) {
		return alloc_all_endpoints_fixed_ep(sc);
        } else if (UMQ_ISTYPE(sc, UMQ_TYPE_YAMAHA)) {
		return alloc_all_endpoints_yamaha(sc);
	} else {
		return alloc_all_endpoints_genuine(sc);
	}
}

static void
free_all_endpoints(struct umidi_softc *sc)
{
	/* assumes that all endpoints are disabled. */
	free(sc->sc_endpoints, M_USBDEV);
	sc->sc_endpoints = sc->sc_out_ep = sc->sc_in_ep = NULL;
}

static usbd_status
alloc_all_endpoints_fixed_ep(struct umidi_softc *sc)
{
	struct umq_fixed_ep_desc *fp;
	usb_endpoint_descriptor_t *epd;
	int i;

	fp = umidi_get_quirk_data_from_type(sc->sc_quirk,
					    UMQ_TYPE_FIXED_EP);
	sc->sc_out_num_jacks = 0;
	sc->sc_in_num_jacks = 0;
	sc->sc_out_num_endpoints = fp->num_out_ep;
	sc->sc_in_num_endpoints = fp->num_in_ep;
	sc->sc_endpoints = malloc(sizeof(*sc->sc_out_ep)*
				  (sc->sc_out_num_endpoints+
				   sc->sc_in_num_endpoints),
				  M_USBDEV, M_WAITOK);
	if (!sc->sc_endpoints) {
		return USBD_NOMEM;
	}
	sc->sc_out_ep = sc->sc_out_num_endpoints ? sc->sc_endpoints : NULL;
	sc->sc_in_ep =
	    sc->sc_in_num_endpoints ?
		sc->sc_endpoints+sc->sc_out_num_endpoints : NULL;
	for (i=0; i<sc->sc_out_num_endpoints; i++) {
		epd = usbd_interface2endpoint_descriptor(
			sc->sc_iface,
			fp->out_ep[i].ep);
		if (!epd) {
			printf("%s: cannot get endpoint descriptor(out:%d)\n",
			       USBDEVNAME(sc->sc_dev), fp->out_ep[i].ep);
			goto error;
		}
		if (UE_GET_XFERTYPE(epd->bmAttributes)!=UE_BULK ||
		    UE_GET_DIR(epd->bEndpointAddress)!=UE_DIR_OUT) {
			printf("%s: illegal endpoint(out:%d)\n",
			       USBDEVNAME(sc->sc_dev), fp->out_ep[i].ep);
			goto error;
		}
		sc->sc_out_ep[i].sc = sc;
		sc->sc_out_ep[i].addr = epd->bEndpointAddress;
		sc->sc_out_ep[i].num_jacks = fp->out_ep[i].num_jacks;
		sc->sc_out_num_jacks += fp->out_ep[i].num_jacks;
		sc->sc_out_ep[i].num_open = 0;
		memset(sc->sc_out_ep[i].jacks, 0,
		       sizeof(sc->sc_out_ep[i].jacks));
	}
	for (i=0; i<sc->sc_in_num_endpoints; i++) {
		epd = usbd_interface2endpoint_descriptor(
			sc->sc_iface,
			fp->in_ep[i].ep);
		if (!epd) {
			printf("%s: cannot get endpoint descriptor(in:%d)\n",
			       USBDEVNAME(sc->sc_dev), fp->in_ep[i].ep);
			goto error;
		}
		if (UE_GET_XFERTYPE(epd->bmAttributes)!=UE_BULK ||
		    UE_GET_DIR(epd->bEndpointAddress)!=UE_DIR_IN) {
			printf("%s: illegal endpoint(in:%d)\n",
			       USBDEVNAME(sc->sc_dev), fp->in_ep[i].ep);
			goto error;
		}
		sc->sc_in_ep[i].sc = sc;
		sc->sc_in_ep[i].addr = epd->bEndpointAddress;
		sc->sc_in_ep[i].num_jacks = fp->in_ep[i].num_jacks;
		sc->sc_in_num_jacks += fp->in_ep[i].num_jacks;
		sc->sc_in_ep[i].num_open = 0;
		memset(sc->sc_in_ep[i].jacks, 0,
		       sizeof(sc->sc_in_ep[i].jacks));
	}

	return USBD_NORMAL_COMPLETION;
error:
	free(sc->sc_endpoints, M_USBDEV);
	sc->sc_endpoints = NULL;
	return USBD_INVAL;
}

static usbd_status
alloc_all_endpoints_yamaha(struct umidi_softc *sc)
{
	/* This driver currently supports max 1in/1out bulk endpoints */
	usb_descriptor_t *desc;
	int out_ep, in_ep, out_addr, in_addr;
	size_t remain, descsize;

	/* count jacks */
	desc = TO_D(usbd_get_interface_descriptor(sc->sc_iface));
        desc = NEXT_D(desc); /* ifd -> csifd */
	remain = ((size_t)UGETW(TO_CSIFD(desc)->wTotalLength) -
		  (size_t)desc->bLength);
        desc = NEXT_D(desc);

	sc->sc_out_num_jacks = sc->sc_in_num_jacks = 0;
	out_ep = in_ep = -1;

	while (remain>=sizeof(usb_descriptor_t)) {
		descsize = desc->bLength;
		if (descsize>remain || descsize==0)
			break;
		if (desc->bDescriptorType==UDESC_CS_INTERFACE &&
		    remain>=UMIDI_JACK_DESCRIPTOR_SIZE) {
			if (desc->bDescriptorSubtype==UMIDI_OUT_JACK)
				sc->sc_out_num_jacks++;
			else if (desc->bDescriptorSubtype==UMIDI_IN_JACK)
				sc->sc_in_num_jacks++;
		} else if (desc->bDescriptorType==UDESC_ENDPOINT &&
			   remain>=USB_ENDPOINT_DESCRIPTOR_SIZE &&
			   (UE_GET_XFERTYPE(TO_EPD(desc)->bmAttributes)
			    == UE_BULK)) {
			if ((UE_GET_DIR(TO_EPD(desc)->bEndpointAddress)
			     ==UE_DIR_OUT) &&
			    out_addr==-1)
				out_addr = TO_EPD(desc)->bEndpointAddress;
			else if ((UE_GET_DIR(TO_EPD(desc)->bEndpointAddress)
			     ==UE_DIR_IN) &&
			    in_addr==-1)
				in_addr = TO_EPD(desc)->bEndpointAddress;
		}
			   
		desc = NEXT_D(desc);
		remain-=descsize;
	}

	/* validate some parameter */
	if (sc->sc_out_num_jacks>UMIDI_MAX_EPJACKS)
		sc->sc_out_num_jacks = UMIDI_MAX_EPJACKS;
	if (sc->sc_in_num_jacks>UMIDI_MAX_EPJACKS)
		sc->sc_in_num_jacks = UMIDI_MAX_EPJACKS;
	if (sc->sc_out_num_jacks && out_addr!=-1) {
		sc->sc_out_num_endpoints = 1;
	} else {
		sc->sc_out_num_endpoints = 0;
		sc->sc_out_num_jacks = 0;
	}
	if (sc->sc_in_num_jacks && in_addr!=-1) {
		sc->sc_in_num_endpoints = 1;
	} else {
		sc->sc_in_num_endpoints = 0;
		sc->sc_in_num_jacks = 0;
	}
	sc->sc_endpoints = malloc(sizeof(struct umidi_endpoint)*
				  (sc->sc_out_num_endpoints+
				   sc->sc_in_num_endpoints),
				  M_USBDEV, M_WAITOK);
	if (!sc->sc_endpoints)
		return USBD_NOMEM;
	if (sc->sc_out_num_endpoints) {
		sc->sc_out_ep = sc->sc_endpoints;
		sc->sc_out_ep->sc = sc;
		sc->sc_out_ep->addr = out_addr;
		sc->sc_out_ep->num_jacks = sc->sc_out_num_jacks;
		sc->sc_out_ep->num_open = 0;
		memset(sc->sc_out_ep->jacks, 0, sizeof(sc->sc_out_ep->jacks));
	} else
		sc->sc_out_ep = NULL;

	if (sc->sc_in_num_endpoints) {
		sc->sc_in_ep = sc->sc_endpoints+sc->sc_out_num_endpoints;
		sc->sc_in_ep->sc = sc;
		sc->sc_in_ep->addr = in_addr;
		sc->sc_in_ep->num_jacks = sc->sc_in_num_jacks;
		sc->sc_in_ep->num_open = 0;
		memset(sc->sc_in_ep->jacks, 0, sizeof(sc->sc_in_ep->jacks));
	} else
		sc->sc_in_ep = NULL;

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
alloc_all_endpoints_genuine(struct umidi_softc *sc)
{
	usb_descriptor_t *desc;
	int num_ep, i;
	size_t remain, descsize;
	struct umidi_endpoint *p, *q, *lowest, *endep, tmpep;
	int epaddr;

	desc = TO_D(usbd_get_interface_descriptor(sc->sc_iface));
	num_ep = TO_IFD(desc)->bNumEndpoints;
        desc = NEXT_D(desc); /* ifd -> csifd */
	remain = ((size_t)UGETW(TO_CSIFD(desc)->wTotalLength) -
		  (size_t)desc->bLength);
        desc = NEXT_D(desc);

	sc->sc_endpoints = p = malloc(sizeof(struct umidi_endpoint)*num_ep,
				      M_USBDEV, M_WAITOK);
	if (!p)
		return USBD_NOMEM;

	sc->sc_out_num_jacks = sc->sc_in_num_jacks = 0;
	sc->sc_out_num_endpoints = sc->sc_in_num_endpoints = 0;
	epaddr = -1;

	/* get the list of endpoints for midi stream */
	while (remain>=sizeof(usb_descriptor_t)) {
		descsize = desc->bLength;
		if (descsize>remain || descsize==0)
			break;
		if (desc->bDescriptorType==UDESC_ENDPOINT &&
		    remain>=USB_ENDPOINT_DESCRIPTOR_SIZE &&
		    UE_GET_XFERTYPE(TO_EPD(desc)->bmAttributes) == UE_BULK) {
			epaddr = TO_EPD(desc)->bEndpointAddress;
		} else if (desc->bDescriptorType==UDESC_CS_ENDPOINT &&
			   remain>=UMIDI_CS_ENDPOINT_DESCRIPTOR_SIZE &&
			   epaddr!=-1) {
			num_ep--;
			if (num_ep>0) {
				p->sc = sc;
				p->addr = epaddr;
				p->num_jacks = TO_CSEPD(desc)->bNumEmbMIDIJack;
				if (UE_GET_DIR(epaddr)==UE_DIR_OUT) {
					sc->sc_out_num_endpoints++;
					sc->sc_out_num_jacks += p->num_jacks;
				} else {
					sc->sc_in_num_endpoints++;
					sc->sc_in_num_jacks += p->num_jacks;
				}
				p++;
			}
		} else
			epaddr = -1;
		desc = NEXT_D(desc);
		remain-=descsize;
	}

	/* sort endpoints */
	num_ep = sc->sc_out_num_endpoints + sc->sc_in_num_endpoints;
	p = sc->sc_endpoints;
	endep = p + num_ep;
	while (p<endep) {
		lowest = p;
		for (q=p+1; q<endep; q++) {
			if ((UE_GET_DIR(lowest->addr)==UE_DIR_IN &&
			     UE_GET_DIR(q->addr)==UE_DIR_OUT) ||
			    ((UE_GET_DIR(lowest->addr)==
			      UE_GET_DIR(q->addr)) &&
			     (UE_GET_ADDR(lowest->addr)>
			      UE_GET_ADDR(q->addr))))
				lowest = q;
		}
		if (lowest != p) {
			memcpy((void *)&tmpep, (void *)p, sizeof(tmpep));
			memcpy((void *)p, (void *)lowest, sizeof(tmpep));
			memcpy((void *)lowest, (void *)&tmpep, sizeof(tmpep));
		}
	}
	
	sc->sc_out_ep = sc->sc_out_num_endpoints ? sc->sc_endpoints : NULL;
	sc->sc_in_ep =
	    sc->sc_in_num_endpoints ?
		sc->sc_endpoints+sc->sc_out_num_endpoints : NULL;
	for (i=0; i<num_ep; i++) {
		sc->sc_endpoints[i].num_open = 0;
	}

	return USBD_NORMAL_COMPLETION;
}


/* */
static usbd_status
alloc_all_jacks(struct umidi_softc *sc)
{
	int i, j;
	struct umidi_endpoint *ep;
	struct umidi_jack *jack, **rjack;

	/* allocate/initialize structures */
	sc->sc_jacks =
	    malloc(sizeof(*sc->sc_out_jacks)*(sc->sc_in_num_jacks+
					      sc->sc_out_num_jacks),
		   M_USBDEV, M_WAITOK);
	if (!sc->sc_jacks)
		return USBD_NOMEM;
	sc->sc_out_jacks =
	    sc->sc_out_num_jacks ? sc->sc_jacks : NULL;
	sc->sc_in_jacks =
	    sc->sc_in_num_jacks ? sc->sc_jacks+sc->sc_out_num_jacks : NULL;

	jack = &sc->sc_out_jacks[0];
	for (i=0; i<sc->sc_out_num_jacks; i++) {
		init_packet(&jack->packet);
		jack->cable_number = i;
		jack->mididev = NULL;
		jack++;
	}
	jack = &sc->sc_in_jacks[0];
	for (i=0; i<sc->sc_in_num_jacks; i++) {
		init_packet(&jack->packet);
		jack->cable_number = i;
		jack->mididev = NULL;
		jack++;
	}

	/* assign each jacks to each endpoints */
	jack = &sc->sc_out_jacks[0];
	ep = &sc->sc_out_ep[0];
	for (i=0; i<sc->sc_out_num_endpoints; i++) {
		rjack = &ep->jacks[0];
		for (j=0; j<ep->num_jacks; j++) {
			*rjack = jack;
			jack->endpoint = ep;
			jack++;
			rjack++;
		}
		ep++;
	}
	jack = &sc->sc_in_jacks[0];
	ep = &sc->sc_in_ep[0];
	for (i=0; i<sc->sc_in_num_endpoints; i++) {
		rjack = &ep->jacks[0];
		for (j=0; j<ep->num_jacks; j++) {
			*rjack = jack;
			jack->endpoint = ep;
			jack++;
			rjack++;
		}
		ep++;
	}

	return USBD_NORMAL_COMPLETION;
}

static void
free_all_jacks(struct umidi_softc *sc)
{
	int s;

	s = splaudio();
	if (sc->sc_out_jacks) {
		free(sc->sc_jacks, M_USBDEV);
		sc->sc_jacks = sc->sc_in_jacks = sc->sc_out_jacks = NULL;
	}
	splx(s);
}

static usbd_status
bind_jacks_to_mididev(struct umidi_softc *sc,
		      struct umidi_jack *out_jack,
		      struct umidi_jack *in_jack,
		      struct umidi_mididev *mididev)
{
	if (mididev->opened || mididev->out_jack || mididev->in_jack)
		return USBD_IN_USE;

	mididev->in_jack = in_jack;
	mididev->out_jack = out_jack;
	if (in_jack)
		in_jack->mididev = mididev;
	if (out_jack)
		out_jack->mididev = mididev;

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
unbind_jacks_from_mididev(struct umidi_mididev *mididev)
{
	if (mididev->opened) {
		if ((mididev->flags&FWRITE) && mididev->out_jack)
			close_jack(mididev->out_jack);
		if ((mididev->flags&FWRITE) && mididev->in_jack)
			close_jack(mididev->in_jack);
		mididev->out_jack = mididev->in_jack = NULL;
	}

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
unbind_all_jacks(struct umidi_softc *sc)
{
	usbd_status err;
	int i;

	if (sc->sc_mididevs)
		for (i=0; i<sc->sc_num_mididevs; i++) {
			err = unbind_jacks_from_mididev(&sc->sc_mididevs[i]);
			if (err!=USBD_NORMAL_COMPLETION)
				return err;
		}

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
assign_all_jacks_automatically(struct umidi_softc *sc)
{
	usbd_status err;
	int i;
	struct umidi_jack *out, *in;

	err =
	    alloc_all_mididevs(sc,
			       max(sc->sc_out_num_jacks, sc->sc_in_num_jacks));
	if (err!=USBD_NORMAL_COMPLETION)
		return err;

	for (i=0; i<sc->sc_num_mididevs; i++) {
		out = (i<sc->sc_out_num_jacks) ? &sc->sc_out_jacks[i]:NULL;
		in = (i<sc->sc_in_num_jacks) ? &sc->sc_in_jacks[i]:NULL;
		err = bind_jacks_to_mididev(sc, out, in, &sc->sc_mididevs[i]);
		if (err!=USBD_NORMAL_COMPLETION) {
			free_all_mididevs(sc);
			return err;
		}
	}

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
open_jack(struct umidi_jack *jack)
{
	return enable_an_endpoint(jack->endpoint);
}

static void
close_jack(struct umidi_jack *jack)
{
	disable_an_endpoint(jack->endpoint, 0);
}


static usbd_status
attach_mididev(struct umidi_softc *sc, struct umidi_mididev *mididev)
{
	if (mididev->sc)
		return USBD_IN_USE;

	mididev->sc = sc;
	mididev->opened = 0;
	mididev->dying = 0;

	mididev->mdev = midi_attach_mi(&umidi_hw_if, mididev, &sc->sc_dev);

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
detach_mididev(struct umidi_mididev *mididev, int flags)
{
	if (!mididev->sc)
		return USBD_NO_ADDR;

	if (mididev->opened) {
		umidi_close(mididev);
	}

	mididev->iintr = NULL;
	mididev->ointr = NULL;
	mididev->arg = NULL;

	if (mididev->mdev)
		config_detach(mididev->mdev, flags);

	mididev->sc = NULL;

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
deactivate_mididev(struct umidi_mididev *mididev)
{
	if (!mididev->sc)
		return USBD_NO_ADDR;

	mididev->dying = 1;
	config_deactivate(mididev->mdev);

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
alloc_all_mididevs(struct umidi_softc *sc, int nmidi)
{
	sc->sc_num_mididevs = nmidi;
	sc->sc_mididevs = malloc(sizeof(*sc->sc_mididevs)*nmidi,
				 M_USBDEV, M_WAITOK);
	memset(sc->sc_mididevs, 0, sizeof(*sc->sc_mididevs)*nmidi);
	if (!sc->sc_mididevs)
		return USBD_NOMEM;

	return USBD_NORMAL_COMPLETION;
}

static void
free_all_mididevs(struct umidi_softc *sc)
{
	sc->sc_num_mididevs = 0;
	if (sc->sc_mididevs)
		free(sc->sc_mididevs, M_USBDEV);
}

static usbd_status
attach_all_mididevs(struct umidi_softc *sc)
{
	usbd_status err;
	int i;

	if (sc->sc_mididevs)
		for (i=0; i<sc->sc_num_mididevs; i++) {
			err = attach_mididev(sc, &sc->sc_mididevs[i]);
			if (err!=USBD_NORMAL_COMPLETION)
				return err;
		}

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
detach_all_mididevs(struct umidi_softc *sc, int flags)
{
	usbd_status err;
	int i;

	if (sc->sc_mididevs)
		for (i=0; i<sc->sc_num_mididevs; i++) {
			err = detach_mididev(&sc->sc_mididevs[i], flags);
			if (err!=USBD_NORMAL_COMPLETION)
				return err;
		}

	return USBD_NORMAL_COMPLETION;
}

static usbd_status
deactivate_all_mididevs(struct umidi_softc *sc)
{
	usbd_status err;
	int i;

	if (sc->sc_mididevs)
		for (i=0; i<sc->sc_num_mididevs; i++) {
			err = deactivate_mididev(&sc->sc_mididevs[i]);
			if (err!=USBD_NORMAL_COMPLETION)
				return err;
		}

	return USBD_NORMAL_COMPLETION;
}

#ifdef UMIDI_DEBUG
static void
dump_sc(struct umidi_softc *sc)
{
	int i;

	DPRINTFN(10, ("%s: dump_sc\n", USBDEVNAME(sc->sc_dev)));
	for (i=0; i<sc->sc_out_num_endpoints; i++) {
		DPRINTFN(10, ("\tout_ep(%p):\n", &sc->sc_out_ep[i]));
		dump_ep(&sc->sc_out_ep[i]);
	}
	for (i=0; i<sc->sc_in_num_endpoints; i++) {
		DPRINTFN(10, ("\tin_ep(%p):\n", &sc->sc_in_ep[i]));
		dump_ep(&sc->sc_in_ep[i]);
	}
}

static void
dump_ep(struct umidi_endpoint *ep)
{
	int i;
	for (i=0; i<ep->num_jacks; i++) {
		DPRINTFN(10, ("\t\tjack(%p):\n", ep->jacks[i]));
		dump_jack(ep->jacks[i]);
	}
}
static void
dump_jack(struct umidi_jack *jack)
{
	DPRINTFN(10, ("\t\t\tep=%p, mididev=%p\n",
		      jack->endpoint, jack->mididev));
}

#endif /* UMIDI_DEBUG */



/*
 * MUX MIDI PACKET
 */

static int packet_length[16] = {
	/*0*/	-1,
	/*1*/	-1,
	/*2*/	2,
	/*3*/	3,
	/*4*/	3,
	/*5*/	1,
	/*6*/	2,
	/*7*/	3,
	/*8*/	3,
	/*9*/	3,
	/*A*/	3,
	/*B*/	3,
	/*C*/	2,
	/*D*/	2,
	/*E*/	3,
	/*F*/	1,
};

static struct {
	int		cin;
	packet_state_t	next;
} packet_0xFX[16] = {
	/*F0: SysEx */	{ 0x04, PS_EXCL_1 },
	/*F1: MTC */	{ 0x02, PS_NORMAL_1OF2 },
	/*F2: S.POS */	{ 0x03, PS_NORMAL_1OF3 },
	/*F3: S.SEL */	{ 0x02, PS_NORMAL_1OF2 },
	/*F4: UNDEF */	{ 0x00, PS_INITIAL },
	/*F5: UNDEF */	{ 0x00, PS_INITIAL },
	/*F6: Tune */	{ 0x0F, PS_END },
	/*F7: EofEx */	{ 0x00, PS_INITIAL },
	/*F8: Timing */	{ 0x0F, PS_END },
	/*F9: UNDEF */	{ 0x00, PS_INITIAL },
	/*FA: Start */	{ 0x0F, PS_END },
	/*FB: Cont */	{ 0x0F, PS_END },
	/*FC: Stop */	{ 0x0F, PS_END },
	/*FD: UNDEF */	{ 0x00, PS_INITIAL },
	/*FE: ActS */	{ 0x0F, PS_END },
	/*FF: Reset */	{ 0x0F, PS_END },
};

#define	GET_CN(p)		(((unsigned char)(p)>>4)&0x0F)
#define GET_CIN(p)		((unsigned char)(p)&0x0F)
#define MIX_CN_CIN(cn, cin) \
	((unsigned char)((((unsigned char)(cn)&0x0F)<<4)| \
	                  ((unsigned char)(cin)&0x0F)))

static void
init_packet(struct umidi_packet *packet)
{
	memset(packet->buffer, 0, UMIDI_PACKET_SIZE);
	packet->state = PS_INITIAL;
}

static void
in_packet_to_mididev(struct umidi_endpoint *ep, uByte *packet)
{
	int cn, len, i;
	struct umidi_jack *jack;
	struct umidi_mididev *mididev;

	cn = GET_CN(packet[0]);
	len = packet_length[GET_CIN(packet[0])];
	jack = ep->jacks[cn];

	if (cn>=ep->num_jacks || !jack) {
		DPRINTF(("%s: stray umidi packet (in): %02X %02X %02X %02X\n",
			 USBDEVNAME(ep->sc->sc_dev),
			 (unsigned)packet[0],
			 (unsigned)packet[1],
			 (unsigned)packet[2],
			 (unsigned)packet[3]));
		return;
	}
	mididev = jack->mididev;
	if (!mididev || !mididev->opened || mididev->dying)
		return;
	DPR_PACKET(in, ep->sc, &jack->packet);
	if (mididev->iintr) {
		for (i=0; i<len; i++) {
			(*mididev->iintr)(mididev->arg, packet[i+1]);
		}
	}
}

static usbd_status
start_input_transfer(struct umidi_endpoint *ep)
{
	usbd_setup_xfer(ep->xfer, ep->pipe,
			(usbd_private_handle)ep,
			ep->buffer, UMIDI_PACKET_SIZE,
			0, USBD_NO_TIMEOUT, in_intr);
	return usbd_transfer(ep->xfer);
}

static usbd_status
start_output_transfer(struct umidi_endpoint *ep)
{
	usbd_setup_xfer(ep->xfer, ep->pipe,
			(usbd_private_handle)ep,
			ep->buffer, UMIDI_PACKET_SIZE,
			0, USBD_NO_TIMEOUT, out_intr);
	return usbd_transfer(ep->xfer);
}

static void
in_intr(usbd_xfer_handle xfer, usbd_private_handle priv, usbd_status status)
{
	struct umidi_endpoint *ep = (struct umidi_endpoint *)priv;

	if (ep->sc->sc_dying)
		return;

	in_packet_to_mididev(ep, ep->buffer);

	(void)start_input_transfer(ep);
}

static void
out_intr(usbd_xfer_handle xfer, usbd_private_handle priv, usbd_status status)
{
	struct umidi_endpoint *ep = (struct umidi_endpoint *)priv;
	struct umidi_softc *sc = ep->sc;
	struct umidi_jack *jack;
	struct umidi_mididev *mididev;

	if (sc->sc_dying)
		return;

	jack = SIMPLEQ_FIRST(&ep->queue);
	mididev = jack->mididev;

	SIMPLEQ_REMOVE_HEAD(&ep->queue, jack, queue);
	if (!SIMPLEQ_EMPTY(&ep->queue)) {
		memcpy(ep->buffer,
		       SIMPLEQ_FIRST(&ep->queue)->packet.buffer,
		       UMIDI_PACKET_SIZE);
		(void)start_output_transfer(ep);
	}
	if (mididev && mididev->ointr) {
		(*mididev->ointr)(mididev->arg);
	}
}

static void
out_build_packet(int cable_number, struct umidi_packet *packet, uByte in)
{
	int cin;

retry:
	switch (packet->state) {
	case PS_END:
	case PS_INITIAL:
		memset(packet->buffer, 0, UMIDI_PACKET_SIZE);
		if (in<0x80) {
			/* ??? */
			packet->state = PS_INITIAL;
			break;
		}
		if (in>=0xf0) {
			cin=packet_0xFX[in&0x0F].cin;
			packet->state=packet_0xFX[in&0x0F].next;
		} else {
			cin=(unsigned char)in>>4;
			switch (packet_length[cin]) {
			case 2:
				packet->state = PS_NORMAL_1OF2;
				break;
			case 3:
				packet->state = PS_NORMAL_1OF3;
				break;
			default:
				/* ??? */
				packet->state = PS_INITIAL;
			}
		}
		packet->buffer[0] = MIX_CN_CIN(cable_number, cin);
		packet->buffer[1] = in;
		break;
	case PS_NORMAL_1OF3:
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[2] = in;
		packet->state = PS_NORMAL_2OF3;
		break;
	case PS_NORMAL_2OF3:
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[3] = in;
		packet->state = PS_END;
		break;
	case PS_NORMAL_1OF2:
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[2] = in;
		packet->state = PS_END;
		break;
	case PS_EXCL_0:
		memset(packet->buffer, 0, UMIDI_PACKET_SIZE);
		if (in==0xF7) {
			packet->buffer[0] = MIX_CN_CIN(cable_number, 0x05);
			packet->buffer[1] = 0xF7;
			packet->state = PS_END;
			break;
		}
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[1] = in;
		packet->state = PS_EXCL_1;
		break;
	case PS_EXCL_1:
		if (in==0xF7) {
			packet->buffer[0] = MIX_CN_CIN(cable_number, 0x06);
			packet->buffer[2] = 0xF7;
			packet->state = PS_END;
			break;
		}
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[2] = in;
		packet->state = PS_EXCL_2;
		break;
	case PS_EXCL_2:
		if (in==0xF7) {
			packet->buffer[0] = MIX_CN_CIN(cable_number, 0x07);
			packet->buffer[3] = 0xF7;
			packet->state = PS_END;
			break;
		}
		if (in>=0x80) { /* ??? */ packet->state = PS_END; break; }
		packet->buffer[0] = MIX_CN_CIN(cable_number, 0x04);
		packet->buffer[3] = in;
		packet->state = PS_EXCL_0;
		break;
	default:
		printf("umidi: ambiguous state.\n");
		packet->state = PS_INITIAL;
		goto retry;
	}
}


