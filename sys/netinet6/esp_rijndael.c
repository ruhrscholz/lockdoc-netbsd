/*	$NetBSD: esp_rijndael.c,v 1.1 2000/10/02 17:21:26 itojun Exp $	*/
/*	$KAME: esp_rijndael.c,v 1.1 2000/09/20 18:15:22 itojun Exp $	*/

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

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include "opt_inet.h"
#include "opt_inet6.h"
#endif
#ifdef __NetBSD__
#include "opt_inet.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet6/ipsec.h>
#include <netinet6/esp.h>
#include <netinet6/esp_rijndael.h>

#include <crypto/rijndael/rijndael.h>

#include <net/net_osdep.h>

/* as rijndael uses assymetric scheduled keys, we need to do it twice. */
int
esp_rijndael_schedlen(algo)
	const struct esp_algorithm *algo;
{

	return sizeof(keyInstance) * 2;
}

int
esp_rijndael_schedule(algo, sav)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
{
	keyInstance *k;
	char keymat[256 / 4 + 1];
	u_char *p, *ep;
	char *q, *eq;

	/* rijndael_makeKey wants hex string for the key */
	if (_KEYLEN(sav->key_enc) * 2 > sizeof(keymat) - 1)
		return -1;
	p = _KEYBUF(sav->key_enc); 
	ep = p + _KEYLEN(sav->key_enc);
	q = keymat;
	eq = &keymat[sizeof(keymat) - 1];
	while (p < ep && q < eq) {
		sprintf(q, "%02x", *p);
		q += 2;
		p++;
	}
	*eq = '\0';

	k = (keyInstance *)sav->sched;
	if (rijndael_makeKey(&k[0], DIR_DECRYPT, _KEYLEN(sav->key_enc) * 8,
	    keymat) < 0)
		return -1;
	if (rijndael_makeKey(&k[1], DIR_ENCRYPT, _KEYLEN(sav->key_enc) * 8,
	    keymat) < 0)
		return -1;
	return 0;
}

int
esp_rijndael_blockdecrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (rijndael_blockDecrypt(&c, &p[0], s, algo->padbound * 8, d) < 0)
		return -1;
	return 0;
}

int
esp_rijndael_blockencrypt(algo, sav, s, d)
	const struct esp_algorithm *algo;
	struct secasvar *sav;
	u_int8_t *s;
	u_int8_t *d;
{
	cipherInstance c;
	keyInstance *p;

	/* does not take advantage of CBC mode support */
	bzero(&c, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, NULL) < 0)
		return -1;
	p = (keyInstance *)sav->sched;
	if (rijndael_blockEncrypt(&c, &p[1], s, algo->padbound * 8, d) < 0)
		return -1;
	return 0;
}
