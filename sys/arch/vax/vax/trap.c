/*      $NetBSD: trap.c,v 1.9 1995/04/22 20:29:09 christos Exp $     */

/*
 * Copyright (c) 1994 Ludd, University of Lule}, Sweden.
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
 *     This product includes software developed at Ludd, University of Lule}.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

 /* All bugs are subject to removal without further notice */
		


#include "sys/types.h"
#include "sys/param.h"
#include "sys/proc.h"
#include "sys/user.h"
#include "sys/syscall.h"
#include "sys/systm.h"
#include "sys/signalvar.h"
#include "vm/vm.h"
#include "vm/vm_kern.h"
#include "vm/vm_page.h"
#include "vax/include/mtpr.h"
#include "vax/include/pte.h"
#include "vax/include/pcb.h"
#include "vax/include/trap.h"
#include "vax/include/pmap.h"
#include "kern/syscalls.c"


extern 	int want_resched,whichqs;
volatile int startsysc=0,ovalidstart=0,faultdebug=0,haltfault=0;


userret(p, pc, psl)
	struct proc *p;
	u_int pc, psl;
{
	int s,sig;

        while ((sig = CURSIG(p)) !=0)
                postsig(sig);
        p->p_priority = p->p_usrpri;
        if (want_resched) {
                /*
                 * Since we are curproc, clock will normally just change
                 * our priority without moving us from one queue to another
                 * (since the running process is not on a queue.)
                 * If that happened after we setrq ourselves but before we
                 * swtch()'ed, we might not be on the queue indicated by
                 * our priority.
                 */
                s=splstatclock();
                setrunqueue(curproc);
                cpu_switch();
                splx(s);
                while ((sig = CURSIG(curproc)) != 0)
                        postsig(sig);
        }

        curpriority = curproc->p_priority;
}

char *traptypes[]={
	"reserved addressing",
	"privileged instruction",
	"reserved operand",
	"breakpoint instruction",
	"Nothing",
	"system call ",
	"arithmetic trap",
	"asynchronous system trap",
	"page table length fault",
	"translation violation fault",
	"trace trap",
	"compatibility mode fault",
	"access violation fault",
};

arithflt(frame)
	struct trapframe *frame;
{
	u_int	sig, type=frame->trap,trapsig=1,s;
	u_int	rv, addr,*i,j;
	struct	proc *p=curproc;
	struct	pmap *pm;
	vm_map_t map;
	vm_prot_t ftype;
	extern vm_map_t	pte_map;
	
	if((frame->psl & PSL_U) == PSL_U)
		type|=T_USER;

	type&=~(T_WRITE|T_PTEFETCH);
	switch(type){

	default:
faulter:
		if(frame->trap<12)
			printf("\nKernel fault: %s. Stack dump:\n\n",
				traptypes[frame->trap]);
		else
			printf("\nKernel fault: %d. Stack dump:\n\n",
				frame->trap);
		printf("FP %8x AP %8x R0 %8x R1 %8x\n",frame->fp,
			frame->ap, frame->r0, frame->r1);
		printf("R2 %8x R3 %8x R4 %8x R5 %8x\n",frame->r2,frame->r3,
			frame->r4,frame->r5);
		printf("TRAP %2x CODE %6x PC %8x PSL %8x\n",frame->trap,
			frame->code, frame->pc, frame->psl);
			i=(u_int*)&(frame->psl);
		printf("(RET PC) %8x, (RET PSL) %8x\n",i[1],i[2]);
		for(j=3;j<15;j+=4)
			printf("%8x %8x %8x %8x\n",i[j],i[j+1],i[j+2],
				i[j+3]);
		asm("halt");
		printf("trap type %x, code %x, pc %x, psl %x\n",
			frame->trap, frame->code, frame->pc, frame->psl);
		showstate(curproc);
		asm("halt");
		panic("trap");

	case T_TRANSFLT|T_USER:
	case T_TRANSFLT: /* Translation invalid - may be simul page ref */
		if(frame->trap&T_PTEFETCH){
			u_int	*ptep, *pte, *pte1;

			if(frame->code<0x40000000)
				ptep=(u_int *)p->p_addr->u_pcb.P0BR;
			else
				ptep=(u_int *)p->p_addr->u_pcb.P1BR;
			pte1=(u_int *)trunc_page(&ptep[(frame->code
				&0x3fffffff)>>PG_SHIFT]);
			pte=(u_int*)&Sysmap[((u_int)pte1&0x3fffffff)>>PG_SHIFT];	
			if(*pte&PG_SREF){ /* Yes, simulated */
				s=splhigh();

				*pte|=PG_REF|PG_V;*pte&=~PG_SREF;pte++;
				*pte|=PG_REF|PG_V;*pte&=~PG_SREF;
				mtpr(0,PR_TBIA);
				splx(s);
				goto uret;
			}
		} else {
			u_int   *ptep, *pte;

			frame->code=trunc_page(frame->code);
			if(frame->code<0x40000000){
				ptep=(u_int *)p->p_addr->u_pcb.P0BR;
				pte=&ptep[(frame->code>>PG_SHIFT)];
			} else if(frame->code>0x7fffffff){
				pte=(u_int *)&Sysmap[((u_int)frame->code&
					0x3fffffff)>>PG_SHIFT];
			} else {
				ptep=(u_int *)p->p_addr->u_pcb.P1BR;
				pte=&ptep[(frame->code&0x3fffffff)>>PG_SHIFT];
			}
			if(*pte&PG_SREF){
				s=splhigh();
				*pte|=PG_REF|PG_V;*pte&=~PG_SREF;pte++;
				*pte|=PG_REF|PG_V;*pte&=~PG_SREF;
			/*	mtpr(frame->code,PR_TBIS); */
			/*	mtpr(frame->code+NBPG,PR_TBIS); */
				mtpr(0,PR_TBIA);
				splx(s);
				goto uret;
			}
		}
		/* Fall into... */
	case T_ACCFLT:
	case T_ACCFLT|T_USER:
        {u_int tmpo=frame->code&0x7fffff00;
         u_int tmpp=frame->pc&0x7fffff00;
	 extern u_int sigsida;

        if((tmpo==0x7fffe000)&&(tmpp==0x7fffe000)){
                u_int *hej=(u_int *)(mfpr(PR_P1BR)+0x7fffc0);
/*		printf("Faultar sigsida: pid %d\n", p->p_pid); */
                *hej=0xf8000000|(sigsida>>PG_SHIFT);
                mtpr(0x7fffe000,PR_TBIS);
                return;
        }}
if(faultdebug)printf("trap accflt type %x, code %x, pc %x, psl %x\n",
                        frame->trap, frame->code, frame->pc, frame->psl);

		if(!p) panic("trap: access fault without process");
		pm=&p->p_vmspace->vm_pmap;
		if(frame->trap&T_PTEFETCH){
			u_int faultaddr,testaddr=(u_int)frame->code&0x3fffffff;
			int P0=0,P1=0,SYS=0;

			if(frame->code==testaddr) P0++;
			else if(frame->code>0x7fffffff) SYS++;
			else P1++;

			if(P0){
				faultaddr=(u_int)pm->pm_pcb->P0BR+
					((testaddr>>PG_SHIFT)<<2);
			} else if(P1){
				faultaddr=(u_int)pm->pm_pcb->P1BR+
					((testaddr>>PG_SHIFT)<<2);
			} else panic("pageflt: PTE fault in SPT\n");
	
			faultaddr&=~PAGE_MASK;
			rv = vm_fault(pte_map, faultaddr, 
				VM_PROT_WRITE|VM_PROT_READ, FALSE);
			if (rv != KERN_SUCCESS) {
	
				sig=SIGSEGV;
			} else trapsig=0;
/*			return; /* We don't know if it was a trap only for PTE*/
			break;
		}
		addr=(frame->code& ~PAGE_MASK);
		if((frame->pc>(unsigned)0x80000000)&&
			(frame->code>(unsigned)0x80000000)){
			map=kernel_map;
		} else {
			map= &p->p_vmspace->vm_map;
		}
		if(frame->trap&T_WRITE) ftype=VM_PROT_WRITE|VM_PROT_READ;
		else ftype = VM_PROT_READ;

		rv = vm_fault(map, addr, ftype, FALSE);
		if (rv != KERN_SUCCESS) {
			if(frame->pc>(u_int)0x80000000){
				if(p->p_addr->u_pcb.iftrap){
					frame->pc=p->p_addr->u_pcb.iftrap;
					return;
				}
				printf("Segv in kernel mode: rv %d\n",rv);
				goto faulter;
			}
			sig=SIGSEGV;
		} else trapsig=0;
		break;

	case T_PTELEN:
	case T_PTELEN|T_USER:	/* Page table length exceeded */
		pm=&p->p_vmspace->vm_pmap;
if(faultdebug)printf("trap ptelen type %x, code %x, pc %x, psl %x\n",
                        frame->trap, frame->code, frame->pc, frame->psl);
		if(frame->code<0x40000000){ /* P0 */
			int i=p->p_vmspace->vm_tsize+p->p_vmspace->vm_dsize;
			if(i>(frame->code>>PAGE_SHIFT)){
				pmap_expandp0(pm,i<<1);
				trapsig=0;
			} else {
				sig=SIGSEGV;
			}
		} else if(frame->code>0x7fffffff){ /* System, segv */
			sig=SIGSEGV;
		} else { /* P1 */
			int i=(u_int)(p->p_vmspace->vm_maxsaddr);
			if(frame->code<i){
				sig=SIGSEGV;
			} else {
				pmap_expandp1(pm);
				trapsig=0;
			}
		}
		break;

	case T_PRIVINFLT|T_USER:
	case T_RESADFLT|T_USER:
	case T_RESOPFLT|T_USER:
		sig=SIGILL;
		break;

	case T_ARITHFLT|T_USER:
		sig=SIGFPE;
		break;

	case T_ASTFLT|T_USER:
		mtpr(AST_NO,PR_ASTLVL);
		trapsig=0;
		break;
	}
	if(trapsig) trapsignal(curproc, sig, frame->code);
uret:
	userret(curproc, frame->pc, frame->psl);
};

showstate(p)
	struct proc *p;
{
if(p){
	printf("\npid %d, command %s\n",p->p_pid, p->p_comm);
	printf("text size %x, data size %x, stack size %x\n",
		p->p_vmspace->vm_tsize, p->p_vmspace->vm_dsize,p->p_vmspace->
		vm_ssize);
	printf("virt text %x, virt data %x, max stack %x\n",
		p->p_vmspace->vm_taddr,p->p_vmspace->vm_daddr,
		p->p_vmspace->vm_maxsaddr);
	printf("user pte p0br %x, user stack addr %x\n",
		p->p_vmspace->vm_pmap.pm_pcb->P0BR, mfpr(PR_USP));
} else {
	printf("No process\n");
}
	printf("kernel stack: %x, interrupt stack %x\n",
		mfpr(PR_KSP),mfpr(PR_ISP));
	printf("P0BR %x, P0LR %x, P1BR %x, P1LR %x\n",
		mfpr(PR_P0BR),mfpr(PR_P0LR),mfpr(PR_P1BR),mfpr(PR_P1LR));
}

void
setregs(p, pack, stack, retval)
        struct proc *p;
        struct exec_package *pack;
	u_long stack;
	register_t *retval;
{
	struct trapframe *exptr;

	exptr = p->p_addr->u_pcb.framep;
	exptr->pc = pack->ep_entry + 2;
	mtpr(stack, PR_USP);
}

syscall(frame)
	struct	trapframe *frame;
{
	struct sysent *callp;
	int nsys;
	int err,rval[2],args[8],sig;
	struct trapframe *exptr;
	struct proc *p=curproc;

if(startsysc)printf("trap syscall %s pc %x, psl %x, ap %x, pid %d\n",
               syscallnames[frame->code], frame->pc, frame->psl,frame->ap,
		curproc->p_pid);

	p->p_addr->u_pcb.framep=frame;
	callp = p->p_emul->e_sysent;
	nsys = p->p_emul->e_nsysent;

	if(frame->code==SYS___syscall){
		int g=*(int *)(frame->ap);

		frame->code=*(int *)(frame->ap+4);
		frame->ap+=8;
		*(int *)(frame->ap)=g-2;
if(startsysc){
		printf("SYS___syscall: ap %x\n",frame->ap);
		asm("halt");
		}
	}

	if(frame->code<0||frame->code>=nsys)
		callp += p->p_emul->e_nosys;
	else
		callp += frame->code;

	rval[0]=0;
	rval[1]=frame->r1;
	if(callp->sy_narg)
		copyin((char*)frame->ap+4, args, callp->sy_argsize);

	err=(*callp->sy_call)(curproc,args,rval);
	exptr=curproc->p_addr->u_pcb.framep;

if(startsysc)
	printf("retur %s pc %x, psl %x, ap %x, pid %d, v{rde %d r0 %d, r1 %d\n",
               syscallnames[exptr->code], exptr->pc, exptr->psl,exptr->ap,
                curproc->p_pid,err,rval[0],rval[1]);

	switch(err){
	case 0:
		exptr->r1=rval[1];
		exptr->r0=rval[0];
		exptr->psl &= ~PSL_C;
		break;
	case EJUSTRETURN:
		return;
	case ERESTART:
		exptr->pc=exptr->pc-2;
		break;
	default:
		exptr->r0=err;
		exptr->psl |= PSL_C;
		break;
	}
	userret(curproc, exptr->pc, exptr->psl);
}

stray(scb, vec){
	printf("stray interrupt scb %d, vec 0x%x\n", scb, vec);
}

struct inta {
	char pushr[2];	/* pushr $3f */
	char pushl[2];	/* pushl $? */
	char nop;	/* nop, for foolish gcc */
	char calls[3];	/* $1,? */
	u_int hoppaddr; /* jump for calls */
	char popr[2];	/* popr $0x3f */
	char rei;	/* rei */
} intasm = {0xbb, 0x3f, 0xdd, 0, 1, 0xfb, 1, 0xef, 0, 0xba, 0x3f, 2};

u_int
settrap(plats, nyrut,arg)
	u_int plats;  /* Pointer to place to copy interrupt routine */
	u_int nyrut;  /* Pointer to new routine to jump to */
	u_int arg;    /* arg number to pass to routine. */
{
	struct inta *introut;

	introut=(void *)((plats&0xfffffffc)+4);
	bcopy(&intasm, introut, sizeof(struct inta));
	introut->pushl[1]=arg;
	introut->hoppaddr=nyrut-(u_int)&introut->popr[0];
	return (u_int)introut;
}
