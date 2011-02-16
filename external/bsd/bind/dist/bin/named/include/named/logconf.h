/*	$NetBSD: logconf.h,v 1.2 2011/02/16 03:46:46 christos Exp $	*/

/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: logconf.h,v 1.17 2007-06-19 23:46:59 tbox Exp */

#ifndef NAMED_LOGCONF_H
#define NAMED_LOGCONF_H 1

/*! \file */

#include <isc/log.h>

isc_result_t
ns_log_configure(isc_logconfig_t *logconf, const cfg_obj_t *logstmt);
/*%<
 * Set up the logging configuration in '*logconf' according to
 * the named.conf data in 'logstmt'.
 */

#endif /* NAMED_LOGCONF_H */
