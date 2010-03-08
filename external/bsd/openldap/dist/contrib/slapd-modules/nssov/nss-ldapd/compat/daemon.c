/*	$NetBSD: daemon.c,v 1.1.1.2 2010/03/08 02:14:20 lukem Exp $	*/

/*
   daemon.c - implementation of daemon() for systems that lack it

   Copyright (C) 2002, 2003, 2008 Arthur de Jong

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA
*/

#include "daemon.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int daemon(int nochdir,int noclose)
{
  /* change directory */
  if (!nochdir)
    if (chdir("/")!=0)
      return -1;
  /* fork() and exit() to detach from the parent process */
  switch (fork())
  {
  case 0: /* we are the child */
    break;
  case -1: /* we are the parent, but have an error */
    return -1;
  default: /* we are the parent and we're done*/
    _exit(0);
  }
  /* become process leader */
  if (setsid()<0)
  {
    return -1;
  }
  /* fork again so we cannot allocate a pty */
  switch (fork())
  {
  case 0: /* we are the child */
    break;
  case -1: /* we are the parent, but have an error */
    return -1;
  default: /* we are the parent and we're done*/
    _exit(0);
  }
  /* close stdin, stdout and stderr and reconnect to /dev/null */
  if (!noclose)
  {
    close(0); /* stdin */
    close(1); /* stdout */
    close(2); /* stderr */
    open("/dev/null",O_RDWR); /* stdin, fd=0 */
    dup(0); /* stdout, fd=1 */
    dup(0); /* stderr, fd=2 */
  }
  return 0;
}
