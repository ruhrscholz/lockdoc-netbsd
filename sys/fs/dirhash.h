/* $NetBSD: dirhash.h,v 1.1 2008/08/29 14:20:26 reinoud Exp $ */

/*
 * Copyright (c) 2008 Reinoud Zandijk
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
 */


#include <sys/cdefs.h>
#ifndef lint
__KERNEL_RCSID(0, "$NetBSD: dirhash.h,v 1.1 2008/08/29 14:20:26 reinoud Exp $");
#endif /* not lint */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/sysctl.h>
#include <sys/namei.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <miscfs/genfs/genfs_node.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/file.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/malloc.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/conf.h>
#include <sys/kauth.h>
#include <dev/clock_subr.h>

#if defined(_KERNEL_OPT)
#include "opt_udf.h"
#endif


#ifndef DIRHASH_SIZE
#	define DIRHASH_SIZE	(1024*1024)
#endif

#define DIRHASH_HASHBITS	5
#define DIRHASH_HASHSIZE	(1<<DIRHASH_HASHBITS)
#define DIRHASH_HASHMASK	(DIRHASH_HASHSIZE -1)


/* dirent's d_namlen is to avoid useless costly fid->dirent translations */
struct dirhash_entry {
	uint32_t		 hashvalue;
	uint64_t		 offset;
	uint32_t		 d_namlen;
	uint32_t		 entry_size;
	LIST_ENTRY(dirhash_entry) next;
};


struct dirhash {
	uint32_t		 flags;
	uint32_t		 size;			/* in bytes */
	uint32_t		 refcnt;
	LIST_HEAD(, dirhash_entry) entries[DIRHASH_HASHSIZE];
	LIST_HEAD(, dirhash_entry) free_entries;
	TAILQ_ENTRY(dirhash) next;
};

#define DIRH_PURGED	0x0001	/* dirhash has been purged */
#define	DIRH_COMPLETE	0x0002	/* dirhash is complete */
#define	DIRH_BROKEN	0x0004	/* dirhash is broken on readin */
#define DIRH_FLAGBITS \
	"\10\1DIRH_PURGED\2DIRH_COMPLETE\3DIRH_BROKEN"


extern uint32_t maxdirhashsize;
extern uint32_t dirhashsize;

void dirhash_purge(struct dirhash **dirh);
void dirhash_purge_entries(struct dirhash *dirh);
void dirhash_get(struct dirhash **dirhp);
void dirhash_put(struct dirhash *dirh);
void dirhash_enter(struct dirhash *dirh,
	struct dirent *dirent, uint64_t offset, uint32_t entry_size, int new);
void dirhash_enter_freed(struct dirhash *dirh, uint64_t offset,
	uint32_t entry_size);
void dirhash_remove(struct dirhash *dirh, struct dirent *dirent,
	uint64_t offset, uint32_t entry_size);
int dirhash_lookup(struct dirhash *dirh, const char *d_name, int d_namlen,
	struct dirhash_entry **result);
int dirhash_lookup_freed(struct dirhash *dirh, uint32_t min_entrysize,
	struct dirhash_entry **result);

