/*	$NetBSD: atactl.c,v 1.23 2003/11/30 14:07:49 yamt Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Ken Hornstein.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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
 * atactl(8) - a program to control ATA devices.
 */
#include <sys/cdefs.h>

#ifndef lint
__RCSID("$NetBSD: atactl.c,v 1.23 2003/11/30 14:07:49 yamt Exp $");
#endif


#include <sys/param.h>
#include <sys/ioctl.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>

#include <dev/ata/atareg.h>
#include <dev/ata/atavar.h>
#include <dev/ic/wdcreg.h>
#include <sys/ataio.h>

struct command {
	const char *cmd_name;
	const char *arg_names;
	void (*cmd_func)(int, char *[]);
};

struct bitinfo {
	u_int bitmask;
	const char *string;
};

int	main(int, char *[]);
void	usage(void);
void	ata_command(struct atareq *);
void	print_bitinfo(const char *, const char *, u_int, struct bitinfo *);
void	print_smart_status(void *vbuf, void *tbuf);
int	is_smart(void);

int	fd;				/* file descriptor for device */
const	char *dvname;			/* device name */
char	dvname_store[MAXPATHLEN];	/* for opendisk(3) */
const	char *cmdname;			/* command user issued */
const	char *argnames;			/* helpstring: expected arguments */

void	device_identify(int, char *[]);
void	device_setidle(int, char *[]);
void	device_idle(int, char *[]);
void	device_checkpower(int, char *[]);
void	device_smart(int, char *[]);

struct command commands[] = {
	{ "identify",	"",			device_identify },
	{ "setidle",	"idle-timer",		device_setidle },
	{ "setstandby",	"standby-timer",	device_setidle },
	{ "idle",	"",			device_idle },
	{ "standby",	"",			device_idle },
	{ "sleep",	"",			device_idle },
	{ "checkpower",	"",			device_checkpower },
	{ "smart",	"enable|disable|status", device_smart },
	{ NULL,		NULL,			NULL },
};

/*
 * Tables containing bitmasks used for error reporting and
 * device identification.
 */

struct bitinfo ata_caps[] = {
	{ WDC_CAP_DMA, "DMA" },
	{ WDC_CAP_LBA, "LBA" },
	{ ATA_CAP_STBY, "ATA standby timer values" },
	{ WDC_CAP_IORDY, "IORDY operation" },
	{ WDC_CAP_IORDY_DSBL, "IORDY disabling" },
	{ 0, NULL },
};

struct bitinfo ata_vers[] = {
	{ WDC_VER_ATA1,	"ATA-1" },
	{ WDC_VER_ATA2,	"ATA-2" },
	{ WDC_VER_ATA3,	"ATA-3" },
	{ WDC_VER_ATA4,	"ATA-4" },
	{ WDC_VER_ATA5,	"ATA-5" },
	{ WDC_VER_ATA6,	"ATA-6" },
	{ WDC_VER_ATA7,	"ATA-7" },
	{ 0, NULL },
};

struct bitinfo ata_cmd_set1[] = {
	{ WDC_CMD1_NOP, "NOP command" },
	{ WDC_CMD1_RB, "READ BUFFER command" },
	{ WDC_CMD1_WB, "WRITE BUFFER command" },
	{ WDC_CMD1_HPA, "Host Protected Area feature set" },
	{ WDC_CMD1_DVRST, "DEVICE RESET command" },
	{ WDC_CMD1_SRV, "SERVICE interrupt" },
	{ WDC_CMD1_RLSE, "release interrupt" },
	{ WDC_CMD1_AHEAD, "look-ahead" },
	{ WDC_CMD1_CACHE, "write cache" },
	{ WDC_CMD1_PKT, "PACKET command feature set" },
	{ WDC_CMD1_PM, "Power Management feature set" },
	{ WDC_CMD1_REMOV, "Removable Media feature set" },
	{ WDC_CMD1_SEC, "Security Mode feature set" },
	{ WDC_CMD1_SMART, "SMART feature set" },
	{ 0, NULL },
};

struct bitinfo ata_cmd_set2[] = {
	{ ATA_CMD2_FCE, "FLUSH CACHE EXT command" },
	{ WDC_CMD2_FC, "FLUSH CACHE command" },
	{ WDC_CMD2_DCO, "Device Configuration Overlay feature set" },
	{ ATA_CMD2_LBA48, "48-bit Address feature set" },
	{ WDC_CMD2_AAM, "Automatic Acoustic Management feature set" },
	{ WDC_CMD2_SM, "SET MAX security extention" },
	{ WDC_CMD2_SFREQ, "SET FEATURES required to spin-up after power-up" },
	{ WDC_CMD2_PUIS, "Power-Up In Standby feature set" },
	{ WDC_CMD2_RMSN, "Removable Media Status Notification feature set" },
	{ ATA_CMD2_APM, "Advanced Power Management feature set" },
	{ ATA_CMD2_CFA, "CFA feature set" },
	{ ATA_CMD2_RWQ, "READ/WRITE DMA QUEUED commands" },
	{ WDC_CMD2_DM, "DOWNLOAD MICROCODE command" },
	{ 0, NULL },
};

struct bitinfo ata_cmd_ext[] = {
	{ ATA_CMDE_TLCONT, "Time-limited R/W feature set R/W Continuous mode" },
	{ ATA_CMDE_TL, "Time-limited Read/Write" },
	{ ATA_CMDE_URGW, "URG bit for WRITE STREAM DMA/PIO" },
	{ ATA_CMDE_URGR, "URG bit for READ STREAM DMA/PIO" },
	{ ATA_CMDE_WWN, "World Wide name" },
	{ ATA_CMDE_WQFE, "WRITE DMA QUEUED FUA EXT command" },
	{ ATA_CMDE_WFE, "WRITE DMA/MULTIPLE FUA EXT commands" },
	{ ATA_CMDE_GPL, "General Purpose Logging feature set" },
	{ ATA_CMDE_STREAM, "Streaming feature set" },
	{ ATA_CMDE_MCPTC, "Media Card Pass Through Command feature set" },
	{ ATA_CMDE_MS, "Media serial number" },
	{ ATA_CMDE_SST, "SMART self-test" },
	{ ATA_CMDE_SEL, "SMART error logging" },
	{ 0, NULL },
};

static const struct {
	const int	id;
	const char	*name;
} smart_attrs[] = {
	{ 1,		"Raw read error rate" },
	{ 2,		"Throughput performance" },
	{ 3,		"Spin-up time" },
	{ 4,		"Start/stop count" },
	{ 5,		"Reallocated sector count" },
	{ 7,		"Seek error rate" },
	{ 8,		"Seek time performance" },
	{ 9,		"Power-on hours count" },
	{ 10,		"Spin retry count" },
	{ 11,		"Calibration retry count" },
	{ 12,		"Device power cycle count" },
	{ 191,		"Gsense error rate" },
	{ 192,		"Power-off retract count" },
	{ 193,		"Load cycle count" },
	{ 194,		"Temperature" },
	{ 196,		"Reallocated event count" },
	{ 197,		"Current pending sector" },
	{ 198,		"Offline uncorrectable" },
	{ 199,		"Ultra DMA CRC error count" },
	{ 0,		"" },
};

int
main(int argc, char *argv[])
{
	int i;

	/* Must have at least: device command */
	if (argc < 3)
		usage();

	/* Skip program name, get and skip device name and command. */
	dvname = argv[1];
	cmdname = argv[2];
	argv += 3;
	argc -= 3;

	/*
	 * Open the device
	 */
	fd = opendisk(dvname, O_RDWR, dvname_store, sizeof(dvname_store), 0);
	if (fd == -1) {
		if (errno == ENOENT) {
			/*
			 * Device doesn't exist.  Probably trying to open
			 * a device which doesn't use disk semantics for
			 * device name.  Try again, specifying "cooked",
			 * which leaves off the "r" in front of the device's
			 * name.
			 */
			fd = opendisk(dvname, O_RDWR, dvname_store,
			    sizeof(dvname_store), 1);
			if (fd == -1)
				err(1, "%s", dvname);
		} else
			err(1, "%s", dvname);
	}

	/*
	 * Point the dvname at the actual device name that opendisk() opened.
	 */
	dvname = dvname_store;

	/* Look up and call the command. */
	for (i = 0; commands[i].cmd_name != NULL; i++)
		if (strcmp(cmdname, commands[i].cmd_name) == 0)
			break;
	if (commands[i].cmd_name == NULL)
		errx(1, "unknown command: %s", cmdname);

	argnames = commands[i].arg_names;

	(*commands[i].cmd_func)(argc, argv);
	exit(0);
}

void
usage(void)
{
	int i;

	fprintf(stderr, "Usage: %s device command [arg [...]]\n",
	    getprogname());

	fprintf(stderr, "   Available device commands:\n");
	for (i=0; commands[i].cmd_name != NULL; i++)
		fprintf(stderr, "\t%s %s\n", commands[i].cmd_name,
					    commands[i].arg_names);

	exit(1);
}

/*
 * Wrapper that calls ATAIOCCOMMAND and checks for errors
 */

void
ata_command(struct atareq *req)
{
	int error;

	error = ioctl(fd, ATAIOCCOMMAND, req);

	if (error == -1)
		err(1, "ATAIOCCOMMAND failed");

	switch (req->retsts) {

	case ATACMD_OK:
		return;
	case ATACMD_TIMEOUT:
		fprintf(stderr, "ATA command timed out\n");
		exit(1);
	case ATACMD_DF:
		fprintf(stderr, "ATA device returned a Device Fault\n");
		exit(1);
	case ATACMD_ERROR:
		if (req->error & WDCE_ABRT)
			fprintf(stderr, "ATA device returned Aborted "
				"Command\n");
		else
			fprintf(stderr, "ATA device returned error register "
				"%0x\n", req->error);
		exit(1);
	default:
		fprintf(stderr, "ATAIOCCOMMAND returned unknown result code "
			"%d\n", req->retsts);
		exit(1);
	}
}

/*
 * Print out strings associated with particular bitmasks
 */

void
print_bitinfo(const char *bf, const char *af, u_int bits, struct bitinfo *binfo)
{

	for (; binfo->bitmask != 0; binfo++)
		if (bits & binfo->bitmask)
			printf("%s%s%s", bf, binfo->string, af);
}

/*
 * Print out SMART attribute thresholds and values
 */

void
print_smart_status(void *vbuf, void *tbuf)
{
	struct ata_smart_attributes *value_buf = vbuf;
	struct ata_smart_thresholds *threshold_buf = tbuf;
	int values[256];
	int thresholds[256];
	int flags[256];
	int i, j;
	int id;
	int8_t checksum;

	for (i = checksum = 0; i < 511; i++)
		checksum += ((int8_t *) value_buf)[i];
	checksum *= -1;
	if (checksum != value_buf->checksum) {
		fprintf(stderr, "SMART attribute values checksum error\n");
		return;
	}

	for (i = checksum = 0; i < 511; i++)
		checksum += ((int8_t *) threshold_buf)[i];
	checksum *= -1;
	if (checksum != threshold_buf->checksum) {
		fprintf(stderr, "SMART attribute thresholds checksum error\n");
		return;
	}

	memset(values, 0, sizeof(values));
	memset(thresholds, 0, sizeof(thresholds));
	memset(flags, 0, sizeof(flags));

	for (i = 0; i < 30; i++) {
		id = value_buf->attributes[i].id;
		values[id] = value_buf->attributes[i].value;
		flags[id] = value_buf->attributes[i].flags;
		id = threshold_buf->thresholds[i].id;
		thresholds[id] = threshold_buf->thresholds[i].value;
	}

	printf("id\tvalue\tthresh\tcrit\tcollect\treliability description\n");
	for (i = 0; i < 256; i++) {
		if (values[i] != 00 && values[i] != 0xFE && values[i] != 0xFF) {
			for (j = 0; smart_attrs[j].id != i && smart_attrs[j].id != 0; j++);
			printf("%3d\t%3d\t%3d\t%s\t%sline\t%stive    %s\n",
			       i, values[i], thresholds[i],
			       flags[i] & WDSM_ATTR_ADVISORY ? "yes" : "no",
			       flags[i] & WDSM_ATTR_COLLECTIVE ? "on" : "off",
			       values[i] > thresholds[i] ? "posi" : "nega",
			       smart_attrs[j].name);
		}
	}
}

/*
 * is_smart:
 *
 *	Detect whether device supports SMART and SMART is enabled.
 */

int
is_smart(void)
{
	int retval = 0;
	struct atareq req;
	unsigned char inbuf[DEV_BSIZE];
	struct ataparams *inqbuf;
	char *status;

	memset(&inbuf, 0, sizeof(inbuf));
	memset(&req, 0, sizeof(req));

	inqbuf = (struct ataparams *) inbuf;

	req.flags = ATACMD_READ;
	req.command = WDCC_IDENTIFY;
	req.databuf = (caddr_t) inbuf;
	req.datalen = sizeof(inbuf);
	req.timeout = 1000;

	ata_command(&req);

	if (inqbuf->atap_cmd_def != 0 && inqbuf->atap_cmd_def != 0xffff) {
		if (!(inqbuf->atap_cmd_set1 & WDC_CMD1_SMART)) {
			fprintf(stderr, "SMART unsupported\n");
		} else {
			if (inqbuf->atap_ata_major <= WDC_VER_ATA5 ||
			    inqbuf->atap_cmd_set2 == 0xffff ||
			    inqbuf->atap_cmd_set2 == 0x0000) {
				status = "status unknown";
				retval = 2;
			} else {
				if (inqbuf->atap_cmd1_en & WDC_CMD1_SMART) {
					status = "enabled";
					retval = 1;
				} else {
					status = "disabled";
				}
			}
			printf("SMART supported, SMART %s\n", status);
		}
	}
	return retval;
}
					
/*
 * DEVICE COMMANDS
 */

/*
 * device_identify:
 *
 *	Display the identity of the device
 */
void
device_identify(int argc, char *argv[])
{
	struct ataparams *inqbuf;
	struct atareq req;
	unsigned char inbuf[DEV_BSIZE];
#if BYTE_ORDER == LITTLE_ENDIAN
	int i;
	u_int16_t *p;
#endif

	/* No arguments. */
	if (argc != 0)
		usage();

	memset(&inbuf, 0, sizeof(inbuf));
	memset(&req, 0, sizeof(req));

	inqbuf = (struct ataparams *) inbuf;

	req.flags = ATACMD_READ;
	req.command = WDCC_IDENTIFY;
	req.databuf = (caddr_t) inbuf;
	req.datalen = sizeof(inbuf);
	req.timeout = 1000;

	ata_command(&req);

#if BYTE_ORDER == LITTLE_ENDIAN
	/*
	 * On little endian machines, we need to shuffle the string
	 * byte order.  However, we don't have to do this for NEC or
	 * Mitsumi ATAPI devices
	 */

	if (!((inqbuf->atap_config & WDC_CFG_ATAPI_MASK) == WDC_CFG_ATAPI &&
	      ((inqbuf->atap_model[0] == 'N' &&
		  inqbuf->atap_model[1] == 'E') ||
	       (inqbuf->atap_model[0] == 'F' &&
		  inqbuf->atap_model[1] == 'X')))) {
		for (i = 0 ; i < sizeof(inqbuf->atap_model); i += 2) {
			p = (u_short *) (inqbuf->atap_model + i);
			*p = ntohs(*p);
		}
		for (i = 0 ; i < sizeof(inqbuf->atap_serial); i += 2) {
			p = (u_short *) (inqbuf->atap_serial + i);
			*p = ntohs(*p);
		}
		for (i = 0 ; i < sizeof(inqbuf->atap_revision); i += 2) {
			p = (u_short *) (inqbuf->atap_revision + i);
			*p = ntohs(*p);
		}
	}
#endif

	/*
	 * Strip blanks off of the info strings.  Yuck, I wish this was
	 * cleaner.
	 */

	if (inqbuf->atap_model[sizeof(inqbuf->atap_model) - 1] == ' ') {
		inqbuf->atap_model[sizeof(inqbuf->atap_model) - 1] = '\0';
		while (inqbuf->atap_model[strlen(inqbuf->atap_model) - 1] == ' ')
			inqbuf->atap_model[strlen(inqbuf->atap_model) - 1] = '\0';
	}

	if (inqbuf->atap_revision[sizeof(inqbuf->atap_revision) - 1] == ' ') {
		inqbuf->atap_revision[sizeof(inqbuf->atap_revision) - 1] = '\0';
		while (inqbuf->atap_revision[strlen(inqbuf->atap_revision) - 1] == ' ')
			inqbuf->atap_revision[strlen(inqbuf->atap_revision) - 1] = '\0';
	}

	if (inqbuf->atap_serial[sizeof(inqbuf->atap_serial) - 1] == ' ') {
		inqbuf->atap_serial[sizeof(inqbuf->atap_serial) - 1] = '\0';
		while (inqbuf->atap_serial[strlen(inqbuf->atap_serial) - 1] == ' ')
			inqbuf->atap_serial[strlen(inqbuf->atap_serial) - 1] = '\0';
	}

	printf("Model: %.*s, Rev: %.*s, Serial #: %.*s\n",
	       (int) sizeof(inqbuf->atap_model), inqbuf->atap_model,
	       (int) sizeof(inqbuf->atap_revision), inqbuf->atap_revision,
	       (int) sizeof(inqbuf->atap_serial), inqbuf->atap_serial);

	printf("Device type: %s, %s\n", inqbuf->atap_config & WDC_CFG_ATAPI ?
	       "ATAPI" : "ATA", inqbuf->atap_config & ATA_CFG_FIXED ? "fixed" :
	       "removable");

	if ((inqbuf->atap_config & WDC_CFG_ATAPI_MASK) == 0)
		printf("Cylinders: %d, heads: %d, sec/track: %d, total "
		       "sectors: %d\n", inqbuf->atap_cylinders,
		       inqbuf->atap_heads, inqbuf->atap_sectors,
		       (inqbuf->atap_capacity[1] << 16) |
		       inqbuf->atap_capacity[0]);

	if (inqbuf->atap_queuedepth & WDC_QUEUE_DEPTH_MASK)
		printf("Device supports command queue depth of %d\n",
		       inqbuf->atap_queuedepth & 0xf);

	printf("Device capabilities:\n");
	print_bitinfo("\t", "\n", inqbuf->atap_capabilities1, ata_caps);

	if (inqbuf->atap_ata_major != 0 && inqbuf->atap_ata_major != 0xffff) {
		printf("Device supports following standards:\n");
		print_bitinfo("", " ", inqbuf->atap_ata_major, ata_vers);
		printf("\n");
	}

	if (inqbuf->atap_cmd_set1 != 0 && inqbuf->atap_cmd_set1 != 0xffff &&
	    inqbuf->atap_cmd_set2 != 0 && inqbuf->atap_cmd_set2 != 0xffff) {
		printf("Command set support:\n");
		print_bitinfo("\t", "\n", inqbuf->atap_cmd_set1, ata_cmd_set1);
		print_bitinfo("\t", "\n", inqbuf->atap_cmd_set2, ata_cmd_set2);
		if (inqbuf->atap_cmd_ext != 0 && inqbuf->atap_cmd_ext != 0xffff)
			print_bitinfo("\t", "\n", inqbuf->atap_cmd_ext,
			    ata_cmd_ext);
	}

	if (inqbuf->atap_cmd_def != 0 && inqbuf->atap_cmd_def != 0xffff) {
		printf("Command sets/features enabled:\n");
		print_bitinfo("\t", "\n", inqbuf->atap_cmd1_en &
			      (WDC_CMD1_SRV | WDC_CMD1_RLSE | WDC_CMD1_AHEAD |
			       WDC_CMD1_CACHE | WDC_CMD1_SEC | WDC_CMD1_SMART),
			       ata_cmd_set1);
		print_bitinfo("\t", "\n", inqbuf->atap_cmd2_en &
			      (WDC_CMD2_RMSN | ATA_CMD2_APM), ata_cmd_set2);
	}

	return;
}

/*
 * device idle:
 *
 * issue the IDLE IMMEDIATE command to the drive
 */

void
device_idle(int argc, char *argv[])
{
	struct atareq req;

	/* No arguments. */
	if (argc != 0)
		usage();

	memset(&req, 0, sizeof(req));

	if (strcmp(cmdname, "idle") == 0)
		req.command = WDCC_IDLE_IMMED;
	else if (strcmp(cmdname, "standby") == 0)
		req.command = WDCC_STANDBY_IMMED;
	else
		req.command = WDCC_SLEEP;

	req.timeout = 1000;

	ata_command(&req);

	return;
}

/*
 * Set the idle timer on the disk.  Set it for either idle mode or
 * standby mode, depending on how we were invoked.
 */

void
device_setidle(int argc, char *argv[])
{
	unsigned long idle;
	struct atareq req;
	char *end;

	/* Only one argument */
	if (argc != 1)
		usage();

	idle = strtoul(argv[0], &end, 0);

	if (*end != '\0') {
		fprintf(stderr, "Invalid idle time: \"%s\"\n", argv[0]);
		exit(1);
	}

	if (idle > 19800) {
		fprintf(stderr, "Idle time has a maximum value of 5.5 "
			"hours\n");
		exit(1);
	}

	if (idle != 0 && idle < 5) {
		fprintf(stderr, "Idle timer must be at least 5 seconds\n");
		exit(1);
	}

	memset(&req, 0, sizeof(req));

	if (idle <= 240*5)
		req.sec_count = idle / 5;
	else
		req.sec_count = idle / (30*60) + 240;

	req.command = cmdname[3] == 's' ? WDCC_STANDBY : WDCC_IDLE;
	req.timeout = 1000;

	ata_command(&req);

	return;
}

/*
 * Query the device for the current power mode
 */

void
device_checkpower(int argc, char *argv[])
{
	struct atareq req;

	/* No arguments. */
	if (argc != 0)
		usage();

	memset(&req, 0, sizeof(req));

	req.command = WDCC_CHECK_PWR;
	req.timeout = 1000;
	req.flags = ATACMD_READREG;

	ata_command(&req);

	printf("Current power status: ");

	switch (req.sec_count) {
	case 0x00:
		printf("Standby mode\n");
		break;
	case 0x80:
		printf("Idle mode\n");
		break;
	case 0xff:
		printf("Active mode\n");
		break;
	default:
		printf("Unknown power code (%02x)\n", req.sec_count);
	}

	return;
}

/*
 * device_smart:
 *
 *	Display SMART status
 */
void
device_smart(int argc, char *argv[])
{
	struct atareq req;
	unsigned char inbuf[DEV_BSIZE];
	unsigned char inbuf2[DEV_BSIZE];

	/* Only one argument */
	if (argc != 1)
		usage();

	if (strcmp(argv[0], "enable") == 0) {
		memset(&req, 0, sizeof(req));

		req.features = WDSM_ENABLE_OPS;
		req.command = WDCC_SMART;
		req.cylinder = htole16(WDSMART_CYL);
		req.timeout = 1000;

		ata_command(&req);

		is_smart();
	} else if (strcmp(argv[0], "disable") == 0) {
		memset(&req, 0, sizeof(req));

		req.features = WDSM_DISABLE_OPS;
		req.command = WDCC_SMART;
		req.cylinder = htole16(WDSMART_CYL);
		req.timeout = 1000;

		ata_command(&req);

		is_smart();
	} else if (strcmp(argv[0], "status") == 0) {
		if (is_smart()) {
			memset(&inbuf, 0, sizeof(inbuf));
			memset(&req, 0, sizeof(req));

			req.features = WDSM_STATUS;
			req.command = WDCC_SMART;
			req.cylinder = htole16(WDSMART_CYL);
			req.timeout = 1000;
	
			ata_command(&req);

			if (req.cylinder != htole16(WDSMART_CYL)) {
				fprintf(stderr, "Threshold exceeds condition\n");
			}

			/* WDSM_RD_DATA and WDSM_RD_THRESHOLDS are optional
			 * features, the following ata_command()'s may error
			 * and exit().
			 */

			memset(&inbuf, 0, sizeof(inbuf));
			memset(&req, 0, sizeof(req));

			req.flags = ATACMD_READ;
			req.features = WDSM_RD_DATA;
			req.command = WDCC_SMART;
			req.databuf = (caddr_t) inbuf;
			req.datalen = sizeof(inbuf);
			req.cylinder = htole16(WDSMART_CYL);
			req.timeout = 1000;
	
			ata_command(&req);

			memset(&inbuf2, 0, sizeof(inbuf2));
			memset(&req, 0, sizeof(req));

			req.flags = ATACMD_READ;
			req.features = WDSM_RD_THRESHOLDS;
			req.command = WDCC_SMART;
			req.databuf = (caddr_t) inbuf2;
			req.datalen = sizeof(inbuf2);
			req.cylinder = htole16(WDSMART_CYL);
			req.timeout = 1000;

			ata_command(&req);

			print_smart_status(inbuf, inbuf2);
		} else {
			fprintf(stderr, "SMART not supported\n");
		}
	} else {
		usage();
	}
	return;
}
