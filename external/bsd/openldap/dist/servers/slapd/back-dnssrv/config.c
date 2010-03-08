/*	$NetBSD: config.c,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

/* config.c - DNS SRV backend configuration file routine */
/* OpenLDAP: pkg/ldap/servers/slapd/back-dnssrv/config.c,v 1.16.2.4 2009/01/22 00:01:06 kurt Exp */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2000-2009 The OpenLDAP Foundation.
 * Portions Copyright 2000-2003 Kurt D. Zeilenga.
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
/* ACKNOWLEDGEMENTS:
 * This work was originally developed by Kurt D. Zeilenga for inclusion
 * in OpenLDAP Software.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "proto-dnssrv.h"

int
dnssrv_back_db_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv )
{
#if 0
	struct ldapinfo	*li = (struct ldapinfo *) be->be_private;

	if ( li == NULL ) {
		fprintf( stderr, "%s: line %d: DNSSRV backend info is null!\n",
		    fname, lineno );
		return( 1 );
	}
#endif

	/* no configuration options (yet) */
	return SLAP_CONF_UNKNOWN;
}
