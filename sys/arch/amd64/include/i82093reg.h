/*	 $NetBSD: i82093reg.h,v 1.1 2003/04/26 18:39:41 fvdl Exp $ */

#include <x86/i82093reg.h>

#define ioapic_asm_ack(num) \
	movl	$0,(_C_LABEL(local_apic)+LAPIC_EOI)(%rip)
