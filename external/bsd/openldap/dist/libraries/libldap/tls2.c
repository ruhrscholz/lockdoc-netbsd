/*	$NetBSD: tls2.c,v 1.1.1.1 2010/03/08 02:14:20 lukem Exp $	*/

/* tls.c - Handle tls/ssl. */
/* OpenLDAP: pkg/ldap/libraries/libldap/tls2.c,v 1.4.2.9 2009/10/30 17:48:16 quanah Exp */
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
/* ACKNOWLEDGEMENTS: restructured by Howard Chu.
 */

#include "portable.h"
#include "ldap_config.h"

#include <stdio.h>

#include <ac/stdlib.h>
#include <ac/errno.h>
#include <ac/socket.h>
#include <ac/string.h>
#include <ac/ctype.h>
#include <ac/time.h>
#include <ac/unistd.h>
#include <ac/param.h>
#include <ac/dirent.h>

#include "ldap-int.h"

#ifdef HAVE_TLS

#include "ldap-tls.h"

#ifdef LDAP_R_COMPILE
#include <ldap_pvt_thread.h>
#endif

static tls_impl *tls_imp = &ldap_int_tls_impl;
#define HAS_TLS( sb )	ber_sockbuf_ctrl( sb, LBER_SB_OPT_HAS_IO, \
				(void *)tls_imp->ti_sbio )

#endif /* HAVE_TLS */

/* RFC2459 minimum required set of supported attribute types
 * in a certificate DN
 */
typedef struct oid_name {
	struct berval oid;
	struct berval name;
} oid_name;

static oid_name oids[] = {
	{ BER_BVC("2.5.4.3"), BER_BVC("cn") },
	{ BER_BVC("2.5.4.4"), BER_BVC("sn") },
	{ BER_BVC("2.5.4.6"), BER_BVC("c") },
	{ BER_BVC("2.5.4.7"), BER_BVC("l") },
	{ BER_BVC("2.5.4.8"), BER_BVC("st") },
	{ BER_BVC("2.5.4.10"), BER_BVC("o") },
	{ BER_BVC("2.5.4.11"), BER_BVC("ou") },
	{ BER_BVC("2.5.4.12"), BER_BVC("title") },
	{ BER_BVC("2.5.4.41"), BER_BVC("name") },
	{ BER_BVC("2.5.4.42"), BER_BVC("givenName") },
	{ BER_BVC("2.5.4.43"), BER_BVC("initials") },
	{ BER_BVC("2.5.4.44"), BER_BVC("generationQualifier") },
	{ BER_BVC("2.5.4.46"), BER_BVC("dnQualifier") },
	{ BER_BVC("1.2.840.113549.1.9.1"), BER_BVC("email") },
	{ BER_BVC("0.9.2342.19200300.100.1.25"), BER_BVC("dc") },
	{ BER_BVNULL, BER_BVNULL }
};

#ifdef HAVE_TLS

void
ldap_pvt_tls_ctx_free ( void *c )
{
	if ( !c ) return;
	tls_imp->ti_ctx_free( c );
}

static void
tls_ctx_ref( tls_ctx *ctx )
{
	if ( !ctx ) return;

	tls_imp->ti_ctx_ref( ctx );
}

#ifdef LDAP_R_COMPILE
/*
 * an extra mutex for the default ctx.
 */
static ldap_pvt_thread_mutex_t tls_def_ctx_mutex;
#endif

void
ldap_int_tls_destroy( struct ldapoptions *lo )
{
	if ( lo->ldo_tls_ctx ) {
		ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
	}

	if ( lo->ldo_tls_certfile ) {
		LDAP_FREE( lo->ldo_tls_certfile );
		lo->ldo_tls_certfile = NULL;
	}
	if ( lo->ldo_tls_keyfile ) {
		LDAP_FREE( lo->ldo_tls_keyfile );
		lo->ldo_tls_keyfile = NULL;
	}
	if ( lo->ldo_tls_dhfile ) {
		LDAP_FREE( lo->ldo_tls_dhfile );
		lo->ldo_tls_dhfile = NULL;
	}
	if ( lo->ldo_tls_cacertfile ) {
		LDAP_FREE( lo->ldo_tls_cacertfile );
		lo->ldo_tls_cacertfile = NULL;
	}
	if ( lo->ldo_tls_cacertdir ) {
		LDAP_FREE( lo->ldo_tls_cacertdir );
		lo->ldo_tls_cacertdir = NULL;
	}
	if ( lo->ldo_tls_ciphersuite ) {
		LDAP_FREE( lo->ldo_tls_ciphersuite );
		lo->ldo_tls_ciphersuite = NULL;
	}
	if ( lo->ldo_tls_crlfile ) {
		LDAP_FREE( lo->ldo_tls_crlfile );
		lo->ldo_tls_crlfile = NULL;
	}
}

/*
 * Tear down the TLS subsystem. Should only be called once.
 */
void
ldap_pvt_tls_destroy( void )
{
	struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   

	ldap_int_tls_destroy( lo );

	tls_imp->ti_tls_destroy();
}

/*
 * Initialize a particular TLS implementation.
 * Called once per implementation.
 */
static int
tls_init(tls_impl *impl )
{
	static int tls_initialized = 0;

	if ( !tls_initialized++ ) {
#ifdef LDAP_R_COMPILE
		ldap_pvt_thread_mutex_init( &tls_def_ctx_mutex );
#endif
	}

	if ( impl->ti_inited++ ) return 0;

#ifdef LDAP_R_COMPILE
	impl->ti_thr_init();
#endif
	return impl->ti_tls_init();
}

/*
 * Initialize TLS subsystem. Called once per implementation.
 */
int
ldap_pvt_tls_init( void )
{
	return tls_init( tls_imp );
}

/*
 * initialize a new TLS context
 */
static int
ldap_int_tls_init_ctx( struct ldapoptions *lo, int is_server )
{
	int rc = 0;
	tls_impl *ti = tls_imp;
	struct ldaptls lts = lo->ldo_tls_info;

	if ( lo->ldo_tls_ctx )
		return 0;

	tls_init( ti );

	if ( is_server && !lts.lt_certfile && !lts.lt_keyfile &&
		!lts.lt_cacertfile && !lts.lt_cacertdir ) {
		/* minimum configuration not provided */
		return LDAP_NOT_SUPPORTED;
	}

#ifdef HAVE_EBCDIC
	/* This ASCII/EBCDIC handling is a real pain! */
	if ( lts.lt_ciphersuite ) {
		lts.lt_ciphersuite = LDAP_STRDUP( lts.lt_ciphersuite );
		__atoe( lts.lt_ciphersuite );
	}
	if ( lts.lt_cacertfile ) {
		lts.lt_cacertfile = LDAP_STRDUP( lts.lt_cacertfile );
		__atoe( lts.lt_cacertfile );
	}
	if ( lts.lt_certfile ) {
		lts.lt_certfile = LDAP_STRDUP( lts.lt_certfile );
		__atoe( lts.lt_certfile );
	}
	if ( lts.lt_keyfile ) {
		lts.lt_keyfile = LDAP_STRDUP( lts.lt_keyfile );
		__atoe( lts.lt_keyfile );
	}
	if ( lts.lt_crlfile ) {
		lts.lt_crlfile = LDAP_STRDUP( lts.lt_crlfile );
		__atoe( lts.lt_crlfile );
	}
	if ( lts.lt_cacertdir ) {
		lts.lt_cacertdir = LDAP_STRDUP( lts.lt_cacertdir );
		__atoe( lts.lt_cacertdir );
	}
	if ( lts.lt_dhfile ) {
		lts.lt_dhfile = LDAP_STRDUP( lts.lt_dhfile );
		__atoe( lts.lt_dhfile );
	}
#endif
	lo->ldo_tls_ctx = ti->ti_ctx_new( lo );
	if ( lo->ldo_tls_ctx == NULL ) {
		Debug( LDAP_DEBUG_ANY,
		   "TLS: could not allocate default ctx.\n",
			0,0,0);
		rc = -1;
		goto error_exit;
	}

	rc = ti->ti_ctx_init( lo, &lts, is_server );

error_exit:
	if ( rc < 0 && lo->ldo_tls_ctx != NULL ) {
		ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
	}
#ifdef HAVE_EBCDIC
	LDAP_FREE( lts.lt_ciphersuite );
	LDAP_FREE( lts.lt_cacertfile );
	LDAP_FREE( lts.lt_certfile );
	LDAP_FREE( lts.lt_keyfile );
	LDAP_FREE( lts.lt_crlfile );
	LDAP_FREE( lts.lt_cacertdir );
	LDAP_FREE( lts.lt_dhfile );
#endif
	return rc;
}

/*
 * initialize the default context
 */
int
ldap_pvt_tls_init_def_ctx( int is_server )
{
	struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   
	int rc;
#ifdef LDAP_R_COMPILE
	ldap_pvt_thread_mutex_lock( &tls_def_ctx_mutex );
#endif
	rc = ldap_int_tls_init_ctx( lo, is_server );
#ifdef LDAP_R_COMPILE
	ldap_pvt_thread_mutex_unlock( &tls_def_ctx_mutex );
#endif
	return rc;
}

static tls_session *
alloc_handle( void *ctx_arg, int is_server )
{
	tls_ctx	*ctx;
	tls_session	*ssl;

	if ( ctx_arg ) {
		ctx = ctx_arg;
	} else {
		struct ldapoptions *lo = LDAP_INT_GLOBAL_OPT();   
		if ( ldap_pvt_tls_init_def_ctx( is_server ) < 0 ) return NULL;
		ctx = lo->ldo_tls_ctx;
	}

	ssl = tls_imp->ti_session_new( ctx, is_server );
	if ( ssl == NULL ) {
		Debug( LDAP_DEBUG_ANY,"TLS: can't create ssl handle.\n",0,0,0);
		return NULL;
	}
	return ssl;
}

static int
update_flags( Sockbuf *sb, tls_session * ssl, int rc )
{
	sb->sb_trans_needs_read  = 0;
	sb->sb_trans_needs_write = 0;

	return tls_imp->ti_session_upflags( sb, ssl, rc );
}

/*
 * Call this to do a TLS connect on a sockbuf. ctx_arg can be
 * a SSL_CTX * or NULL, in which case the default ctx is used.
 *
 * Return value:
 *
 *  0 - Success. Connection is ready for communication.
 * <0 - Error. Can't create a TLS stream.
 * >0 - Partial success.
 *	  Do a select (using information from lber_pvt_sb_needs_{read,write}
 *		and call again.
 */

static int
ldap_int_tls_connect( LDAP *ld, LDAPConn *conn )
{
	Sockbuf *sb = conn->lconn_sb;
	int	err;
	tls_session	*ssl = NULL;

	if ( HAS_TLS( sb )) {
		ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&ssl );
	} else {
		struct ldapoptions *lo;
		tls_ctx *ctx;

		ctx = ld->ld_options.ldo_tls_ctx;

		ssl = alloc_handle( ctx, 0 );

		if ( ssl == NULL ) return -1;

#ifdef LDAP_DEBUG
		ber_sockbuf_add_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)"tls_" );
#endif
		ber_sockbuf_add_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)ssl );

		lo = LDAP_INT_GLOBAL_OPT();   
		if( ctx == NULL ) {
			ctx = lo->ldo_tls_ctx;
			ld->ld_options.ldo_tls_ctx = ctx;
			tls_ctx_ref( ctx );
		}
		if ( ld->ld_options.ldo_tls_connect_cb )
			ld->ld_options.ldo_tls_connect_cb( ld, ssl, ctx,
			ld->ld_options.ldo_tls_connect_arg );
		if ( lo && lo->ldo_tls_connect_cb && lo->ldo_tls_connect_cb !=
			ld->ld_options.ldo_tls_connect_cb )
			lo->ldo_tls_connect_cb( ld, ssl, ctx, lo->ldo_tls_connect_arg );
	}

	err = tls_imp->ti_session_connect( ld, ssl );

#ifdef HAVE_WINSOCK
	errno = WSAGetLastError();
#endif

	if ( err < 0 )
	{
		char buf[256], *msg;
		if ( update_flags( sb, ssl, err )) {
			return 1;
		}

		msg = tls_imp->ti_session_errmsg( ssl, err, buf, sizeof(buf) );
		if ( msg ) {
			if ( ld->ld_error ) {
				LDAP_FREE( ld->ld_error );
			}
			ld->ld_error = LDAP_STRDUP( msg );
#ifdef HAVE_EBCDIC
			if ( ld->ld_error ) __etoa(ld->ld_error);
#endif
		}

		Debug( LDAP_DEBUG_ANY,"TLS: can't connect: %s.\n",
			ld->ld_error ? ld->ld_error : "" ,0,0);

		ber_sockbuf_remove_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT );
#ifdef LDAP_DEBUG
		ber_sockbuf_remove_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT );
#endif
		return -1;
	}

	return 0;
}

/*
 * Call this to do a TLS accept on a sockbuf.
 * Everything else is the same as with tls_connect.
 */
int
ldap_pvt_tls_accept( Sockbuf *sb, void *ctx_arg )
{
	int	err;
	tls_session	*ssl = NULL;

	if ( HAS_TLS( sb )) {
		ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&ssl );
	} else {
		ssl = alloc_handle( ctx_arg, 1 );
		if ( ssl == NULL ) return -1;

#ifdef LDAP_DEBUG
		ber_sockbuf_add_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)"tls_" );
#endif
		ber_sockbuf_add_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT, (void *)ssl );
	}

	err = tls_imp->ti_session_accept( ssl );

#ifdef HAVE_WINSOCK
	errno = WSAGetLastError();
#endif

	if ( err < 0 )
	{
		if ( update_flags( sb, ssl, err )) return 1;

		if ( DebugTest( LDAP_DEBUG_ANY ) ) {
			char buf[256], *msg;
			msg = tls_imp->ti_session_errmsg( ssl, err, buf, sizeof(buf) );
			Debug( LDAP_DEBUG_ANY,"TLS: can't accept: %s.\n",
				msg ? msg : "(unknown)", 0, 0 );
		}

		ber_sockbuf_remove_io( sb, tls_imp->ti_sbio,
			LBER_SBIOD_LEVEL_TRANSPORT );
#ifdef LDAP_DEBUG
		ber_sockbuf_remove_io( sb, &ber_sockbuf_io_debug,
			LBER_SBIOD_LEVEL_TRANSPORT );
#endif
		return -1;
	}
	return 0;
}

int
ldap_pvt_tls_inplace ( Sockbuf *sb )
{
	return HAS_TLS( sb ) ? 1 : 0;
}

int
ldap_tls_inplace( LDAP *ld )
{
	Sockbuf		*sb = NULL;

	if ( ld->ld_defconn && ld->ld_defconn->lconn_sb ) {
		sb = ld->ld_defconn->lconn_sb;

	} else if ( ld->ld_sb ) {
		sb = ld->ld_sb;

	} else {
		return 0;
	}

	return ldap_pvt_tls_inplace( sb );
}

int
ldap_pvt_tls_get_peer_dn( void *s, struct berval *dn,
	LDAPDN_rewrite_dummy *func, unsigned flags )
{
	tls_session *session = s;
	struct berval bvdn;
	int rc;

	rc = tls_imp->ti_session_peer_dn( session, &bvdn );
	if ( rc ) return rc;

	rc = ldap_X509dn2bv( &bvdn, dn, 
			    (LDAPDN_rewrite_func *)func, flags);
	return rc;
}

int
ldap_pvt_tls_check_hostname( LDAP *ld, void *s, const char *name_in )
{
	tls_session *session = s;

	return tls_imp->ti_session_chkhost( ld, session, name_in );
}

int
ldap_int_tls_config( LDAP *ld, int option, const char *arg )
{
	int i;

	switch( option ) {
	case LDAP_OPT_X_TLS_CACERTFILE:
	case LDAP_OPT_X_TLS_CACERTDIR:
	case LDAP_OPT_X_TLS_CERTFILE:
	case LDAP_OPT_X_TLS_KEYFILE:
	case LDAP_OPT_X_TLS_RANDOM_FILE:
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
	case LDAP_OPT_X_TLS_DHFILE:
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		return ldap_pvt_tls_set_option( ld, option, (void *) arg );

	case LDAP_OPT_X_TLS_REQUIRE_CERT:
	case LDAP_OPT_X_TLS:
		i = -1;
		if ( strcasecmp( arg, "never" ) == 0 ) {
			i = LDAP_OPT_X_TLS_NEVER ;

		} else if ( strcasecmp( arg, "demand" ) == 0 ) {
			i = LDAP_OPT_X_TLS_DEMAND ;

		} else if ( strcasecmp( arg, "allow" ) == 0 ) {
			i = LDAP_OPT_X_TLS_ALLOW ;

		} else if ( strcasecmp( arg, "try" ) == 0 ) {
			i = LDAP_OPT_X_TLS_TRY ;

		} else if ( ( strcasecmp( arg, "hard" ) == 0 ) ||
			( strcasecmp( arg, "on" ) == 0 ) ||
			( strcasecmp( arg, "yes" ) == 0) ||
			( strcasecmp( arg, "true" ) == 0 ) )
		{
			i = LDAP_OPT_X_TLS_HARD ;
		}

		if (i >= 0) {
			return ldap_pvt_tls_set_option( ld, option, &i );
		}
		return -1;
	case LDAP_OPT_X_TLS_PROTOCOL_MIN: {
		char *next;
		long l;
		l = strtol( arg, &next, 10 );
		if ( l < 0 || l > 0xff || next == arg ||
			( *next != '\0' && *next != '.' ) )
			return -1;
		i = l << 8;
		if (*next == '.') {
			arg = next + 1;
			l = strtol( arg, &next, 10 );
			if ( l < 0 || l > 0xff || next == arg || *next != '\0' )
				return -1;
			i += l;
		}
		return ldap_pvt_tls_set_option( ld, option, &i );
		}
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		i = -1;
		if ( strcasecmp( arg, "none" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_NONE ;
		} else if ( strcasecmp( arg, "peer" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_PEER ;
		} else if ( strcasecmp( arg, "all" ) == 0 ) {
			i = LDAP_OPT_X_TLS_CRL_ALL ;
		}
		if (i >= 0) {
			return ldap_pvt_tls_set_option( ld, option, &i );
		}
		return -1;
#endif
	}
	return -1;
}

int
ldap_pvt_tls_get_option( LDAP *ld, int option, void *arg )
{
	struct ldapoptions *lo;

	if( ld != NULL ) {
		assert( LDAP_VALID( ld ) );

		if( !LDAP_VALID( ld ) ) {
			return LDAP_OPT_ERROR;
		}

		lo = &ld->ld_options;

	} else {
		/* Get pointer to global option structure */
		lo = LDAP_INT_GLOBAL_OPT();   
		if ( lo == NULL ) {
			return LDAP_NO_MEMORY;
		}
	}

	switch( option ) {
	case LDAP_OPT_X_TLS:
		*(int *)arg = lo->ldo_tls_mode;
		break;
	case LDAP_OPT_X_TLS_CTX:
		*(void **)arg = lo->ldo_tls_ctx;
		if ( lo->ldo_tls_ctx ) {
			tls_ctx_ref( lo->ldo_tls_ctx );
		}
		break;
	case LDAP_OPT_X_TLS_CACERTFILE:
		*(char **)arg = lo->ldo_tls_cacertfile ?
			LDAP_STRDUP( lo->ldo_tls_cacertfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CACERTDIR:
		*(char **)arg = lo->ldo_tls_cacertdir ?
			LDAP_STRDUP( lo->ldo_tls_cacertdir ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CERTFILE:
		*(char **)arg = lo->ldo_tls_certfile ?
			LDAP_STRDUP( lo->ldo_tls_certfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_KEYFILE:
		*(char **)arg = lo->ldo_tls_keyfile ?
			LDAP_STRDUP( lo->ldo_tls_keyfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_DHFILE:
		*(char **)arg = lo->ldo_tls_dhfile ?
			LDAP_STRDUP( lo->ldo_tls_dhfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		*(char **)arg = lo->ldo_tls_crlfile ?
			LDAP_STRDUP( lo->ldo_tls_crlfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
		*(int *)arg = lo->ldo_tls_require_cert;
		break;
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		*(int *)arg = lo->ldo_tls_crlcheck;
		break;
#endif
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
		*(char **)arg = lo->ldo_tls_ciphersuite ?
			LDAP_STRDUP( lo->ldo_tls_ciphersuite ) : NULL;
		break;
	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
		*(int *)arg = lo->ldo_tls_protocol_min;
		break;
	case LDAP_OPT_X_TLS_RANDOM_FILE:
		*(char **)arg = lo->ldo_tls_randfile ?
			LDAP_STRDUP( lo->ldo_tls_randfile ) : NULL;
		break;
	case LDAP_OPT_X_TLS_SSL_CTX: {
		void *retval = 0;
		if ( ld != NULL ) {
			LDAPConn *conn = ld->ld_defconn;
			if ( conn != NULL ) {
				Sockbuf *sb = conn->lconn_sb;
				retval = ldap_pvt_tls_sb_ctx( sb );
			}
		}
		*(void **)arg = retval;
		break;
	}
	case LDAP_OPT_X_TLS_CONNECT_CB:
		*(LDAP_TLS_CONNECT_CB **)arg = lo->ldo_tls_connect_cb;
		break;
	case LDAP_OPT_X_TLS_CONNECT_ARG:
		*(void **)arg = lo->ldo_tls_connect_arg;
		break;
	default:
		return -1;
	}
	return 0;
}

int
ldap_pvt_tls_set_option( LDAP *ld, int option, void *arg )
{
	struct ldapoptions *lo;

	if( ld != NULL ) {
		assert( LDAP_VALID( ld ) );

		if( !LDAP_VALID( ld ) ) {
			return LDAP_OPT_ERROR;
		}

		lo = &ld->ld_options;

	} else {
		/* Get pointer to global option structure */
		lo = LDAP_INT_GLOBAL_OPT();   
		if ( lo == NULL ) {
			return LDAP_NO_MEMORY;
		}
	}

	switch( option ) {
	case LDAP_OPT_X_TLS:
		if ( !arg ) return -1;

		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_NEVER:
		case LDAP_OPT_X_TLS_DEMAND:
		case LDAP_OPT_X_TLS_ALLOW:
		case LDAP_OPT_X_TLS_TRY:
		case LDAP_OPT_X_TLS_HARD:
			if (lo != NULL) {
				lo->ldo_tls_mode = *(int *)arg;
			}

			return 0;
		}
		return -1;

	case LDAP_OPT_X_TLS_CTX:
		if ( lo->ldo_tls_ctx )
			ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = arg;
		tls_ctx_ref( lo->ldo_tls_ctx );
		return 0;
	case LDAP_OPT_X_TLS_CONNECT_CB:
		lo->ldo_tls_connect_cb = (LDAP_TLS_CONNECT_CB *)arg;
		return 0;
	case LDAP_OPT_X_TLS_CONNECT_ARG:
		lo->ldo_tls_connect_arg = arg;
		return 0;
	case LDAP_OPT_X_TLS_CACERTFILE:
		if ( lo->ldo_tls_cacertfile ) LDAP_FREE( lo->ldo_tls_cacertfile );
		lo->ldo_tls_cacertfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CACERTDIR:
		if ( lo->ldo_tls_cacertdir ) LDAP_FREE( lo->ldo_tls_cacertdir );
		lo->ldo_tls_cacertdir = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CERTFILE:
		if ( lo->ldo_tls_certfile ) LDAP_FREE( lo->ldo_tls_certfile );
		lo->ldo_tls_certfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_KEYFILE:
		if ( lo->ldo_tls_keyfile ) LDAP_FREE( lo->ldo_tls_keyfile );
		lo->ldo_tls_keyfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_DHFILE:
		if ( lo->ldo_tls_dhfile ) LDAP_FREE( lo->ldo_tls_dhfile );
		lo->ldo_tls_dhfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_CRLFILE:	/* GnuTLS only */
		if ( lo->ldo_tls_crlfile ) LDAP_FREE( lo->ldo_tls_crlfile );
		lo->ldo_tls_crlfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;
	case LDAP_OPT_X_TLS_REQUIRE_CERT:
		if ( !arg ) return -1;
		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_NEVER:
		case LDAP_OPT_X_TLS_DEMAND:
		case LDAP_OPT_X_TLS_ALLOW:
		case LDAP_OPT_X_TLS_TRY:
		case LDAP_OPT_X_TLS_HARD:
			lo->ldo_tls_require_cert = * (int *) arg;
			return 0;
		}
		return -1;
#ifdef HAVE_OPENSSL_CRL
	case LDAP_OPT_X_TLS_CRLCHECK:	/* OpenSSL only */
		if ( !arg ) return -1;
		switch( *(int *) arg ) {
		case LDAP_OPT_X_TLS_CRL_NONE:
		case LDAP_OPT_X_TLS_CRL_PEER:
		case LDAP_OPT_X_TLS_CRL_ALL:
			lo->ldo_tls_crlcheck = * (int *) arg;
			return 0;
		}
		return -1;
#endif
	case LDAP_OPT_X_TLS_CIPHER_SUITE:
		if ( lo->ldo_tls_ciphersuite ) LDAP_FREE( lo->ldo_tls_ciphersuite );
		lo->ldo_tls_ciphersuite = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		return 0;

	case LDAP_OPT_X_TLS_PROTOCOL_MIN:
		if ( !arg ) return -1;
		lo->ldo_tls_protocol_min = *(int *)arg;
		return 0;
	case LDAP_OPT_X_TLS_RANDOM_FILE:
		if ( ld != NULL )
			return -1;
		if ( lo->ldo_tls_randfile ) LDAP_FREE (lo->ldo_tls_randfile );
		lo->ldo_tls_randfile = arg ? LDAP_STRDUP( (char *) arg ) : NULL;
		break;
	case LDAP_OPT_X_TLS_NEWCTX:
		if ( !arg ) return -1;
		if ( lo->ldo_tls_ctx )
			ldap_pvt_tls_ctx_free( lo->ldo_tls_ctx );
		lo->ldo_tls_ctx = NULL;
		return ldap_int_tls_init_ctx( lo, *(int *)arg );
	default:
		return -1;
	}
	return 0;
}

int
ldap_int_tls_start ( LDAP *ld, LDAPConn *conn, LDAPURLDesc *srv )
{
	Sockbuf *sb = conn->lconn_sb;
	char *host;
	void *ssl;

	if( srv ) {
		host = srv->lud_host;
	} else {
 		host = conn->lconn_server->lud_host;
	}

	/* avoid NULL host */
	if( host == NULL ) {
		host = "localhost";
	}

	(void) tls_init( tls_imp );

	/*
	 * Fortunately, the lib uses blocking io...
	 */
	if ( ldap_int_tls_connect( ld, conn ) < 0 ) {
		ld->ld_errno = LDAP_CONNECT_ERROR;
		return (ld->ld_errno);
	}

	ssl = ldap_pvt_tls_sb_ctx( sb );
	assert( ssl != NULL );

	/* 
	 * compare host with name(s) in certificate
	 */
	if (ld->ld_options.ldo_tls_require_cert != LDAP_OPT_X_TLS_NEVER) {
		ld->ld_errno = ldap_pvt_tls_check_hostname( ld, ssl, host );
		if (ld->ld_errno != LDAP_SUCCESS) {
			return ld->ld_errno;
		}
	}

	return LDAP_SUCCESS;
}

void *
ldap_pvt_tls_sb_ctx( Sockbuf *sb )
{
	void			*p = NULL;
	
	ber_sockbuf_ctrl( sb, LBER_SB_OPT_GET_SSL, (void *)&p );
	return p;
}

int
ldap_pvt_tls_get_strength( void *s )
{
	tls_session *session = s;

	return tls_imp->ti_session_strength( session );
}

int
ldap_pvt_tls_get_my_dn( void *s, struct berval *dn, LDAPDN_rewrite_dummy *func, unsigned flags )
{
	tls_session *session = s;
	struct berval der_dn;
	int rc;

	rc = tls_imp->ti_session_my_dn( session, &der_dn );
	if ( rc == LDAP_SUCCESS )
		rc = ldap_X509dn2bv(&der_dn, dn, (LDAPDN_rewrite_func *)func, flags );
	return rc;
}
#endif /* HAVE_TLS */

int
ldap_start_tls( LDAP *ld,
	LDAPControl **serverctrls,
	LDAPControl **clientctrls,
	int *msgidp )
{
	return ldap_extended_operation( ld, LDAP_EXOP_START_TLS,
		NULL, serverctrls, clientctrls, msgidp );
}

int
ldap_install_tls( LDAP *ld )
{
#ifndef HAVE_TLS
	return LDAP_NOT_SUPPORTED;
#else
	if ( ldap_tls_inplace( ld ) ) {
		return LDAP_LOCAL_ERROR;
	}

	return ldap_int_tls_start( ld, ld->ld_defconn, NULL );
#endif
}

int
ldap_start_tls_s ( LDAP *ld,
	LDAPControl **serverctrls,
	LDAPControl **clientctrls )
{
#ifndef HAVE_TLS
	return LDAP_NOT_SUPPORTED;
#else
	int rc;
	char *rspoid = NULL;
	struct berval *rspdata = NULL;

	/* XXYYZ: this initiates operation only on default connection! */

	if ( ldap_tls_inplace( ld ) ) {
		return LDAP_LOCAL_ERROR;
	}

	rc = ldap_extended_operation_s( ld, LDAP_EXOP_START_TLS,
		NULL, serverctrls, clientctrls, &rspoid, &rspdata );

	if ( rspoid != NULL ) {
		LDAP_FREE(rspoid);
	}

	if ( rspdata != NULL ) {
		ber_bvfree( rspdata );
	}

	if ( rc == LDAP_SUCCESS ) {
		rc = ldap_int_tls_start( ld, ld->ld_defconn, NULL );
	}

	return rc;
#endif
}

/* These tags probably all belong in lber.h, but they're
 * not normally encountered when processing LDAP, so maybe
 * they belong somewhere else instead.
 */

#define LBER_TAG_OID		((ber_tag_t) 0x06UL)

/* Tags for string types used in a DirectoryString.
 *
 * Note that IA5string is not one of the defined choices for
 * DirectoryString in X.520, but it gets used for email AVAs.
 */
#define	LBER_TAG_UTF8		((ber_tag_t) 0x0cUL)
#define	LBER_TAG_PRINTABLE	((ber_tag_t) 0x13UL)
#define	LBER_TAG_TELETEX	((ber_tag_t) 0x14UL)
#define	LBER_TAG_IA5		((ber_tag_t) 0x16UL)
#define	LBER_TAG_UNIVERSAL	((ber_tag_t) 0x1cUL)
#define	LBER_TAG_BMP		((ber_tag_t) 0x1eUL)

static oid_name *
find_oid( struct berval *oid )
{
	int i;

	for ( i=0; !BER_BVISNULL( &oids[i].oid ); i++ ) {
		if ( oids[i].oid.bv_len != oid->bv_len ) continue;
		if ( !strcmp( oids[i].oid.bv_val, oid->bv_val ))
			return &oids[i];
	}
	return NULL;
}

/* Convert a structured DN from an X.509 certificate into an LDAPV3 DN.
 * x509_name must be raw DER. If func is non-NULL, the
 * constructed DN will use numeric OIDs to identify attributeTypes,
 * and the func() will be invoked to rewrite the DN with the given
 * flags.
 *
 * Otherwise the DN will use shortNames from a hardcoded table.
 */
int
ldap_X509dn2bv( void *x509_name, struct berval *bv, LDAPDN_rewrite_func *func,
	unsigned flags )
{
	LDAPDN	newDN;
	LDAPRDN	newRDN;
	LDAPAVA *newAVA, *baseAVA;
	BerElementBuffer berbuf;
	BerElement *ber = (BerElement *)&berbuf;
	char oids[8192], *oidptr = oids, *oidbuf = NULL;
	void *ptrs[2048];
	char *dn_end, *rdn_end;
	int i, navas, nrdns, rc = LDAP_SUCCESS;
	size_t dnsize, oidrem = sizeof(oids), oidsize = 0;
	int csize;
	ber_tag_t tag;
	ber_len_t len;
	oid_name *oidname;

	struct berval	Oid, Val, oid2, *in = x509_name;

	assert( bv != NULL );

	bv->bv_len = 0;
	bv->bv_val = NULL;

	navas = 0;
	nrdns = 0;

	/* A DN is a SEQUENCE of RDNs. An RDN is a SET of AVAs.
	 * An AVA is a SEQUENCE of attr and value.
	 * Count the number of AVAs and RDNs
	 */
	ber_init2( ber, in, LBER_USE_DER );
	tag = ber_peek_tag( ber, &len );
	if ( tag != LBER_SEQUENCE )
		return LDAP_DECODING_ERROR;

	for ( tag = ber_first_element( ber, &len, &dn_end );
		tag == LBER_SET;
		tag = ber_next_element( ber, &len, dn_end )) {
		nrdns++;
		for ( tag = ber_first_element( ber, &len, &rdn_end );
			tag == LBER_SEQUENCE;
			tag = ber_next_element( ber, &len, rdn_end )) {
			tag = ber_skip_tag( ber, &len );
			ber_skip_data( ber, len );
			navas++;
		}
	}

	/* Allocate the DN/RDN/AVA stuff as a single block */    
	dnsize = sizeof(LDAPRDN) * (nrdns+1);
	dnsize += sizeof(LDAPAVA *) * (navas+nrdns);
	dnsize += sizeof(LDAPAVA) * navas;
	if (dnsize > sizeof(ptrs)) {
		newDN = (LDAPDN)LDAP_MALLOC( dnsize );
		if ( newDN == NULL )
			return LDAP_NO_MEMORY;
	} else {
		newDN = (LDAPDN)(char *)ptrs;
	}
	
	newDN[nrdns] = NULL;
	newRDN = (LDAPRDN)(newDN + nrdns+1);
	newAVA = (LDAPAVA *)(newRDN + navas + nrdns);
	baseAVA = newAVA;

	/* Rewind and start extracting */
	ber_rewind( ber );

	tag = ber_first_element( ber, &len, &dn_end );
	for ( i = nrdns - 1; i >= 0; i-- ) {
		newDN[i] = newRDN;

		for ( tag = ber_first_element( ber, &len, &rdn_end );
			tag == LBER_SEQUENCE;
			tag = ber_next_element( ber, &len, rdn_end )) {

			*newRDN++ = newAVA;
			tag = ber_skip_tag( ber, &len );
			tag = ber_get_stringbv( ber, &Oid, LBER_BV_NOTERM );
			if ( tag != LBER_TAG_OID ) {
				rc = LDAP_DECODING_ERROR;
				goto nomem;
			}

			oid2.bv_val = oidptr;
			oid2.bv_len = oidrem;
			if ( ber_decode_oid( &Oid, &oid2 ) < 0 ) {
				rc = LDAP_DECODING_ERROR;
				goto nomem;
			}
			oidname = find_oid( &oid2 );
			if ( !oidname ) {
				newAVA->la_attr = oid2;
				oidptr += oid2.bv_len + 1;
				oidrem -= oid2.bv_len + 1;

				/* Running out of OID buffer space? */
				if (oidrem < 128) {
					if ( oidsize == 0 ) {
						oidsize = sizeof(oids) * 2;
						oidrem = oidsize;
						oidbuf = LDAP_MALLOC( oidsize );
						if ( oidbuf == NULL ) goto nomem;
						oidptr = oidbuf;
					} else {
						char *old = oidbuf;
						oidbuf = LDAP_REALLOC( oidbuf, oidsize*2 );
						if ( oidbuf == NULL ) goto nomem;
						/* Buffer moved! Fix AVA pointers */
						if ( old != oidbuf ) {
							LDAPAVA *a;
							long dif = oidbuf - old;

							for (a=baseAVA; a<=newAVA; a++){
								if (a->la_attr.bv_val >= old &&
									a->la_attr.bv_val <= (old + oidsize))
									a->la_attr.bv_val += dif;
							}
						}
						oidptr = oidbuf + oidsize - oidrem;
						oidrem += oidsize;
						oidsize *= 2;
					}
				}
			} else {
				if ( func ) {
					newAVA->la_attr = oidname->oid;
				} else {
					newAVA->la_attr = oidname->name;
				}
			}
			tag = ber_get_stringbv( ber, &Val, LBER_BV_NOTERM );
			switch(tag) {
			case LBER_TAG_UNIVERSAL:
				/* This uses 32-bit ISO 10646-1 */
				csize = 4; goto to_utf8;
			case LBER_TAG_BMP:
				/* This uses 16-bit ISO 10646-1 */
				csize = 2; goto to_utf8;
			case LBER_TAG_TELETEX:
				/* This uses 8-bit, assume ISO 8859-1 */
				csize = 1;
to_utf8:		rc = ldap_ucs_to_utf8s( &Val, csize, &newAVA->la_value );
				newAVA->la_flags |= LDAP_AVA_FREE_VALUE;
				if (rc != LDAP_SUCCESS) goto nomem;
				newAVA->la_flags = LDAP_AVA_NONPRINTABLE;
				break;
			case LBER_TAG_UTF8:
				newAVA->la_flags = LDAP_AVA_NONPRINTABLE;
				/* This is already in UTF-8 encoding */
			case LBER_TAG_IA5:
			case LBER_TAG_PRINTABLE:
				/* These are always 7-bit strings */
				newAVA->la_value = Val;
			default:
				;
			}
			newAVA->la_private = NULL;
			newAVA->la_flags = LDAP_AVA_STRING;
			newAVA++;
		}
		*newRDN++ = NULL;
		tag = ber_next_element( ber, &len, dn_end );
	}
		
	if ( func ) {
		rc = func( newDN, flags, NULL );
		if ( rc != LDAP_SUCCESS )
			goto nomem;
	}

	rc = ldap_dn2bv_x( newDN, bv, LDAP_DN_FORMAT_LDAPV3, NULL );

nomem:
	for (;baseAVA < newAVA; baseAVA++) {
		if (baseAVA->la_flags & LDAP_AVA_FREE_ATTR)
			LDAP_FREE( baseAVA->la_attr.bv_val );
		if (baseAVA->la_flags & LDAP_AVA_FREE_VALUE)
			LDAP_FREE( baseAVA->la_value.bv_val );
	}

	if ( oidsize != 0 )
		LDAP_FREE( oidbuf );
	if ( newDN != (LDAPDN)(char *) ptrs )
		LDAP_FREE( newDN );
	return rc;
}

