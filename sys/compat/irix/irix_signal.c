/*	$NetBSD: irix_signal.c,v 1.1 2001/12/08 11:17:37 manu Exp $ */

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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: irix_signal.c,v 1.1 2001/12/08 11:17:37 manu Exp $");

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <machine/regnum.h>

#include <compat/svr4/svr4_types.h>

#include <compat/irix/irix_signal.h>
#include <compat/irix/irix_syscallargs.h>

extern const int native_to_svr4_sig[];
extern const int svr4_to_native_sig[];

#define irix_sigmask(n)         (1 << (((n) - 1) & 31))
#define irix_sigword(n)         (((n) - 1) >> 5) 
#define irix_sigemptyset(s)     memset((s), 0, sizeof(*(s)))
#define irix_sigismember(s, n)  ((s)->bits[irix_sigword(n)] & irix_sigmask(n))
#define irix_sigaddset(s, n)    ((s)->bits[irix_sigword(n)] |= irix_sigmask(n))

void
native_to_irix_sigset(bss, sss)
	 const sigset_t *bss;
	 irix_sigset_t *sss;
{
	 int i, newsig;

	 irix_sigemptyset(sss);
	 for (i = 1; i < NSIG; i++) {
		 if (sigismember(bss, i)) {
			 newsig = native_to_svr4_sig[i];
			 if (newsig)
			 	irix_sigaddset(sss, newsig);
		 }
	 }
}

void    
irix_to_native_sigset(sss, bss)
	const irix_sigset_t *sss;
	sigset_t *bss;
{       
	int i, newsig;
	
	sigemptyset(bss);
	for (i = 1; i < SVR4_NSIG; i++) {
		if (irix_sigismember(sss, i)) {
			newsig = svr4_to_native_sig[i];
			if (newsig)
				sigaddset(bss, newsig);
		}
	}
}

void
irix_sendsig(catcher, sig, mask, code)  /* XXX Check me */
        sig_t catcher;
        int sig;
        sigset_t *mask;
        u_long code;
{
        struct proc *p = curproc;
        struct irix_sigframe *fp;
        struct frame *f;
        int i,onstack;
        struct irix_sigframe sf;
 
#ifdef DEBUG_IRIX
        printf("irix_sendsig()\n");
#endif /* DEBUG_IRIX */
        f = (struct frame *)p->p_md.md_regs;

        /*
         * Do we need to jump onto the signal stack?
         */
        onstack =
            (p->p_sigctx.ps_sigstk.ss_flags & (SS_DISABLE | SS_ONSTACK)) == 0 &&
            (SIGACTION(p, sig).sa_flags & SA_ONSTACK) != 0;

        /*
         * not sure it works yet.
         */
        onstack=0;

        /*
         * Allocate space for the signal handler context.
         */
        if (onstack)
                fp = (struct irix_sigframe *)
                    ((caddr_t)p->p_sigctx.ps_sigstk.ss_sp
                    + p->p_sigctx.ps_sigstk.ss_size);
        else
                /* cast for _MIPS_BSD_API == _MIPS_BSD_API_LP32_64CLEAN case */
                fp = (struct irix_sigframe *)(u_int32_t)f->f_regs[SP];

        /*
         * Build stack frame for signal trampoline.
         */
        memset(&sf, 0, sizeof sf);

        native_to_irix_sigset(mask, &sf.isf_sc.isc_sigset);
        for (i=0; i<32; i++) {
                sf.isf_sc.isc_regs[i] = f->f_regs[i];
        }
        sf.isf_sc.isc_regs[0] = 0;
        sf.isf_sc.isc_fp_rounded_result = 0;
	sf.isf_sc.isc_regmask = ~0x1UL;
        sf.isf_sc.isc_mdhi = f->f_regs[MULHI];
        sf.isf_sc.isc_mdlo = f->f_regs[MULLO];
        sf.isf_sc.isc_pc = f->f_regs[PC];
        sf.isf_sc.isc_status = f->f_regs[SR];   /* XXX */
        sf.isf_sc.isc_cause = f->f_regs[CAUSE];
        sf.isf_sc.isc_badvaddr = f->f_regs[BADVADDR];   /* XXX */
        sf.isf_sc.isc_ownedfp = 0;
        sf.isf_sc.isc_ssflags = 0;

        /*
         * Save signal stack.  XXX broken
         */
        /* kregs.sc_onstack = p->p_sigctx.ps_sigstk.ss_flags & SS_ONSTACK; */

        /*
         * Install the sigframe onto the stack
         */
        fp = (struct irix_sigframe *)((unsigned long)fp
	    - sizeof(struct irix_sigframe));
	fp = (struct irix_sigframe *)((unsigned long)fp 
	    & ~0xfUL);		/* 16 bytes alignement */
        if (copyout(&sf, fp, sizeof(sf)) != 0) {
                /*
                 * Process has trashed its stack; give it an illegal
                 * instruction to halt it in its tracks.
                 */
#ifdef DEBUG_IRIX
                printf("irix_sendsig: stack trashed\n");
#endif /* DEBUG_IRIX */
                sigexit(p, SIGILL);
                /* NOTREACHED */
        }

        /* Set up the registers to return to sigcode. */
        f->f_regs[A0] = native_to_svr4_sig[sig];
        f->f_regs[A1] = 0;
        f->f_regs[A2] = (unsigned long)&fp->isf_sc;

#ifdef DEBUG_IRIX
        printf("sigcontext is at %p\n", &fp->isf_sc);
#endif /* DEBUG_IRIX */

	f->f_regs[RA] = (unsigned long)p->p_sigctx.ps_sigcode;
        f->f_regs[SP] = (unsigned long)fp;
        f->f_regs[T9] = (unsigned long)catcher;
        f->f_regs[A3] = (unsigned long)catcher;
        f->f_regs[PC] = (unsigned long)catcher;

        /* Remember that we're now on the signal stack. */
        if (onstack)
                p->p_sigctx.ps_sigstk.ss_flags |= SS_ONSTACK;

#ifdef DEBUG_IRIX
	printf("returning from irix_sendsig()\n");
#endif
        return;
}

int      
irix_sys_sigreturn(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{       
	struct irix_sys_sigreturn_args /* {
		syscallarg(struct irix_sigframe *) sf;
	} */ *uap = v;
	struct irix_sigframe *sf, ksf;
	struct frame *f; 
	sigset_t mask;
	int i, error;

#ifdef DEBUG_IRIX
	printf("irix_sys_sigreturn()\n");
#endif /* DEBUG_IRIX */

	/*
	 * The trampoline code hands us the context.
	 * It is unsafe to keep track of it ourselves, in the event that a
	 * program jumps out of a signal handler.
	 */
	sf = SCARG(uap, sf);

	if ((error = copyin(sf, &ksf, sizeof(ksf))) != 0)
		return (error);

	/* Restore the register context. */
	f = (struct frame *)p->p_md.md_regs;
	for (i=0; i<32; i++)
		f->f_regs[i] = ksf.isf_sc.isc_regs[i];
	f->f_regs[MULLO] = ksf.isf_sc.isc_mdlo;
	f->f_regs[MULHI] = ksf.isf_sc.isc_mdhi;
	f->f_regs[PC] = ksf.isf_sc.isc_pc;
	f->f_regs[BADVADDR] = ksf.isf_sc.isc_badvaddr;
	f->f_regs[CAUSE] = ksf.isf_sc.isc_cause;
	f->f_regs[SR] = ksf.isf_sc.isc_status;

	/* Restore signal stack. */
	p->p_sigctx.ps_sigstk.ss_flags &= ~SS_ONSTACK;

	/* Restore signal mask. */
	irix_to_native_sigset((irix_sigset_t *)&ksf.isf_sc.isc_sigset, &mask);
	(void)sigprocmask1(p, SIG_SETMASK, &mask, 0);

	return (EJUSTRETURN);
}

int
irix_sys_sginap(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{
	struct irix_sys_sginap_args /* {
		syscallarg(long) ticks;
	} */ *uap = v;
	int rticks = SCARG(uap, ticks);
	struct timeval tvb, tve, tvd;
	long long delta;
	int dontcare;

	if (rticks != 0)
		microtime(&tvb);

	if ((tsleep(&dontcare, PCATCH, 0, rticks) != 0) && (rticks != 0)) {
		microtime(&tve);
		timersub(&tve, &tvb, &tvd);
		delta = ((tvd.tv_sec * 1000000) + tvd.tv_usec); /* XXX */
		*retval = (register_t)(rticks - (delta / tick));
	}

	return 0;
}
