/* $NetBSD: font_bold.h,v 1.2 1996/06/03 21:35:10 mark Exp $ */

/*
 * Copyright (c) 1994 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
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
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * RiscBSD kernel project
 *
 * font_terminal14_bold.h
 *
 * Font for physical console driver
 *
 * Created      : 18/09/94
 *
 * Based on kate/display/14bold.h
 */
 
unsigned char font_terminal_14bold_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x24, 0x24, 0x7e, 0x7e, 0x24, 0x7e, 0x7e, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x38, 0x6c, 0x0c, 0x38, 0x60, 0x6c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x6c, 0x76, 0x3c, 0x30, 0x18, 0x78, 0xdc, 0x6c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x34, 0x34, 0x98, 0x7c, 0x66, 0x66, 0xbc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x30, 0x30, 0x18, 0x18, 0x18, 0x18, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x54, 0x38, 0xfe, 0x38, 0x54, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x66, 0x66, 0x66, 0x66, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x1a, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x70, 0x38, 0x1c, 0x0e, 0x66, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x60, 0x38, 0x60, 0x60, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x70, 0x68, 0x64, 0x66, 0xfe, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3e, 0x06, 0x06, 0x3e, 0x60, 0x60, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x60, 0x30, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x8e, 0xe6, 0xb6, 0xb6, 0xe6, 0x0e, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x76, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x36, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0x66, 0x36, 0x1e, 0x1e, 0x36, 0x66, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x6e, 0x6e, 0x76, 0x76, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x76, 0x3c, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x36, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x3e, 0x7c, 0x60, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xd6, 0xd6, 0xd6, 0x7c, 0x28, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0x60, 0x70, 0x38, 0x1c, 0x0e, 0x06, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x38, 0x6c, 0x0c, 0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x1e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 0x66, 0x36, 0x1e, 0x1e, 0x36, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0xd6, 0xd6, 0xd6, 0xd6, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xcc, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x46, 0x3e, 0x7c, 0x62, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x3e, 0x0c, 0x0c, 0x0c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x2c, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xd6, 0xd6, 0xd6, 0xfe, 0x6c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x70, 0x38, 0x1c, 0x0e, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x70, 0x18, 0x18, 0x30, 0x1c, 0x30, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0e, 0x18, 0x18, 0x0c, 0x38, 0x0c, 0x18, 0x18, 0x18, 0x0e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x4c, 0x7e, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xfc, 0x36, 0x36, 0x76, 0x36, 0x36, 0x36, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xd6, 0xf6, 0x16, 0xd6, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x3c, 0x66, 0x06, 0x66, 0x3c, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x78, 0x4c, 0x0c, 0x3e, 0x0c, 0x0c, 0xde, 0x7a, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x38, 0x6c, 0x6c, 0x38, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x66, 0x3c, 0x18, 0x7e, 0x18, 0x7e, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x0e, 0x3c, 0x66, 0x66, 0x3c, 0x70, 0x66, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7e, 0xc3, 0xdb, 0xcb, 0xcb, 0xdb, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x30, 0x3c, 0x36, 0x3c, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd8, 0x6c, 0x36, 0x6c, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7e, 0x7e, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x7e, 0x81, 0x9d, 0xad, 0x9d, 0xad, 0x81, 0x7e, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1c, 0x36, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x32, 0x30, 0x1c, 0x06, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x32, 0x18, 0x30, 0x32, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0xfe, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfc, 0x5e, 0x5e, 0x5e, 0x5e, 0x5c, 0x58, 0x58, 0x58, 0x58, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x0c, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x1c, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x36, 0x36, 0x36, 0x1c, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x36, 0x6c, 0x36, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x0e, 0x8c, 0x4c, 0x3e, 0x10, 0x68, 0x74, 0x6a, 0xfd, 0x60, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x0e, 0x8c, 0x4c, 0x3e, 0x10, 0x68, 0xd4, 0xc2, 0x61, 0x30, 0xf0, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x1e, 0x98, 0x4c, 0x38, 0x1e, 0x6c, 0x74, 0x6a, 0xfd, 0x60, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x0c, 0x46, 0x66, 0x3c, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x18, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x5c, 0x3a, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x00, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x2c, 0x2c, 0x18, 0x18, 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xfc, 0x36, 0x36, 0x76, 0x3e, 0x36, 0x36, 0xf6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x10, 0x18, 0x00, 0x00,
    0x00, 0x18, 0x30, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x30, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x6c, 0x6c, 0x00, 0x7c, 0x0c, 0x0c, 0x3c, 0x0c, 0x0c, 0x0c, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x18, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1c, 0x2c, 0x4c, 0x4c, 0x5e, 0x4c, 0x2c, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x5c, 0x3a, 0x00, 0x66, 0x66, 0x6e, 0x6e, 0x76, 0x76, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x5c, 0x3a, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0x78, 0x6c, 0x6c, 0x7c, 0x7c, 0x6c, 0x6c, 0x3c, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x0c, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0x3c, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1e, 0x0c, 0x3c, 0x6c, 0x6c, 0x3c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x36, 0x66, 0x66, 0x66, 0x36, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x58, 0x34, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x6c, 0x6c, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3c, 0x6e, 0x3c, 0x00, 0x3c, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xb2, 0xfc, 0x36, 0xb6, 0x6c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x66, 0x3c, 0x10, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x3c, 0x66, 0x7e, 0x06, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x36, 0x36, 0x00, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x24, 0x18, 0x1c, 0x30, 0x7c, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x5c, 0x3a, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x5c, 0x3a, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x7e, 0x7e, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x6c, 0x7c, 0x7c, 0x6c, 0x3a, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x18, 0x3c, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x66, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x3c, 0x6c, 0x6c, 0x6c, 0x6c, 0x3c, 0x0c, 0x0c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x66, 0x3c, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
    0x00, 0x00, 0x00, 0x08, 0x1c, 0x2a, 0x49, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x49, 0x2a, 0x1c, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0xff, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x40, 0xff, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
};

font_struct font_terminal_14bold = {
    199,
    1,
    16,
    8,
    16,
    8,
    16,
    3184,
    font_terminal_14bold_data
};

/* End of font14bold.h */
