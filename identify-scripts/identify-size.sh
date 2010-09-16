#!/bin/sh

sysdir=`/var/disks/front_tray/identify-sys.sh $1`

if [ x"$sysdir" != "x" ]; then
	cat $sysdir/block/*/size
fi

