/* $NetBSD: parse.c,v 1.12 2001/11/03 13:35:39 lukem Exp $ */

/*-
 * Copyright (c) 1980, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)parse.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: parse.c,v 1.12 2001/11/03 13:35:39 lukem Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>

#include <stdlib.h>
#include <string.h>

#if __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif

#include "csh.h"
#include "extern.h"

static void asyntax(struct wordent *, struct wordent *);
static void asyn0(struct wordent *, struct wordent *);
static void asyn3(struct wordent *, struct wordent *);
static struct wordent *freenod(struct wordent *, struct wordent *);
static struct command *syn0(struct wordent *, struct wordent *, int);
static struct command *syn1(struct wordent *, struct wordent *, int);
static struct command *syn1a(struct wordent *, struct wordent *, int);
static struct command *syn1b(struct wordent *, struct wordent *, int);
static struct command *syn2(struct wordent *, struct wordent *, int);
static struct command *syn3(struct wordent *, struct wordent *, int);

#define ALEFT 21		/* max of 20 alias expansions	 */
#define HLEFT 11		/* max of 10 history expansions	 */
/*
 * Perform aliasing on the word list lex
 * Do a (very rudimentary) parse to separate into commands.
 * If word 0 of a command has an alias, do it.
 * Repeat a maximum of 20 times.
 */
static int aleft;
extern int hleft;

void
alias(struct wordent *lexp)
{
    jmp_buf osetexit;

    aleft = ALEFT;
    hleft = HLEFT;
    getexit(osetexit);
    (void)setexit();
    if (haderr) {
	resexit(osetexit);
	reset();
    }
    if (--aleft == 0)
	stderror(ERR_ALIASLOOP);
    asyntax(lexp->next, lexp);
    resexit(osetexit);
}

static void
asyntax(struct wordent *p1, struct wordent *p2)
{
    while (p1 != p2)
	if (any(";&\n", p1->word[0]))
	    p1 = p1->next;
	else {
	    asyn0(p1, p2);
	    return;
	}
}

static void
asyn0(struct wordent *p1, struct wordent *p2)
{
    struct wordent *p;
    int l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    if (l < 0)
		stderror(ERR_TOOMANYRP);
	    continue;
	case '>':
	    if (p->next != p2 && eq(p->next->word, STRand))
		p = p->next;
	    continue;
	case '&':
	case '|':
	case ';':
	case '\n':
	    if (l != 0)
		continue;
	    asyn3(p1, p);
	    asyntax(p->next, p2);
	    return;
	}
    if (l == 0)
	asyn3(p1, p2);
}

static void
asyn3(struct wordent *p1, struct wordent *p2)
{
    struct varent *ap;
    struct wordent alout;
    bool redid;

    if (p1 == p2)
	return;
    if (p1->word[0] == '(') {
	for (p2 = p2->prev; p2->word[0] != ')'; p2 = p2->prev)
	    if (p2 == p1)
		return;
	if (p2 == p1->next)
	    return;
	asyn0(p1->next, p2);
	return;
    }
    ap = adrof1(p1->word, &aliases);
    if (ap == 0)
	return;
    alhistp = p1->prev;
    alhistt = p2;
    alvec = ap->vec;
    redid = lex(&alout);
    alhistp = alhistt = 0;
    alvec = 0;
    if (seterr) {
	freelex(&alout);
	stderror(ERR_OLD);
    }
    if (p1->word[0] && eq(p1->word, alout.next->word)) {
	Char *cp;

	cp = alout.next->word;
	alout.next->word = Strspl(STRQNULL, cp);
	xfree((ptr_t) cp);
    }
    p1 = freenod(p1, redid ? p2 : p1->next);
    if (alout.next != &alout) {
	p1->next->prev = alout.prev->prev;
	alout.prev->prev->next = p1->next;
	alout.next->prev = p1;
	p1->next = alout.next;
	xfree((ptr_t)alout.prev->word);
	xfree((ptr_t)(alout.prev));
    }
    reset();			/* throw! */
}

static struct wordent *
freenod(struct wordent *p1, struct wordent *p2)
{
    struct wordent *retp;

    retp = p1->prev;
    while (p1 != p2) {
	xfree((ptr_t)p1->word);
	p1 = p1->next;
	xfree((ptr_t) (p1->prev));
    }
    retp->next = p2;
    p2->prev = retp;
    return (retp);
}

#define	PHERE	1
#define	PIN	2
#define	POUT	4
#define	PERR	8

/*
 * syntax
 *	empty
 *	syn0
 */
struct command *
syntax(struct wordent *p1, struct wordent *p2, int flags)
{
    while (p1 != p2)
	if (any(";&\n", p1->word[0]))
	    p1 = p1->next;
	else
	    return (syn0(p1, p2, flags));
    return (0);
}

/*
 * syn0
 *	syn1
 *	syn1 & syntax
 */
static struct command *
syn0(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *p;
    struct command *t, *t1;
    int l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    if (l < 0)
		seterror(ERR_TOOMANYRP);
	    continue;
	case '|':
	    if (p->word[1] == '|')
		continue;
	    /* FALLTHROUGH */
	case '>':
	    if (p->next != p2 && eq(p->next->word, STRand))
		p = p->next;
	    continue;
	case '&':
	    if (l != 0)
		break;
	    if (p->word[1] == '&')
		continue;
	    t1 = syn1(p1, p, flags);
	    if (t1->t_dtyp == NODE_LIST ||
		t1->t_dtyp == NODE_AND ||
		t1->t_dtyp == NODE_OR) {
		t = (struct command *)xcalloc(1, sizeof(*t));
		t->t_dtyp = NODE_PAREN;
		t->t_dflg = F_AMPERSAND | F_NOINTERRUPT;
		t->t_dspr = t1;
		t1 = t;
	    }
	    else
		t1->t_dflg |= F_AMPERSAND | F_NOINTERRUPT;
	    t = (struct command *)xcalloc(1, sizeof(*t));
	    t->t_dtyp = NODE_LIST;
	    t->t_dflg = 0;
	    t->t_dcar = t1;
	    t->t_dcdr = syntax(p, p2, flags);
	    return (t);
	}
    if (l == 0)
	return (syn1(p1, p2, flags));
    seterror(ERR_TOOMANYLP);
    return (0);
}

/*
 * syn1
 *	syn1a
 *	syn1a ; syntax
 */
static struct command *
syn1(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *p;
    struct command *t;
    int l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    continue;
	case ';':
	case '\n':
	    if (l != 0)
		break;
	    t = (struct command *) xcalloc(1, sizeof(*t));
	    t->t_dtyp = NODE_LIST;
	    t->t_dcar = syn1a(p1, p, flags);
	    t->t_dcdr = syntax(p->next, p2, flags);
	    if (t->t_dcdr == 0)
		t->t_dcdr = t->t_dcar, t->t_dcar = 0;
	    return (t);
	}
    return (syn1a(p1, p2, flags));
}

/*
 * syn1a
 *	syn1b
 *	syn1b || syn1a
 */
static struct command *
syn1a(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *p;
    struct command *t;
    int l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    continue;
	case '|':
	    if (p->word[1] != '|')
		continue;
	    if (l == 0) {
		t = (struct command *)xcalloc(1, sizeof(*t));
		t->t_dtyp = NODE_OR;
		t->t_dcar = syn1b(p1, p, flags);
		t->t_dcdr = syn1a(p->next, p2, flags);
		t->t_dflg = 0;
		return (t);
	    }
	    continue;
	}
    return (syn1b(p1, p2, flags));
}

/*
 * syn1b
 *	syn2
 *	syn2 && syn1b
 */
static struct command *
syn1b(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *p;
    struct command *t;
    int l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    continue;
	case '&':
	    if (p->word[1] == '&' && l == 0) {
		t = (struct command *)xcalloc(1, sizeof(*t));
		t->t_dtyp = NODE_AND;
		t->t_dcar = syn2(p1, p, flags);
		t->t_dcdr = syn1b(p->next, p2, flags);
		t->t_dflg = 0;
		return (t);
	    }
	    continue;
	}
    return (syn2(p1, p2, flags));
}

/*
 * syn2
 *	syn3
 *	syn3 | syn2
 *	syn3 |& syn2
 */
static struct command *
syn2(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *p, *pn;
    struct command *t;
    int f, l;

    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    l++;
	    continue;
	case ')':
	    l--;
	    continue;
	case '|':
	    if (l != 0)
		continue;
	    t = (struct command *)xcalloc(1, sizeof(*t));
	    f = flags | POUT;
	    pn = p->next;
	    if (pn != p2 && pn->word[0] == '&') {
		f |= PERR;
		t->t_dflg |= F_STDERR;
	    }
	    t->t_dtyp = NODE_PIPE;
	    t->t_dcar = syn3(p1, p, f);
	    if (pn != p2 && pn->word[0] == '&')
		p = pn;
	    t->t_dcdr = syn2(p->next, p2, flags | PIN);
	    return (t);
	}
    return (syn3(p1, p2, flags));
}

static char RELPAR[] = {'<', '>', '(', ')', '\0'};

/*
 * syn3
 *	( syn0 ) [ < in  ] [ > out ]
 *	word word* [ < in ] [ > out ]
 *	KEYWORD ( word* ) word* [ < in ] [ > out ]
 *
 *	KEYWORD = (@ exit foreach if set switch test while)
 */
static struct command *
syn3(struct wordent *p1, struct wordent *p2, int flags)
{
    struct wordent *lp, *p, *rp;
    struct command *t;
    Char **av;
    int c, l, n;
    bool specp;

    specp = 0;
    if (p1 != p2) {
	p = p1;
again:
	switch (srchx(p->word)) {
	case T_ELSE:
	    p = p->next;
	    if (p != p2)
		goto again;
	    break;
	case T_EXIT:
	case T_FOREACH:
	case T_IF:
	case T_LET:
	case T_SET:
	case T_SWITCH:
	case T_WHILE:
	    specp = 1;
	    break;
	}
    }
    n = 0;
    l = 0;
    for (p = p1; p != p2; p = p->next)
	switch (p->word[0]) {
	case '(':
	    if (specp)
		n++;
	    l++;
	    continue;
	case ')':
	    if (specp)
		n++;
	    l--;
	    continue;
	case '>':
	case '<':
	    if (l != 0) {
		if (specp)
		    n++;
		continue;
	    }
	    if (p->next == p2)
		continue;
	    if (any(RELPAR, p->next->word[0]))
		continue;
	    n--;
	    continue;
	default:
	    if (!specp && l != 0)
		continue;
	    n++;
	    continue;
	}
    if (n < 0)
	n = 0;
    t = (struct command *)xcalloc(1, sizeof(*t));
    av = (Char **)xcalloc((size_t)(n + 1), sizeof(Char **));
    t->t_dcom = av;
    n = 0;
    if (p2->word[0] == ')')
	t->t_dflg = F_NOFORK;
    lp = 0;
    rp = 0;
    l = 0;
    for (p = p1; p != p2; p = p->next) {
	c = p->word[0];
	switch (c) {
	case '(':
	    if (l == 0) {
		if (lp != 0 && !specp)
		    seterror(ERR_BADPLP);
		lp = p->next;
	    }
	    l++;
	    goto savep;
	case ')':
	    l--;
	    if (l == 0)
		rp = p;
	    goto savep;
	case '>':
	    if (l != 0)
		goto savep;
	    if (p->word[1] == '>')
		t->t_dflg |= F_APPEND;
	    if (p->next != p2 && eq(p->next->word, STRand)) {
		t->t_dflg |= F_STDERR, p = p->next;
		if (flags & (POUT | PERR)) {
		    seterror(ERR_OUTRED);
		    continue;
		}
	    }
	    if (p->next != p2 && eq(p->next->word, STRbang))
		t->t_dflg |= F_OVERWRITE, p = p->next;
	    if (p->next == p2) {
		seterror(ERR_MISRED);
		continue;
	    }
	    p = p->next;
	    if (any(RELPAR, p->word[0])) {
		seterror(ERR_MISRED);
		continue;
	    }
	    if ((flags & POUT) && ((flags & PERR) == 0 || t->t_drit))
		seterror(ERR_OUTRED);
	    else
		t->t_drit = Strsave(p->word);
	    continue;
	case '<':
	    if (l != 0)
		goto savep;
	    if (p->word[1] == '<')
		t->t_dflg |= F_READ;
	    if (p->next == p2) {
		seterror(ERR_MISRED);
		continue;
	    }
	    p = p->next;
	    if (any(RELPAR, p->word[0])) {
		seterror(ERR_MISRED);
		continue;
	    }
	    if ((flags & PHERE) && (t->t_dflg & F_READ))
		seterror(ERR_REDPAR);
	    else if ((flags & PIN) || t->t_dlef)
		seterror(ERR_INRED);
	    else
		t->t_dlef = Strsave(p->word);
	    continue;
	savep:
	    if (!specp)
		continue;
	    /* FALLTHROUGH */
	default:
	    if (l != 0 && !specp)
		continue;
	    if (seterr == 0)
		av[n] = Strsave(p->word);
	    n++;
	    continue;
	}
    }
    if (lp != 0 && !specp) {
	if (n != 0)
	    seterror(ERR_BADPLPS);
	t->t_dtyp = NODE_PAREN;
	t->t_dspr = syn0(lp, rp, PHERE);
    }
    else {
	if (n == 0)
	    seterror(ERR_NULLCOM);
	t->t_dtyp = NODE_COMMAND;
    }
    return (t);
}

void
freesyn(struct command *t)
{
    Char **v;

    if (t == 0)
	return;
    switch (t->t_dtyp) {
    case NODE_COMMAND:
	for (v = t->t_dcom; *v; v++)
	    xfree((ptr_t) * v);
	xfree((ptr_t)(t->t_dcom));
	xfree((ptr_t)t->t_dlef);
	xfree((ptr_t)t->t_drit);
	break;
    case NODE_PAREN:
	freesyn(t->t_dspr);
	xfree((ptr_t)t->t_dlef);
	xfree((ptr_t)t->t_drit);
	break;
    case NODE_AND:
    case NODE_OR:
    case NODE_PIPE:
    case NODE_LIST:
	freesyn(t->t_dcar), freesyn(t->t_dcdr);
	break;
    }
    xfree((ptr_t)t);
}
