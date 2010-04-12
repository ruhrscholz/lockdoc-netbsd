/*	$NetBSD: t_snapshot.c,v 1.1 2010/04/12 22:58:53 pooka Exp $	*/

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

#include <dev/fssvar.h>

#include <rump/rump.h>
#include <rump/rump_syscalls.h>

#include <ufs/ufs/ufsmount.h>

#include <atf-c.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../h_macros.h"

#define IMGNAME "ffs.img"
#define NEWFS "newfs -F -s 10000 " IMGNAME
#define BAKNAME "/mnt/bak.img"

ATF_TC_WITH_CLEANUP(snapshot);
ATF_TC_HEAD(snapshot, tc)
{

	atf_tc_set_md_var(tc, "descr", "basic snapshot features");
}

static void
makefile(const char *path)
{
	int fd;

	fd = rump_sys_open(path, O_CREAT | O_RDWR, 0777);
	if (fd == -1)
		atf_tc_fail_errno("create %s", path);
	rump_sys_close(fd);
}

ATF_TC_BODY(snapshot, tc)
{
	char buf[1024];
	struct ufs_args uargs;
	struct fss_set fss;
	int fssfd;
	int fd, fd2;

	if (system(NEWFS) == -1)
		atf_tc_fail_errno("cannot create file system");

	rump_init();

	if (rump_sys_mkdir("/mnt", 0777) == -1)
		atf_tc_fail_errno("mount point create");
	if (rump_sys_mkdir("/snap", 0777) == -1)
		atf_tc_fail_errno("mount point 2 create");

	rump_pub_etfs_register("./ffs.img", "./ffs.img", RUMP_ETFS_BLK);

	uargs.fspec = __UNCONST("./ffs.img");

	if (rump_sys_mount(MOUNT_FFS, "/mnt", 0, &uargs, sizeof(uargs)) == -1)
		atf_tc_fail_errno("mount ffs");

#define TESTSTR1 "huihai\n"
#define TESTSZ1 (sizeof(TESTSTR1)-1)
#define TESTSTR2 "baana liten\n"
#define TESTSZ2 (sizeof(TESTSTR2)-1)

	fd = rump_sys_open("/mnt/myfile", O_RDWR | O_CREAT, 0777);
	if (fd == -1)
		atf_tc_fail_errno("create file");
	if (rump_sys_write(fd, TESTSTR1, TESTSZ1) != TESTSZ1)
		atf_tc_fail_errno("write fail");

	fssfd = rump_sys_open("/dev/rfss0", O_RDWR);
	if (fd == -1)
		atf_tc_fail_errno("cannot open fss");
	makefile(BAKNAME);
	memset(&fss, 0, sizeof(fss));
	fss.fss_mount = __UNCONST("/mnt");
	fss.fss_bstore = __UNCONST(BAKNAME);
	fss.fss_csize = 0;
	if (rump_sys_ioctl(fssfd, FSSIOCSET, &fss) == -1)
		atf_tc_fail_errno("create snapshot");

	if (rump_sys_pwrite(fd, TESTSTR2, TESTSZ2, 0) != TESTSZ2)
		atf_tc_fail_errno("write fail");
	rump_sys_sync();

	uargs.fspec = __UNCONST("/dev/fss0");

	/* technically we should fsck it first? */
	if (rump_sys_mount(MOUNT_FFS, "/snap", 0, &uargs, sizeof(uargs)) == -1)
		atf_tc_fail_errno("mount snapshot");

	/* check for old contents */
	fd2 = rump_sys_open("/snap/myfile", O_RDONLY);
	if (fd2 == -1)
		atf_tc_fail_errno("fail");
	memset(buf, 0, sizeof(buf));
	if (rump_sys_read(fd2, buf, sizeof(buf)) == -1)
		atf_tc_fail_errno("read snap");

	/* check that new files are invisible in the snapshot */
	makefile("/mnt/newfile");
	if (rump_sys_open("/snap/newfile", O_RDONLY) != -1)
		atf_tc_fail("newfile exists in snapshot");
	if (errno != ENOENT)
		atf_tc_fail_errno("newfile open should fail with ENOENT");

	/* check that removed files are still visible in the snapshot */
	rump_sys_unlink("/mnt/myfile");

	if (rump_sys_open("/snap/myfile", O_RDONLY) == -1)
		atf_tc_fail_errno("unlinked file no longer in snapshot");

	ATF_CHECK(strcmp(buf, TESTSTR1) == 0);
}

ATF_TC_CLEANUP(snapshot, tc)
{

	unlink(IMGNAME);
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, snapshot);
	return 0;
}
