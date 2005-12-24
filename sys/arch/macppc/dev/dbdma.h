/*	$NetBSD: dbdma.h,v 1.4 2005/12/24 23:24:01 perry Exp $	*/

/*
 * Copyright 1991-1998 by Open Software Foundation, Inc. 
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 * 
 */

#ifndef _POWERMAC_DBDMA_H_
#define _POWERMAC_DBDMA_H_

#define	DBDMA_CMD_OUT_MORE	0
#define	DBDMA_CMD_OUT_LAST	1
#define	DBDMA_CMD_IN_MORE	2
#define	DBDMA_CMD_IN_LAST	3
#define	DBDMA_CMD_STORE_QUAD	4
#define	DBDMA_CMD_LOAD_QUAD	5
#define	DBDMA_CMD_NOP		6
#define	DBDMA_CMD_STOP		7

/* Keys */

#define	DBDMA_KEY_STREAM0	0
#define	DBDMA_KEY_STREAM1	1
#define	DBDMA_KEY_STREAM2	2
#define	DBDMA_KEY_STREAM3	3

/* value 4 is reserved */
#define	DBDMA_KEY_REGS		5
#define	DBDMA_KEY_SYSTEM	6
#define	DBDMA_KEY_DEVICE	7

#define	DBDMA_INT_NEVER		0
#define	DBDMA_INT_IF_TRUE	1
#define	DBDMA_INT_IF_FALSE	2
#define	DBDMA_INT_ALWAYS	3

#define	DBDMA_BRANCH_NEVER	0
#define	DBDMA_BRANCH_IF_TRUE	1
#define	DBDMA_BRANCH_IF_FALSE	2
#define	DBDMA_BRANCH_ALWAYS	3

#define	DBDMA_WAIT_NEVER	0
#define	DBDMA_WAIT_IF_TRUE	1
#define DBDMA_WAIT_IF_FALSE	2
#define	DBDMA_WAIT_ALWAYS	3


/* Channels */

#define	DBDMA_SCSI0		0x0
#define	DBDMA_CURIO_SCSI	DBDMA_SCSI0
#define	DBDMA_FLOPPY		0x1
#define	DBDMA_ETHERNET_TX	0x2
#define	DBDMA_ETHERNET_RV	0x3
#define	DBDMA_SCC_XMIT_A	0x4
#define	DBDMA_SCC_RECV_A	0x5
#define	DBDMA_SCC_XMIT_B	0x6
#define	DBDMA_SCC_RECV_B	0x7
#define	DBDMA_AUDIO_OUT		0x8
#define	DBDMA_AUDIO_IN		0x9
#define	DBDMA_SCSI1		0xA

/* Control register values (in little endian) */

#define	DBDMA_STATUS_MASK	0x000000ff	/* Status Mask */
#define	DBDMA_CNTRL_BRANCH	0x00000100
				/* 0x200 reserved */
#define	DBDMA_CNTRL_ACTIVE	0x00000400
#define	DBDMA_CNTRL_DEAD	0x00000800
#define	DBDMA_CNTRL_WAKE	0x00001000
#define	DBDMA_CNTRL_FLUSH	0x00002000
#define	DBDMA_CNTRL_PAUSE	0x00004000
#define	DBDMA_CNTRL_RUN		0x00008000

#define	DBDMA_SET_CNTRL(x)	( ((x) | (x) << 16) )
#define	DBDMA_CLEAR_CNTRL(x)	( (x) << 16)


#define	DBDMA_REGMAP(channel) \
		(dbdma_regmap_t *)((v_u_char *) POWERMAC_IO(PCI_DMA_BASE_PHYS) \
				+ (channel << 8))

/* This struct is layout in little endian format */

struct dbdma_command {
	u_int16_t	d_count;
	u_int16_t	d_command;
	u_int32_t	d_address;
	u_int32_t	d_cmddep;
	u_int16_t	d_resid;
	u_int16_t	d_status;
};

typedef struct dbdma_command dbdma_command_t;

#define	DBDMA_BUILD_CMD(d, cmd, key, interrupt, wait, branch) {		\
		dbdma_st16(&(d)->d_command,				\
				((cmd) << 12) | ((key) << 8) |		\
				((interrupt) << 4) |			\
				((branch) << 2) | (wait));		\
	}

#define	DBDMA_BUILD(d, cmd, key, count, address, interrupt, wait, branch) { \
		dbdma_st16(&(d)->d_count, count);			\
		dbdma_st32(&(d)->d_address, address);			\
		(d)->d_resid = 0;					\
		(d)->d_status = 0;					\
		(d)->d_cmddep = 0;					\
		dbdma_st16(&(d)->d_command,				\
				((cmd) << 12) | ((key) << 8) |		\
				((interrupt) << 4) |			\
				((branch) << 2) | (wait));		\
	}

static inline void dbdma_st32(volatile u_int32_t *, u_int32_t);
static inline void dbdma_st16(volatile u_int16_t *, u_int16_t);
static inline u_int32_t dbdma_ld32(volatile u_int32_t *);
static inline u_int16_t dbdma_ld16(volatile u_int16_t *);

static inline void
dbdma_st32(a, x)
	volatile u_int32_t *a;
	u_int32_t x;
{
	__asm volatile
		("stwbrx %0,0,%1" : : "r" (x), "r" (a) : "memory");
}

static inline void
dbdma_st16(a, x)
	volatile u_int16_t *a;
	u_int16_t x;
{
	__asm volatile
		("sthbrx %0,0,%1" : : "r" (x), "r" (a) : "memory");
}

static inline u_int32_t
dbdma_ld32(a)
	volatile u_int32_t *a;
{
	u_int32_t swap;

	__asm volatile
		("lwbrx %0,0,%1" :  "=r" (swap) : "r" (a));

	return	swap;
}

static inline u_int16_t
dbdma_ld16(a)
	volatile u_int16_t *a;
{
	u_int16_t swap;

	__asm volatile
		("lhbrx %0,0,%1" :  "=r" (swap) : "r" (a));

	return	swap;
}

#define	DBDMA_LD4_ENDIAN(a) 	dbdma_ld32(a)
#define	DBDMA_ST4_ENDIAN(a, x) 	dbdma_st32(a, x)

/*
 * DBDMA Channel layout
 *
 * NOTE - This structure is in little-endian format. 
 */

struct dbdma_regmap {
	u_int32_t	d_control;	/* Control Register */
	u_int32_t	d_status;	/* DBDMA Status Register */
	u_int32_t	d_cmdptrhi;	/* MSB of command pointer (not used yet) */
	u_int32_t	d_cmdptrlo;	/* LSB of command pointer */
	u_int32_t	d_intselect;	/* Interrupt Select */
	u_int32_t	d_branch;	/* Branch selection */
	u_int32_t	d_wait;		/* Wait selection */
	u_int32_t	d_transmode;	/* Transfer modes */
	u_int32_t	d_dataptrhi;	/* MSB of Data Pointer */
	u_int32_t	d_dataptrlo;	/* LSB of Data Pointer */
	u_int32_t	d_reserved;	/* Reserved for the moment */
	u_int32_t	d_branchptrhi;	/* MSB of Branch Pointer */
	u_int32_t	d_branchptrlo;	/* LSB of Branch Pointer */
	/* The remaining fields are undefinied and unimplemented */
};

typedef volatile struct dbdma_regmap dbdma_regmap_t;

/* DBDMA routines */

void	dbdma_start(dbdma_regmap_t *channel, dbdma_command_t *commands);
void	dbdma_stop(dbdma_regmap_t *channel);	
void	dbdma_flush(dbdma_regmap_t *channel);
void	dbdma_reset(dbdma_regmap_t *channel);
void	dbdma_continue(dbdma_regmap_t *channel);
void	dbdma_pause(dbdma_regmap_t *channel);

dbdma_command_t	*dbdma_alloc(int);	/* Allocate command structures */

#endif /* !defined(_POWERMAC_DBDMA_H_) */
