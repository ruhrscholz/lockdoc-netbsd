/*
 * Copyright (c) 1997,1999 Martin Husemann <martin@duskware.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
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

/*
 * Last Edit-Date: [Sat Jan  6 12:30:04 2001]
 *
 * daicctl - maintenance tool and debug utility
 * main.c: argument parsing and function dispatching
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netisdn/i4b_ioctl.h>
#include "daicctl.h"

static void usage(void);
static void listall(int);

int
main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	int isdndev;
	int controller = -1;
	int lflag, dflag, xflag, iflag, sflag, vflag, pthrou;
	char *dnload = NULL, ch;

	pthrou = lflag = dflag = xflag = iflag = sflag = vflag = 0;
	while ((ch = getopt(argc, argv, "d:xsvlp")) != -1)
	    switch(ch) {
		case 'd':
			dnload = optarg;
			dflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'x':
			xflag = 1;
			break;
		case 'v':
			vflag = 1;
			break;
		case 'p':
			pthrou = 1;
			break;
		case '?':
		default:
			usage();
	    }

	argc -= optind;
	argv += optind;
	if (argc == 0)
		lflag = 1;
	else if (argc != 1)
		usage();
	else
		controller = (int)strtol(*argv, NULL, 10);

#ifdef __NetBSD__		
	isdndev = open(PATH_ISDN_DEV, O_RDWR|O_EXLOCK, 0);
#else
	isdndev = open(PATH_ISDN_DEV, O_RDWR, 0);
#endif
	if (isdndev < 0) {
		perror(PATH_ISDN_DEV);
		return 1;
	}

	if (lflag)
		listall(isdndev);

	if (controller < 0) {
		if (dflag || vflag || xflag || sflag || pthrou)
			fprintf(stderr, "\nYou must specify the controller number!\n");
	} else {
		if (dflag)
			download(isdndev, controller, dnload);
		if (vflag)
			xversion(isdndev, controller);
		if (xflag)
			xlog(isdndev, controller);
		if (sflag)
			istat(isdndev, controller);
		if (pthrou)
			passthrough(isdndev, controller);
	}

	close(isdndev);
	return 0;
}

void usage()
{
	fprintf(stderr, "usage: daicctl [-d (file)] [-xsie] controller\n"
		" -l          list available controllers\n"
		" -d (file)   download microcode from (file) to the card\n"
		" -x          xlog utility: show on card log\n"
		" -s          statistics (istat utility): show controller statistics\n"
		" -v          show version of microcode running on controller\n");
	exit(1);
}

static void
listall(int fd)
{
	int i, num;
	msg_ctrl_info_req_t info;

	memset(&info, 0, sizeof info);
	ioctl(fd, I4B_CTRL_INFO_REQ, &info);
	num = info.ncontroller;
	printf("There are %d controllers available:\n", num);
	for (i = 0; i < num; i++) {
		info.controller = i;
		ioctl(fd, I4B_CTRL_INFO_REQ, &info);
		switch (info.ctrl_type) {
			case CTRL_PASSIVE:
				printf("controller #%d: Siemens based passive card: ", i);
				switch (info.card_type) {
					case CARD_TYPEP_8:
						printf("Teles S0\n");
						break;
					case CARD_TYPEP_16:
						printf("Teles S0/16\n");
						break;
					case CARD_TYPEP_16_3:
						printf("Teles S0/16.3\n");
						break;
					case CARD_TYPEP_AVMA1:
						printf("AVM A1\n");
						break;
					case CARD_TYPEP_163P:
						printf("Teles S0/PnP\n");
						break;
					case CARD_TYPEP_CS0P:
						printf("Creatix S0/P&P\n");
						break;
					case CARD_TYPEP_USRTA:
						printf("USR Sportster internal TA\n");
						break;
					case CARD_TYPEP_DRNNGO:
						printf("Dr. Neuhaus Niccy GO@\n");
						break;
					case CARD_TYPEP_SWS:
						printf("Sedlbauer Win Speed\n");
						break;
					case CARD_TYPEP_DYNALINK:
						printf("Dynalink IS64PH\n");
						break;
					case CARD_TYPEP_BLMASTER:
						printf("ISDN Blaster / ISDN Master\n");
						break;
					case CARD_TYPEP_PCFRITZ:
						printf("AVM PCMCIA Fritz!Card\n");
						break;
					case CARD_TYPEP_ELSAQS1ISA:
						printf("ELSA QuickStep 1000pro ISA\n");
						break;
					case CARD_TYPEP_ELSAQS1PCI:
						printf("ELSA QuickStep 1000pro PCI\n");
						break;
					case CARD_TYPEP_SIEMENSITALK:
						printf("Siemens I-Talk\n");
						break;
					case CARD_TYPEP_ELSAMLIMC:
						printf("ELSA MicroLink ISDN/MC\n");
						break;
					case CARD_TYPEP_ELSAMLMCALL:
						printf("ELSA MicroLink MCall\n");
						break;
					case CARD_TYPEP_ITKIX1:
						printf("ITK ix1 micro\n");
						break;
					case CARD_TYPEP_AVMA1PCI:
						printf("AVM Fritz!Card PCI\n");
						break;
					case CARD_TYPEP_ASUSCOMIPAC:
						printf ("Asuscom ISDNlink 128 K PnP\n");
						break;
					case CARD_TYPEP_WINB6692:
						printf ("Winbond W6692 based\n");
						break;
					case CARD_TYPEP_16_3C:
						printf ("Teles S0/16.3c PnP\n");
						break;
					case CARD_TYPEP_ACERP10:
						printf ("Acer ISDN P10\n");
						break;
					case CARD_TYPEP_TELEINT_NO_1:
						printf ("TELEINT ISDN SPEED No. 1\n");
						break;
					default:
						printf("unknown card type %d\n", info.card_type);
						break;
				}
				break;
			case CTRL_DAIC:
				printf("controller #%d: Diehl active ISDN card: ", i);
				switch (info.card_type) {
					case CARD_TYPEA_DAIC_S:
						printf("S\n");
						break;
					case CARD_TYPEA_DAIC_SX:
						printf("SX\n");
						break;
					case CARD_TYPEA_DAIC_SCOM:
						printf("SCOM\n");
						break;
					case CARD_TYPEA_DAIC_QUAD:
						printf("QUADRO\n");
						break;
					default:
						printf("unknown type %d\n", info.card_type);
						break;
				}
				break;
			default:
				printf("unknown controller type\n");
				break;
		}
	}
}

void passthrough(int fd, int controller)
{
	static u_int8_t in_data[] = {
		0x70, 0x07, 0x81,
		'9', '8', '9', '0', '2', '0',
		0x00
	};
	u_int8_t out_data[16];

	struct isdn_diagnostic_request req;

	printf("passing through: hardcoded dial request\n");
	memset(&req, 0, sizeof(req));
	req.controller = controller;
	req.cmd = 0x05;
	req.in_param_len = sizeof in_data;
	req.in_param = &in_data;
	req.out_param_len = sizeof out_data;
	req.out_param = &out_data;

	if (ioctl(fd, I4B_ACTIVE_DIAGNOSTIC, &req) == -1) {
		perror("ioctl(I4B_ACTIVE_DIAGNOSTIC)");
		return;
	}
	printf("Return code: 0x%02x\n", out_data[0]);
}
