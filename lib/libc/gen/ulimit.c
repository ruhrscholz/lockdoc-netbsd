/*	$NetBSD: ulimit.c,v 1.1 1999/09/13 18:38:06 kleink Exp $	*/

/*-
 * Copyright (c) 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
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

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: ulimit.c,v 1.1 1999/09/13 18:38:06 kleink Exp $");
#endif /* LIBC_SCCS and not lint */

#include <sys/resource.h>
#include <errno.h>
#include <ulimit.h>

#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

long int
#if __STDC__
ulimit(int cmd, ...)
#else
ulimit(cmd, va_alist)
	int cmd;
	va_dcl
#endif
{
	va_list ap;
	struct rlimit rlimit;
	long int new_limit, result;

#if __STDC__
	va_start(ap, cmd);
#else
	va_start(ap);
#endif

	result = -1L;
	switch (cmd) {
	case UL_GETFSIZE:
		if (getrlimit(RLIMIT_FSIZE, &rlimit) == 0)
			result = (long int)(rlimit.rlim_cur / 512);
		break;
	case UL_SETFSIZE:
		new_limit = va_arg(ap, long int);
		rlimit.rlim_cur = rlimit.rlim_max = (rlim_t)new_limit * 512;
		if (setrlimit(RLIMIT_FSIZE, &rlimit) == 0)
			result = new_limit;
		break;
	default:
		errno = EINVAL;
	}

	va_end(ap);

	return (result);
}
