/*	$NetBSD: irix_signal.h,v 1.1 2001/12/08 11:17:37 manu Exp $ */

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emmanuel Dreyfus.
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

#ifndef _IRIX_SIGNAL_H_
#define _IRIX_SIGNAL_H_

#include <sys/types.h>
#include <sys/signal.h>

#include <compat/irix/irix_types.h>

/* From IRIX's <sys/signal.h> */

typedef struct irix_sigcontext {
	__uint32_t	isc_regmask;
	__uint32_t	isc_status;
	__uint64_t	isc_pc;
	__uint64_t	isc_regs[32];
	__uint64_t	isc_fpregs[32];
	__uint32_t	isc_ownedfp;
	__uint32_t	isc_fpc_csr;
	__uint32_t	isc_fpc_eir;
	__uint32_t	isc_ssflags;
	__uint64_t	isc_mdhi;
	__uint64_t	isc_mdlo;
	__uint64_t	isc_cause;
	__uint64_t	isc_badvaddr;
	__uint64_t	isc_triggersave;
	irix_sigset_t	isc_sigset;
	__uint64_t	isc_fp_rounded_result;
	__uint64_t	isc_pad[31];
} irix_sigcontext_t;

struct irix_sigframe {
	struct irix_sigcontext isf_sc;
};

#ifdef _KERNEL
__BEGIN_DECLS
void native_to_irix_sigset __P((const sigset_t *, irix_sigset_t *));
void irix_to_native_sigset __P((const irix_sigset_t *, sigset_t *));
void irix_sendsig __P((sig_t, int, sigset_t *, u_long));
__END_DECLS
#endif /* _KERNEL */


#endif /* _IRIX_SIGNAL_H_ */
