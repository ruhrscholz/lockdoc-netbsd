/*	$NetBSD: rf_callback.c,v 1.13 2004/02/29 04:03:50 oster Exp $	*/
/*
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Jim Zelenka
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/*****************************************************************************
 *
 * callback.c -- code to manipulate callback descriptor
 *
 ****************************************************************************/


#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: rf_callback.c,v 1.13 2004/02/29 04:03:50 oster Exp $");

#include <dev/raidframe/raidframevar.h>
#include <sys/pool.h>

#include "rf_archs.h"
#include "rf_threadstuff.h"
#include "rf_callback.h"
#include "rf_debugMem.h"
#include "rf_general.h"
#include "rf_shutdown.h"

static struct pool rf_callback_pool;
#define RF_MAX_FREE_CALLBACK 64
#define RF_CALLBACK_INC       4
#define RF_CALLBACK_INITIAL  32

static void rf_ShutdownCallback(void *);
static void 
rf_ShutdownCallback(void *ignored)
{
	pool_destroy(&rf_callback_pool);
}

int 
rf_ConfigureCallback(listp)
	RF_ShutdownList_t **listp;
{

	pool_init(&rf_callback_pool, sizeof(RF_CallbackDesc_t), 0, 0, 0,
		  "rf_callbackpl", NULL);
	pool_sethiwat(&rf_callback_pool, RF_MAX_FREE_CALLBACK);
	pool_prime(&rf_callback_pool, RF_CALLBACK_INITIAL);

	rf_ShutdownCreate(listp, rf_ShutdownCallback, NULL);

	return (0);
}

RF_CallbackDesc_t *
rf_AllocCallbackDesc()
{
	RF_CallbackDesc_t *p;

	p = pool_get(&rf_callback_pool, PR_WAITOK);
	return (p);
}

void 
rf_FreeCallbackDesc(p)
	RF_CallbackDesc_t *p;
{
	pool_put(&rf_callback_pool, p);
}
