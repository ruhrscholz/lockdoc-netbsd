/*
 * Copyright (c) 1994 Christian E. Hopps
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
 *      This product includes software developed by Christian E. Hopps.
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
 *
 *	$Id: cc.c,v 1.4 1994/03/25 16:30:03 chopps Exp $
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/queue.h>

#include <amiga/amiga/custom.h>
#include <amiga/amiga/cc.h>

#if defined (__GNUC__)
#define INLINE inline
#else
#define INLINE
#endif

/* init all the "custom chips" */
void
custom_chips_init ()
{
	cc_init_chipmem ();
	cc_init_vbl ();
	cc_init_audio ();
	cc_init_blitter ();
	cc_init_copper ();
}

/*
 * Vertical blank iterrupt sever chains.
 */
LIST_HEAD(vbllist, vbl_node) vbl_list;

void 
turn_vbl_function_off (n)
	struct vbl_node *n;
{
	if (!(n->flags & VBLNF_OFF)) {
		n->flags |= VBLNF_TURNOFF;
		while (!(n->flags & VBLNF_OFF)) 
			;
	}
}

/* allow function to be called on next vbl interrupt. */
void
turn_vbl_function_on (n)
	struct vbl_node *n;
{
	n->flags &= (short) ~(VBLNF_OFF);
}

void                    
add_vbl_function (add, priority, data)
	struct vbl_node *add;
	short priority;
	void *data;
{
	int s;
	struct vbl_node *n, *prev;
	
	s = spl3();
	prev = NULL;
	for (n = vbl_list.lh_first; n != NULL; n = n->link.le_next) {
		if (add->priority > n->priority) {
			/* insert add_node before. */
			if (prev == NULL) {
				LIST_INSERT_HEAD(&vbl_list, add, link);
			} else {
				LIST_INSERT_AFTER(prev, add, link);
			}
			add = NULL;
			break;
		}
		prev = n;
	}
	if (add) {
		if (prev == NULL) {
			LIST_INSERT_HEAD(&vbl_list, add, link);
		} else {
			LIST_INSERT_AFTER(prev, add, link);
		}
	}
	splx(s);
}

void
remove_vbl_function (n)
	struct vbl_node *n;
{
	int s;

	s = spl3();
	LIST_REMOVE(n, link);
	splx(s);
}

/* Level 3 hardware interrupt */
void
vbl_handler ()
{
	struct vbl_node *n;

	/* handle all vbl functions */
	for (n = vbl_list.lh_first; n != NULL; n = n->link.le_next) {
		if (n->flags & VBLNF_TURNOFF) {
			n->flags |= VBLNF_OFF;
			n->flags &= ~(VBLNF_TURNOFF);
		} else {
			if (n != NULL)
				n->function(n->data);
		}
	}
	custom.intreq = INTF_VERTB;
}

void
cc_init_vbl ()
{
	LIST_INIT(&vbl_list);
	/* init vertical blank interrupts */
	custom.intena = INTF_SETCLR | INTF_VERTB; 
}


/*
 * Blitter stuff.
 */

void
cc_init_blitter ()
{
}

/* test twice to cover blitter bugs if BLTDONE (BUSY) is set it is not done. */
int
is_blitter_busy ()
{
	volatile u_short bb = (custom.dmaconr & DMAF_BLTDONE);
	if ((custom.dmaconr & DMAF_BLTDONE) || bb) 
		return (1);
	return (0);
}

void
wait_blit ()
{
	/* V40 state this covers all blitter bugs. */
	while (is_blitter_busy ()) 
		;
}

void
blitter_handler ()
{
	custom.intreq = INTF_BLIT;
}


void
do_blit (size)
	u_short size;
{
	custom.bltsize = size;
}

void
set_blitter_control (con0, con1)
	u_short con0, con1;
{
	custom.bltcon0 = con0;
	custom.bltcon1 = con1;
}

void
set_blitter_mods (a, b, c, d)
	u_short a, b, c, d;
{
	custom.bltamod = a;
	custom.bltbmod = b;
	custom.bltcmod = c;
	custom.bltdmod = d;
}

void
set_blitter_masks (fm, lm)
	u_short fm, lm;
{
	custom.bltafwm = fm;
	custom.bltalwm = lm;
}

void
set_blitter_data (da, db, dc)
	u_short da, db, dc;
{
	custom.bltadat = da;
	custom.bltbdat = db;
	custom.bltcdat = dc;
}

void
set_blitter_pointers (a, b, c, d)
	void *a, *b, *c, *d;
{
	custom.bltapt = a;
	custom.bltbpt = b;
	custom.bltcpt = c;
	custom.bltdpt = d;
}

/*
 * Copper Stuff.
 */


/* Wait till end of frame. We should probably better use the
 * sleep/wakeup system newly introduced in the vbl manager (ch?) */
void
wait_tof ()
{
	/* wait until bottom of frame. */
	while (!(custom.vposr & 0x0007))
		;
	
	/* wait until until top of frame. */
	while (custom.vposr & 0x0007) 
		;
	
	
	if (!custom.vposr & 0x8000) 
		while (!(custom.vposr & 0x8000)) /* we are on short frame. */	
			;	/* wait for long frame bit set. */
}

cop_t *
find_copper_inst (l, inst)
	cop_t *l;
	u_short inst;
{
	cop_t *r = NULL;
	while ((l->cp.data & 0xff01ff01) != 0xff01ff00) {
		if (l->cp.inst.opcode == inst) {
			r = l;
			break;
		}
		l++;
	}
	return (r);
}

void
install_copper_list (l)
	cop_t *l;
{
	wait_tof();
	wait_tof();
	custom.cop1lc = l;
}


void
cc_init_copper ()
{
}

/* level 3 interrupt */
void
copper_handler ()
{
	custom.intreq = INTF_COPER;  
}

/*
 * Audio stuff.
 */

struct audio_channel {
	u_short  play_count;
};

/* - channel[4] */
/* the data for each audio channel and what to do with it. */
static struct audio_channel channel[4];

/* audio vbl node for vbl function  */
struct vbl_node audio_vbl_node;    

void
cc_init_audio ()
{
	int i;

	/* disable all audio interupts */
	custom.intena = INTF_AUD0|INTF_AUD1|INTF_AUD2|INTF_AUD3;

	/* initialize audio channels to off. */
	for (i=0; i < 4; i++) {
		channel[i].play_count = 0;
	};
}


/* Audio Interrupt Handler */
void
audio_handler ()
{
	u_short audio_dma = custom.dmaconr;
	u_short disable_dma = 0;
	int i;

	/* only check channels who have DMA enabled. */
	audio_dma &= (DMAF_AUD0|DMAF_AUD1|DMAF_AUD2|DMAF_AUD3);

	/* disable all audio interupts with DMA set */
	custom.intena = (audio_dma << 7);

	/* if no audio dma enabled then exit quick. */
	if (!audio_dma) {
		/* clear all interrupts. */
		custom.intreq = INTF_AUD0|INTF_AUD1|INTF_AUD2|INTF_AUD3; 
		goto end_int;		/* cleanup. */
	}

	for (i = 0; i < 4; i++) {
		u_short flag = (1 << i);
		u_short ir = custom.intreqr;
		/* check if this channel's interrupt is set */
		if (ir & (flag << 7)) {
			if (channel[i].play_count) {
				channel[i].play_count--;
			} else {
				/* disable DMA to this channel. */
				custom.dmacon = flag;
		
				/* disable interrupts to this channel. */
				custom.intena = (flag << 7);
			}
			 /* clear this channels interrupt. */
			custom.intreq = (flag << 7);
		}
	}

	end_int:
	/* enable audio interupts with dma still set. */
	audio_dma = custom.dmaconr;
	audio_dma &= (DMAF_AUD0|DMAF_AUD1|DMAF_AUD2|DMAF_AUD3);
	custom.intena = INTF_SETCLR| (audio_dma << 7);
}

void
play_sample (len, data, period, volume, channels, count)
	u_short len, *data, period, volume, channels;
	u_long count;
{
	u_short dmabits = channels & 0xf;
	u_short ch;

	custom.dmacon = dmabits;	/* turn off the correct channels */

	/* load the channels */
	for (ch = 0; ch < 4; ch++) {
		if (dmabits & (ch << ch)) {
			custom.aud[ch].len = len;
			custom.aud[ch].lc = data;
			custom.aud[ch].per = period;
			custom.aud[ch].vol = volume;
			channel[ch].play_count = count;
		}
	}
	/* turn on interrupts for channels */
	custom.intena = INTF_SETCLR | (dmabits << 7);
	 /* turn on the correct channels */
	custom.dmacon = DMAF_SETCLR | dmabits;
}

/*
 * Chipmem allocator.
 */

static CIRCLEQ_HEAD(chiplist, mem_node) chip_list;
static CIRCLEQ_HEAD(freelist, mem_node) free_list;
static u_long   chip_total;		/* total free. */
static u_long   chip_size;		/* size of it all. */

void
cc_init_chipmem ()
{
	int s = splhigh ();
	struct mem_node *mem;
	extern u_char *chipmem_end, *chipmem_start;

	chip_size = chipmem_end - (chipmem_start+NBPG);

	chip_total = chip_size - sizeof(*mem);
    
	mem = (struct mem_node *)chipmem_steal(chip_size);
	mem->size = chip_total;

	CIRCLEQ_INIT(&chip_list);
	CIRCLEQ_INIT(&free_list);
    
	CIRCLEQ_INSERT_HEAD(&chip_list, mem, link);
	CIRCLEQ_INSERT_HEAD(&free_list, mem, free_link);
	splx (s);
}

void *
alloc_chipmem (size)
	u_long size;
{
	void *mem;
	int s;
	struct mem_node *mn, *new;

	if (size == 0)
		return NULL;

	s = splhigh();

	if (size & ~(CM_BLOCKMASK)) 
		size = (size & CM_BLOCKMASK) + CM_BLOCKSIZE;

	/* walk list of available nodes. */
	mn = free_list.cqh_first;
	while (size > mn->size && mn != (void *)&free_list) {
		mn = mn->free_link.cqe_next;
	}
	if (mn == (void *)&free_list)
		return(NULL);

	if ((mn->size - size) <= sizeof (*mn)) {
		/*
		 * our allocation would not leave room 
		 * for a new node in between.
		 */
		CIRCLEQ_REMOVE(&free_list, mn, free_link);
		mn->free_link.cqe_next = NULL;
		size = mn->size; /* increase size. (or same) */
		chip_total -= mn->size;
		splx (s);
		return((void *)&mn[1]);
	}

	/* split the node's memory. */
	new = mn;
	new->size -= size + sizeof(struct mem_node);
	mn = (struct mem_node *)(MNODES_MEM(new) + new->size);
	mn->size = size;

	/* add split node to node list */
	CIRCLEQ_INSERT_AFTER(&chip_list, new, mn, link);
	/* and mark as not on free list */
	mn->free_link.cqe_next = NULL;

	chip_total -= size + sizeof(struct mem_node);
	splx (s);
	return((void *)&mn[1]);
}

void
free_chipmem(mem)
	void *mem;
{
	int s;
	struct mem_node *mn, *next, *prev;

	if (mem == NULL)
		return;

	s = splhigh();
	mn = (struct mem_node *)mem - 1;
	next = mn->link.cqe_next;
	prev = mn->link.cqe_prev;

	/* check ahead of us. */
	if (next->link.cqe_next != (void *)&chip_list && 
	    next->free_link.cqe_next) {
		/* if next is: a valid node and a free node. ==> merge */
		CIRCLEQ_INSERT_BEFORE(&free_list, next, mn, free_link);
		CIRCLEQ_REMOVE(&chip_list, next, link);
		CIRCLEQ_REMOVE(&chip_list, next, free_link);
		chip_total += mn->size + sizeof(struct mem_node);
		mn->size += next->size + sizeof(struct mem_node);
	}
	if (prev->link.cqe_prev != (void *)&chip_list &&
	    prev->free_link.cqe_prev) {
		/* if prev is: a valid node and a free node. ==> merge */
		if (mn->free_link.cqe_next == NULL)
			chip_total += mn->size + sizeof(struct mem_node);
		else {
			/* already on free list */
			CIRCLEQ_REMOVE(&free_list, mn, free_link);
			chip_total += sizeof(struct mem_node);
		}
		CIRCLEQ_REMOVE(&chip_list, mn, link);
		prev->size += mn->size + sizeof(struct mem_node);
	} else if (mn->free_link.cqe_next == NULL) {
		/*
		 * we still are not on free list and we need to be.
		 * <-- | -->
		 */
		while (next->link.cqe_next != (void *)&chip_list && 
		    prev->link.cqe_prev != (void *)&chip_list) {
			if (next->free_link.cqe_next) {
				CIRCLEQ_INSERT_BEFORE(&free_list, next, mn,
				    free_link);
				break;
			}
			if (prev->free_link.cqe_next) {
				CIRCLEQ_INSERT_AFTER(&free_list, prev, mn,
				    free_link);
				break;
			}
			prev = prev->link.cqe_prev;
			next = next->link.cqe_next;
		}
		if (mn->free_link.cqe_next == NULL) {
			if (next->link.cqe_next == (void *)&chip_list) {
				/*
				 * we are not on list so we can add
				 * ourselves to the tail. (we walked to it.)
				 */
				CIRCLEQ_INSERT_TAIL(&free_list,mn,free_link);
			} else {
				CIRCLEQ_INSERT_HEAD(&free_list,mn,free_link);
			}
		}
		chip_total += mn->size;	/* add our helpings to the pool. */
	}
	splx (s);
}

u_long
sizeof_chipmem (mem)
	void *mem;
{
	if (mem) {
		struct mem_node *mn = mem;
		mn--;
		return (mn->size);
	}
	return (0);
}

u_long
avail_chipmem (largest)
	int largest;
{
	u_long val = 0;
	
	if (largest) {
		int s = splhigh ();
		struct mem_node *mn;

		for (mn = free_list.cqh_first; mn != (void *)&free_list;
		     mn = mn->free_link.cqe_next) {
			if (mn->size > val) 
				val = mn->size;
		}
		splx (s);
	} else 
		val = chip_total;
	return (val);
}	      

