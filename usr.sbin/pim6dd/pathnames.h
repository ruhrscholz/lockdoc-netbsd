/*	$NetBSD: pathnames.h,v 1.5 2000/12/04 07:05:48 itojun Exp $	*/
/*	$KAME: pathnames.h,v 1.3 2000/12/04 06:33:10 itojun Exp $	*/

/*
 *  Copyright (c) 1998 by the University of Southern California.
 *  All rights reserved.
 *
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation in source and binary forms for lawful
 *  purposes and without fee is hereby granted, provided
 *  that the above copyright notice appear in all copies and that both
 *  the copyright notice and this permission notice appear in supporting
 *  documentation, and that any documentation, advertising materials,
 *  and other materials related to such distribution and use acknowledge
 *  that the software was developed by the University of Southern
 *  California and/or Information Sciences Institute.
 *  The name of the University of Southern California may not
 *  be used to endorse or promote products derived from this software
 *  without specific prior written permission.
 *
 *  THE UNIVERSITY OF SOUTHERN CALIFORNIA DOES NOT MAKE ANY REPRESENTATIONS
 *  ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  THIS SOFTWARE IS
 *  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TITLE, AND 
 *  NON-INFRINGEMENT.
 *
 *  IN NO EVENT SHALL USC, OR ANY OTHER CONTRIBUTOR BE LIABLE FOR ANY
 *  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, WHETHER IN CONTRACT,
 *  TORT, OR OTHER FORM OF ACTION, ARISING OUT OF OR IN CONNECTION WITH,
 *  THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  Other copyrights might apply to parts of this software and are so
 *  noted when applicable.
 */
/*
 *  Questions concerning this software should be directed to 
 *  Pavlin Ivanov Radoslavov (pavlin@catarina.usc.edu)
 */
/*
 * Part of this program has been derived from mrouted.
 * The mrouted program is covered by the license in the accompanying file
 * named "LICENSE.mrouted".
 *
 * The mrouted program is COPYRIGHT 1989 by The Board of Trustees of
 * Leland Stanford Junior University.
 *
 */


#define _PATH_PIM6D_CONF		"/usr/local/v6/etc/pim6dd.conf"

#if (defined(BSD) && (BSD >= 199103))
#define _PATH_PIM6D_PID		"/var/run/pim6dd.pid"
#define _PATH_PIM6D_GENID	"/var/run/pim6dd.genid"
#define _PATH_PIM6D_DUMP	"/var/run/pim6dd.dump"
#define _PATH_PIM6D_CACHE	"/var/run/pim6dd.cache"
#else
#define _PATH_PIM6D_PID		"/etc/pim6dd.pid"
#define _PATH_PIM6D_GENID	"/etc/pim6dd.genid"
#define _PATH_PIM6D_DUMP	"/etc/pim6dd.dump"
#define _PATH_PIM6D_CACHE	"/etc/pim6dd.cache"
#endif
