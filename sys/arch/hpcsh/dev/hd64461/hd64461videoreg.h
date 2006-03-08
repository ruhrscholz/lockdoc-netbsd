/*	$NetBSD: hd64461videoreg.h,v 1.3 2006/03/08 23:46:23 lukem Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by UCHIYAMA Yasushi.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _HPCSH_DEV_HD64461VIDEOREG_H_
#define _HPCSH_DEV_HD64461VIDEOREG_H_

/* 
 * LCD Controller Control Register 
 */

/* Base Address Register */
#define HD64461_LCDCBAR_REG16			0xb0001000
#define HD64461_LCDCBAR_MASK			0x3fff
#define HD64461_LCDCBAR_SHIFT			12
#define HD64461_LCDCBAR_BASEADDR(x)					\
	(((x) & HD64461_LCDCBAR_MASK) << HD64461_LCDCBAR_SHIFT)

/* Line Address Offset Register */
#define HD64461_LCDCLOR_REG16			0xb0001002
#define HD64461_LCDCLOR_MASK			0x07ff
#define HD64461_LCDCLOR(x)	((x) & HD64461_LCDCLOR_MASK)

/* LCDC Control Register */
#define HD64461_LCDCCR_REG16			0xb0001004
#define HD64461_LCDCCR_STBAK			0x0400
#define HD64461_LCDCCR_STREQ			0x0100
#define HD64461_LCDCCR_MOFF			0x0080
#define HD64461_LCDCCR_REFSEL			0x0040
#define HD64461_LCDCCR_EPON			0x0020
#define HD64461_LCDCCR_SPON			0x0010
#define HD64461_LCDCCR_DSPSEL_MASK		0x7
#define HD64461_LCDCCR_DSPSEL(x)	((x) & HD64461_LCDCCR_DSPSEL_MASK)
#define HD64461_LCDCCR_DSPSEL_LCD_CRT		0x4
#define HD64461_LCDCCR_DSPSEL_CRT		0x2
#define HD64461_LCDCCR_DSPSEL_LCD		0x1

/* LCD Display Register */
/* 1 */
#define HD64461_LCDLDR1_REG16			0xb0001010
#define HD64461_LCDLDR1_DINV			0x0100
#define HD64461_LCDLDR1_DON			0x0001
/* 2 */
#define HD64461_LCDLDR2_REG16			0xb0001012
#define HD64461_LCDLDR2_CC1			0x0080
#define HD64461_LCDLDR2_CC2			0x0040
#define HD64461_LCDLDR2_LM_MASK			0x7
#define HD64461_LCDLDR2_LM(x)		((x) & HD64461_LCDLDR2_LM_MASK)
#define HD64461_LCDLDR2_LM_COLOR		0x4
#define HD64461_LCDLDR2_LM_GRAY8		0x1
#define HD64461_LCDLDR2_LM_GRAY4		0x0
/* 3 */
#define HD64461_LCDLDR3_REG16			0xb000101e
#define HD64461_LCDLDR3_CS_SHIFT		5
#define HD64461_LCDLDR3_CS_MASK			0x1f
#define HD64461_LCDLDR3_CS(cr)						\
	(((cr) >> HD64461_LCDLDR3_CS_SHIFT) &				\
	HD64461_LCDLDR3_CS_MASK)
#define HD64461_LCDLDR3_CS_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDLDR3_CS_SHIFT) &			\
	(HD64461_LCDLDR3_CS_MASK << HD64461_LCDLDR3_CS_SHIFT)))
#define HD64461_LCDLDR3_CG_MASK			0xf
#define HD64461_LCDLDR3_CG(cr)						\
	((cr)  & HD64461_LCDLDR3_CG_MASK)
#define HD64461_LCDLDR3_CG_CLR(cr)					\
	((cr)  & ~HD64461_LCDLDR3_CG_MASK)
#define HD64461_LCDLDR3_CG_SET(cr, val)					\
	((cr) | ((val) & HD64461_LCDLDR3_CG_MASK))

/*
 *	select CL2 frequency
 *	0x0	15 MHz (color) 15/2 (monochrome)
 *	0x1	2.5 MHz
 *	0x2	3.75 MHz
 *	0x4	5 MHz
 *	0x8	7.5 MHz
 *	0x10	10 MHz
 */
#define HD64461_LCDLDR3_CG_COLOR16	0x8
#define HD64461_LCDLDR3_CG_COLOR8	0x4
#define HD64461_LCDLDR3_CG_GRAY6	0x3
#define HD64461_LCDLDR3_CG_GRAY4	0x2
#define HD64461_LCDLDR3_CG_GRAY2	0x1
#define HD64461_LCDLDR3_CG_GRAY1	0x0

/* LCD Number of Characters in Horizontal Register */
#define HD64461_LCDLDHNCR_REG16			0xb0001014
#define HD64461_LCDLDHNCR_NHD_SHIFT		8
#define HD64461_LCDLDHNCR_NHD_MASK		0xff
#define HD64461_LCDLDHNCR_NHD(cr)					\
	(((cr) >> HD64461_LCDLDHNCR_NHD_SHIFT) &			\
	HD64461_LCDLDHNCR_NHD_MASK)
#define HD64461_LCDLDHNCR_NHD_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDHNCR_NHD_SHIFT) &		\
	(HD64461_LCDLDHNCR_NHD_MASK << HD64461_LCDLDHNCR_NHD_SHIFT)))
#define HD64461_LCDLDHNCR_NHT_SHIFT		0
#define HD64461_LCDLDHNCR_NHT_MASK		0xff
#define HD64461_LCDLDHNCR_NHT(cr)					\
	(((cr) >> HD64461_LCDLDHNCR_NHT_SHIFT) &			\
	HD64461_LCDLDHNCR_NHT_MASK)
#define HD64461_LCDLDHNCR_NHT_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDHNCR_NHT_SHIFT) &		\
	(HD64461_LCDLDHNCR_NHT_MASK << HD64461_LCDLDHNCR_NHT_SHIFT)))

/* Start Position of Horizontal Register */
#define HD64461_LCDLDHNSR_REG16			0xb0001016
#define HD64461_LCDLDHNSR_HSW_SHIFT		8
#define HD64461_LCDLDHNSR_HSW_MASK		0xf
#define HD64461_LCDLDHNSR_HSW(cr)					\
	(((cr) >> HD64461_LCDLDHNSR_HSW_SHIFT) &			\
	HD64461_LCDLDHNSR_HSW_MASK)
#define HD64461_LCDLDHNSR_HSW_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDHNSR_HSW_SHIFT) &		\
	(HD64461_LCDLDHNSR_HSW_MASK << HD64461_LCDLDHNSR_HSW_SHIFT)))
#define HD64461_LCDLDHNSR_HSP_SHIFT		0
#define HD64461_LCDLDHNSR_HSP_MASK		0xff
#define HD64461_LCDLDHNSR_HSP(cr)					\
	(((cr) >> HD64461_LCDLDHNSR_HSP_SHIFT) &			\
	HD64461_LCDLDHNSR_HSP_MASK)
#define HD64461_LCDLDHNSR_HSP_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDHNSR_HSP_SHIFT) &		\
	(HD64461_LCDLDHNSR_HSP_MASK << HD64461_LCDLDHNSR_HSP_SHIFT)))

/* Total Vertical Lines Register */
#define HD64461_LCDLDVNTR_REG16			0xb0001018
#define HD64461_LCDLDVNTR_VTL_SHIFT		0
#define HD64461_LCDLDVNTR_VTL_MASK		0x3ff
#define HD64461_LCDLDVNTR_VTL(cr)					\
	(((cr) >> HD64461_LCDLDVNTR_VTL_SHIFT) &			\
	HD64461_LCDLDVNTR_VTL_MASK)
#define HD64461_LCDLDVNTR_VTL_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDVNTR_VTL_SHIFT) &		\
	(HD64461_LCDLDVNTR_VTL_MASK << HD64461_LCDLDVNTR_VTL_SHIFT)))

/* Display Vertical Lines Register */
#define HD64461_LCDLDVNDR_REG16			0xb000101a
#define HD64461_LCDLDVNDR_VDL_SHIFT		0
#define HD64461_LCDLDVNDR_VDL_MASK		0x3ff
#define HD64461_LCDLDVNDR_VDL(cr)					\
	(((cr) >> HD64461_LCDLDVNDR_VDL_SHIFT) &			\
	HD64461_LCDLDVNDR_VDL_MASK)
#define HD64461_LCDLDVNDR_VDL_SET(cr, val)				\
	((cr) | (((val) << HD64461_LCDLDVNDR_VDL_SHIFT) &		\
	(HD64461_LCDLDVNDR_VDL_MASK << HD64461_LCDLDVNDR_VDL_SHIFT)))

/* Vertical Synchronization Position Register */
#define HD64461_LCDLDVSPR_REG16			0xb000101c
#define HD64461_LCDLDVSPR_VSP_SHIFT		0
#define HD64461_LCDLDVSPR_VSP_MASK		0x3ff
#define HD64461_LCDLDVSPR_VSP(cr)					\
	((cr) &	HD64461_LCDLDVSPR_VSP_MASK)
#define HD64461_LCDLDVSPR_VSP_SET(cr, val)				\
	((cr) | ((val) & HD64461_LCDLDVSPR_VSP_MASK))

/*
 * CRT Control Register
 */
/* CRTC Total Vertical Lines Register */
#define HD64461_LCDCRTVTR_REG16			0xb0001020
#define HD64461_LCDCRTVTR_SHIFT		0
#define HD64461_LCDCRTVTR_MASK		0x3ff
#define HD64461_LCDCRTVTR(cr)						\
	(((cr) >> HD64461_LCDCRTVTR_SHIFT) &				\
	HD64461_LCDCRTVTR_MASK)
#define HD64461_LCDCRTVTR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDCRTVTR_SHIFT) &			\
	(HD64461_LCDCRTVTR_MASK << HD64461_LCDCRTVTR_SHIFT)))

/* CRTC Vertical Retrace Start Register */
#define HD64461_LCDCRTVRSR_REG16		0xb0001022
#define HD64461_LCDCRTVRSR_SHIFT	0
#define HD64461_LCDCRTVRSR_MASK		0x3ff
#define HD64461_LCDCRTVRSR(cr)						\
	(((cr) >> HD64461_LCDCRTVRSR_SHIFT) &				\
	HD64461_LCDCRTVRSR_MASK)
#define HD64461_LCDCRTVRSR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDCRTVRSR_SHIFT) &			\
	(HD64461_LCDCRTVRSR_MASK << HD64461_LCDCRTVRSR_SHIFT)))

/* CRTC Vertical Retrace End Register */
#define HD64461_LCDCRTVRER_REG16		0xb0001024
#define HD64461_LCDCRTVRER_SHIFT	0
#define HD64461_LCDCRTVRER_MASK		0xf
#define HD64461_LCDCRTVRER(cr)						\
	(((cr) >> HD64461_LCDCRTVRER_SHIFT) & HD64461_LCDCRTVRER_MASK)
	
#define HD64461_LCDCRTVRER_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDCRTVRER_SHIFT) &			\
	(HD64461_LCDCRTVRER_MASK << HD64461_LCDCRTVRER_SHIFT)))

/*
 * Palette Register
 */
/* Color Palette Write Address Register */
#define HD64461_LCDCPTWAR_REG16			0xb0001030
#define HD64461_LCDCPTWAR_SHIFT		8
#define HD64461_LCDCPTWAR_MASK		0xff
#define HD64461_LCDCPTWAR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDCPTWAR_SHIFT) &			\
	(HD64461_LCDCPTWAR_MASK << HD64461_LCDCPTWAR_SHIFT)))

/* Color Palette Write Data Register */
#define HD64461_LCDCPTWDR_REG16			0xb0001032
#define HD64461_LCDCPTWDR_SHIFT		0
#define HD64461_LCDCPTWDR_MASK		0x3f
#define HD64461_LCDCPTWDR_SET(cr, val)					\
	((cr) | ((val) & HD64461_LCDCPTWDR_MASK))

/* Color Palette READ Address Register */
#define HD64461_LCDCPTRAR_REG16			0xb0001034
#define HD64461_LCDCPTRAR_SHIFT		8
#define HD64461_LCDCPTRAR_MASK		0xff
#define HD64461_LCDCPTRAR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDCPTRAR_SHIFT) &			\
	(HD64461_LCDCPTRAR_MASK << HD64461_LCDCPTRAR_SHIFT)))

/* Color Palette READ Data Register */
#define HD64461_LCDCPTRDR_REG16			0xb0001036
#define HD64461_LCDCPTRDR_SHIFT		0
#define HD64461_LCDCPTRDR_MASK		0x3f
#define HD64461_LCDCPTRDR(cr)	((cr) & HD64461_LCDCPTRDR_MASK)

/*
 * Acceleration Common Register
 */
/* Display Resolution Offset Register */
#define HD64461_LCDGRDOR_REG16			0xb0001040
#define HD64461_LCDGRDOR_SHIFT		0
#define HD64461_LCDGRDOR_MASK		0x7ff
#define HD64461_LCDGRDOR(cr)						\
	(((cr) >> HD64461_LCDGRDOR_SHIFT) &				\
	HD64461_LCDGRDOR_MASK)
#define HD64461_LCDGRDOR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDGRDOR_SHIFT) &			\
	(HD64461_LCDGRDOR_MASK << HD64461_LCDGRDOR_SHIFT)))

/* Solid Color Register */
#define HD64461_LCDGRSCR_REG16			0xb0001042

/* Accelerator Configuration Register */
#define HD64461_LCDGRCFGR_REG16			0xb0001044
#define HD64461_LCDGRCFGR_ACCSTATUS		0x0010
#define HD64461_LCDGRCFGR_ACCRESET		0x0008
#define HD64461_LCDGRCFGR_ACCSTART_MASK		0x6
#define HD64461_LCDGRCFGR_ACCSTART_BITBLT	0x0006
#define HD64461_LCDGRCFGR_ACCSTART_LINE		0x0004
#define HD64461_LCDGRCFGR_ACCSTART_OFF		0x0000
#define HD64461_LCDGRCFGR_COLORDEPTH_8BPP	0x0001

/*
 * Line Drawing Register
 */
/* Line Start Address Register */
#define HD64461_LCDLNSARH_REG16			0xb0001046
#define HD64461_LCDLNSARH_MASK		0x0007
#define HD64461_LCDLNSARL_REG16			0xb0001048
#define HD64461_LCDLNSARL_MASK		0xffff

/* Axis Pixel Length Register */
#define HD64461_LCDLNAXLR_REG16			0xb000104a
#define HD64461_LCDLNAXLR_MASK		0x07ff

/* Diagonal Regsiter */
#define HD64461_LCDLNDGR_REG16			0xb000104c
#define HD64461_LCDLNDGR_LNDGR_SIGN		0x8000
#define HD64461_LCDLNDGR_LNDGR_MASK	0x07ff
#define HD64461_LCDLNDGR_LNDGR_SET(cr, x)				\
	((cr) | ((x) & HD64461_LCDLNDGR_LNDGR_MASK))

/* Axial Register */
#define HD64461_LCDLNAXR_REG16			0xb000104e
#define HD64461_LCDLNAXR_LNAXR_MASK	0x0fff
#define HD64461_LCDLNAXR_LNAXR_SET(cr, x)				\
	((cr) | ((x) & HD64461_LCDLNAXR_LNAXR_MASK))

/* Start Error Term Register */
#define HD64461_LCDLNERTR_REG16			0xb0001050
#define HD64461_LCDLNERTR_LNERTR_SIGN		0x8000
#define HD64461_LCDLNERTR_LNERTR_MASK	0x07ff
#define HD64461_LCDLNERTR_LNERTR_SET(cr, x)				\
	((cr) | ((x) & HD64461_LCDLNERTR_LNERTR_MASK))

/* Line Mode Register */
#define HD64461_LCDLNMDR_REG16			0xb0001052
#define HD64461_LCDLNMDR_MASK		0x0003
/* 
 *    2  1
 *  3      0
 *  4      7
 *    5  6
 *
 * 1 or 5 ... 3
 * 2 or 6 ... 2
 * 0 or 4 ... 1
 * 3 or 7 ... 0
 */

/*
 * BitBLT Register
 */
/* Source Start Address Register (19 bit) */
#define HD64461_LCDBBTSSARH_REG16		0xb0001054
#define HD64461_LCDBBTSSARH_SHIFT	16
#define HD64461_LCDBBTSSARH_MASK	0x0007
#define HD64461_LCDBBTSSARL_REG16		0xb0001056
#define HD64461_LCDBBTSSARL_MASK	0xffff
#define HD64461_LCDBBTSSARH(x)						\
	(((x) >> HD64461_LCDBBTSSARH_SHIFT) & HD64461_LCDBBTSSARH_MASK)
#define HD64461_LCDBBTSSARL(x)	((x) & HD64461_LCDBBTSSARL_MASK)

/* Destination Start Address Register (19 bit) */
#define HD64461_LCDBBTDSARH_REG16		0xb0001058
#define HD64461_LCDBBTDSARH_SHIFT	16
#define HD64461_LCDBBTDSARH_MASK	0x0007
#define HD64461_LCDBBTDSARL_REG16		0xb000105a
#define HD64461_LCDBBTDSARL_MASK	0xffff
#define HD64461_LCDBBTDSARH(x)						\
	(((x) >> HD64461_LCDBBTSSARH_SHIFT) & HD64461_LCDBBTSSARH_MASK)
#define HD64461_LCDBBTDSARL(x)	((x) & HD64461_LCDBBTSSARL_MASK)

/* Destination Block Width Register */
#define HD64461_LCDBBTDWR_REG16			0xb000105c
#define HD64461_LCDBBTDWR_MASK		0x07ff

/* Destination Block Height Register */
#define HD64461_LCDBBTDHR_REG16			0xb000105e
#define HD64461_LCDBBTDHR_MASK		0x07ff

/* Pattern Start Address Register (19 bit) */
#define HD64461_LCDBBTPARH_REG16		0xb0001060
#define HD64461_LCDBBTPARH_MASK		0x0007
#define HD64461_LCDBBTPARL_REG16		0xb0001062

/* Mask Start Address Register (19 bit) */
#define HD64461_LCDBBTMARH_REG16		0xb0001064
#define HD64461_LCDBBTMARH_MASK		0x0007
#define HD64461_LCDBBTMARL_REG16		0xb0001066

/* ROP Register */
#define HD64461_LCDBBTROPR_REG16		0xb0001068

/* BitBLT Mode Register */
#define HD64461_LCDBBTMDR_REG16			0xb000106a
#define HD64461_LCDBBTMDR_MASKENABLE		0x0020
#define HD64461_LCDBBTMDR_PATSELECT_SOLIDCOLOR	0x0010
#define HD64461_LCDBBTMDR_SHIFT		2
#define HD64461_LCDBBTMDR_MASK		0x3
#define HD64461_LCDBBTMDR(cr)						\
	(((cr) >> HD64461_LCDBBTMDR_SHIFT) & HD64461_LCDBBTMDR_MASK)
	
#define HD64461_LCDBBTMDR_SET(cr, val)					\
	((cr) | (((val) << HD64461_LCDBBTMDR_SHIFT) &			\
	(HD64461_LCDBBTMDR_MASK << HD64461_LCDBBTMDR_SHIFT)))
#define HD64461_LCDBBTMDR_ON_SCREEN_TO_ON_SCREEN	0
#define HD64461_LCDBBTMDR_ON_SCREEN_TO_OFF_SCREEN	1
#define HD64461_LCDBBTMDR_OFF_SCREEN_TO_ON_SCREEN	3

#define HD64461_LCDBBTMDR_SCANDRCT	0x0001
#define HD64461_LCDBBTMDR_SCANDRCT_RL_BT	0x1
#define HD64461_LCDBBTMDR_SCANDRCT_LR_TB	0x0

/*
 * BitBLT Function
 */
#define HD64461_LCDC_BITBLT_SRCAND		0x0088
#define HD64461_LCDC_BITBLT_SRCCOPY		0x00cc
#define HD64461_LCDC_BITBLT_SRCINVERT		0x0066
#define HD64461_LCDC_BITBLT_SRCPAINT		0x00ee
#define HD64461_LCDC_BITBLT_PATCOPY		0x00f0
#define HD64461_LCDC_BITBLT_PATINVERT		0x005a
#define HD64461_LCDC_BITBLT_DSTINVERT		0x0055
#define HD64461_LCDC_BITBLT_BLACKNESS		0x0000
#define HD64461_LCDC_BITBLT_WHITENESS		0x00ff
#define HD64461_LCDC_BITBLT_MASKEDSRCCOPY	0xccaa

#endif /* !_HPCSH_DEV_HD64461VIDEOREG_H_ */
