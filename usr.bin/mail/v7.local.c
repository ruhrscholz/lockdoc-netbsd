/*	$NetBSD: v7.local.c,v 1.14 2003/08/07 11:14:42 agc Exp $	*/

/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
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
#if 0
static char sccsid[] = "@(#)v7.local.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD: v7.local.c,v 1.14 2003/08/07 11:14:42 agc Exp $");
#endif
#endif /* not lint */

/*
 * Mail -- a mail program
 *
 * Version 7
 *
 * Local routines that are installation dependent.
 */

#include "rcv.h"
#include "extern.h"

/*
 * Locate the user's mailbox file (ie, the place where new, unread
 * mail is queued).
 */
void
findmail(char *user, char *buf)
{
	char *mbox;

	if (!(mbox = getenv("MAIL")))
		(void)snprintf(buf, PATHSIZE, "%s/%s", _PATH_MAILDIR, user);
	else {
		(void)strncpy(buf, mbox, PATHSIZE - 1);
		buf[PATHSIZE - 1] = '\0';
	}
}

/*
 * Get rid of the queued mail.
 */
void
demail(void)
{

	/*
	 * Do not remove the spool file, just truncate it to zero
	 * bytes if possible, since we wouldn't preserve
	 * owner/permissions otherwise.
	 */
	if (value("keep") != NULL || truncate(mailname, 0) < 0)
		(void)close(creat(mailname, 0600));
}

/*
 * Discover user login name.
 */
const char *
username(void)
{
	const char *np;
	uid_t uid;

	if ((np = getenv("USER")) != NULL)
		return np;
	if ((np = getname(uid = getuid())) != NULL)
		return np;
	printf("Cannot associate a name with uid %u\n", (unsigned)uid);
	return NULL;
}
