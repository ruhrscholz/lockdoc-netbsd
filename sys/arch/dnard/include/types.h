/* $NetBSD: types.h,v 1.3 2002/02/05 18:26:08 thorpej Exp $ */

#ifndef _ARM32_TYPES_H_
#define _ARM32_TYPES_H_

#include <arm/arm32/types.h>

/* We need to invoke FIQs indirectly. */ 
#define	__ARM_FIQ_INDIRECT

#define __HAVE_DEVICE_REGISTER
#define __HAVE_NWSCONS

#endif
