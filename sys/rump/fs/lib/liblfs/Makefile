#	$NetBSD: Makefile,v 1.14.22.1 2020/08/17 10:30:22 martin Exp $
#

.PATH:  ${.CURDIR}/../../../../ufs/lfs

LIB=	rumpfs_lfs
COMMENT=Log-structured File System

SRCS=	lfs_alloc.c lfs_balloc.c lfs_bio.c lfs_cksum.c lfs_debug.c	\
	lfs_inode.c lfs_itimes.c lfs_pages.c lfs_rename.c lfs_rfw.c	\
	lfs_segment.c lfs_subr.c lfs_syscalls.c lfs_vfsops.c lfs_vnops.c

SRCS+=	ulfs_bmap.c ulfs_dirhash.c ulfs_extattr.c 			\
	ulfs_inode.c ulfs_lookup.c ulfs_quota.c ulfs_quota1.c		\
	ulfs_quota2.c ulfs_quota1_subr.c ulfs_quota2_subr.c		\
	ulfs_snapshot.c ulfs_vfsops.c ulfs_vnops.c


CFLAGS+=        -DLFS_KERNEL_RFW
.if ${MACHINE_ARCH} == "vax"
COPTS.lfs_inode.c+=-O0
.endif

# Follow the suit of Makefile.kern.inc; needed for the lfs64 union
# accessors -- they don't actually dereference the resulting pointer,
# just use it for type-checking.
CWARNFLAGS.clang+=	-Wno-error=address-of-packed-member

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
