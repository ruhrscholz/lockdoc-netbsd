/* $NetBSD: tabs.c,v 1.1 2008/12/11 11:18:35 roy Exp $ */

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Roy Marples.
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

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 2008 \
The NetBSD Foundation, inc. All rights reserved.");
__RCSID("$NetBSD: tabs.c,v 1.1 2008/12/11 11:18:35 roy Exp $");
#endif /* not lint */

#include <sys/ioctl.h>
#include <sys/types.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <unistd.h>

#define NSTOPS 20

struct tabspec {
	const char *opt;
	const char *spec;
};
static const struct tabspec tabspecs[] = {
	{"a",	"1,10,16,36,72"},
	{"a2",	"1,10,16,40,72"},
	{"c",	"1,8,12,16,20,55"},
	{"c2",	"1,6,10,14,49"},
	{"c3",	"1,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,67"},
	{"f",	"1,7,11,15,19,23"},
	{"p",	"1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61"},
	{"s",	"1,10,55"},
	{"u",	"1,12,20,44"}
};
static const size_t ntabspecs = sizeof(tabspecs) / sizeof(tabspecs[0]);

static char tbuf[2048];

static void
usage(void)
{
	fprintf(stderr,
		"usage: tabs [-n|-a|-a2|-c|-c2|-c3|-f|-p|-s|-u] [+m[n]]"
		" [-T type]\n"
		"       tabs [-T type] [+[n]] n1[,n2,...]\n");
	exit(EXIT_FAILURE);
	/* NOTREACHED */
}

int
main(int argc, char **argv)
{
        char *term, *arg, *tbp, *token, *end, *tabs = NULL, *p;
	const char *cr, *ct, *st, *ML, *spec = NULL;
        int i, j, n, inc = 8, stops[NSTOPS], nstops, last, cols, margin = 0;
	struct winsize ws;

        term = getenv("TERM");
        for (i = 1; i < argc; i++) {
		if (argv[i][0] == '+') {
			arg = argv[i] + 1;
			if (arg[0] == 'm')
				arg++;
			if (arg[0] == '\0')
				margin = 10;
			else {
				errno = 0;
				margin = strtol(arg, &end, 10);
				if (errno != 0 || *end != '\0' || margin < 0)
					errx(EXIT_FAILURE,
					     "%s: invalid margin", arg);
			}
			continue;
		}
		if (argv[i][0] != '-') {
			tabs = argv[i];
			break;
		}
		arg = argv[i] + 1;
		if (arg[0] == '\0') 
			usage();
		if (arg[0] == '-' && arg[1] == '\0') {
			if (argv[i + 1] != '\0')
				tabs = argv[i + 1];
			break;
		}
		if (arg[0] == 'T' && arg[1] == '\0') {
			term = argv[++i];
			if (term == NULL)
				usage();
			continue;
		}
		if (isdigit((int)arg[0])) {
			if (arg[1] != '\0')
				errx(EXIT_FAILURE,
				     "%s: invalid increament", arg);
			inc = arg[0] - '0';
			continue;
		}
		for (j = 0; j < ntabspecs; j++) {
			if (arg[0] == tabspecs[j].opt[0] &&
			    arg[1] == tabspecs[j].opt[1])
			{
				spec = tabspecs[j].spec;
				break;
			}
		}
		if (j == ntabspecs)
			usage();
	}
	if (tabs == NULL && spec != NULL)
		tabs = strdup(spec);

	if (tabs != NULL)
		last = nstops = 0;
	else
		nstops = -1;
	p = tabs;
	while ((token = strsep(&p, ", ")) != NULL) {
		if (*token == '\0')
			continue;
		if (nstops >= NSTOPS)
			errx(EXIT_FAILURE,
			     "too many tab stops (max %d)", NSTOPS);
		errno = 0;
		n = strtol(token, &end, 10);
		if (errno != 0 || *end != '\0' || n <= 0)
			errx(EXIT_FAILURE, "%s: invalid tab stop", token);
		if (*token == '+') {
			if (nstops == 0)
				errx(EXIT_FAILURE,
				     "first tab stop may not be relative");
			n += last;
		}
		if (last > n)
			errx(EXIT_FAILURE, "tab stops may not go backwards");
		last = stops[nstops++] = n;
	}

	if (term == NULL)
		errx(EXIT_FAILURE, "no value for $TERM and -T not given");
	switch (tgetent(tbuf, term)) {
	case -1:
		err(EXIT_FAILURE, "termcap database");
	case 0:
		errx(EXIT_FAILURE, "%s: no termcap entry", term);
	}
	tbp = tbuf;
	cr = tgetstr("cr", &tbp);
	if (cr == NULL)
		cr = "\r";
	ct = tgetstr("ct", &tbp);
	if (ct == NULL)
		errx(EXIT_FAILURE, "terminal cannot clear tabs");
	st = tgetstr("st", &tbp);
	if (st == NULL)
		errx(EXIT_FAILURE, "terminal cannot set tabs");
	ML = tgetstr("ML", &tbp);

	/* Clear existing tabs */
	tputs(cr, 1, putchar);
	tputs(ct, 1, putchar);
	tputs(cr, 1, putchar);

	if (ML != NULL) {
		printf("%*s", margin, "");
		tputs(ML, 1, putchar);
	} else if (margin != 0)
		warnx("terminal cannot set left margin");

	if (nstops >= 0) {
		printf("%*s", stops[0] - 1, "");
		tputs(st, 1, putchar);
		for (i = 1; i < nstops; i++) {
			printf("%*s", stops[i] - stops[i - 1], "");
			tputs(st, 1, putchar);
		}
	} else if (inc > 0) {
		cols = 0;
		term = getenv("COLUMNS");
		if (term != NULL) {
			errno = 0;
			cols = strtol(term, &end, 10);
			if (errno != 0 || *end != '\0' || cols < 0)
				cols = 0;
		}
		if (cols == 0) {
			if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
				cols = ws.ws_col;
			else {
				cols = tgetnum("co");
				if (cols == 0) {
					cols = 80;
					warnx("terminal does not specify number"
					      "columns; defaulting to %d",
					      cols);
				}
			}
		}
		for (i = 0; i < cols / inc; i++) {
			printf("%*s", inc, "");
			tputs(st, 1, putchar);
		}
	}
	tputs(cr, 1, putchar);

	exit(EXIT_SUCCESS);
	/* NOTREACHED */
}
