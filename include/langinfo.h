/*	$NetBSD: langinfo.h,v 1.7 2003/07/26 17:35:00 salo Exp $	*/

/*
 * Written by J.T. Conklin <jtc@NetBSD.org>
 * Public domain.
 */

#ifndef _LANGINFO_H_
#define _LANGINFO_H_

#include <sys/cdefs.h>
#include <nl_types.h>

#define D_T_FMT		0	/* String for formatting date and time */
#define D_FMT		1	/* Date format string */
#define	T_FMT		2	/* Time format string */
#define T_FMT_AMPM	3	/* Time format string with 12 hour clock */
#define AM_STR		4	/* Ante Meridiem afix */
#define PM_STR		5	/* Post Meridiem afix */

#define DAY_1		6	/* Name of the first day of the week */
#define DAY_2		7
#define DAY_3		8
#define DAY_4		9
#define DAY_5		10
#define DAY_6		11
#define DAY_7		12

#define ABDAY_1		13	/* Abbrev. name of the first day of the week */
#define ABDAY_2		14
#define ABDAY_3		15
#define ABDAY_4		16
#define ABDAY_5		17
#define ABDAY_6		18
#define ABDAY_7		19

#define MON_1		20	/* Name of the first month */
#define MON_2		21
#define MON_3		22
#define MON_4		23
#define MON_5		24
#define MON_6		25
#define MON_7		26
#define MON_8		27
#define MON_9		28
#define MON_10		29
#define MON_11		30
#define MON_12		31

#define ABMON_1		32	/* Abbrev. name of the first month */
#define ABMON_2		33
#define ABMON_3		34
#define ABMON_4		35
#define ABMON_5		36
#define ABMON_6		37
#define ABMON_7		38
#define ABMON_8		39
#define ABMON_9		40
#define ABMON_10	41
#define ABMON_11	42
#define ABMON_12	43

#define RADIXCHAR	44	/* Radix character */
#define THOUSEP		45	/* Separator for thousands */
#define YESSTR		46	/* Affirmitive response for yes/no queries */
#define YESEXPR		47	/* Affirmitive response for yes/no queries */
#define NOSTR		48	/* Negative response for yes/no queries */
#define NOEXPR		49	/* Negative response for yes/no queries */
#define CRNCYSTR	50	/* Currency symbol */

#define CODESET		51	/* codeset name */

#define ERA		52	/* Era description segments */
#define ERA_D_FMT	53	/* Era date format string */
#define ERA_D_T_FMT	54	/* Era date and time format string */
#define ERA_T_FMT	55	/* Era time format string */

#define ALT_DIGITS	56	/* Alternative symbols for digits */

__BEGIN_DECLS
char *nl_langinfo __P((nl_item));
__END_DECLS

#endif	/* _LANGINFO_H_ */
