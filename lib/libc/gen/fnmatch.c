/*	$NetBSD: fnmatch.c,v 1.15 1999/09/16 11:44:56 lukem Exp $	*/

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)fnmatch.c	8.2 (Berkeley) 4/16/94";
#else
__RCSID("$NetBSD: fnmatch.c,v 1.15 1999/09/16 11:44:56 lukem Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

/*
 * Function fnmatch() as specified in POSIX 1003.2-1992, section B.6.
 * Compares a filename or pathname to a pattern.
 */

#include "namespace.h"

#include <assert.h>
#include <fnmatch.h>
#include <string.h>

#ifdef __weak_alias
__weak_alias(fnmatch,_fnmatch);
#endif

#define	EOS	'\0'

static const char *rangematch __P((const char *, int, int));

int
fnmatch(pattern, string, flags)
	const char *pattern, *string;
	int flags;
{
	const char *stringstart;
	char c, test;

	_DIAGASSERT(pattern != NULL);
	_DIAGASSERT(string != NULL);
#ifdef _DIAGNOSTIC
	if (pattern == NULL || string == NULL)
		return (FNM_NOMATCH + 1);
			/* per SUS, return non zero but not FNM_NOMATCH */
#endif

	for (stringstart = string;;)
		switch (c = *pattern++) {
		case EOS:
			return (*string == EOS ? 0 : FNM_NOMATCH);
		case '?':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && (flags & FNM_PATHNAME))
				return (FNM_NOMATCH);
			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);
			++string;
			break;
		case '*':
			c = *pattern;
			/* Collapse multiple stars. */
			while (c == '*')
				c = *++pattern;

			if (*string == '.' && (flags & FNM_PERIOD) &&
			    (string == stringstart ||
			    ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return (FNM_NOMATCH);

			/* Optimize for pattern with * at end or before /. */
			if (c == EOS) {
				if (flags & FNM_PATHNAME)
					return (strchr(string, '/') == NULL ?
					    0 : FNM_NOMATCH);
				else
					return (0);
			} else if (c == '/' && flags & FNM_PATHNAME) {
				if ((string = strchr(string, '/')) == NULL)
					return (FNM_NOMATCH);
				break;
			}

			/* General case, use recursion. */
			while ((test = *string) != EOS) {
				if (!fnmatch(pattern, string, flags & ~FNM_PERIOD))
					return (0);
				if (test == '/' && flags & FNM_PATHNAME)
					break;
				++string;
			}
			return (FNM_NOMATCH);
		case '[':
			if (*string == EOS)
				return (FNM_NOMATCH);
			if (*string == '/' && flags & FNM_PATHNAME)
				return (FNM_NOMATCH);
			if ((pattern =
			    rangematch(pattern, *string, flags)) == NULL)
				return (FNM_NOMATCH);
			++string;
			break;
		case '\\':
			if (!(flags & FNM_NOESCAPE)) {
				if ((c = *pattern++) == EOS) {
					c = '\\';
					--pattern;
				}
			}
			/* FALLTHROUGH */
		default:
			if (c != *string++)
				return (FNM_NOMATCH);
			break;
		}
	/* NOTREACHED */
}

static const char *
rangematch(pattern, test, flags)
	const char *pattern;
	int test, flags;
{
	int negate, ok;
	char c, c2;

	_DIAGASSERT(pattern != NULL);

	/*
	 * A bracket expression starting with an unquoted circumflex
	 * character produces unspecified results (IEEE 1003.2-1992,
	 * 3.13.2).  This implementation treats it like '!', for
	 * consistency with the regular expression syntax.
	 * J.T. Conklin (conklin@ngai.kaleida.com)
	 */
	if ((negate = (*pattern == '!' || *pattern == '^')) != 0)
		++pattern;
	
	for (ok = 0; (c = *pattern++) != ']';) {
		if (c == '\\' && !(flags & FNM_NOESCAPE))
			c = *pattern++;
		if (c == EOS)
			return (NULL);
		if (*pattern == '-' 
		    && (c2 = *(pattern+1)) != EOS && c2 != ']') {
			pattern += 2;
			if (c2 == '\\' && !(flags & FNM_NOESCAPE))
				c2 = *pattern++;
			if (c2 == EOS)
				return (NULL);
			if (c <= test && test <= c2)
				ok = 1;
		} else if (c == test)
			ok = 1;
	}
	return (ok == negate ? NULL : pattern);
}
