.\"	$NetBSD: 1.0.t,v 1.2 1998/01/09 06:54:40 perry Exp $
.\"
.\" Copyright (c) 1983, 1993, 1994
.\"	The Regents of the University of California.  All rights reserved.
.\"
.\" Redistribution and use in source and binary forms, with or without
.\" modification, are permitted provided that the following conditions
.\" are met:
.\" 1. Redistributions of source code must retain the above copyright
.\"    notice, this list of conditions and the following disclaimer.
.\" 2. Redistributions in binary form must reproduce the above copyright
.\"    notice, this list of conditions and the following disclaimer in the
.\"    documentation and/or other materials provided with the distribution.
.\" 3. All advertising materials mentioning features or use of this software
.\"    must display the following acknowledgement:
.\"	This product includes software developed by the University of
.\"	California, Berkeley and its contributors.
.\" 4. Neither the name of the University nor the names of its contributors
.\"    may be used to endorse or promote products derived from this software
.\"    without specific prior written permission.
.\"
.\" THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
.\" ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
.\" IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
.\" ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
.\" FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
.\" DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
.\" OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
.\" HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
.\" LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
.\" OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
.\" SUCH DAMAGE.
.\"
.\"	@(#)1.0.t	8.3 (Berkeley) 5/26/94
.\"
.Sh 1 "Kernel primitives
.PP
The facilities available to a user process are logically
divided into two parts: kernel facilities directly implemented by
code running in the operating system, and system facilities
implemented either by the system, or in cooperation with a
\fIserver process\fP.
The kernel facilities are described in section
.Xr 1 .
.PP
The facilities implemented in the kernel are those which define the
\fI4.4BSD virtual machine\fP in which each process runs.
Like many real machines, this virtual machine has memory management hardware,
an interrupt facility, timers and counters.  The 4.4BSD
virtual machine allows access to files and other objects through a set of
\fIdescriptors\fP.  Each descriptor resembles a device controller,
and supports a set of operations.  Like devices on real machines, some
of which are internal to the machine and some of which are external,
parts of the descriptor machinery are built-in to the operating system, while
other parts are implemented in server processes on other machines.
The facilities provided through the descriptor machinery are described in
section
.Xr 2 .
