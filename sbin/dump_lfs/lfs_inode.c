/*      $NetBSD: lfs_inode.c,v 1.8 2003/08/07 10:04:15 agc Exp $ */

/*-
 * Copyright (c) 1980, 1991, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1980, 1991, 1993, 1994\n\
        The Regents of the University of California.  All rights reserved.\n");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)main.c      8.6 (Berkeley) 5/1/95";
#else
__RCSID("$NetBSD: lfs_inode.c,v 1.8 2003/08/07 10:04:15 agc Exp $");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ufs/ufs/dir.h>
#include <ufs/ufs/dinode.h>
#include <sys/mount.h>
#include <ufs/lfs/lfs.h>

#include <protocols/dumprestore.h>

#include <ctype.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dump.h"

#define MAXIFPB        (MAXBSIZE / sizeof(IFILE))

#define	HASDUMPEDFILE	0x1
#define	HASSUBDIRS	0x2

struct lfs *sblock;

int is_ufs2 = 0;

/*
 * Read the superblock from disk, and check its magic number.
 * Determine whether byte-swapping needs to be done on this filesystem.
 */
int
fs_read_sblock(char *superblock)
{
	char tbuf[LFS_SBPAD];
	int ns = 0;
	off_t sboff = LFS_LABELPAD;

	sblock = (struct lfs *)superblock;
	while(1) {
		rawread(sboff, (char *) sblock, LFS_SBPAD);
		if (sblock->lfs_magic != LFS_MAGIC) {
#ifdef notyet
			if (sblock->lfs_magic == bswap32(LFS_MAGIC)) {
				lfs_sb_swap(sblock, sblock, 0);
				ns = 1;
			} else
#endif
				quit("bad sblock magic number\n");
		}
		if (fsbtob(sblock, sblock->lfs_sboffs[0]) != sboff) {
			sboff = fsbtob(sblock, sblock->lfs_sboffs[0]);
			continue;
		}
		break;
	}

	/*
	 * Read the secondary and take the older of the two
	 */
	rawread(fsbtob(sblock, sblock->lfs_sboffs[1]), tbuf, LFS_SBPAD);
#ifdef notyet
	if (ns)
		lfs_sb_swap(tbuf, tbuf, 0);
#endif
	if (((struct lfs *)tbuf)->lfs_magic != LFS_MAGIC) {
		msg("Warning: secondary superblock at 0x%x bad magic\n",
			fsbtodb(sblock, sblock->lfs_sboffs[1]));
	} else {
		if (sblock->lfs_version > 1) {
			if (((struct lfs *)tbuf)->lfs_serial < sblock->lfs_serial) {
				memcpy(sblock, tbuf, LFS_SBPAD);
				sboff = fsbtob(sblock, sblock->lfs_sboffs[1]);
			}
		} else {
			if (((struct lfs *)tbuf)->lfs_otstamp < sblock->lfs_otstamp) {
				memcpy(sblock, tbuf, LFS_SBPAD);
				sboff = fsbtob(sblock, sblock->lfs_sboffs[1]);
			}
		}
	}
	if (sboff != LFS_SBPAD) {
		msg("Using superblock at alternate location 0x%lx\n",
		    (unsigned long)(btodb(sboff)));
	}

	return ns;
}

/*
 * Fill in the ufsi struct, as well as the maxino and dev_bsize global
 * variables.
 */
struct ufsi *
fs_parametrize(void)
{
	static struct ufsi ufsi;

	spcl.c_flags = iswap32(iswap32(spcl.c_flags) | DR_NEWINODEFMT);

	ufsi.ufs_dsize = fsbtodb(sblock,sblock->lfs_size);
	if (sblock->lfs_version == 1) 
		ufsi.ufs_dsize = sblock->lfs_size >> sblock->lfs_blktodb;
	ufsi.ufs_bsize = sblock->lfs_bsize;
	ufsi.ufs_bshift = sblock->lfs_bshift;
	ufsi.ufs_fsize = sblock->lfs_fsize;
	ufsi.ufs_frag = sblock->lfs_frag;
	ufsi.ufs_fsatoda = sblock->lfs_fsbtodb;
	if (sblock->lfs_version == 1)
		ufsi.ufs_fsatoda = 0;
	ufsi.ufs_nindir = sblock->lfs_nindir;
	ufsi.ufs_inopb = sblock->lfs_inopb;
	ufsi.ufs_maxsymlinklen = sblock->lfs_maxsymlinklen;
	ufsi.ufs_bmask = ~(sblock->lfs_bmask);
	ufsi.ufs_qbmask = sblock->lfs_bmask;
	ufsi.ufs_fmask = ~(sblock->lfs_ffmask);
	ufsi.ufs_qfmask = sblock->lfs_ffmask;

	dev_bsize = sblock->lfs_bsize >> sblock->lfs_blktodb;

	return &ufsi;
}

ino_t
fs_maxino(void)
{
	return ((getino(sblock->lfs_ifile)->dp1.di_size
		   - (sblock->lfs_cleansz + sblock->lfs_segtabsz)
		   * sblock->lfs_bsize)
		  / sblock->lfs_bsize) * sblock->lfs_ifpb - 1;
}

void
fs_mapinodes(ino_t maxino, u_int64_t *tapesz, int *anydirskipped)
{
	ino_t ino;

	for (ino = ROOTINO; ino < maxino; ino++)
		mapfileino(ino, tapesz, anydirskipped);
}

/*
 * XXX KS - I know there's a better way to do this.
 */
#define BASE_SINDIR (NDADDR)
#define BASE_DINDIR (NDADDR+NINDIR(fs))
#define BASE_TINDIR (NDADDR+NINDIR(fs)+NINDIR(fs)*NINDIR(fs))

#define D_UNITS (NINDIR(fs))
#define T_UNITS (NINDIR(fs)*NINDIR(fs))

static daddr_t
lfs_bmap(struct lfs *fs, struct ufs1_dinode *idinode, daddr_t lbn)
{
	daddr_t residue, up;
	int off=0;
	char bp[MAXBSIZE];
	
	if(lbn > 0 && lbn > lblkno(fs, idinode->di_size)) {
		return UNASSIGNED;
	}
	/*
	 * Indirect blocks: if it is a first-level indirect, pull its
	 * address from the inode; otherwise, call ourselves to find the
	 * address of the parent indirect block, and load that to find
	 * the desired address.
	 */
	if(lbn < 0) {
		lbn *= -1;
		if(lbn == NDADDR) {
			/* printf("lbn %d: single indir base\n", -lbn); */
			return idinode->di_ib[0]; /* single indirect */
		} else if(lbn == BASE_DINDIR+1) {
			/* printf("lbn %d: double indir base\n", -lbn); */
			return idinode->di_ib[1]; /* double indirect */
		} else if(lbn == BASE_TINDIR+2) {
			/* printf("lbn %d: triple indir base\n", -lbn); */
			return idinode->di_ib[2]; /* triple indirect */
		}

		/*
		 * Find the immediate parent. This is essentially finding the
		 * residue of modulus, and then rounding accordingly.
		 */
		residue = (lbn-NDADDR) % NINDIR(fs);
		if(residue == 1) {
			/* Double indirect.  Parent is the triple. */
			up = idinode->di_ib[2];
			off = (lbn-2-BASE_TINDIR)/(NINDIR(fs)*NINDIR(fs));
			if(up == UNASSIGNED || up == LFS_UNUSED_DADDR)
				return UNASSIGNED;
			/* printf("lbn %d: parent is the triple\n", -lbn); */
			bread(fsbtodb(sblock, up), bp, sblock->lfs_bsize);
			/* XXX ondisk32 */
			return (daddr_t)((int32_t *)bp)[off];
		} else /* residue == 0 */ {
			/* Single indirect.  Two cases. */
			if(lbn < BASE_TINDIR) {
				/* Parent is the double, simple */
				up = -(BASE_DINDIR) - 1;
				off = (lbn-BASE_DINDIR) / D_UNITS;
				/* printf("lbn %d: parent is %d/%d\n", -lbn, up,off); */
			} else {
				/* Ancestor is the triple, more complex */
				up = ((lbn-BASE_TINDIR) / T_UNITS)
					* T_UNITS + BASE_TINDIR + 1;
				off = (lbn/D_UNITS) - (up/D_UNITS);
				up = -up;
				/* printf("lbn %d: parent is %d/%d\n", -lbn, up,off); */
			}
		}
	} else {
		/* Direct block.  Its parent must be a single indirect. */
		if(lbn < NDADDR)
			return idinode->di_db[lbn];
		else {
			/* Parent is an indirect block. */
			up = -(((lbn-NDADDR) / D_UNITS) * D_UNITS + NDADDR);
			off = (lbn-NDADDR) % D_UNITS;
			/* printf("lbn %d: parent is %d/%d\n", lbn,up,off); */
		}
	}
	up = lfs_bmap(fs,idinode,up);
	if(up == UNASSIGNED || up == LFS_UNUSED_DADDR)
		return UNASSIGNED;
	bread(fsbtodb(sblock, up), bp, sblock->lfs_bsize);
	/* XXX ondisk32 */
	return (daddr_t)((int32_t *)bp)[off];
}

static struct ifile *
lfs_ientry(ino_t ino)
{
	static struct ifile ifileblock[MAXIFPB];
	static daddr_t ifblkno;
	daddr_t lbn;
	daddr_t blkno;
	union dinode *dp;
    
	lbn = ino/sblock->lfs_ifpb + sblock->lfs_cleansz + sblock->lfs_segtabsz;
	dp = getino(sblock->lfs_ifile);
	blkno = lfs_bmap(sblock, &dp->dp1 ,lbn);
	if (blkno != ifblkno)
		bread(fsbtodb(sblock, blkno), (char *)ifileblock,
		    sblock->lfs_bsize);
	return ifileblock + (ino % sblock->lfs_ifpb);
}

/* Search a block for a specific dinode. */
static struct ufs1_dinode *
lfs_ifind(struct lfs *fs, ino_t ino, struct ufs1_dinode *dip)
{
	register int cnt;

	for(cnt=0;cnt<INOPB(fs);cnt++)
		if(dip[cnt].di_inumber == ino)
			return &(dip[cnt]);
	return NULL;
}

union dinode *
getino(inum)
	ino_t inum;
{
	static daddr_t inoblkno;
	daddr_t blkno;
	static struct ufs1_dinode inoblock[MAXBSIZE / sizeof (struct ufs1_dinode)];
	static struct ufs1_dinode ifile_dinode; /* XXX fill this in */
	static struct ufs1_dinode empty_dinode; /* Always stays zeroed */
	struct ufs1_dinode *dp;

	if(inum == sblock->lfs_ifile) {
		/* Load the ifile inode if not already */
		if(ifile_dinode.di_u.inumber == 0) {
			blkno = sblock->lfs_idaddr;
			bread(fsbtodb(sblock, blkno), (char *)inoblock, 
				(int)sblock->lfs_bsize);
			dp = lfs_ifind(sblock, inum, inoblock);
			ifile_dinode = *dp; /* Structure copy */
		}
		return (union dinode *)&ifile_dinode;
	}

	curino = inum;
	blkno = lfs_ientry(inum)->if_daddr;
	if(blkno == LFS_UNUSED_DADDR)
		return (union dinode *)&empty_dinode;

	if(blkno != inoblkno) {
		bread(fsbtodb(sblock, blkno), (char *)inoblock, 
			(int)sblock->lfs_bsize);
#ifdef notyet
		if (needswap)
			for (i = 0; i < MAXINOPB; i++)
				ffs_dinode_swap(&inoblock[i], &inoblock[i]);
#endif
	}
	return (union dinode *)lfs_ifind(sblock, inum, inoblock);
}
