/*	$NetBSD: mach_machdep.c,v 1.3 2002/11/15 21:48:06 manu Exp $ */

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas and Emmanuel Dreyfus.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
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
__KERNEL_RCSID(0, "$NetBSD: mach_machdep.c,v 1.3 2002/11/15 21:48:06 manu Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/exec.h>
#include <sys/user.h>
#include <sys/filedesc.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/signal.h>
#include <sys/signalvar.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <sys/exec_elf.h>
#include <sys/exec_macho.h>

#include <compat/mach/mach_types.h>
#include <compat/mach/mach_host.h>

#include <machine/cpu.h>
#include <machine/psl.h>
#include <machine/reg.h>
#include <machine/vmparam.h>

#include <uvm/uvm_extern.h>

void mach_trap __P((struct trapframe *));

/*
 * Fast syscall gate trap...
 */
void
mach_trap(frame)
	struct trapframe *frame;
{
	extern struct emul emul_mach;
	struct proc *p = curproc;

	if (p->p_emul != &emul_mach) {
		DPRINTF(("mach trap %d on bad emulation\n", frame->exc));
		trapsignal(p, SIGBUS, 0);
		return;
	}

	switch (frame->exc) {
	case 0:
		DPRINTF(("mach_pthread_self();\n"));
		break;
	case 1:
		DPRINTF(("mach_pthread_set_self();\n"));
		break;
	default:
		uprintf("unknown mach trap %d\n", frame->exc);
		break;
	}
}

void
mach_host_basic_info(info)
    struct mach_host_basic_info *info;
{
	/* XXX fill this  accurately */
	info->max_cpus = 1;
	info->avail_cpus = 1;
	info->memory_size = uvmexp.active + uvmexp.inactive;
	info->cpu_type = MACHO_CPU_TYPE_POWERPC;
	info->cpu_subtype = MACHO_CPU_SUBTYPE_POWERPC_ALL;
}
