/*	$NetBSD: cmds.c,v 1.2 2000/06/16 23:17:41 explorer Exp $	*/

/*
 * Copyright (c) 1999-2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Luke Mewburn.
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

/*
 * Copyright (c) 1985, 1988, 1990, 1992, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (C) 1997 and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: cmds.c,v 1.2 2000/06/16 23:17:41 explorer Exp $");
#endif /* not lint */

#include <sys/param.h>
#include <sys/stat.h>

#include <arpa/ftp.h>

#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tzfile.h>
#include <unistd.h>

#ifdef KERBEROS5
#include <krb5/krb5.h>
#endif

#include "extern.h"

typedef struct {
	const char	*path;		/* full pathname */
	const char	*display;	/* name to display */
	struct stat	*stat;		/* stat of path */
	struct stat	*pdirstat;	/* stat of path's parent dir */
	int		 iscurdir;	/* nonzero if name is the current dir */
} factelem;

static void	ack(const char *);
static void	fact_type(const char *, FILE *, factelem *);
static void	fact_size(const char *, FILE *, factelem *);
static void	fact_modify(const char *, FILE *, factelem *);
static void	fact_perm(const char *, FILE *, factelem *);
static void	fact_unique(const char *, FILE *, factelem *);
static void	fact_unix_group(const char *, FILE *, factelem *);
static void	fact_unix_mode(const char *, FILE *, factelem *);
static void	fact_unix_owner(const char *, FILE *, factelem *);
static int	matchgroup(gid_t, gid_t *, int);
static void	mlsname(FILE *, factelem *);
static void	replydirname(const char *, const char *);

struct ftpfact {
	const char	 *name;		/* name of fact */
	int		  enabled;	/* if fact is enabled */
	void		(*display)(const char *, FILE *, factelem *);
					/* function to display fact */
};

struct ftpfact facttab[] = {
	{ "Type",	1, fact_type },
	{ "Size",	1, fact_size },
	{ "Modify",	1, fact_modify },
	{ "Perm",	1, fact_perm },
	{ "Unique",	1, fact_unique },
	{ "UNIX.mode",	1, fact_unix_mode },
	{ "UNIX.owner",	0, fact_unix_owner },
	{ "UNIX.group",	0, fact_unix_group },
	/* "Create" */
	/* "Lang" */
	/* "Media-Type" */
	/* "CharSet" */
	{ NULL, NULL, },
};


void
cwd(const char *path)
{

	if (chdir(path) < 0)
		perror_reply(550, path);
	else {
		show_chdir_messages(250);
		ack("CWD");
	}
}

void
delete(const char *name)
{
	char *p = NULL;

	if (remove(name) < 0) {
		p = strerror(errno);
		perror_reply(550, name);
	} else
		ack("DELE");
	logcmd("delete", -1, name, NULL, NULL, p);
}

void
feat(void)
{
	int i;

	lreply(211, "Features supported");
	lreply(-1,  " MDTM");
	lreply(-2,  " MLST ");
	for (i = 0; facttab[i].name; i++)
		lreply(-2, "%s%s;", facttab[i].name,
		    facttab[i].enabled ? "*" : "");
	lreply(-1, "");
	lreply(-1,  " REST STREAM");
	lreply(-1,  " SIZE");
	lreply(-1,  " TVFS");
	reply(211,  "End");
}

void
makedir(const char *name)
{
	char *p = NULL;

	if (mkdir(name, 0777) < 0) {
		p = strerror(errno);
		perror_reply(550, name);
	} else
		replydirname(name, "directory created.");
	logcmd("mkdir", -1, name, NULL, NULL, p);
}

void
mlsd(const char *path)
{
	FILE *dout;
	DIR *dirp;
	struct dirent *dp;
	struct stat sb, pdirstat;
	char name[MAXPATHLEN];
	factelem f;

	if (path == NULL)
		path = ".";
	if (stat(path, &pdirstat) == -1) {
 mlsdperror:
		perror_reply(550, path);
		return;
	}
	if (! S_ISDIR(pdirstat.st_mode)) {
		errno = ENOTDIR;
		goto mlsdperror;
	}
#ifdef DEBUG		/* send mlsd to ctrl connection not data connection */
	dout = stdout;
	lreply(250, "MLSD %s", path);
#else
	dout = dataconn("MLSD", (off_t)-1, "w");
	if (dout == NULL)
		return;
#endif

	if ((dirp = opendir(path)) == NULL)
		goto mlsdperror;
	f.stat = &sb;
	while ((dp = readdir(dirp)) != NULL) {
		snprintf(name, sizeof(name), "%s/%s", path, dp->d_name);
/* printf("got >%s< >%s<\n", dp->d_name, name); */
		if (stat(name, &sb) == -1)
			continue;
		f.path = name;
		if (ISDOTDIR(dp->d_name)) {	/* special case curdir: */
			f.display = path;	/*   set name to real name */
			f.iscurdir = 1;		/*   flag name is curdir */
			f.pdirstat = NULL;	/*   require stat of parent */
		} else {
			f.display = dp->d_name;
			f.iscurdir = 0;
			if (ISDOTDOTDIR(dp->d_name))
				f.pdirstat = NULL;
			else
				f.pdirstat = &pdirstat;	/* cache parent stat */
		}
		mlsname(dout, &f);
	}
	(void)closedir(dirp);

#ifdef DEBUG
	reply(250, "End");
#else
	if (ferror(dout) != 0)
		perror_reply(550, "Data connection");
	else
		reply(226, "MLSD complete.");
	(void) fclose(dout);
	total_xfers_out++;
	total_xfers++;
#endif
}

void
mlst(const char *path)
{
	struct stat sb;
	factelem f;

	if (path == NULL)
		path = ".";
	if (stat(path, &sb) == -1) {
		perror_reply(550, path);
		return;
	}
	lreply(250, "MLST %s", path);
	f.path = path;
	f.display = path;
	f.stat = &sb;
	f.pdirstat = NULL;
	f.iscurdir = 0;
	mlsname(stdout, &f);
	reply(250, "End");
}


void
opts(const char *command)
{
	struct tab *c;
	char *ep;

	if ((ep = strchr(command, ' ')) != NULL)
		*ep++ = '\0';
	c = lookup(cmdtab, command);
	if (c == NULL) {
		reply(502, "Unknown command %s.", command);
		return;
	}
	if (! CMD_IMPLEMENTED(c)) {
		reply(501, "%s command not implemented.", c->name);
		return;
	}
	if (! CMD_HAS_OPTIONS(c)) {
		reply(501, "%s command does not support persistent options.",
		    c->name);
		return;
	}

			/* special case: MLST */
	if (strcasecmp(command, "MLST") == 0) {
		int	 i, onedone;
		char	*p;

		for (i = 0; facttab[i].name; i++)
			facttab[i].enabled = 0;
		if (ep == NULL || *ep == '\0')
			goto displaymlstopts;

				/* don't like spaces, and need trailing ; */
		if (strchr(ep, ' ') != NULL || ep[strlen(ep) - 1] != ';') {
			reply(501, "Invalid MLST options");
			return;
		}
		while ((p = strsep(&ep, ";")) != NULL) {
			for (i = 0; facttab[i].name; i++)
				if (strcasecmp(p, facttab[i].name) == 0) {
					facttab[i].enabled = 1;
					break;
				}
		}

 displaymlstopts:
		lreply(-2, "200 MLST OPTS");
		for (i = onedone = 0; facttab[i].name; i++) {
			if (facttab[i].enabled) {
				lreply(-2, "%s%s;", onedone ? "" : " ",
				    facttab[i].name);
				onedone++;
			}
		}
		lreply(-1, "");
		return;
	}

			/* default cases */
	if (ep != NULL && *ep != '\0')
		REASSIGN(c->options, xstrdup(ep));
	if (c->options != NULL)
		reply(200, "Options for %s are '%s'.", c->name,
		    c->options);
	else
		reply(200, "No options defined for %s.", c->name);
}

void
pwd(void)
{
	char path[MAXPATHLEN];

	if (getcwd(path, sizeof(path) - 1) == NULL)
		reply(550, "Can't get the current directory: %s.",
		    strerror(errno));
	else
		replydirname(path, "is the current directory.");
}

void
removedir(const char *name)
{
	char *p = NULL;

	if (rmdir(name) < 0) {
		p = strerror(errno);
		perror_reply(550, name);
	} else
		ack("RMD");
	logcmd("rmdir", -1, name, NULL, NULL, p);
}

char *
renamefrom(const char *name)
{
	struct stat st;

	if (stat(name, &st) < 0) {
		perror_reply(550, name);
		return (NULL);
	}
	reply(350, "File exists, ready for destination name");
	return (xstrdup(name));
}

void
renamecmd(const char *from, const char *to)
{
	char *p = NULL;

	if (rename(from, to) < 0) {
		p = strerror(errno);
		perror_reply(550, "rename");
	} else
		ack("RNTO");
	logcmd("rename", -1, from, to, NULL, p);
}

void
sizecmd(const char *filename)
{
	switch (type) {
	case TYPE_L:
	case TYPE_I: {
		struct stat stbuf;
		if (stat(filename, &stbuf) < 0 || !S_ISREG(stbuf.st_mode))
			reply(550, "%s: not a plain file.", filename);
		else
			reply(213, "%qu", (qufmt_t)stbuf.st_size);
		break; }
	case TYPE_A: {
		FILE *fin;
		int c;
		off_t count;
		struct stat stbuf;
		fin = fopen(filename, "r");
		if (fin == NULL) {
			perror_reply(550, filename);
			return;
		}
		if (fstat(fileno(fin), &stbuf) < 0 || !S_ISREG(stbuf.st_mode)) {
			reply(550, "%s: not a plain file.", filename);
			(void) fclose(fin);
			return;
		}

		count = 0;
		while((c=getc(fin)) != EOF) {
			if (c == '\n')	/* will get expanded to \r\n */
				count++;
			count++;
		}
		(void) fclose(fin);

		reply(213, "%qd", (qdfmt_t)count);
		break; }
	default:
		reply(504, "SIZE not implemented for Type %c.", "?AEIL"[type]);
	}
}

void
statfilecmd(const char *filename)
{
	FILE *fin;
	int c;
	char *argv[] = { INTERNAL_LS, "-lgA", "", NULL };

	argv[2] = (char *)filename;
	fin = ftpd_popen(argv, "r", STDOUT_FILENO);
	lreply(211, "status of %s:", filename);
	while ((c = getc(fin)) != EOF) {
		if (c == '\n') {
			if (ferror(stdout)){
				perror_reply(421, "control connection");
				(void) ftpd_pclose(fin);
				dologout(1);
				/* NOTREACHED */
			}
			if (ferror(fin)) {
				perror_reply(551, filename);
				(void) ftpd_pclose(fin);
				return;
			}
			(void) putchar('\r');
			total_bytes++;
			total_bytes_out++;
		}
		(void) putchar(c);
		total_bytes++;
		total_bytes_out++;
	}
	(void) ftpd_pclose(fin);
	reply(211, "End of Status");
}

/* -- */

static void
ack(const char *s)
{

	reply(250, "%s command successful.", s);
}

static void
fact_modify(const char *fact, FILE *fd, factelem *fe)
{
	struct tm *t;

	t = gmtime(&(fe->stat->st_mtime));
	fprintf(fd, "%s=%04d%02d%02d%02d%02d%02d;", fact,
	    TM_YEAR_BASE + t->tm_year,
	    t->tm_mon+1, t->tm_mday,
	    t->tm_hour, t->tm_min, t->tm_sec);
}

static void
fact_perm(const char *fact, FILE *fd, factelem *fe)
{
	int		rok, wok, xok, pdirwok, ngid;
	struct stat	*pdir;
	gid_t		gids[NGROUPS_MAX];

	ngid = getgroups(sizeof(gids), gids);
	if (fe->stat->st_uid == geteuid()) {
		rok = ((fe->stat->st_mode & S_IRUSR) != 0);
		wok = ((fe->stat->st_mode & S_IWUSR) != 0);
		xok = ((fe->stat->st_mode & S_IXUSR) != 0);
	} else if (matchgroup(fe->stat->st_gid, gids, ngid)) {
		rok = ((fe->stat->st_mode & S_IRGRP) != 0);
		wok = ((fe->stat->st_mode & S_IWGRP) != 0);
		xok = ((fe->stat->st_mode & S_IXGRP) != 0);
	} else {
		rok = ((fe->stat->st_mode & S_IROTH) != 0);
		wok = ((fe->stat->st_mode & S_IWOTH) != 0);
		xok = ((fe->stat->st_mode & S_IXOTH) != 0);
	}

	fprintf(fd, "%s=", fact);

			/*
			 * if parent info not provided, look it up, but
			 * only if the current class has modify rights,
			 * since we only need this info in such a case.
			 */
	pdir = fe->pdirstat;
	if (pdir == NULL && curclass.modify) {
		size_t		len;
		char		realdir[MAXPATHLEN], *p;
		struct stat	dir;

		len = strlcpy(realdir, fe->path, sizeof(realdir));
/* printf("[path=%s]", fe->path); */
		if (len < sizeof(realdir) - 4) {
			if (S_ISDIR(fe->stat->st_mode))
				strlcat(realdir, "/..", sizeof(realdir));
			else {
					/* if has a /, move back to it */
					/* otherwise use '..' */
				if ((p = strrchr(realdir, '/')) != NULL) {
					if (p == realdir)
						p++;
					*p = '\0';
				} else
					strlcpy(realdir, "..", sizeof(realdir));
			}
/* printf("[real=%s]", realdir); */
			if (stat(realdir, &dir) == 0)
				pdir = &dir;
		}
	}
	pdirwok = 0;
	if (pdir != NULL) {
		if (pdir->st_uid == geteuid())
			pdirwok = ((pdir->st_mode & S_IWUSR) != 0);
		else if (matchgroup(pdir->st_gid, gids, ngid))
			pdirwok = ((pdir->st_mode & S_IWGRP) != 0);
		else
			pdirwok = ((pdir->st_mode & S_IWOTH) != 0);
	}

/* printf("[euid=%d,r%d,w%d,x%d,pw%d]", geteuid(), rok, wok, xok, pdirwok);
*/

			/* 'a': can APPE to file */
	if (wok && curclass.upload && S_ISREG(fe->stat->st_mode))
		fputs("a", fd);

			/* 'c': can create or append to files in directory */
	if (wok && curclass.modify && S_ISDIR(fe->stat->st_mode))
		fputs("c", fd);

			/* 'd': can delete file or directory */
	if (pdirwok && curclass.modify) {
		int candel;

		candel = 1;
		if (S_ISDIR(fe->stat->st_mode)) {
			DIR *dirp;
			struct dirent *dp;

			if ((dirp = opendir(fe->display)) == NULL)
				candel = 0;
			else {
				while ((dp = readdir(dirp)) != NULL) {
					if (ISDOTDIR(dp->d_name) ||
					    ISDOTDOTDIR(dp->d_name))
						continue;
					candel = 0;
					break;
				}
				closedir(dirp);
			}
		}
		if (candel)
			fputs("d", fd);
	}

			/* 'e': can enter directory */
	if (xok && S_ISDIR(fe->stat->st_mode))
		fputs("e", fd);

			/* 'f': can rename file or directory */
	if (pdirwok && curclass.modify)
		fputs("f", fd);

			/* 'l': can list directory */
	if (rok && xok && S_ISDIR(fe->stat->st_mode))
		fputs("l", fd);

			/* 'm': can create directory */
	if (wok && curclass.modify && S_ISDIR(fe->stat->st_mode))
		fputs("m", fd);

			/* 'p': can remove files in directory */
	if (wok && curclass.modify && S_ISDIR(fe->stat->st_mode))
		fputs("p", fd);

			/* 'r': can RETR file */
	if (rok && S_ISREG(fe->stat->st_mode))
		fputs("r", fd);

			/* 'w': can STOR file */
	if (wok && curclass.upload && S_ISREG(fe->stat->st_mode))
		fputs("w", fd);

	fputc(';', fd);
}

static void
fact_size(const char *fact, FILE *fd, factelem *fe)
{

	if (S_ISREG(fe->stat->st_mode))
		fprintf(fd, "%s=%lld;", fact, (long long)fe->stat->st_size);
}

static void
fact_type(const char *fact, FILE *fd, factelem *fe)
{

	fprintf(fd, "%s=", fact);
	switch (fe->stat->st_mode & S_IFMT) {
	case S_IFDIR:
		if (fe->iscurdir || ISDOTDIR(fe->display))
			fputs("cdir", fd);
		else if (ISDOTDOTDIR(fe->display))
			fputs("pdir", fd);
		else
			fputs("dir", fd);
		break;
	case S_IFREG:
		fputs("file", fd);
		break;
	case S_IFIFO:
		fputs("OS.unix=fifo", fd);
		break;
	case S_IFLNK:
		fputs("OS.unix=slink", fd);
		break;
	case S_IFSOCK:
		fputs("OS.unix=socket", fd);
		break;
	case S_IFBLK:
	case S_IFCHR:
		fprintf(fd, "OS.unix=%s-%d/%d",
		    S_ISBLK(fe->stat->st_mode) ? "blk" : "chr",
		    major(fe->stat->st_rdev), minor(fe->stat->st_rdev));
		break;
	default:
		fprintf(fd, "OS.unix=UNKNOWN(0%o)", fe->stat->st_mode & S_IFMT);
		break;
	}
	fputc(';', fd);
}

static void
fact_unique(const char *fact, FILE *fd, factelem *fe)
{

	fprintf(fd, "%s=%04x%08x;", fact, fe->stat->st_dev, fe->stat->st_ino);
}

static void
fact_unix_mode(const char *fact, FILE *fd, factelem *fe)
{

	fprintf(fd, "%s=%03o;", fact, fe->stat->st_mode & ACCESSPERMS);
}

static void
fact_unix_owner(const char *fact, FILE *fd, factelem *fe)
{

	fprintf(fd, "%s=%d;", fact, (int)fe->stat->st_uid);
}

static void
fact_unix_group(const char *fact, FILE *fd, factelem *fe)
{

	fprintf(fd, "%s=%d;", fact, (int)fe->stat->st_gid);
}

static int
matchgroup(gid_t gid, gid_t *gidlist, int ngids)
{
	int	i;

	for (i = 0; i < ngids; i++)
		if (gid == gidlist[i])
			return(1);
	return (0);
}

static void
mlsname(FILE *fp, factelem *fe)
{
	int i;

	fputs(" ", fp);
	for (i = 0; facttab[i].name; i++) {
		if (facttab[i].enabled)
			(facttab[i].display)(facttab[i].name, fp, fe);
	}
	fprintf(fp, " %s\r\n", fe->display);
}

static void
replydirname(const char *name, const char *message)
{
	char npath[MAXPATHLEN];
	int i;

	for (i = 0; *name != '\0' && i < sizeof(npath) - 1; i++, name++) {
		npath[i] = *name;
		if (*name == '"')
			npath[++i] = '"';
	}
	npath[i] = '\0';
	reply(257, "\"%s\" %s", npath, message);
}
