/*	$NetBSD: signal.c,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

/* OpenLDAP: pkg/ldap/libraries/liblutil/signal.c,v 1.10.2.4 2009/01/22 00:00:58 kurt Exp */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2009 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "portable.h"

#ifdef HAVE_SIGACTION
#include <ac/string.h>
#include <ac/signal.h>

lutil_sig_t
lutil_sigaction(int sig, lutil_sig_t func)
{
	struct sigaction action, oaction;

	memset( &action, '\0', sizeof(action) );

	action.sa_handler = func;
	sigemptyset( &action.sa_mask );
#ifdef SA_RESTART
	action.sa_flags |= SA_RESTART;
#endif
	
	if( sigaction( sig, &action, &oaction ) != 0 ) {
		return NULL;
	}

	return oaction.sa_handler;
}
#endif
