/*	$NetBSD: atomic_init_testset.c,v 1.7 2009/12/14 00:39:00 matt Exp $	*/

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

/*
 * libc glue for atomic operations where the hardware does not provide
 * compare-and-swap.  It's assumed that this will only be used on 32-bit
 * platforms.
 *
 * This should be compiled with '-fno-reorder-blocks -fomit-frame-pointer'
 * if using gcc.
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD: atomic_init_testset.c,v 1.7 2009/12/14 00:39:00 matt Exp $");

#include "atomic_op_namespace.h"

#include <sys/types.h>
#include <sys/atomic.h>
#include <sys/lock.h>
#include <sys/ras.h>
#include <sys/sysctl.h>

#include <string.h>

#define	I2	__SIMPLELOCK_UNLOCKED, __SIMPLELOCK_UNLOCKED,
#define	I16	I2 I2 I2 I2 I2 I2 I2 I2
#define	I128	I16 I16 I16 I16 I16 I16 I16 I16

static __cpu_simple_lock_t atomic_locks[128] = { I128 };

#ifdef	__HAVE_ASM_ATOMIC_CAS_UP
extern uint32_t _atomic_cas_up(volatile uint32_t *, uint32_t, uint32_t);
#else
static uint32_t _atomic_cas_up(volatile uint32_t *, uint32_t, uint32_t);
#endif

static uint32_t (*_atomic_cas_fn)(volatile uint32_t *, uint32_t, uint32_t) =
    _atomic_cas_up;

void	__libc_atomic_init(void) __attribute__ ((visibility("hidden")));

RAS_DECL(_atomic_cas);

#ifndef	__HAVE_ASM_ATOMIC_CAS_UP
static uint32_t
_atomic_cas_up(volatile uint32_t *ptr, uint32_t old, uint32_t new)
{
	uint32_t ret;

	RAS_START(_atomic_cas);
	ret = *ptr;
	if (__predict_false(ret != old)) {
		return ret;
	}
	*ptr = new;
	RAS_END(_atomic_cas);

	return ret;
}
#endif

static uint32_t
_atomic_cas_mp(volatile uint32_t *ptr, uint32_t old, uint32_t new)
{
	__cpu_simple_lock_t *lock;
	uint32_t ret;

	lock = &atomic_locks[((uintptr_t)ptr >> 3) & 127];
	__cpu_simple_lock(lock);
	ret = *ptr;
	if (__predict_true(ret == old)) {
		*ptr = new;
	}
	__cpu_simple_unlock(lock);

	return ret;
}

uint32_t
_atomic_cas_32(volatile uint32_t *ptr, uint32_t old, uint32_t new)
{

	return (*_atomic_cas_fn)(ptr, old, new);
}

void
__libc_atomic_init(void)
{
	int ncpu, mib[2];
	size_t len;

	_atomic_cas_fn = _atomic_cas_mp;

	mib[0] = CTL_HW;
	mib[1] = HW_NCPU; 
	len = sizeof(ncpu);
	if (sysctl(mib, 2, &ncpu, &len, NULL, 0) == -1)
		return;
	if (ncpu > 1)
		return;
	if (rasctl(RAS_ADDR(_atomic_cas), RAS_SIZE(_atomic_cas),
	    RAS_INSTALL) == 0) {
		_atomic_cas_fn = _atomic_cas_up;
		return;
	}
}

#undef atomic_cas_32
#undef atomic_cas_uint
#undef atomic_cas_ulong
#undef atomic_cas_ptr

atomic_op_alias(atomic_cas_32,_atomic_cas_32)
atomic_op_alias(atomic_cas_uint,_atomic_cas_32)
__strong_alias(_atomic_cas_uint,_atomic_cas_32)
atomic_op_alias(atomic_cas_ulong,_atomic_cas_32)
__strong_alias(_atomic_cas_ulong,_atomic_cas_32)
atomic_op_alias(atomic_cas_ptr,_atomic_cas_32)
__strong_alias(_atomic_cas_ptr,_atomic_cas_32)

atomic_op_alias(atomic_cas_32_ni,_atomic_cas_32)
__strong_alias(_atomic_cas_32_ni,_atomic_cas_32)
atomic_op_alias(atomic_cas_uint_ni,_atomic_cas_32)
__strong_alias(_atomic_cas_uint_ni,_atomic_cas_32)
atomic_op_alias(atomic_cas_ulong_ni,_atomic_cas_32)
__strong_alias(_atomic_cas_ulong_ni,_atomic_cas_32)
atomic_op_alias(atomic_cas_ptr_ni,_atomic_cas_32)
__strong_alias(_atomic_cas_ptr_ni,_atomic_cas_32)
