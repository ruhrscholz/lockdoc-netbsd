/*++
/* NAME
/*	stream_trigger 3
/* SUMMARY
/*	wakeup stream server
/* SYNOPSIS
/*	#include <trigger.h>
/*
/*	int	stream_trigger(service, buf, len, timeout)
/*	const char *service;
/*	const char *buf;
/*	int	len;
/*	int	timeout;
/* DESCRIPTION
/*	stream_trigger() wakes up the named stream server by making
/*	a brief connection to it and writing the named buffer.
/*
/*	Arguments:
/* .IP service
/*	Name of the communication endpoint.
/* .IP buf
/*	Address of data to be written.
/* .IP len
/*	Amount of data to be written.
/* .IP timeout
/*	Deadline in seconds. Specify a value <= 0 to disable
/*	the time limit.
/* DIAGNOSTICS
/*	The result is zero in case of success, -1 in case of problems.
/* SEE ALSO
/*	stream_connect(3), stream client
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <unistd.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <connect.h>
#include <iostuff.h>
#include <trigger.h>

/* stream_trigger - wakeup stream server */

int     stream_trigger(const char *service, const char *buf, int len, int timeout)
{
    const char *myname = "stream_trigger";
    int     fd;

    if (msg_verbose > 1)
	msg_info("%s: service %s", myname, service);

    /*
     * Connect...
     */
    if ((fd = stream_connect(service, BLOCKING, timeout)) < 0) {
	if (msg_verbose)
	    msg_warn("%s: connect to %s: %m", myname, service);
	return (-1);
    }

    /*
     * Write the request...
     */
    if (write_buf(fd, buf, len, timeout) < 0)
	if (msg_verbose)
	    msg_warn("%s: write to %s: %m", myname, service);

    /*
     * Disconnect.
     */
    if (close(fd) < 0)
	if (msg_verbose)
	    msg_warn("%s: close %s: %m", myname, service);
    return (0);
}
