# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gcc,v 1.20 2006/06/09 15:30:52 skrll Exp 
# Generated from: NetBSD: mknative.common,v 1.8 2006/05/26 19:17:21 mrg Exp 
#
G_INCLUDES=-I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/.  -I${GNUHOSTDIST}/gcc/config -I${GNUHOSTDIST}/gcc/../include
G_CRTSTUFF_CFLAGS=-O2 -DIN_GCC   -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -isystem ./include  -I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/.  -I${GNUHOSTDIST}/gcc/config -I${GNUHOSTDIST}/gcc/../include  -g0  -finhibit-size-directive -fno-inline-functions -fno-exceptions  -fno-zero-initialized-in-bss
G_CRTSTUFF_T_CFLAGS=-fPIC
G_tm_defines=NETBSD_ENABLE_PTHREADS
G_xm_file=ansidecl.h  pa/pa.h dbxelf.h elfos.h svr4.h netbsd.h netbsd-elf.h 		 pa/pa-netbsd.h pa/pa32-regs.h pa/pa32-netbsd.h defaults.h
G_xm_defines=POSIX
