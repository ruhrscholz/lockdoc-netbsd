/*	$NetBSD: authdes.c,v 1.3 1999/07/02 15:58:36 simonb Exp $	*/

/*
 * authdes.c -  dummy encryption routines for destinations outside the USA.
 *
 * Sorry, folks; I hate this, too. Send me your e-mail address in an
 * envelope bearing a US postmark and I'll send you the decryption key
 * for the des program normally distributed with Unix in the USA. Outside
 * the USA you are on your own; however, you should be able quickly to
 * obtain the source from lots of places, homegrown or otherwise.
 *
 * to decrypt the des routine, mumble the following:
 *
 * des -d -k key authdes.c.des authdes.c
 *
 * , where key is as above, and rebuild. To restore the distribution
 * to its exportable state, copy this file to authdes.c .
 */
#include <sys/types.h>
#include "ntp_stdlib.h"

/*
 * This routine is normally called to compute the key schedule.
 */
void
DESauth_subkeys(key, encryptkeys, decryptkeys)
	const u_int32 *key;
	u_char *encryptkeys;
	u_char *decryptkeys;
{
}

/*
 * This routine is normally called to encrypt and decrypt the data. This
 * is done in-place using the Digital Encryption Standard (DES) Cipher-
 * Block Chaining (CBC) method as described in the NTP specification.
 */
void
DESauth_des(data, subkeys)
	u_int32 *data;
	u_char *subkeys;
{
}
