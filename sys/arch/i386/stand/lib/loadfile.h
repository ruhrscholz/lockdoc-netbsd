/*	$NetBSD: loadfile.h,v 1.3 1999/01/29 18:44:09 christos Exp $	 */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
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

#define BOOT_AOUT
#define BOOT_ELF
#define ELFSIZE 32

/*
 * Array indices in the u_long position array
 */
#define MARK_START	0
#define MARK_ENTRY	1
#define	MARK_NSYM	2
#define MARK_SYM	3
#define	MARK_END	4
#define	MARK_MAX	5

/*
 * Bit flags for sections to load
 */
#define	LOAD_TEXT	0x0001
#define	LOAD_DATA	0x0002
#define	LOAD_BSS	0x0004
#define	LOAD_SYM	0x0008
#define	LOAD_HDR	0x0010
#define LOAD_ALL	0x001f

#define	COUNT_TEXT	0x0100
#define	COUNT_DATA	0x0200
#define	COUNT_BSS	0x0400
#define	COUNT_SYM	0x0800
#define	COUNT_HDR	0x1000
#define COUNT_ALL	0x1f00

int loadfile __P((const char *, u_long *, int));

#ifdef _STANDALONE

#define LOADADDR(a)		((((u_long)(a)) & 0x00ffffff) + offset)
#define ALIGNENTRY(a)		((u_long)(a) & 0x00100000)
#define READ(f, b, c)		pread((f), (void *)LOADADDR(b), (c))
#define BCOPY(s, d, c)		vpbcopy((s), (void *)LOADADDR(d), (c))
#define BZERO(d, c)		pbzero((void *)LOADADDR(d), (c))
#define	WARN(a)			(void)(printf a, \
				    printf((errno ? ": %s\n" : "\n"), \
				    strerror(errno)))
#define PROGRESS(a)		(void) printf a
#define ALLOC(a)		alloc(a)
#define FREE(a, b)		free(a, b)
#define OKMAGIC(a)		((a) == ZMAGIC)

void vpbcopy __P((const void *, void *, size_t));
void pbzero __P((void *, size_t));
ssize_t pread __P((int, void *, size_t));

#else

#define LOADADDR(a)		(((u_long)(a)) + offset)
#define ALIGNENTRY(a)		((u_long)(a))
#define READ(f, b, c)		read((f), (void *)LOADADDR(b), (c))
#define BCOPY(s, d, c)		memcpy((void *)LOADADDR(d), (void *)(s), (c))
#define BZERO(d, c)		memset((void *)LOADADDR(d), 0, (c))
#define WARN(a)			warn a
#define PROGRESS(a)		/* nothing */
#define ALLOC(a)		malloc(a)
#define FREE(a, b)		free(a)
#define OKMAGIC(a)		((a) == OMAGIC)

ssize_t vread __P((int, u_long, u_long *, size_t));
void vcopy __P((u_long, u_long, u_long *, size_t));
void vzero __P((u_long, u_long *, size_t));

#endif
