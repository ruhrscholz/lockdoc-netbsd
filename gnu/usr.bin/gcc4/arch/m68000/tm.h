/* This file is automatically generated.  DO NOT EDIT! */
/* Generated from: 	NetBSD: mknative-gcc,v 1.20 2006/06/09 15:30:52 skrll Exp  */
/* Generated from: NetBSD: mknative.common,v 1.8 2006/05/26 19:17:21 mrg Exp  */

#ifndef GCC_TM_H
#define GCC_TM_H
#define TARGET_CPU_DEFAULT (0)
#ifndef MOTOROLA
# define MOTOROLA
#endif
#ifndef USE_GAS
# define USE_GAS
#endif
#ifdef IN_GCC
# include "options.h"
# include "config/m68k/m68k.h"
# include "config/dbxelf.h"
# include "config/elfos.h"
# include "config/netbsd.h"
# include "config/netbsd-elf.h"
# include "config/m68k/netbsd-elf.h"
# include "defaults.h"
#endif
#if defined IN_GCC && !defined GENERATOR_FILE && !defined USED_FOR_TARGET
# include "insn-constants.h"
# include "insn-flags.h"
#endif
#endif /* GCC_TM_H */
