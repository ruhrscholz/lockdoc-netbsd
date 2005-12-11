/*	$NetBSD: key_var.h,v 1.16 2005/12/11 00:02:28 elad Exp $	*/
/*	$KAME: key_var.h,v 1.12 2001/11/06 03:48:29 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NETKEY_KEY_VAR_H_
#define _NETKEY_KEY_VAR_H_

#ifdef __NetBSD__
#if defined(_KERNEL_OPT)
#include "opt_inet.h"
#endif
#endif

/* sysctl */
#define KEYCTL_DEBUG_LEVEL		1
#define KEYCTL_SPI_TRY			2
#define KEYCTL_SPI_MIN_VALUE		3
#define KEYCTL_SPI_MAX_VALUE		4
/*#define KEYCTL_RANDOM_INT		5*/
#define KEYCTL_LARVAL_LIFETIME		6
#define KEYCTL_BLOCKACQ_COUNT		7
#define KEYCTL_BLOCKACQ_LIFETIME	8
#define KEYCTL_ESP_KEYMIN		9
#define KEYCTL_ESP_AUTH			10
#define KEYCTL_AH_KEYMIN		11
/* 12: reserved */
#define KEYCTL_DUMPSA			13
#define KEYCTL_DUMPSP			14
#define KEYCTL_MAXID			15

#define KEYCTL_NAMES { \
	{ 0, 0 }, \
	{ "debug", CTLTYPE_INT }, \
	{ "spi_try", CTLTYPE_INT }, \
	{ "spi_min_value", CTLTYPE_INT }, \
	{ "spi_max_value", CTLTYPE_INT }, \
	{ 0, 0 /* "random_int", CTLTYPE_INT */ }, \
	{ "larval_lifetime", CTLTYPE_INT }, \
	{ "blockacq_count", CTLTYPE_INT }, \
	{ "blockacq_lifetime", CTLTYPE_INT }, \
	{ "esp_keymin", CTLTYPE_INT }, \
	{ "esp_auth", CTLTYPE_INT }, \
	{ "ah_keymin", CTLTYPE_INT }, \
	{ 0, 0, }, \
	{ 0, 0, }, \
	{ 0, 0, }, \
}

#ifdef _KERNEL
#define _ARRAYLEN(p)	(sizeof(p)/sizeof(p[0]))
#define _KEYLEN(key)	((u_int)((key)->sadb_key_bits >> 3))
#define _KEYBITS(key)	((u_int)((key)->sadb_key_bits))
#define _KEYBUF(key)	((u_int8_t *)(key) + sizeof(struct sadb_key))
#endif /*_KERNEL*/

#endif /* !_NETKEY_KEY_VAR_H_ */
