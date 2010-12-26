/* $NetBSD: t_memcpy.c,v 1.1 2010/12/26 13:35:54 pgoyette Exp $ */

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by 
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

#include <atf-c.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <md5.h>

#include <sys/types.h>

#define	ALIGNMENTS 16
#define	LENGTHS	    4
#define BLOCKTYPES 4

MD5_CTX mc[1];

typedef	unsigned char testBlock_t[ALIGNMENTS * LENGTHS];

testBlock_t bss1, bss2;

unsigned char *start[BLOCKTYPES] = {
		bss1, bss2
};

char result[100];
const char goodResult[] = "7b405d24bc03195474c70ddae9e1f8fb";

static void
runTest(unsigned char *b1, unsigned char *b2)
{
	int	i, j, k, m;
	size_t	n;

	for (i = 0; i < ALIGNMENTS; ++i) {
		for (j = 0; j < ALIGNMENTS; ++j) {
			k = sizeof(testBlock_t) - (i > j ? i : j);
			for (m = 0; m < k; ++m) {
				for (n = 0; n < sizeof(testBlock_t); ++n) {
					b1[n] = (unsigned char)random();
					b2[n] = (unsigned char)random();
				}
				memcpy(b1 + i, b2 + j, m);
				MD5Update(mc, b1, sizeof(testBlock_t));
				MD5Update(mc, b2, sizeof(testBlock_t));
			}
		}
	}
}

ATF_TC(check_memcpy);

ATF_TC_HEAD(check_memcpy, tc)
{
  
	atf_tc_set_md_var(tc, "descr", "Test memcpy results");
}
 
ATF_TC_BODY(check_memcpy, tc)
{
	int i, j;
	testBlock_t auto1, auto2;

	start[2] = auto1;
	start[3] = auto2;

	srandom(0L);
	MD5Init(mc);
	for (i = 0; i < BLOCKTYPES; ++i)
		for (j = 0; j < BLOCKTYPES; ++j)
			if (i != j)
				runTest(start[i], start[j]);
	MD5End(mc, result);
	ATF_REQUIRE_EQ(strcmp(result, goodResult), 0);
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, check_memcpy);

	return atf_no_error();
}
