/*	$NetBSD: cdefs.h,v 1.6 1998/07/01 02:35:24 tv Exp $	*/

/*
 * Written by J.T. Conklin <jtc@wimsey.com> 01/17/95.
 * Public domain.
 */

#ifndef	_MACHINE_CDEFS_H_
#define	_MACHINE_CDEFS_H_

#ifdef __ELF__
#define	_C_LABEL(x)	x
#else
#define	_C_LABEL(x)	__CONCAT(_,x)
#endif

#ifdef __GNUC__
#define	__RENAME(x)	__asm__(___STRING(_C_LABEL(x)))
#endif

#ifdef __GNUC__
#ifdef __STDC__
#define __indr_reference(sym,alias)	\
	__asm__(".stabs \"_" #alias "\",11,0,0,0");	\
	__asm__(".stabs \"_" #sym "\",1,0,0,0");
#define __warn_references(sym,msg)	\
	__asm__(".stabs \"" msg "\",30,0,0,0");		\
	__asm__(".stabs \"_" #sym "\",1,0,0,0");
#else
#define __indr_reference(sym,alias)	\
	__asm__(".stabs \"_/**/alias\",11,0,0,0");	\
	__asm__(".stabs \"_/**/sym\",1,0,0,0");
#define __warn_references(sym,msg)	\
	__asm__(".stabs msg,30,0,0,0");			\
	__asm__(".stabs \"_/**/sym\",1,0,0,0");
#endif
#else
#define __indr_reference(sym,alias)
#define __warn_references(sym,msg)
#endif

#endif /* !_MACHINE_CDEFS_H_ */
