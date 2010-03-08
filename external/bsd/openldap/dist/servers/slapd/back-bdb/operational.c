/*	$NetBSD: operational.c,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

/* operational.c - bdb backend operational attributes function */
/* OpenLDAP: pkg/ldap/servers/slapd/back-bdb/operational.c,v 1.29.2.5 2009/03/05 22:24:29 quanah Exp */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2000-2009 The OpenLDAP Foundation.
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

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"
#include "back-bdb.h"

/*
 * sets *hasSubordinates to LDAP_COMPARE_TRUE/LDAP_COMPARE_FALSE
 * if the entry has children or not.
 */
int
bdb_hasSubordinates(
	Operation	*op,
	Entry		*e,
	int		*hasSubordinates )
{
	struct bdb_info *bdb = (struct bdb_info *) op->o_bd->be_private;
	struct bdb_op_info	*opinfo;
	OpExtra *oex;
	DB_TXN		*rtxn;
	int		rc;
	
	assert( e != NULL );

	/* NOTE: this should never happen, but it actually happens
	 * when using back-relay; until we find a better way to
	 * preserve entry's private information while rewriting it,
	 * let's disable the hasSubordinate feature for back-relay.
	 */
	if ( BEI( e ) == NULL ) {
		return LDAP_OTHER;
	}

	/* Check for a txn in a parent op, otherwise use reader txn */
	LDAP_SLIST_FOREACH( oex, &op->o_extra, oe_next ) {
		if ( oex->oe_key == bdb )
			break;
	}
	opinfo = (struct bdb_op_info *) oex;
	if ( opinfo && opinfo->boi_txn ) {
		rtxn = opinfo->boi_txn;
	} else {
		rc = bdb_reader_get(op, bdb->bi_dbenv, &rtxn);
		if ( rc ) return LDAP_OTHER;
	}

retry:
	/* FIXME: we can no longer assume the entry's e_private
	 * field is correctly populated; so we need to reacquire
	 * it with reader lock */
	rc = bdb_cache_children( op, rtxn, e );

	switch( rc ) {
	case DB_LOCK_DEADLOCK:
	case DB_LOCK_NOTGRANTED:
		goto retry;

	case 0:
		*hasSubordinates = LDAP_COMPARE_TRUE;
		break;

	case DB_NOTFOUND:
		*hasSubordinates = LDAP_COMPARE_FALSE;
		rc = LDAP_SUCCESS;
		break;

	default:
		Debug(LDAP_DEBUG_ARGS, 
			"<=- " LDAP_XSTRING(bdb_hasSubordinates)
			": has_children failed: %s (%d)\n", 
			db_strerror(rc), rc, 0 );
		rc = LDAP_OTHER;
	}

	return rc;
}

/*
 * sets the supported operational attributes (if required)
 */
int
bdb_operational(
	Operation	*op,
	SlapReply	*rs )
{
	Attribute	**ap;

	assert( rs->sr_entry != NULL );

	for ( ap = &rs->sr_operational_attrs; *ap; ap = &(*ap)->a_next )
		/* just count */ ;

	if ( SLAP_OPATTRS( rs->sr_attr_flags ) ||
			ad_inlist( slap_schema.si_ad_hasSubordinates, rs->sr_attrs ) )
	{
		int	hasSubordinates, rc;

		rc = bdb_hasSubordinates( op, rs->sr_entry, &hasSubordinates );
		if ( rc == LDAP_SUCCESS ) {
			*ap = slap_operational_hasSubordinate( hasSubordinates == LDAP_COMPARE_TRUE );
			assert( *ap != NULL );

			ap = &(*ap)->a_next;
		}
	}

	return LDAP_SUCCESS;
}

