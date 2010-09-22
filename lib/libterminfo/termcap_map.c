/* $NetBSD: termcap_map.c,v 1.3 2010/09/22 06:10:51 roy Exp $ */

/*
 * Copyright (c) 2009 The NetBSD Foundation, Inc.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Roy Marples.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD: termcap_map.c,v 1.3 2010/09/22 06:10:51 roy Exp $");

typedef struct {
	const char id[3];
	int ti;
} TENTRY;

static const TENTRY _ti_cap_flagids[] = {
	{ "bw", TICODE_bw },
	{ "am", TICODE_am },
	{ "ut", TICODE_bce },
	{ "cc", TICODE_ccc },
	{ "xs", TICODE_xhp },
	{ "YA", TICODE_xhpa },
	{ "YF", TICODE_cpix },
	{ "YB", TICODE_crxm },
	{ "xt", TICODE_xt },
	{ "xn", TICODE_xenl },
	{ "eo", TICODE_eo },
	{ "gn", TICODE_gn },
	{ "hc", TICODE_hc },
	{ "HC", TICODE_chts },
	{ "km", TICODE_km },
	{ "YC", TICODE_daisy },
	{ "hs", TICODE_hs },
	{ "hl", TICODE_hls },
	{ "in", TICODE_in },
	{ "YG", TICODE_lpix },
	{ "da", TICODE_da },
	{ "db", TICODE_db },
	{ "mi", TICODE_mir },
	{ "ms", TICODE_msgr },
	{ "nx", TICODE_nxon },
	{ "xb", TICODE_xsb },
	{ "NP", TICODE_npc },
	{ "ND", TICODE_ndscr },
	{ "NR", TICODE_nrrmc },
	{ "os", TICODE_os } ,
	{ "5i", TICODE_mc5i },
	{ "YD", TICODE_xvpa },
	{ "YE", TICODE_sam },
	{ "es", TICODE_eslok },
	{ "hz", TICODE_hz },
	{ "ul", TICODE_ul },
	{ "xo", TICODE_xon }
};

static const TENTRY _ti_cap_numids[] = {
	{ "Yo", TICODE_bitwin },
	{ "Yp", TICODE_bitype },
	{ "Ya", TICODE_bufsz },
	{ "BT", TICODE_btns },
	{ "co", TICODE_cols },
	{ "Yc", TICODE_spinh },
	{ "Yb", TICODE_spinv },
	{ "it", TICODE_it },
	{ "lh", TICODE_lh },
	{ "lw", TICODE_lw },
	{ "li", TICODE_lines },
	{ "lm", TICODE_lm },
	{ "ma", TICODE_ma },
	{ "sg", TICODE_xmc },
	{ "Co", TICODE_colors },
	{ "Yd", TICODE_maddr },
	{ "Ye", TICODE_mjump },
	{ "pa", TICODE_pairs },
	{ "MW", TICODE_wnum },
	{ "Yf", TICODE_mcs },
	{ "Yg", TICODE_mls },
	{ "NC", TICODE_ncv },
	{ "Nl", TICODE_nlab },
	{ "Yh", TICODE_npins },
	{ "Yi", TICODE_orc },
	{ "Yj", TICODE_orl },
	{ "Yk", TICODE_orhi },
	{ "Yl", TICODE_orvi },
	{ "pb", TICODE_pb },
	{ "Ym", TICODE_cps },
	{ "vt", TICODE_vt },
	{ "Yn", TICODE_widcs },
	{ "ws", TICODE_wsl }
};

static const TENTRY _ti_cap_strids[] = {
	{ "ac", TICODE_acsc },
	{ "S8", TICODE_scesa },
	{ "bt", TICODE_cbt },
	{ "bl", TICODE_bel },
	{ "Yv", TICODE_bicr },
	{ "Zz", TICODE_binel },
	{ "Xy", TICODE_birep },
	{ "cr", TICODE_cr },
	{ "ZA", TICODE_cpi },
	{ "ZB", TICODE_lpi },
	{ "ZC", TICODE_chr },
	{ "ZD", TICODE_cvr },
	{ "cs", TICODE_csr },
	{ "rP", TICODE_rmp },
	{ "Zy", TICODE_csnm },
	{ "ct", TICODE_tbc },
	{ "MC", TICODE_mgc },
	{ "cl", TICODE_clear },
	{ "cb", TICODE_el1 },
	{ "ce", TICODE_el },
	{ "cd", TICODE_ed },
	{ "ci", TICODE_csin },
	{ "Yw", TICODE_colornm },
	{ "ch", TICODE_hpa },
	{ "CC", TICODE_cmdch },
	{ "cm", TICODE_cup },
	{ "do", TICODE_cud1 },
	{ "ho", TICODE_home },
	{ "vi", TICODE_civis },
	{ "le", TICODE_cub1 },
	{ "CM", TICODE_mrcup },
	{ "ve", TICODE_cnorm },
	{ "nd", TICODE_cuf1 },
	{ "ll", TICODE_ll },
	{ "up", TICODE_cuu1 },
	{ "vs", TICODE_cvvis },
	{ "Yx", TICODE_defbi },
	{ "ZE", TICODE_defc },
	{ "dc", TICODE_dch1 },
	{ "dl", TICODE_dl1 },
	{ "dv", TICODE_devt },
	{ "DI", TICODE_dial },
	{ "ds", TICODE_dsl },
	{ "DK", TICODE_dclk },
	{ "S1", TICODE_dispc },
	{ "hd", TICODE_hd },
	{ "eA", TICODE_enacs },
	{ "Yy", TICODE_endbi },
	{ "as", TICODE_smacs },
	{ "SA", TICODE_smam },
	{ "mb", TICODE_blink },
	{ "md", TICODE_bold },
	{ "ti", TICODE_smcup },
	{ "dm", TICODE_smdc },
	{ "mh", TICODE_dim },
	{ "ZF", TICODE_swidm },
	{ "ZG", TICODE_sdrfq },
	{ "Xh", TICODE_ehhlm },
	{ "im", TICODE_smir },
	{ "ZH", TICODE_sitm },
	{ "Xl", TICODE_elhlm },
	{ "ZI", TICODE_slm },
	{ "Xo", TICODE_elohlm },
	{ "ZJ", TICODE_smicm },
	{ "ZK", TICODE_snlq },
	{ "ZL", TICODE_snrmq },
	{ "S2", TICODE_smpch },
	{ "mp", TICODE_prot },
	{ "mr", TICODE_rev },
	{ "Xr", TICODE_erhlm },
	{ "S4", TICODE_smsc },
	{ "mk", TICODE_invis },
	{ "ZM", TICODE_sshm },
	{ "so", TICODE_smso },
	{ "ZN", TICODE_ssubm },
	{ "ZO", TICODE_ssupm },
	{ "Xt", TICODE_ethlm },
	{ "us", TICODE_smul },
	{ "ZP", TICODE_sum },
	{ "Xv", TICODE_evhlm },
	{ "SX", TICODE_smxon },
	{ "ec", TICODE_ech },
	{ "ae", TICODE_rmacs },
	{ "RA", TICODE_rmam },
	{ "me", TICODE_sgr0 },
	{ "te", TICODE_rmcup },
	{ "ed", TICODE_rmdc },
	{ "ZQ", TICODE_rwidm },
	{ "ei", TICODE_rmir },
	{ "ZR", TICODE_ritm },
	{ "ZS", TICODE_rlm },
	{ "ZT", TICODE_rmicm },
	{ "S3", TICODE_rmpch },
	{ "S5", TICODE_rmsc },
	{ "ZU", TICODE_rshm },
	{ "se", TICODE_rmso },
	{ "ZV", TICODE_rsubm },
	{ "ZW", TICODE_rsupm },
	{ "ue", TICODE_rmul },
	{ "ZX", TICODE_rum },
	{ "RX", TICODE_rmxon },
	{ "PA", TICODE_pause },
	{ "fh", TICODE_hook },
	{ "vb", TICODE_flash },
	{ "ff", TICODE_ff },
	{ "fs", TICODE_fsl },
	{ "Gm", TICODE_getm },
	{ "WG", TICODE_wingo },
	{ "HU", TICODE_hup },
	{ "i1", TICODE_is1 },
	{ "i2", TICODE_is2 },
	{ "i3", TICODE_is3 },
	{ "is", TICODE_is2 },
	{ "if", TICODE_if },
	{ "iP", TICODE_iprog },
	{ "Ic", TICODE_initc },
	{ "Ip", TICODE_initp },
	{ "ic", TICODE_ich1 },
	{ "al", TICODE_il1 },
	{ "ip", TICODE_ip },
	{ "K1", TICODE_ka1 },
	{ "K3", TICODE_ka3 },
	{ "K2", TICODE_kb2 },
	{ "kb", TICODE_kbs },
	{ "kB", TICODE_kcbt },
	{ "K4", TICODE_kc1 },
	{ "K5", TICODE_kc3 },
	{ "ka", TICODE_ktbc },
	{ "kC", TICODE_kclr },
	{ "kt", TICODE_kctab },
	{ "kD", TICODE_kdch1 },
	{ "kL", TICODE_kdl1 },
	{ "kd", TICODE_kcud1 },
	{ "kM", TICODE_krmir },
	{ "kE", TICODE_kel },
	{ "kS", TICODE_ked },
	{ "k0", TICODE_kf0 },
	{ "k1", TICODE_kf1 },
	{ "k2", TICODE_kf2 },
	{ "k3", TICODE_kf3 },
	{ "k4", TICODE_kf4 },
	{ "k5", TICODE_kf5 },
	{ "k6", TICODE_kf6 },
	{ "k7", TICODE_kf7 },
	{ "k8", TICODE_kf8 },
	{ "k9", TICODE_kf9 },
	{ "k;", TICODE_kf10 },
	{ "F1", TICODE_kf11 },
	{ "F2", TICODE_kf12 },
	{ "F3", TICODE_kf13 },
	{ "F4", TICODE_kf14 },
	{ "F5", TICODE_kf15 },
	{ "F6", TICODE_kf16 },
	{ "F7", TICODE_kf17 },
	{ "F8", TICODE_kf18 },
	{ "F9", TICODE_kf19 },
	{ "FA", TICODE_kf20 },
	{ "FB", TICODE_kf21 },
	{ "FC", TICODE_kf22 },
	{ "FD", TICODE_kf23 },
	{ "FE", TICODE_kf24 },
	{ "FF", TICODE_kf25 },
	{ "FG", TICODE_kf26 },
	{ "FH", TICODE_kf27 },
	{ "FI", TICODE_kf28 },
	{ "FJ", TICODE_kf29 },
	{ "FK", TICODE_kf30 },
	{ "FL", TICODE_kf31 },
	{ "FM", TICODE_kf32 },
	{ "FN", TICODE_kf33 },
	{ "FO", TICODE_kf34 },
	{ "FP", TICODE_kf35 },
	{ "FQ", TICODE_kf36 },
	{ "FR", TICODE_kf37 },
	{ "FS", TICODE_kf38 },
	{ "FT", TICODE_kf39 },
	{ "FU", TICODE_kf40 },
	{ "FV", TICODE_kf41 },
	{ "FW", TICODE_kf42 },
	{ "FX", TICODE_kf43 },
	{ "FY", TICODE_kf44 },
	{ "FZ", TICODE_kf45 },
	{ "Fa", TICODE_kf46 },
	{ "Fb", TICODE_kf47 },
	{ "Fc", TICODE_kf48 },
	{ "Fd", TICODE_kf49 },
	{ "Fe", TICODE_kf50 },
	{ "Ff", TICODE_kf51 },
	{ "Fg", TICODE_kf52 },
	{ "Fh", TICODE_kf53 },
	{ "Fi", TICODE_kf54 },
	{ "Fj", TICODE_kf55 },
	{ "Fk", TICODE_kf56 },
	{ "Fl", TICODE_kf57 },
	{ "Fm", TICODE_kf58 },
	{ "Fn", TICODE_kf59 },
	{ "Fo", TICODE_kf60 },
	{ "Fp", TICODE_kf61 },
	{ "Fq", TICODE_kf62 },
	{ "Fr", TICODE_kf63 },
	{ "%1", TICODE_khlp },
	{ "kh", TICODE_khome },
	{ "kI", TICODE_kich1 },
	{ "kA", TICODE_kil1 },
	{ "kl", TICODE_kcub1 },
	{ "kH", TICODE_kll },
	{ "%2", TICODE_kmrk },
	{ "%3", TICODE_kmsg },
	{ "Km", TICODE_kmous },
	{ "%4", TICODE_kmov },
	{ "%5", TICODE_knxt },
	{ "kN", TICODE_knp },
	{ "%6", TICODE_kopn },
	{ "%7", TICODE_kopt },
	{ "kP", TICODE_kpp },
	{ "%8", TICODE_kprv },
	{ "%9", TICODE_kprt },
	{ "%0", TICODE_krdo },
	{ "&1", TICODE_kref },
	{ "&2", TICODE_krfr },
	{ "&3", TICODE_krpl },
	{ "&4", TICODE_krst },
	{ "&5", TICODE_kres },
	{ "kr", TICODE_kcuf1 },
	{ "&6", TICODE_ksav },
	{ "&9", TICODE_kBEG },
	{ "&0", TICODE_kCAN },
	{ "*1", TICODE_kCMD },
	{ "*2", TICODE_kCPY },
	{ "*3", TICODE_kCRT },
	{ "*4", TICODE_kDC },
	{ "*5", TICODE_kDL },
	{ "*6", TICODE_kslt },
	{ "*7", TICODE_kEND },
	{ "*8", TICODE_kEOL },
	{ "*9", TICODE_kEXT },
	{ "kF", TICODE_kind },
	{ "*0", TICODE_kFND },
	{ "#1", TICODE_kHLP },
	{ "#2", TICODE_kHOM },
	{ "#3", TICODE_kIC },
	{ "#4", TICODE_kLFT },
	{ "%a", TICODE_kMSG },
	{ "%b", TICODE_kMOV },
	{ "%c", TICODE_kNXT },
	{ "%d", TICODE_kOPT },
	{ "%e", TICODE_kPRV },
	{ "%f", TICODE_kPRT },
	{ "kR", TICODE_kri },
	{ "%g", TICODE_kRDO },
	{ "%h", TICODE_kRPL },
	{ "%i", TICODE_kRIT },
	{ "%j", TICODE_kRES },
	{ "!1", TICODE_kSAV },
	{ "!2", TICODE_kSPD },
	{ "kT", TICODE_khts },
	{ "!3", TICODE_kUND },
	{ "&7", TICODE_kspd },
	{ "&8", TICODE_kund },
	{ "ku", TICODE_kcuu1 },
	{ "ke", TICODE_rmkx },
	{ "ks", TICODE_smkx },
	{ "l0", TICODE_lf0 },
	{ "l1", TICODE_lf1 },
	{ "l2", TICODE_lf2 },
	{ "l3", TICODE_lf3 },
	{ "l4", TICODE_lf4 },
	{ "l5", TICODE_lf5 },
	{ "l6", TICODE_lf6 },
	{ "l7", TICODE_lf7 },
	{ "l8", TICODE_lf8 },
	{ "l9", TICODE_lf9 },
	{ "la", TICODE_lf10 },
	{ "Lf", TICODE_fln },
	{ "LF", TICODE_rmln },
	{ "LO", TICODE_smln },
	{ "mo", TICODE_rmm },
	{ "mm", TICODE_smm },
	{ "ZY", TICODE_mhpa },
	{ "ZZ", TICODE_mcud1 },
	{ "Za", TICODE_mcub1 },
	{ "Zb", TICODE_mcuf1 },
	{ "Zc", TICODE_mvpa },
	{ "Zd", TICODE_mcuu1 },
	{ "Mi", TICODE_minfo },
	{ "nw", TICODE_nel },
	{ "Ze", TICODE_porder },
	{ "oc", TICODE_oc },
	{ "op", TICODE_op },
	{ "pc", TICODE_pad },
	{ "DC", TICODE_dch },
	{ "DL", TICODE_dl },
	{ "DO", TICODE_cud },
	{ "Zf", TICODE_mcud },
	{ "IC", TICODE_ich },
	{ "SF", TICODE_indn },
	{ "AL", TICODE_il },
	{ "LE", TICODE_cub },
	{ "Zg", TICODE_mcub },
	{ "RI", TICODE_cuf },
	{ "Zh", TICODE_mcuf },
	{ "SR", TICODE_rin },
	{ "UP", TICODE_cuu },
	{ "Zi", TICODE_mcuu },
	{ "S6", TICODE_pctrm },
	{ "pk", TICODE_pfkey },
	{ "pl", TICODE_pfloc },
	{ "xl", TICODE_pfxl },
	{ "px", TICODE_pfx },
	{ "pn", TICODE_pln },
	{ "ps", TICODE_mc0 },
	{ "pO", TICODE_mc5p },
	{ "pf", TICODE_mc4 },
	{ "po", TICODE_mc5 },
	{ "PU", TICODE_pulse },
	{ "QD", TICODE_qdial },
	{ "RC", TICODE_rmclk },
	{ "rp", TICODE_rep },
	{ "RF", TICODE_rfi },
	{ "RQ", TICODE_reqmp },
	{ "r1", TICODE_rs1 },
	{ "r2", TICODE_rs2 },
	{ "r3", TICODE_rs3 },
	{ "rf", TICODE_rf },
	{ "rc", TICODE_rc },
	{ "cv", TICODE_vpa },
	{ "sc", TICODE_sc },
	{ "S7", TICODE_scesc },
	{ "sf", TICODE_ind },
	{ "sr", TICODE_ri },
	{ "Zj", TICODE_scs },
	{ "s0", TICODE_s0ds },
	{ "s1", TICODE_s1ds },
	{ "s2", TICODE_s2ds },
	{ "s3", TICODE_s3ds },
	{ "sA", TICODE_sgr1 },
	{ "AB", TICODE_setab },
	{ "AF", TICODE_setaf },
	{ "sa", TICODE_sgr },
	{ "Sb", TICODE_setb },
	{ "Zk", TICODE_smgb },
	{ "Zl", TICODE_smgbp },
	{ "SC", TICODE_sclk },
	{ "Yz", TICODE_slines },
	{ "sL", TICODE_slength },
	{ "sp", TICODE_scp },
	{ "Sf", TICODE_setf },
	{ "ML", TICODE_smgl }, /* We should fallback to TICODE_smglr */
	{ "Zm", TICODE_smglp },
	{ "YZ", TICODE_slines },
	{ "YI", TICODE_slength },
	{ "MR", TICODE_smgr },
	{ "Zn", TICODE_smgrp },
	{ "st", TICODE_hts },
	{ "MT", TICODE_smgtb },
	{ "Zo", TICODE_smgt },
	{ "Zp", TICODE_smgtp },
	{ "wi", TICODE_wind },
	{ "Zq", TICODE_sbim },
	{ "Zr", TICODE_scsd },
	{ "Zs", TICODE_rbim },
	{ "Zt", TICODE_rcsd },
	{ "Zu", TICODE_subcs },
	{ "Zv", TICODE_supcs },
	{ "ta", TICODE_ht },
	{ "Zw", TICODE_docr },
	{ "ts", TICODE_tsl },
	{ "TO", TICODE_tone },
	{ "u0", TICODE_u0 },
	{ "u1", TICODE_u1 },
	{ "u2", TICODE_u2 },
	{ "u3", TICODE_u3 },
	{ "u4", TICODE_u4 },
	{ "u5", TICODE_u5 },
	{ "u6", TICODE_u6 },
	{ "u7", TICODE_u7 },
	{ "u8", TICODE_u8 },
	{ "u9", TICODE_u9 },
	{ "uc", TICODE_uc },
	{ "hu", TICODE_hu },
	{ "WA", TICODE_wait },
	{ "XF", TICODE_xoffc },
	{ "XN", TICODE_xonc },
	{ "Zx", TICODE_zerom },

	/* NetBSD extensions */
	{ "@0", TICODE_kfnd },
	{ "@1", TICODE_kbeg },
	{ "@2", TICODE_kcan },
	{ "@3", TICODE_kclo },
	{ "@4", TICODE_kcmd },
	{ "@5", TICODE_kcpy },
	{ "@6", TICODE_kcrt },
	{ "@7", TICODE_kend },
	{ "@8", TICODE_kent },
	{ "@9", TICODE_kext },
};
