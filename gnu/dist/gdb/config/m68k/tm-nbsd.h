/* Macro definitions for m68k running under NetBSD.
   Copyright 1994 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef TM_NBSD_H
#define TM_NBSD_H

/* Generic definitions.  */
#include "m68k/tm-m68k.h"
#include "tm-nbsd.h"

/* NetBSD uses trap 15 for both user and kernel breakpoints. */
#define BPT_VECTOR 0xf
#define REMOTE_BPT_VECTOR 0xf

/* Address of end of stack space.  */
#if 0 /* XXX: Need to determine this at run-time! */
extern CORE_ADDR get_stack_end_addr();
#define STACK_END_ADDR get_stack_end_addr();
/* For NetBSD, sigtramp is 32 bytes before STACK_END_ADDR.  */
#define SIGTRAMP_START (STACK_END_ADDR - 32)
#define SIGTRAMP_END (STACK_END_ADDR)
#endif

/* Saved Pc.  Get it from sigcontext if within sigtramp.  */

/* Offset to saved PC in sigcontext, from <machine/signal.h>.  */
#define SIGCONTEXT_PC_OFFSET 20

#endif /* TM_NBSD_H */
