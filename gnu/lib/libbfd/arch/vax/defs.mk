# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: toolchain2netbsd,v 1.12 2001/08/14 05:17:59 tv Exp 
#
G_libbfd_la_DEPENDENCIES=elf32-vax.lo elf32.lo elf.lo elflink.lo dwarf1.lo vaxnetbsd.lo aout32.lo coff-vax.lo elf32-gen.lo cpu-vax.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo cache.lo coffgen.lo  corefile.lo format.lo init.lo libbfd.lo opncls.lo reloc.lo section.lo  syms.lo targets.lo hash.lo linker.lo srec.lo binary.lo tekhex.lo  ihex.lo stabs.lo stab-syms.lo dwarf2.lo
G_DEFS=-DHAVE_CONFIG_H -I. -I${DIST}/bfd -I.
G_INCLUDES=-D_GNU_SOURCE  -DNETBSD_CORE   -I. -I${DIST}/bfd -I${DIST}/bfd/../include  -I${DIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf32_vax_vec -DSELECT_VECS='&bfd_elf32_vax_vec,&vaxnetbsd_vec,&vaxcoff_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_vax_arch' -DHAVE_bfd_elf32_vax_vec -DHAVE_vaxnetbsd_vec -DHAVE_vaxcoff_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec
G_archdefs=-DARCH_vax
G_BFD_MACHINES=vax-dis.lo
G_libopcodes_la_SOURCES=dis-buf.c disassemble.c
