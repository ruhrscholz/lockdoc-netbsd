/*	$NetBSD: download.c,v 1.3 2001/08/20 12:00:47 wiz Exp $	*/

/*
   Hacked by Phil Nelson for use with NetBSD, 10/5/95.
   This source is in the public domain except for the makeraw function.

   The upload part of this program is taken from Bruce's ROM debugger
   code.

   For NetBSD, the usages are:

       download file 
              - open "file" and ship it out standard output with the
	        format expected by the pc532 ROM monitor download
		command.

       download -r nfile
              - standard input was generated by a "download file" command.
	        Capture it, checking the CRC and store it in "nfile".


    Note: This program uses termios.

Date: Tue, 27 Feb 90 11:47:38 pst
From: Bruce Culbertson <culberts@hplwbc.hpl.hp.com>
To: pc532@daver.bungi.com
Subject: Re:  ROM Debugger -- download command..

John L. Connin <johnc%manatee%uunet@daver> writes:
> Dave, what data format / protocol does the ROM debugger 'download' command 
> expect ??   

Here is the download program which I run at the other end of the
serial line -- in my case, on an AT clone.  The download protocol
is explained in the comments in the program.  I just recently added
the download command to the monitor so let me know if you have
problems with it or if you wish it worked differently.

Bruce Culbertson
----------------------------------------------------------------------
*/
/* MS-DOS Program for downloading to the NSC32000 Monitor.  Use this as a
 * template for writing downloaders for other OS's.  Compile the MS-DOS
 * version with the Microsoft C compiler.
 * 
 * Bruce Culbertson  18 February 1990
 */

/* Instructions for use:
 *
 * machine      prompt, command, etc.
 * -------------------------------------------------------------------
 * 32000	Command (? for help): download <address>
 * MS-DOS	[exit terminal emulator]
 * MS-DOS	C> <this program> <file name to download>
 * MS-DOS	[re-enter terminal emulator]
 * 32000	[hit return to get status of download]
 * 32000	Command (? for help): ...
 *
 * At any point you can send control-C (e.g. using your terminal emulator)
 * to the 32000 monitor to abort the download and return to the monitor
 * prompt.
 */

/* Download protocol:
 *
 * <start mark> <length> <data> <CRC>
 *
 * Below, the sending machine is called SRC, receiving machine is DST.
 * Eight bit characters are used.
 *
 * Control-C (0x03) aborts the transfer.  This capability is nice
 * to have if, for example, length is garbled and the DST expects
 * billions of characters.  Since any byte of <length>, <data>,
 * or <CRC> could be control-C, we need to have a quote character.
 * I use ESC (0x1b).  Thus, control-C and ESC are sent as
 * {0x1b 0x03} and {0x1b 0x1b}, respectively.
 *
 * Start mark:
 *   This is a colon.  When SRC begins sending, DST loops until it sees
 *   the start mark.  Thus, if spurious characters are sent as the
 *   user switches from terminal emulator to download program on SRC,
 *   DST can ignore them.
 *
 * Length:
 *   Four bytes, least significant first.  The length is number of
 *   data bytes to be transfered, not including quote characters.
 *   The two CRC bytes are also not included in the length.
 *
 * Data:
 *   A byte is sent as a byte, with quoting if necessary.
 *
 * CRC:
 *   Two bytes, least significant first.  Use CCITT CRC generator
 *   polynomial (x^16 + x^12 + x^5 + 1).  Compute on data only (not
 *   length or start) and exclude quotes.  (This is the same CRC
 *   as computed by Minix's CRC command.)
 */

#include <stdio.h>
#include <fcntl.h>

#ifdef MSDOS

#  define OPEN_FLAGS    (O_RDONLY | O_BINARY)
#  define off_t		long
#  define DEFAULT_PORT    1
long lseek();
int port_num = DEFAULT_PORT;
int port;

#else

#  include <unistd.h>
#  define OPEN_FLAGS    O_RDONLY

#endif

#define CCITT_GEN	0x11021		/* x^16 + x^12 + x^5 + 1 */
#define BUFSZ		0x1000
#define ESC		0x1b
#define CTL_C		0x03
#define START		':'

char buf[BUFSZ];
long write_data(), write_header();

void putch ();

void 
usage(name)
     char *name;
{
  fprintf (stderr, "usage: %s [-r] <file>\n", name);
  exit (1);
}

/* Output a character.  If it is a CLT_C or ESC, then quote (precede)
 * it with a ESC.
 */
void
write_ch (c)
int c;
{
  if (c == ESC || c == CTL_C)
    putch (ESC);
  putch (c);
}

/* Write two CRC bytes, LSB first.
 */
void
write_crc (crc)
long crc;
{
  write_ch ((int)((crc >> 0) & 0xff));
  write_ch ((int)((crc >> 8) & 0xff));
}


/* Given old CRC and new character, return new CRC.  Uses standard
 * CCITT CRC generator polynomial.
 */
unsigned long
update_crc (crc, ch)
long crc;
int ch;
{
  int i;

  for (i = 0x80; i; i >>= 1) {
    crc = (crc << 1) | (i & ch? 1: 0);
    if (crc & 0x10000) crc ^= CCITT_GEN;
  }
  return crc;
}

/* Write header.  Format is a colon followed by four byte length,
 * LSB first.  Length is the number of data bytes after quotes are
 * removed.
 */
long
write_header (fd)
int fd;
{
  long len;

  if (0 == (len = lseek (fd, (off_t)0, 2))) {
    fprintf (stderr, "file length is zero\n");
    exit (1);
  }
  lseek (fd, (off_t)0, 0);
  write_ch (START);
  write_ch ((int)((len >> 0) & 0xff));
  write_ch ((int)((len >> 8) & 0xff));
  write_ch ((int)((len >> 16) & 0xff));
  write_ch ((int)((len >> 24) & 0xff));
  return len;
}

/* Write data.
 */
long
write_data (fd)
int fd;
{
  long len, crc = 0;
  char *p;

  for (;;) {
    len = read (fd, buf, BUFSZ);
    if (-1 == len) {
      fprintf (stderr, "read failed\n");
      exit (1);
    }
    if (len == 0) break;
    for (p = buf; p < buf + len; ++p) {
      write_ch (*p);
      crc = update_crc (crc, *p);
    }
  }
  return crc;
}


#ifdef MSDOS
/* Write hardware directly since BIOS and DOS are not reliable.
 */
#define COM_WR      0
#define COM_RD      0
#define COM_IER     1
#define COM_CTL     3
#define COM_STAT    5
#define COM_CTL_VAL 3                   /* 8 bits, 1 stop, no parity */
#define COM_IER_VAL 0                   /* interrupts off */
#define COM_TX_RDY  0x20

int old_control, old_ier;

/* Output a character to the serial port.
 */
void
putch (c)
int c;
{
  int stat;
  
  for (;;) {
    stat = inp (port + COM_STAT);
    if (stat & COM_TX_RDY) break;
  }
  outp (port + COM_WR, c);
}

/* Initialize serial port and save old values.  Assume baud rate
 * already set.
 */
init_port()
{
  old_control = inp (port + COM_IER);
  old_ier = inp (port + COM_CTL);
  outp (port + COM_CTL, COM_CTL_VAL);
  outp (port + COM_IER, COM_IER_VAL);
}

/* Restore serial port to old configuration.
 */
restore_port()
{
  outp (port + COM_CTL, old_control);
  outp (port + COM_IER, old_ier);
}

#else

/* dummy routines for Unix! */
void
putch (int c)
{
  putchar (c);
}

#include <termios.h>

struct termios oldstate;
struct termios newstate;
int atty;

/* The following function is covered by: */

/*-
 * Copyright (c) 1989, 1993
 *      The Regents of the University of California.  All rights reserved.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
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
 * Make a pre-existing termios structure into "raw" mode: character-at-a-time
 * mode with no characters interpreted, 8-bit data path.
 */
void
makeraw(t)
        struct termios *t;
{
        t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
        t->c_oflag &= ~OPOST;
        t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
        t->c_cflag &= ~(CSIZE|PARENB);
        t->c_cflag |= CS8;
}

/* End of copyrighted material. */

void
init_port(int fd)
{
  atty = isatty(fd);
  if (atty) {
    tcgetattr (fd, &oldstate);
    newstate = oldstate;
    makeraw(&newstate);
    tcsetattr (fd, TCSANOW, &newstate);
  }
}

void
restore_port(int fd)
{
  if (atty) {
    tcsetattr (fd, TCSANOW, &oldstate);
  }
}

int
get_int (val, len)
unsigned long *val;
long len;
{
  unsigned char c;
  int shift = 0;

  *val = 0;

  while (len > 0) {
    c = getchar ();
    if (c == CTL_C) return 0;		/* handle control-C */
    if (c == ESC) c = getchar ();	/* handle quote */
    *val = *val + (c<<(8*shift++));
    --len;
  }
  return 1;
}

int
get_file (f, len, crc)
FILE *f;
unsigned long len, *crc;
{
  unsigned char c;

  while (len > 0) {
    c = getchar ();
    if (c == CTL_C) return 0;		/* handle control-C */
    if (c == ESC) c = getchar ();		/* handle quote */
    fputc(c,f);
    *crc = update_crc (*crc, c);	/* compute crc, skip quote */
    --len;
  }
  return 1;
}

/* Upload end of the commands for Unix! */

void
upload (name)
char *name;
{
  unsigned long crc, adr, len;
  unsigned char c;
  unsigned long xcrc;
  FILE *f;

  /* Open the file. */
  f = fopen (name, "w");
  if (f == NULL) {
    printf ("Could not open \"%s\" for writing.\n", name);
    exit(1);
  }

  /* set raw mode for input. */
  init_port(0);

  /* get start character */
  for (;;) {
    c = getchar();
    if (c == START) break;
    if (c == CTL_C) return;
  }

  /* get len in little endian form */
  if (!get_int (&len, 4)) {
    fprintf (stderr, "Upload interrupted.\n");
    restore_port(0);
    exit(2);
  }
  crc = 0;				/* crc on data only */
  if (!get_file (f, len, &crc)) {	/* get data */
    fprintf (stderr, "Upload interrupted.\n");
    restore_port(0);
    exit(3);
  }
  /* get crc in little endian */
  if (!get_int (&xcrc, 2)) {
    fprintf (stderr, "Upload interrupted.\n");
    restore_port(0);
    exit(4);
  }
  if (crc == xcrc)			/* print status */
    printf ("CRC ok, length = %d\n", len);
  else
    printf ("CRC error, received %d, expected %d, length %d\n",
	    xcrc, crc, len);

  fclose (f);

  /* Restore tty parameters. */
  restore_port(0);
}

#endif


/* Main program */
int
main (argc, argv)
     int argc;
     char **argv;
{
  int fd;
  long crc, len;

#ifdef MSDOS
  /* MSDOS argument processing */
  if (argc == 3) {
    if (1 != sscanf (argv[2], "%d", &port_num)) {
      fprintf (stderr, "Bad serial port, use 1 or 2\n");
      exit (1);
    }
    --argc;
  }
  if (argc != 2) {
    fprintf (stderr, "usage: %s <file> [<serial port>]\n", argv[0]);
    exit (1);
  }
  if (port_num == 1) port = 0x3f8;
  else if (port_num == 2) port = 0x2f8;
  else {
    fprintf (stderr, "Bad serial port, use 1 or 2\n");
    exit (1);
  }

#else

  int do_receive = 0;
  int ch;
  extern int optind;

  /* Unix argument processing */
  while ((ch = getopt(argc, argv, "r")) != -1) {
    switch (ch) {

    case 'r': 		/* Receive end of a download, use "upload()". */
      do_receive = 1;
      break;

    case '?':
    default:
      usage(argv[0]);
      return (1);
    }
  }

  if (argc-1 != optind)
    usage(argv[0]);
 
  if (do_receive) {
    upload (argv[2]);
    exit (0);
  }

#endif

  if (0 > (fd = open (argv[1], OPEN_FLAGS))) {
    fprintf (stderr, "can not open \"%s\" for reading\n", argv[1]);
    exit (1);
  }

  init_port(1);
  len = write_header (fd);
  crc = write_data (fd);
  write_crc (crc);
  printf ("Length=%ld CRC=%ld\n", len, crc);
  restore_port(1);
  exit (0);
}
