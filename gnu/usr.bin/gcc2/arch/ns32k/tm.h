/* Definitions of target machine for GNU compiler.
   PC532 with National 32532, running NetBSD 0.9 or later.
   Copyright (C) 1992 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "ns32k/ns32k.h"

/* The NetBSD predefines ... */

#undef CPP_PREDEFINES
#define CPP_PREDEFINES "-Dns32532 -Dpc532 -Dunix -D__NetBSD__ -Dns32k"

/* Compile for the floating point unit & 32532 by default;
   also presume SB is zero */

#define TARGET_DEFAULT 25

/* Write DBX debugging info for gdb to read */

#define DBX_DEBUGGING_INFO

/* Use the re-entrant and potentially faster method */

#undef PCC_STATIC_STRUCT_RETURN

/* 32-bit alignment for efficiency */
#undef POINTER_BOUNDARY
#define POINTER_BOUNDARY 32

/* 32-bit alignment for efficiency */
#undef FUNCTION_BOUNDARY
#define FUNCTION_BOUNDARY 32

/* 32532 spec says it can handle any alignment.  Rumor from tm-ns32k.h
   tells this might not be actually true (but it's for 32032, perhaps
   National has fixed the bug for 32532).  You might have to change this
   if the bug still exists. */

#undef STRICT_ALIGNMENT
#define STRICT_ALIGNMENT 0

/* Use pc relative addressing whenever possible,
   it's more efficient than absolute (ns32k.c)
   You have to fix a bug in gas 1.38.1 to make this work with gas,
   patch available from jkp@cs.hut.fi. */

#define PC_RELATIVE

/* Operand of bsr or jsr should be just the address.  */

#define CALL_MEMREF_IMPLICIT

/* movd insns may have floating point constant operands.  */

#define MOVD_FLOAT_OK

/* Every address needs to use a base reg.  */

#define BASE_REG_NEEDED

/* Don't default to pcc-struct-return, because gcc is the only compiler, and
   we want to retain compatibility with older gcc versions.  */

#define DEFAULT_PCC_STRUCT_RETURN 0

/* Change the built-in type ideas ... */

#undef SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef WCHAR_TYPE
#define WCHAR_TYPE "short unsigned int"

#define WCHAR_UNSIGNED 1

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE 16

/* NetBSD does have atexit.  */

#define HAVE_ATEXIT

