/*	$NetBSD: atari_init.c,v 1.2 1995/04/28 11:41:53 leo Exp $	*/

/*
 * Copyright (c) 1995 Leo Weppelman
 * Copyright (c) 1994 Michael L. Hitch
 * Copyright (c) 1993 Markus Wild
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
 *      This product includes software developed by Markus Wild.
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

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <vm/vm.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/msgbuf.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/dkbad.h>
#include <sys/reboot.h>
#include <sys/exec.h>
#include <vm/pmap.h>
#include <machine/vmparam.h>
#include <machine/pte.h>
#include <machine/cpu.h>
#include <machine/iomap.h>
#include <machine/mfp.h>
#include <machine/scu.h>
#include <atari/atari/misc.h>

extern u_int 	lowram;
extern u_int	Sysptmap, Sysptsize, Sysseg, Umap, proc0paddr;
u_int		*Sysmap;
int		machineid, mmutype, cpu040, astpending;
char		*vmmap;
pv_entry_t	pv_table;

/*
 * Need-to-know for kernel reload code.
 */
static u_long	boot_ttphystart, boot_ttphysize, boot_stphysize;

extern char	*esym;

/*
 * The following vars become valid just before pmap_bootstrap is called.
 * They give info about free memory with the kernel and the tables allocated
 * in 'start_c' excluded.
 */
extern vm_offset_t st_ramstart;	/* First available ST RAM address	*/
extern vm_offset_t st_ramend;	/* First ST RAM address	not available	*/
extern vm_offset_t tt_ramstart;	/* First available TT RAM address	*/
extern vm_offset_t tt_ramend;	/* First TT RAM address	not available	*/


/*
 * This is the virtual address of physical page 0. Used by 'do_boot()'.
 */
vm_offset_t	page_zero;

/*
 * Crude support for allocation in ST-ram. Currently only used to allocate
 * video ram.
 * The physical address is also returned because the video init needs it to
 * setup the controller at the time the vm-system is not yet operational so
 * 'kvtop()' cannot be used.
 */
#define	ST_POOL_SIZE	(40*NBPG)	/* XXX: enough? */

u_long	st_pool_size = ST_POOL_SIZE;	/* Patchable	*/
u_long	st_pool_virt, st_pool_phys;

/*
 * this is the C-level entry function, it's called from locore.s.
 * Preconditions:
 *	Interrupts are disabled
 *	PA == VA, we don't have to relocate addresses before enabling
 *		the MMU
 * 	Exec is no longer available (because we're loaded all over 
 *		low memory, no ExecBase is available anymore)
 *
 * It's purpose is:
 *	Do the things that are done in locore.s in the hp300 version, 
 *		this includes allocation of kernel maps and enabling the MMU.
 * 
 * Some of the code in here is `stolen' from Amiga MACH, and was 
 * written by Bryan Ford and Niklas Hallqvist.
 * 
 * Very crude 68040 support by Michael L. Hitch.
 */

void
start_c(id, ttphystart, ttphysize, stphysize, esym_addr)
int	id;			/* Machine id				*/
u_int	ttphystart, ttphysize;	/* Start address and size of TT-ram	*/
u_int	stphysize;		/* Size of ST-ram	 		*/
char	*esym_addr;		/* Address of kernel '_esym' symbol	*/
{
	extern char	end[];
	extern void	etext();
	extern u_long	protorp[2];
	u_int		pstart;		/* Next available physical address*/
	u_int		vstart;		/* Next available virtual address */
	u_int		avail;
	u_int		pt, ptsize;
	u_int		tc, i;
	u_int		*sg, *pg, *pg2;
	u_int		sg_proto, pg_proto;
	u_int		umap;
	u_int		p0_pt, p0_u_area, end_loaded;
	u_int		ptextra;
	u_long		kbase;

	boot_ttphystart = ttphystart;
	boot_ttphysize  = ttphysize;
	boot_stphysize  = stphysize;

	/*
	 * The following is a hack. We do not know how much ST memory we
	 * really need until after configuration has finished. At this
	 * time I have no idea how to grab ST memory at that time.
	 * The round_page() call is ment to correct errors made by
	 * binpatching!
	 */
	st_pool_size   = atari_round_page(st_pool_size);
	st_pool_phys   = stphysize - st_pool_size;
	stphysize      = st_pool_phys;

	machineid      = id;
	esym           = esym_addr;

	/* 
	 * the kernel ends at end() or esym.
	 */
	if(esym == NULL)
		end_loaded = (u_int)end;
	else end_loaded = (u_int)esym;


	/*
	 * If we have enough fast-memory to put the kernel in, do it!
	 */
	if(ttphysize >= end_loaded)
		kbase = ttphystart;
	else kbase = 0;

	/*
	 * update these as soon as possible!
	 */
	PAGE_SIZE  = NBPG;
	PAGE_MASK  = NBPG-1;
	PAGE_SHIFT = PG_SHIFT;

	/*
	 * We run the kernel from ST memory at the moment.
	 * The kernel segment table is put just behind the loaded image.
	 * pstart: start of usable ST memory
	 * avail : size of ST memory available.
	 */
	pstart = (u_int)end_loaded;
	pstart = atari_round_page(pstart);
	avail  = stphysize - pstart;
  
	/*
	 * allocate the kernel segment table
	 */
	Sysseg  = pstart;
	pstart += NBPG;
	avail  -= NBPG;
  
	/*
	 * We only allocate 1 extra pagetable. it currently only contains
	 * entries for Sysmap.
	 */
	ptextra = 0;

	/*
	 * allocate initial page table pages
	 */
	pt      = pstart;
	ptsize  = (Sysptsize + howmany(ptextra, NPTEPG)) << PGSHIFT;
	pstart += ptsize;
	avail  -= ptsize;
  
	/*
	 * allocate kernel page table map
	 */
	Sysptmap = pstart;
	pstart  += NBPG;
	avail   -= NBPG;

	/*
	 * Set Sysmap; mapped after page table pages. Because I too (LWP)
	 * didn't understand the reason for this, I borrowed the following
	 * (sligthly modified) comment from mac68k/locore.s:
	 * LAK:  There seems to be some confusion here about the next line,
	 * so I'll explain.  The kernel needs some way of dynamically modifying
	 * the page tables for its own virtual memory.  What it does is that it
	 * has a page table map.  This page table map is mapped right after the
	 * kernel itself (in our implementation; in HP's it was after the I/O
	 * space). Therefore, the first three (or so) entries in the segment
	 * table point to the first three pages of the page tables (which
	 * point to the kernel) and the next entry in the segment table points
	 * to the page table map (this is done later).  Therefore, the value
	 * of the pointer "Sysmap" will be something like 16M*3 = 48M.  When
	 * the kernel addresses this pointer (e.g., Sysmap[0]), it will get
	 * the first longword of the first page map (== pt[0]).  Since the
	 * page map mirrors the segment table, addressing any index of Sysmap
	 * will give you a PTE of the page maps which map the kernel.
	 */
	Sysmap = (u_int *)(ptsize << (SEGSHIFT - PGSHIFT));

	/*
	 * Initialize segment table and page table map.
	 */
	sg_proto = (pt + kbase) | SG_RW | SG_V;
	pg_proto = (pt + kbase) | PG_RW | PG_CI | PG_V;
	/*
	 * map so many segs
	 */
	sg = (u_int *)Sysseg;
	pg = (u_int *)Sysptmap; 
	while(sg_proto < (pstart + kbase)) {
		*sg++ = sg_proto;
		*pg++ = pg_proto;
		sg_proto += NBPG;
		pg_proto += NBPG;
	}
	/* 
	 * invalidate the remainder of the tables
	 * (except last entry)
	 */
	do {
		*sg++ = SG_NV;
		*pg++ = PG_NV;
	} while(sg < (u_int *)(Sysseg + ATARI_STSIZE - 4));

	/*
	 * the end of the last segment (0xFF000000) 
	 * of KVA space is used to map the u-area of
	 * the current process (u + kernel stack).
	 */
	sg_proto = (pstart + kbase) | SG_RW | SG_V; /* use next availabe PA */
	pg_proto = (pstart + kbase) | PG_RW | PG_CI | PG_V;
	umap     = pstart;	/* remember for later map entry */

	/*
	 * enter the page into the segment table 
	 * (and page table map)
	 */
	*sg++     = sg_proto;
	*pg++     = pg_proto;

	/*
	 * invalidate all pte's (will validate u-area afterwards)
	 */
	for(pg = (u_int *) pstart; pg < (u_int *) (pstart + NBPG); )
		*pg++ = PG_NV;

	/*
	 * account for the allocated page
	 */
	pstart   += NBPG;
	avail    -= NBPG;

	/*
	 * record KVA at which to access current u-area PTE(s)
	 */
	Umap = (u_int)Sysmap + ATARI_MAX_PTSIZE - UPAGES * 4;
  
	/*
	 * initialize kernel page table page(s).
	 * Assume load at VA 0.
	 * - Text pages are RO
	 */
	pg_proto = (0 + kbase) | PG_RO | PG_V;
	pg       = (u_int *) pt;
	for(i = 0; i < (u_int)etext; i += NBPG, pg_proto += NBPG)
		*pg++ = pg_proto;

	/* 
	 * data, bss and dynamic tables are read/write
	 */
	pg_proto = (pg_proto & PG_FRAME) | PG_RW | PG_V;

	/*
	 * go till end of data allocated so far
	 * plus proc0 PT/u-area (to be allocated)
	 */
	for(; i < pstart + (UPAGES + 1)*NBPG; i += NBPG, pg_proto += NBPG)
		*pg++ = pg_proto;

	/*
	 * invalidate remainder of kernel PT
	 */
	while(pg < (u_int *)(pt + ptsize))
		*pg++ = PG_NV;

	/*
	 * Go back and validate internal IO PTEs. They MUST be Cache inhibited!
	 */
	pg       = (u_int *) pt + (AD_IO / NBPG);
	pg_proto = AD_IO | PG_RW | PG_CI | PG_V;
	while(pg_proto < AD_EIO) {
		*pg++     = pg_proto;
		pg_proto += NBPG;
	}

	/*
	 * Setup page table for process 0.
	 * We set up page table access for the kernel via Usrptmap (usrpt)
	 * [no longer used?] and access to the u-area itself via Umap (u).
	 * First available page (pstart) is used for proc0 page table.
	 * Next UPAGES page(s) following are for u-area.
	 */
  
	p0_pt   = pstart;
	pstart += NBPG;
	avail  -= NBPG;

	p0_u_area = pstart;		/* base of u-area and end of PT */
  
	/*
	 * invalidate entire page table
	 */
	for(pg = (u_int *)p0_pt; pg < (u_int *) p0_u_area; )
		*pg++ = PG_NV;

	/*
	 * now go back and validate u-area PTE(s) in PT and in Umap
	 */
	pg  -= UPAGES;
	pg2  = (u_int *) (umap + 4*(NPTEPG - UPAGES));
	pg_proto = (p0_u_area + kbase) | PG_RW | PG_V;
	for(i = 0; i < UPAGES; i++, pg_proto += NBPG) {
		*pg++  = pg_proto;
		*pg2++ = pg_proto;
	}
	bzero((u_char *)p0_u_area, UPAGES * NBPG);

	/*
	 * save KVA of proc0 u-area
	 */
	proc0paddr = pstart;
	pstart    += UPAGES * NBPG;
	avail     -= UPAGES * NBPG;

	/*
	 * At this point, virtual and physical allocation starts to divert.
	 */
	vstart     = pstart;

	/*
	 * Map the allocated space in ST-ram now. In the contig-case, there
	 * is no need to make a distinction between virtual and physical
	 * adresses. But I make it anyway to be prepared.
	 * Physcal space is already reserved!
	 */
	st_pool_virt = vstart;
	pg           = (u_int *) pt + (vstart / NBPG);
	pg_proto     = st_pool_phys | PG_RW | PG_CI | PG_V;
	vstart      += st_pool_size;
	while(pg_proto < (st_pool_phys + st_pool_size)) {
		*pg++     = pg_proto;
		pg_proto += NBPG;
	}

	/*
	 * Map physical page zero (First ST-ram page)
	 */
	page_zero  = vstart;
	pg         = (u_int *) pt + (vstart / NBPG);
	*pg        = PG_RW | PG_CI | PG_V;
	vstart    += NBPG;

	lowram  = 0 >> PGSHIFT; /* XXX */

	/*
	 * Initialize memory sizes
	 */
	st_ramstart    = 0;
	st_ramend      = stphysize;
	tt_ramstart    = ttphystart;
	tt_ramend      = ttphystart + ttphysize;
	if(kbase) {
		/*
		 * First page of ST-ram is unusable.
		 */
		st_ramstart  = NBPG;
		tt_ramstart += pstart;
	}
	else {
		/*
		 * Because the first 8 addresses of ST-memory are mapped to
		 * ROM, we remap them. This makes the debugger stack-trace
		 * work.
		 */
		extern	u_long	first_8_bytes[];
			u_long	*sp, *dp;

		/*
		 * Copy page zero and set first 8 bytes.
		 */
		sp = (u_long *)0;
		dp = (u_long *)pstart;
		while(dp < (u_long *)(pstart+NBPG))
			*dp++ = *sp++;
		dp    = (u_long *)pstart;
		*dp++ = first_8_bytes[0];
		*dp   = first_8_bytes[1];

		/*
		 * Enter into the page-table
		 */
		pg  = (u_int *)pt;
		*pg = pstart | PG_RO | PG_V;


		/*
		 * Reserve space for page 0
		 */
		pstart += NBPG;

		st_ramstart += pstart;
	}

	physmem = ((st_ramend-st_ramstart)+(tt_ramend-tt_ramstart)) >> PGSHIFT;
  
	/*
	 * get the pmap module in sync with reality.
	 */
	pmap_bootstrap(vstart);


	/*
	 * Prepare to enable the MMU.
	 * Setup and load SRP nolimit, share global, 4 byte PTE's
	 */
	protorp[0] = 0x80000202;
	protorp[1] = Sysseg + kbase;	/* + segtable address */

	/*
	 * copy over the kernel (and all now initialized variables) 
	 * to fastram.  DONT use bcopy(), this beast is much larger 
	 * than 128k !
	 */
	if(kbase) {
		register u_long	*lp, *le, *fp;
		extern	 u_long	first_8_bytes[];

		lp = (u_long *)0;
		le = (u_long *)pstart;
		fp = (u_long *)kbase;
		while(lp < le)
			*fp++ = *lp++;

		/*
		 * Fill in reset stuff
		 */
		fp    = (u_long *)kbase;
		*fp++ = first_8_bytes[0];
		*fp   = first_8_bytes[1];
	}

	asm volatile ("pmove %0@,srp" : : "a" (&protorp[0]));
	/*
	 * setup and load TC register.
	 * enable_cpr, enable_srp, pagesize=8k,
	 * A = 8 bits, B = 11 bits
	 */
	tc = 0x82d08b00;
	asm volatile ("pmove %0@,tc" : : "a" (&tc));

	/* Is this to fool the optimizer?? */
	i = *(int *)proc0paddr;
	*(volatile int *)proc0paddr = i;

	/*
	 * Initialize both MFP chips to generate auto-vectored interrupts
	 * with EOI. The active-edge registers are set up. The interrupt
	 * enable registers are set to disable all interrupts.
	 */
	MFP->mf_iera  = MFP->mf_ierb = 0;
	MFP->mf_imra  = MFP->mf_imrb = 0;
	MFP->mf_aer   = 0;
	MFP->mf_vr    = 0x40;
	MFP2->mf_iera = MFP2->mf_ierb = 0;
	MFP2->mf_imra = MFP2->mf_imrb = 0;
	MFP2->mf_aer  = 0x80;
	MFP2->mf_vr   = 0x50;

	/*
	 * Initialize the SCU, to enable interrupts on the SCC (ipl5),
	 * MFP (ipl6) and softints (ipl1).
	 */
	SCU->sys_mask = SCU_MFP | SCU_SCC | SCU_SYS_SOFT;

	/*
	 * Initialize stmem allocator
	 */
	init_stmem();
}

#ifdef DEBUG
void
dump_segtable(stp)
	u_int *stp;
{
	u_int *s, *es;
	int shift, i;

	s = stp;
	{
		es = s + (ATARI_STSIZE >> 2);
		shift = SG_ISHIFT;
	}

	/* 
	 * XXX need changes for 68040 
	 */
	for (i = 0; s < es; s++, i++)
		if (*s & SG_V)
			printf("$%08lx: $%08lx\t", i << shift, *s & SG_FRAME);
	printf("\n");
}

void
dump_pagetable(ptp, i, n)
	u_int *ptp, i, n;
{
	u_int *p, *ep;

	p = ptp + i;
	ep = p + n;
	for (; p < ep; p++, i++)
		if (*p & PG_V)
			printf("$%08lx -> $%08lx\t", i, *p & PG_FRAME);
	printf("\n");
}

u_int
vmtophys(ste, vm)
	u_int *ste, vm;
{
	ste = (u_int *) (*(ste + (vm >> SEGSHIFT)) & SG_FRAME);
		ste += (vm & SG_PMASK) >> PGSHIFT;
	return((*ste & -NBPG) | (vm & (NBPG - 1)));
}

#endif
