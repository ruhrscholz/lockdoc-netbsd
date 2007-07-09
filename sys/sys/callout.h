/*	$NetBSD: callout.h,v 1.24 2007/07/09 21:11:32 ad Exp $	*/

/*-
 * Copyright (c) 2000, 2003, 2006, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center, and by Andrew Doran.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#ifndef _SYS_CALLOUT_H_
#define _SYS_CALLOUT_H_

#include <sys/types.h>

/*
 * The callout implementation is private to kern_timeout.c yet uses
 * caller-supplied storage, as lightweight callout operations are
 * critical to system performance.
 *
 * The size of callout_t must remain constant in order to ensure ABI
 * compatibility for kernel modules: it may become smaller, but must
 * not grow.  If more space is required, rearrange the members of
 * callout_impl_t.
 */
typedef struct callout {
	uint64_t _c_store[5];
} callout_t;

/* Internal flags. */
#define	CALLOUT_PENDING		0x0002	/* callout is on the queue */
#define	CALLOUT_FIRED		0x0004	/* callout has fired */
#define	CALLOUT_INVOKING	0x0008	/* callout function is being invoked */

/* End-user flags. */
#define	CALLOUT_MPSAFE		0x0100	/* does not need kernel_lock */
#define	CALLOUT_FLAGMASK	0xff00

#ifdef _KERNEL
void	callout_startup(void);
void	callout_startup2(void);
void	callout_hardclock(void);

void	callout_init(callout_t *, u_int);
void	callout_destroy(callout_t *);
void	callout_setfunc(callout_t *, void (*)(void *), void *);
void	callout_reset(callout_t *, int, void (*)(void *), void *);
void	callout_schedule(callout_t *, int);
bool	callout_stop(callout_t *);
bool	callout_pending(callout_t *);
bool	callout_expired(callout_t *);
bool	callout_active(callout_t *);
bool	callout_invoking(callout_t *);
void	callout_ack(callout_t *);
#endif	/* _KERNEL */

#endif /* !_SYS_CALLOUT_H_ */
