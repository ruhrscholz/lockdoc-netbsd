/*	$NetBSD: disklabel_mbr.h,v 1.11 2003/07/07 12:00:23 dsl Exp $	*/

/*
 * Copyright (c) 1994, 1998 Christopher G. Demetriou
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
 *      This product includes software developed by Christopher G. Demetriou.
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

#ifndef _SYS_DISKLABEL_MBR_H_
#define	_SYS_DISKLABEL_MBR_H_

/*
 * From i386 disklabel.h rev 1.6, with cleanups and modifications to
 * make it easier to use as MI code.
 *
 * Note, all fields (including magic number) are little-endian!
 */

/* MBR ("Master Boot Record"; DOS) partition table -- located in boot block */
#define	MBR_BBSECTOR	0		/* MBR relative sector # */
#define	MBR_BOOTSELOFF	404		/* offset of mbr_bootsel */
#define	MBR_PARTOFF	446		/* offset of MBR partition table */
#define	MBR_MAGICOFF	510		/* offset of magic number */
#define	MBR_MAGIC	0xaa55		/* MBR magic number */
#define	NMBRPART	4

/*
 * Each entry in the boot select table is a nul-terminated string
 * of 8 bytes (not including the 0). A zero-length string
 * (ie if the first char is 0) indicates an unused entry.
 */
#define	PARTNAMESIZE	8		/* for bootselect menu */

#define	DEFAULT_BOOTDIR		"/usr/mdec"
#define	DEFAULT_BOOTCODE	"mbr"
#define	DEFAULT_BOOTSELCODE	"mbr_bootsel"
#define	DEFAULT_BOOTEXTCODE	"mbr_ext"

/* flags for mbr_bootsel */
#define	BFL_SELACTIVE	0x01	/* Bootselector active (or code present) */
#define	BFL_EXTINT13	0x02	/* Set by fdisk if LBA needed (deprecated) */
#define	BFL_READ_LBA	0x04	/* Force LBA reads - even for low numbers */
#define	BFL_EXTLBA	0x08	/* Extended ptn capable (LBA reads) */
#define	BFL_NEWMBR	0x80	/* New code: menu user 1..9 for ptns */

/* Scan values for the various keys we use, as returned by the BIOS */
#define	SCAN_ENTER	0x1c
#define	SCAN_F1		0x3b
#define	SCAN_1		0x2

#ifndef __ASSEMBLER__
typedef struct mbr_partition mbr_partition_t;
struct mbr_partition {
	u_int8_t	mbrp_flag;	/* bootstrap flags */
	u_int8_t	mbrp_shd;	/* starting head */
	u_int8_t	mbrp_ssect;	/* starting sector */
	u_int8_t	mbrp_scyl;	/* starting cylinder */
	u_int8_t	mbrp_typ;	/* partition type (see below) */
	u_int8_t	mbrp_ehd;	/* end head */
	u_int8_t	mbrp_esect;	/* end sector */
	u_int8_t	mbrp_ecyl;	/* end cylinder */
	u_int32_t	mbrp_start;	/* absolute starting sector number */
	u_int32_t	mbrp_size;	/* partition size in sectors */
} __attribute__((__packed__));

typedef struct mbr_bootsel mbr_bootsel_t;
struct mbr_bootsel {
	uint8_t		mbrb_defkey;
	uint8_t		mbrb_flags;
	uint16_t	mbrb_timeo;
	char		mbrb_nametab[NMBRPART][PARTNAMESIZE + 1];
	uint16_t	mbrb_magic;
};

typedef struct mbr_sector mbr_sector_t;
struct mbr_sector {
	uint8_t		mbr_bootinst[MBR_BOOTSELOFF];
	mbr_bootsel_t	mbr_bootsel;
	mbr_partition_t	mbr_parts[NMBRPART];
	uint16_t	mbr_signature;
} __attribute__((__packed__));

#if defined(_KERNEL) || defined(_STANDALONE)
int xlat_mbr_fstype(int);
#endif
#endif

/* Known MBR flags: */
#define	MBR_FLAGS_ACTIVE	0x80	/* This is the boot partition */

/* Known MBR partition types: */
#define	MBR_PTYPE_NETBSD	0xa9	/* NetBSD partition type */
#define	MBR_PTYPE_OPENBSD	0xa6	/* OpenBSD partition type */
#define	MBR_PTYPE_386BSD	0xa5	/* 386BSD partition type */
#define	MBR_PTYPE_FAT12		0x01	/* 12-bit FAT */
#define	MBR_PTYPE_FAT16S	0x04	/* 16-bit FAT, less than 32M */
#define	MBR_PTYPE_EXT		0x05	/* extended partition */
#define	MBR_PTYPE_FAT16B	0x06	/* 16-bit FAT, more than 32M */
#define	MBR_PTYPE_NTFS		0x07	/* OS/2 HPFS, NTFS, QNX2, Adv. UNIX */
#define	MBR_PTYPE_FAT32		0x0b	/* 32-bit FAT */
#define	MBR_PTYPE_FAT32L	0x0c	/* 32-bit FAT, LBA-mapped */
#define	MBR_PTYPE_FAT16L	0x0e	/* 16-bit FAT, LBA-mapped */
#define	MBR_PTYPE_EXT_LBA	0x0f	/* extended partition, LBA-mapped */
#define MBR_PTYPE_LNXSWAP	0x82	/* Linux swap or Solaris */
#define	MBR_PTYPE_LNXEXT2	0x83	/* Linux native */
#define	MBR_PTYPE_EXT_LNX	0x85	/* Linux extended partition */
#define MBR_PTYPE_NTFSVOL	0x87	/* NTFS volume set or HPFS mirrored */
#define	MBR_PTYPE_PREP		0x41	/* PReP */
#define	MBR_PTYPE_APPLEUFS 	0xa8	/* Apple UFS */

/* Isolate the relevant bits to get sector and cylinder. */
#define	MBR_PSECT(s)	((s) & 0x3f)
#define	MBR_PCYL(c, s)	((c) + (((s) & 0xc0) << 2))

#define MBR_IS_EXTENDED(x)	((x) == MBR_PTYPE_EXT || \
				 (x) == MBR_PTYPE_EXT_LBA || \
				 (x) == MBR_PTYPE_EXT_LNX)

#endif /* _SYS_DISKLABEL_MBR_H_ */
