/*	$NetBSD: time.h,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

/* Generic time.h */
/* OpenLDAP: pkg/ldap/contrib/ldapc++/src/ac/time.h,v 1.7.2.5 2009/01/22 00:00:44 kurt Exp */
/*
 * Copyright 1998-2009 The OpenLDAP Foundation, Redwood City, California, USA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted only
 * as authorized by the OpenLDAP Public License.  A copy of this
 * license is available at http://www.OpenLDAP.org/license.html or
 * in file LICENSE in the top-level directory of the distribution.
 */

#ifndef _AC_TIME_H
#define _AC_TIME_H

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#elif defined HAVE_SYS_TIME_H
# include <sys/time.h>
# ifdef HAVE_SYS_TIMEB_H
#  include <sys/timeb.h>
# endif
#else
# include <time.h>
#endif

#endif /* _AC_TIME_H */
