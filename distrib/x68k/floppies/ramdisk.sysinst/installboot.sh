#! /bin/sh
#
#	Special version of installboot for the installer
#
#	$NetBSD: installboot.sh,v 1.1 1999/12/05 16:26:40 minoura Exp $

echo "WARNING: This version of $0 works only for sd's!"
echo "WARNING: Also the boot block is specially prepared"
echo "         to be as big just as 8192 byte."

if [ "$1" = "-v" ]; then
    shift
fi
echo dd if=$1 of=$2
dd if=$1 of=$2
