/*++
/* NAME
/*	cleanup_masquerade 3
/* SUMMARY
/*	address masquerading
/* SYNOPSIS
/*	#include <cleanup.h>
/*
/*	void	cleanup_masquerade_external(addr, masq_domains)
/*	VSTRING	*addr;
/*	ARGV	*masq_domains;
/*
/*	void	cleanup_masquerade_internal(addr, masq_domains)
/*	VSTRING	*addr;
/*	ARGV	*masq_domains;
/*
/*	void	cleanup_masquerade_tree(tree, masq_domains)
/*	TOK822	*tree;
/*	ARGV	*masq_domains;
/* DESCRIPTION
/*	This module masquerades addresses, that is, it strips subdomains
/*	below domain names that are listed in the masquerade_domains
/*	configuration parameter, except for user names listed in the
/*	masquerade_exceptions configuration parameter.
/*
/*	cleanup_masquerade_external() rewrites the external (quoted) string
/*	form of an address.
/*
/*	cleanup_masquerade_internal() is a wrapper around the
/*	cleanup_masquerade_external() routine that transforms from
/*	internal (quoted) string form to external form and back.
/*
/*	cleanup_masquerade_tree() is a wrapper around the
/*	cleanup_masquerade_external() routine that transforms from
/*	internal parse tree form to external form and back.
/* DIAGNOSTICS
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <string.h>

#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <argv.h>
#include <htable.h>
#include <mymalloc.h>
#include <stringops.h>

/* Global library. */

#include <mail_params.h>
#include <tok822.h>
#include <quote_822_local.h>

/* Application-specific. */

#include "cleanup.h"

#define STR	vstring_str

/* cleanup_masquerade_external - masquerade address external form */

void    cleanup_masquerade_external(VSTRING *addr, ARGV *masq_domains)
{
    char   *domain;
    int     domain_len;
    char  **masqp;
    char   *masq;
    int     masq_len;
    char   *parent;
    int     truncate;

    /* Stuff for excluded names. */
    char   *name;
    int     excluded;

    /*
     * Find the domain part.
     */
    if ((domain = strrchr(STR(addr), '@')) == 0)
	return;
    domain += 1;
    domain_len = strlen(domain);

    /*
     * Don't masquerade excluded names (regardless of domain).
     */
    if (*var_masq_exceptions) {
	name = mystrndup(STR(addr), domain - 1 - STR(addr));
	excluded = (string_list_match(cleanup_masq_exceptions, lowercase(name)) != 0);
	myfree(name);
	if (excluded)
	    return;
    }

    /*
     * If any parent domain matches the list of masquerade domains, replace
     * the domain in the address and terminate. If the domain matches a
     * masquerade domain, leave it alone. Order of specification matters.
     */
    for (masqp = masq_domains->argv; (masq = *masqp) != 0; masqp++) {
	for (truncate = 1; *masq == '!'; masq++)
	    truncate = !truncate;
	masq_len = strlen(masq);
	if (masq_len == 0)
	    continue;
	if (masq_len == domain_len) {
	    if (strcasecmp(masq, domain) == 0)
		break;
	} else if (masq_len < domain_len) {
	    parent = domain + domain_len - masq_len;
	    if (parent[-1] == '.' && strcasecmp(masq, parent) == 0) {
		if (truncate) {
		    if (msg_verbose)
			msg_info("masquerade: %s -> %s", domain, masq);
		    vstring_truncate(addr, domain - STR(addr));
		    vstring_strcat(addr, masq);
		}
		break;
	    }
	}
    }
}

/* cleanup_masquerade_tree - masquerade address node */

void    cleanup_masquerade_tree(TOK822 *tree, ARGV *masq_domains)
{
    VSTRING *temp = vstring_alloc(100);

    tok822_externalize(temp, tree->head, TOK822_STR_DEFL);
    cleanup_masquerade_external(temp, masq_domains);
    tok822_free_tree(tree->head);
    tree->head = tok822_scan(STR(temp), &tree->tail);

    vstring_free(temp);
}

/* cleanup_masquerade_internal - masquerade address internal form */

void    cleanup_masquerade_internal(VSTRING *addr, ARGV *masq_domains)
{
    VSTRING *temp = vstring_alloc(100);

    quote_822_local(temp, STR(addr));
    cleanup_masquerade_external(temp, masq_domains);
    unquote_822_local(addr, STR(temp));

    vstring_free(temp);
}

 /*
  * Code for stand-alone testing. Instead of using main.cf, specify the strip
  * list and the candidate domain on the command line. Specify null arguments
  * for data that should be empty.
  */
#ifdef TEST

#include <vstream.h>

char *var_masq_exceptions;
STRING_LIST *cleanup_masq_exceptions;

int main(int argc, char **argv)
{
    VSTRING *addr;
    ARGV   *masq_domains;

    if (argc != 4)
	msg_fatal("usage: %s exceptions masquerade_list address", argv[0]);

    var_masq_exceptions = argv[1];
    cleanup_masq_exceptions =
  	string_list_init(MATCH_FLAG_NONE, var_masq_exceptions);
    masq_domains = argv_split(argv[2], " ,\t\r\n");
    addr = vstring_alloc(1);
    if (strchr(argv[3], '@') == 0)
	msg_fatal("address must be in user@domain form");
    vstring_strcpy(addr, argv[3]);

    vstream_printf("----------\n");
    vstream_printf("exceptions: %s\n", argv[1]);
    vstream_printf("masq_list:  %s\n", argv[2]);
    vstream_printf("address:    %s\n", argv[3]);

    cleanup_masquerade_external(addr, masq_domains);

    vstream_printf("result:     %s\n", STR(addr));
    vstream_fflush(VSTREAM_OUT);

    vstring_free(addr);
    argv_free(masq_domains);

    return (0);
}

#endif
